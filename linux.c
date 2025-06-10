/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: Apple TV Linux bootloader - main file
 * SPDX-License-Identifier: MIT
 */

#include <linux.h>

// Kernel and initramfs files.

#include "kernel_bin.h"

#ifdef INITRAMFS
#include "initramfs_bin.h"
#else
unsigned char   initramfs_bin[] = {};
unsigned int    initramfs_bin_len = 0;
#endif

#define LINUX_KERNEL_LOAD_INCREMENT 0x100000
#define SMBIOS_TABLE_LOW 0xF0000

// Video parameters
extern linear_framebuffer_t fb;

static uint32_t round_up(uintn_t num, uintn_t multiple)
{
    uint32_t mask = multiple - 1;
    return (num + mask) & ~mask;
}

static void *get_rsdp_from_systbl(efi_system_table_32_t *systbl)
{
    void *rsdp;

    efi_config_table_32_t *config_tables = (efi_config_table_32_t *) systbl->tables;

    for (int i = 0; i < systbl->nr_tables; i++)
    {
        if (efi_guidcmp(config_tables[i].guid, ACPI_20_TABLE_GUID) == 0)
        {
            rsdp = config_tables[i].table;
        }
    }

    if (!rsdp)
        fail(__FILE__, __LINE__, "No RSDP found!\n");

    return rsdp;
}

static void copy_smbios_to_lowmem(efi_system_table_32_t *systbl)
{
    void *smbios_tbl;

    efi_config_table_32_t *config_tables = (efi_config_table_32_t *) systbl->tables;

    for (int i = 0; i < systbl->nr_tables; i++)
    {
        if (efi_guidcmp(config_tables[i].guid, SMBIOS_TABLE_GUID) == 0)
        {
            smbios_tbl = config_tables[i].table;
        }
    }
    if (smbios_tbl)
    {
        trace("Copying SMBIOS to low memory...");
        memcpy((void *) SMBIOS_TABLE_LOW, smbios_tbl, SIZE_OF_SMBIOS_TABLE_HEADER);
        dprintf("done.\n");
    }
}

noreturn void load_linux(void)
{
    struct boot_params  bp;

    trace("Initializing Linux loader...\n");

    // Check that we are loading a Linux kernel
    uint32_t *signature = (uint32_t *) (kernel_bin + 0x202);
    if (*signature != 'SrdH')
    {
        fail(__FILE__, __LINE__, "Kernel has invalid signature! Did you specify the wrong file?");
    }

    trace("Found valid Linux kernel.\n");

    // Find actual length of the kernel
    uint32_t real_kernel_len = kernel_bin_len - ((kernel_bin[0x1f1] + 1) * 512);

    // Determine where a safe place in memory to copy the kernel to is
    uint32_t kernel_loadaddr = round_up(gBA->kernel_base + gBA->kernel_size + initramfs_bin_len,
                                        LINUX_KERNEL_LOAD_INCREMENT);

    // Copy kernel to the correct address
    trace("Copying kernel to 0x%X...", kernel_loadaddr);
    memcpy((void *) kernel_loadaddr, &kernel_bin[(kernel_bin[0x1f1] + 1) * 512], real_kernel_len);
    dprintf("done.\n");

    // Zero the boot parameters
    memset(&bp, 0, sizeof(struct boot_params));

    // Copy the existing setup_header from the kernel
    struct setup_header *setup_header = &bp.hdr;
    uint32_t setup_header_end = kernel_bin[0x201] + 0x202;
    memcpy(setup_header, (kernel_bin + 0x1f1), setup_header_end - 0x1f1);

    // Configure the setup_header
    setup_header->cmd_line_ptr      = (uint32_t) ((uint8_t *) gBA->cmdline);
    setup_header->vid_mode          = 0xffff; // "normal"
    setup_header->type_of_loader    = 0xff; // unassigned

    if (!(setup_header->loadflags & LOADED_HIGH))
    {
        fail(__FILE__, __LINE__, "Kernels that load at 0x10000 are unsupported!");
    }

    // Configure the initramfs
    if (initramfs_bin_len)
    {

        // we should actually copy the initramfs to high memory to avoid kernel oops at free_init_pages()
        // counterintuitively, this seems to lead to more available RAM once booted.
        uint32_t ramdisk_loadaddr = gBA->kernel_base + gBA->kernel_size;

        trace("Copying initramfs to 0x%X...", ramdisk_loadaddr);
        memcpy((void *) ramdisk_loadaddr, &initramfs_bin, initramfs_bin_len);
        dprintf("done.\n");

        setup_header->ramdisk_image = ramdisk_loadaddr;
        setup_header->ramdisk_size  = initramfs_bin_len;
    }

    // Configure video
    struct screen_info *screen_info = &bp.screen_info;

    screen_info->capabilities       = VIDEO_CAPABILITY_SKIP_QUIRKS;
    screen_info->flags              = VIDEO_FLAGS_NOCURSOR;
    screen_info->lfb_base           = (uint32_t) fb.base;
    screen_info->lfb_size           = (uint32_t) fb.size;
    screen_info->lfb_width          = fb.width;
    screen_info->lfb_height         = fb.height;
    screen_info->lfb_depth          = fb.depth;
    screen_info->lfb_linelength     = fb.pitch;
    screen_info->red_size           = fb.red_size;
    screen_info->red_pos            = fb.red_shift;
    screen_info->green_size         = fb.green_size;
    screen_info->green_pos          = fb.green_shift;
    screen_info->blue_size          = fb.blue_size;
    screen_info->blue_pos           = fb.blue_shift;
    screen_info->rsvd_size          = fb.reserved_size;
    screen_info->rsvd_pos           = fb.reserved_shift;

    screen_info->orig_video_isVGA   = VIDEO_TYPE_EFI;

    // Setup ACPI
    bp.acpi_rsdp_addr = (uint32_t) get_rsdp_from_systbl((efi_system_table_32_t *) gBA->efi_sys_tbl);

    // Some kernels seem to not play well with the Apple TVs EFI and will triple fault if the EFI loader signature
    // is specified.
    // For this reason, we do NOT load the kernel in EFI mode; instead we copy the SMBIOS to low memory so that
    // Linux can see it even if it's not booted in EFI mode.

    copy_smbios_to_lowmem((efi_system_table_32_t *) gBA->efi_sys_tbl);

    // Setup E820
    bp.e820_entries = efi_to_e820_map((efi_memory_desc_t *) gBA->efi_mem_map_ptr,
                                      gBA->efi_mem_map_size,
                                      gBA->efi_mem_desc_size,
                                      bp.e820_table);

    // We should be good to start the Linux kernel now.
    // Jump to the kernel load address!
    trace("Starting kernel...");
    asm("jmp *%0"::"r"(kernel_loadaddr), "S"(&bp));

    // we should never get here
    fail(__FILE__, __LINE__, "UNREACHABLE");
}
/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: XNU boot arguments.
 * SPDX-License-Identifier: MIT
*/

#pragma once

#define MACH_CMDLINE            1024

#define DISPLAY_MODE_GRAPHICS   1
#define DISPLAY_MODE_TEXT       2

// See xnu-1228 pexpert/pexpert/i386/boot.h
typedef struct _mach_video_t
{
    uint32_t    base_addr;
    uint32_t    display_mode; // DISPLAY_MODE_TEXT in verbose/single user mode, DISPLAY_MODE_GRAPHICS otherwise
    uint32_t    pitch;
    uint32_t    width;
    uint32_t    height;
    uint32_t    depth;
} __attribute__((aligned(4))) mach_video_t;

typedef struct _mach_boot_args_t
{
    uint16_t    revision;
    uint16_t    version;
    /*
     * Command line string comes from 3 sources:
     * `boot-args` NVRAM variable
     * `Kernel Flags` key in `com.apple.Boot.plist`
     * Key combinations:
     *      Command-V:  Verbose mode (-v argument)
     *      Command-S:  Single user mode (-s argument)
     *      Shift:      Safe mode (-x argument)
     */
    char            cmdline[1024];

    uint32_t        efi_mem_map_ptr;
    uint32_t        efi_mem_map_size;
    uint32_t        efi_mem_desc_size;
    uint32_t        efi_mem_desc_ver;

    mach_video_t    video;

    uint32_t        device_tree_ptr;
    uint32_t        device_tree_len;

    uint32_t        kernel_base;
    uint32_t        kernel_size;

    uint32_t        efi_runtime_page_ptr;
    uint32_t        efi_runtime_page_count;
    uint32_t        efi_sys_tbl;

    uint8_t         efi_mode;

    uint8_t         __reserved1[3];
    uint32_t        __reserved2[7];
} __attribute__((aligned(4))) mach_boot_args_t;
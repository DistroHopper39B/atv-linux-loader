/*
 * Copyright (C) 2025 Sylas Hollander & Linux kernel authors (see mentioned files)
 * PURPOSE: Apple TV Linux bootloader - EFI to E820 memory map conversion
 * SPDX-License-Identifier: MIT
 */

#include <linux.h>

static uint32_t efi_convert_to_e820_type(uint32_t efi_type)
{
    switch (efi_type)
    {
        // Unusable memory types
        case EFI_RESERVED_TYPE:
        case EFI_UNUSABLE_MEMORY:
        case EFI_MEMORY_MAPPED_IO:
        case EFI_MEMORY_MAPPED_IO_PORT_SPACE:
        case EFI_PAL_CODE:
        case EFI_RUNTIME_SERVICES_CODE:
        case EFI_RUNTIME_SERVICES_DATA:
            return E820_RESERVED;

        // Types usable after ACPI initialization
        case EFI_ACPI_RECLAIM_MEMORY:
            return E820_ACPI;

        // ACPI NVS memory
        case EFI_ACPI_MEMORY_NVS:
            return E820_NVS;

        // Usable memory
        case EFI_BOOT_SERVICES_CODE:
        case EFI_BOOT_SERVICES_DATA:
        case EFI_CONVENTIONAL_MEMORY:
        case EFI_LOADER_CODE:
        case EFI_LOADER_DATA:
            return E820_RAM;

        default:
            err("Unknown memory type, memory map probably corrupted!\n");
            return E820_UNUSABLE;
    }
}

static void e820_add_memory_region(struct boot_e820_entry *memory_map,
                                uint8_t *num_entries,
                                uint64_t addr,
                                uint64_t size,
                                uint32_t type)
{
    uint32_t x = *num_entries;
    if (x == E820_MAX_ENTRIES_ZEROPAGE)
        fail(__FILE__, __LINE__, "Too many BIOS memory descriptors!");

    // Add on to existing entry if possible
    if ((x > 0)
        && (memory_map[x - 1].addr + memory_map[x - 1].size == addr)
        && (memory_map[x - 1].type == type))
    {
        memory_map[x - 1].size  += size;
    }
    else
    {
        memory_map[x].addr      = addr;
        memory_map[x].size      = size;
        memory_map[x].type      = type;
        (*num_entries)++;
    }
}

// Convert an EFI memory map to an E820 memory map.
// Returns the number of entries.
uint8_t efi_to_e820_map(efi_memory_desc_t *efi_map,
                         uint32_t efi_map_size,
                         uint32_t efi_desc_size,
                         struct boot_e820_entry *output_map)
{
    uint32_t efi_num_entries = (efi_map_size / efi_desc_size);
    uint8_t bios_num_entries = 0;

    for (int i = 0; i < efi_num_entries; i++)
    {
        e820_add_memory_region(output_map,
                               &bios_num_entries,
                               efi_map->phys_addr,
                               (efi_map->num_pages << EFI_PAGE_SHIFT),
                               efi_convert_to_e820_type(efi_map->type));

        efi_map = next_memdesc(efi_map, efi_desc_size);
    }

    return bios_num_entries;
}
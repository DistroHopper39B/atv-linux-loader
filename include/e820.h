/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: Apple TV Linux bootloader - main header
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

/* ADAPTED FROM linux/arch/x86/include/uapi/asm/setup_data.h */
struct boot_e820_entry {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed));

/* ADAPTED FROM linux/arch/x86/include/uapi/asm/e820.h */

#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3
#define E820_NVS	4
#define E820_UNUSABLE	5
#define E820_PMEM	7

extern uint8_t efi_to_e820_map(efi_memory_desc_t *efi_map, uint32_t efi_map_size, uint32_t efi_desc_size, struct boot_e820_entry *output_map);

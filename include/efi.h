/*
 * Copyright (C) 2025 Sylas Hollander & Linux kernel authors (see mentioned files)
 * PURPOSE: Apple TV Linux bootloader - EFI support
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

/* ADAPTED FROM Linux/include/linux/uuid.h */
#define UUID_SIZE 16

typedef struct {
    uint8_t b[UUID_SIZE];
} guid_t;

/* ADAPTED FROM Linux/include/linux/efi.h */
typedef guid_t efi_guid_t __attribute((aligned(__alignof__(uint32_t))));

#define EFI_GUID(a, b, c, d...) ((efi_guid_t){ {				\
	(a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,	\
	(b) & 0xff, ((b) >> 8) & 0xff,						\
	(c) & 0xff, ((c) >> 8) & 0xff, d } })

// REQUIRED GUIDS
#define ACPI_20_TABLE_GUID			EFI_GUID(0x8868e871, 0xe4f1, 0x11d3,  0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81)
#define SMBIOS_TABLE_GUID           EFI_GUID(0xeb9d2d31, 0x2d88, 0x11d3,  0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d)

#define SIZE_OF_SMBIOS_TABLE_HEADER 0x20

/*
 * Generic EFI table header
 */
typedef	struct {
    uint64_t signature;
    uint32_t revision;
    uint32_t headersize;
    uint32_t crc32;
    uint32_t reserved;
} efi_table_hdr_t;

typedef struct {
    efi_guid_t guid;
    void *table;
} efi_config_table_32_t;

typedef struct {
    efi_table_hdr_t hdr;
    uint32_t fw_vendor;	/* physical addr of CHAR16 vendor string */
    uint32_t fw_revision;
    uint32_t con_in_handle;
    uint32_t con_in;
    uint32_t con_out_handle;
    uint32_t con_out;
    uint32_t stderr_handle;
    uint32_t stderr;
    uint32_t runtime;
    uint32_t boottime;
    uint32_t nr_tables;
    uint32_t tables;
} efi_system_table_32_t;

/*
 * Memory map descriptor:
 */

/* Memory types: */
#define EFI_RESERVED_TYPE		        0
#define EFI_LOADER_CODE			        1
#define EFI_LOADER_DATA			        2
#define EFI_BOOT_SERVICES_CODE		    3
#define EFI_BOOT_SERVICES_DATA		    4
#define EFI_RUNTIME_SERVICES_CODE	    5
#define EFI_RUNTIME_SERVICES_DATA	    6
#define EFI_CONVENTIONAL_MEMORY		    7
#define EFI_UNUSABLE_MEMORY		        8
#define EFI_ACPI_RECLAIM_MEMORY		    9
#define EFI_ACPI_MEMORY_NVS		        10
#define EFI_MEMORY_MAPPED_IO		    11
#define EFI_MEMORY_MAPPED_IO_PORT_SPACE	12
#define EFI_PAL_CODE			        13
#define EFI_PERSISTENT_MEMORY		    14
#define EFI_UNACCEPTED_MEMORY		    15
#define EFI_MAX_MEMORY_TYPE		        16

#define EFI_PAGE_SHIFT		12
#define EFI_PAGE_SIZE		(1UL << EFI_PAGE_SHIFT)
#define EFI_PAGES_MAX		(U64_MAX >> EFI_PAGE_SHIFT)

typedef struct {
    uint32_t type;
    uint32_t pad;
    uint64_t phys_addr;
    uint64_t virt_addr;
    uint64_t num_pages;
    uint64_t attribute;
} efi_memory_desc_t;

#define next_memdesc(desc, desc_size) \
    (efi_memory_desc_t *) ((char *) (desc) + (desc_size))

static inline int
efi_guidcmp (efi_guid_t left, efi_guid_t right)
{
    return (memcmp(&left, &right, sizeof(efi_guid_t)));
}


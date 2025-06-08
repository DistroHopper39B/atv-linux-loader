/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: ATVLib main header file.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "types.h"
#include "cons.h"
#include "baselibc_string.h"
#include "boot_args.h"
#include "tinyprintf.h"
#include "debug.h"

extern mach_boot_args_t     *gBA;
extern boolean_t            verbose;
extern noreturn void halt(void);
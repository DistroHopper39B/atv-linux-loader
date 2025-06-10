/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: ATVLib debug support header file.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define dprintf(fmt, ...)   if (verbose) printf(fmt, ##__VA_ARGS__)
#define trace(fmt, ...)     dprintf("(%s:%d) trace: ", __FILE__, __LINE__); \
                            dprintf(fmt, ##__VA_ARGS__)
#define warn(fmt, ...)      dprintf("(%s:%d) warn: ", __FILE__, __LINE__); \
                            dprintf(fmt, ##__VA_ARGS__)
#define err(fmt, ...)       printf("(%s:%d) err: ", __FILE__, __LINE__); \
                            printf(fmt, ##__VA_ARGS__)

extern noreturn void fail(char *file, uint32_t line, const char *err);
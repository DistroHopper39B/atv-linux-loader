/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: ATVLib debug support code.
 * SPDX-License-Identifier: MIT
 */

#include <atvlib.h>

noreturn void fail(char *file, uint32_t line, const char *err)
{
    cons_change_fg_color(COLOR_YELLOW);

    printf("A fatal error has occurred in the file %s at line %d!! The Apple TV cannot continue.\n", file, line);
    printf("The error is: %s\n", err);
    printf("\n!!!! SYSTEM HALTED !!!!\n");

    halt();
}
/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: ATVLib main file.
 * SPDX-License-Identifier: MIT
 */

#include <atvlib.h>

mach_boot_args_t    *gBA;
boolean_t           verbose;

// C entry point for Apple TV code
noreturn void atvlib_init(mach_boot_args_t *ba)
{
    gBA = ba;

    // Initialize console.
    if (!cons_init(&ba->video, COLOR_WHITE, COLOR_BLACK))
        halt();

    // Enable verbose output.
    // This is triggered by adding "-v" or "-x" to the "Kernel Flags" key in com.apple.Boot.plist
    // or holding Command-V or Command-S on the keyboard.
    verbose = (ba->video.display_mode == DISPLAY_MODE_TEXT);
    if (verbose)
        cons_clear_screen(COLOR_BLACK);

    trace("Hello, world!\n");

    fail(__FILE__, __LINE__, "Test");
    halt();
}
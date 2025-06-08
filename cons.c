/*
 * Copyright (C) 2025 Sylas Hollander.
 * PURPOSE: General purpose video functions for linear framebuffers
 * SPDX-License-Identifier: MIT
 */

#include <atvlib.h>
#include <font.h>

linear_framebuffer_t    fb;
console_priv_t          con;
char                    print_buf[PRINT_BUFFER_SIZE];

static
void video_print_char(char c, uint32_t x, uint32_t y, uint32_t fg_color, uint32_t bg_color)
{
    uint32_t    char_position = c * ISO_CHAR_HEIGHT;

    // Set up pixel location
    uint32_t *pixel = (uint32_t *) ((char *) fb.base + (y * ISO_CHAR_HEIGHT) * fb.pitch + (x * ISO_CHAR_WIDTH * 4));

    // Print character to screen
    for (uint8_t line = 0; line < ISO_CHAR_HEIGHT; line++)
    {
        uint8_t char_line = iso_font[char_position];
        for (uint8_t column = 0; column < ISO_CHAR_WIDTH; column++)
        {
            uint8_t mask = char_line >> column;
            pixel[column] = ((mask & 1) ? fg_color : bg_color);
        }
        char_position++;
        pixel = (uint32_t *) ((char *) pixel + fb.pitch);
    }
}

static
void cons_print_char(void *p, char c)
{
    (void)(p); // Unused parameter.

    // CASE 1: Video not enabled yet.
    // Return silently on this case.
    if (!fb.enabled)
        return;

    // CASE 2: Text wrapped around.
    // Add a newline in this case.
    if (con.cursor_x >= con.width)
    {
        con.cursor_x = 0;
        con.cursor_y++;
    }

    // CASE 3: Screen is full.
    // Scroll down (this function is very slow)
    if (con.cursor_y >= con.height)
    {
        uint32_t    top = (fb.pitch * ISO_CHAR_HEIGHT);

        // We should double buffer here. But we don't. Who cares.
        memmove((void *) fb.base, (const void *) fb.base + top, fb.size - top);
        memset((void *) fb.base + fb.size - top, 0, top);
        con.cursor_y--;
    }

    // Now we print the character.
    switch (c)
    {
        // CASE 1: newline.
        case '\r': // CR
        case '\n': // LF
        {
            con.cursor_x = 0;
            con.cursor_y++;
            break;
        }
        // CASE 2: backspace.
        case '\b':
        {
            con.cursor_x--;
            video_print_char(c, con.cursor_x, con.cursor_y, con.fg_color, con.bg_color);
            break;
        }
        // CASE 3: any other character.
        default:
        {
            video_print_char(c, con.cursor_x, con.cursor_y, con.fg_color, con.bg_color);
            con.cursor_x++;
            break;
        }
    }
}

boolean_t cons_clear_screen(uint32_t color)
{
    if (!fb.enabled)
        return false;

    uint32_t    native_color = RGBA_TO_NATIVE(fb, color);

    for (uint32_t line = 0; line < fb.height; line++)
    {
        uint32_t *screen = (uint32_t *) ((char *) fb.base + line * fb.pitch);
        for (uint32_t column = 0; column < fb.width; column++)
        {
            *screen++ = native_color;
        }
    }

    con.cursor_x = 0;
    con.cursor_y = 0;

    return true;
}

boolean_t cons_change_fg_color(uint32_t fg_color)
{
    if (!fb.enabled)
        return false;

    con.fg_color        = RGBA_TO_NATIVE(fb, fg_color);

    return true;
}

boolean_t cons_change_bg_color(uint32_t bg_color)
{
    if (!fb.enabled)
        return false;

    con.bg_color        = RGBA_TO_NATIVE(fb, bg_color);

    return true;
}

// Platform specific video initialization code.
// This must be implemented differently on every platform this is ported to.
// After this, we will have a working printf.
boolean_t cons_init(void *video_params, uint32_t fg_color, uint32_t bg_color)
{
    if (!video_params)
        return false;

    mach_video_t        *mv = video_params;

    memset(&fb, 0, sizeof(fb));
    memset(&con, 0, sizeof(con));

    // set up screen
    fb.enabled          = false;

    fb.base             = mv->base_addr;
    fb.size             = (mv->pitch * mv->height);

    fb.pitch            = mv->pitch;
    fb.width            = mv->width;
    fb.height           = mv->height;
    fb.depth            = 32;
    fb.pixels_per_row   = (mv->pitch * 4);

    fb.red_size         = 8;
    fb.red_shift        = 16;
    fb.green_size       = 8;
    fb.green_shift      = 8;
    fb.blue_size        = 8;
    fb.blue_shift       = 0;
    fb.reserved_size    = 8;
    fb.reserved_shift   = 0;

    // set up text console
    con.cursor_x        = 0;
    con.cursor_y        = 0;
    con.width           = fb.width / ISO_CHAR_WIDTH;
    con.height          = fb.height / ISO_CHAR_HEIGHT;
    con.size            = con.width * con.height;
    con.fg_color        = RGBA_TO_NATIVE(fb, fg_color);
    con.bg_color        = RGBA_TO_NATIVE(fb, bg_color);

    // initialize printf function
    init_printf(print_buf, cons_print_char);

    fb.enabled = true;

    return true;
}
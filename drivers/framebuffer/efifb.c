/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cyanea/console.h>
#include <cyanea/string.h>
#include <cyanea/errno.h>
#include <cyanea/init.h>

#include <asm-generic/ioremap.h>

#include "font.h"

# define __DRIVER_ID__ "tty"

static u8 *framebuffer;
static unsigned long cursor_x, cursor_y;
static unsigned long screen_width, screen_height;
static unsigned long pitch;

# define PIXEL_WHITE ((PIXEL) { 160, 160, 160, 0 })

typedef struct {
    u8 blue, green, red, reserved;
} PIXEL;

/* Plotting Pixel. */
static void efifb_ppixel(int x, int y, PIXEL pixel)
{
    volatile PIXEL *p = (volatile PIXEL *)(framebuffer + y * pitch + x * sizeof(PIXEL));

    *p = pixel;
}

static void efifb_draw_char(int x, int y, char ch)
{
    int char_x, char_y;

    for (char_y = 0; char_y < CHAR_HEIGHT; char_y++) {
        for (char_x = 0; char_x < CHAR_WIDTH; char_x++) {
            if (__font_char_pixel(ch, char_y, char_x))
                efifb_ppixel(x + char_x, y + char_y, PIXEL_WHITE);
        }
    }
}

static void efifb_scroll_screen(void)
{
    size_t size = (screen_height - CHAR_HEIGHT) * pitch;

    /* SCROLL up! */
    memcpy(&framebuffer[0], &framebuffer[CHAR_HEIGHT * pitch], size);
    memset(&framebuffer[size], 0, pitch * CHAR_HEIGHT);

    cursor_y -= CHAR_HEIGHT;
}

static void efifb_put_char(char ch)
{
    if (ch == '\n') {
        cursor_x = 0;
        cursor_y += CHAR_HEIGHT;

        if (cursor_y >= screen_height)
            efifb_scroll_screen();
    } else {
        efifb_draw_char(cursor_x, cursor_y, ch);

        cursor_x += CHAR_WIDTH;
        if (cursor_x >= screen_width) {
            cursor_x = 0;
            cursor_y += CHAR_HEIGHT;

            if (cursor_y >= screen_height)
                efifb_scroll_screen();
        }
    }
}

static void efifb_clear_screen(void)
{
    memset(framebuffer, 0, screen_height * pitch);

    cursor_x = 0;
    cursor_y = 0;
}

static void con_write(struct console *con, const char *s, size_t count)
{
    int i;

    for (i = 0; i < count; i++, s++)
        efifb_put_char(*s);
}

static int con_setup(struct console *con, char *options)
{
    screen_width = boot_params.screen_info.screen_width;
    screen_height = boot_params.screen_info.screen_height;
    pitch = boot_params.screen_info.pixels_per_scan_line * sizeof(PIXEL);

    framebuffer = ioremap(boot_params.screen_info.framebuffer, boot_params.screen_info.framebuffer_size,
            PAGE_KERNEL_NOCACHE);
    if (!framebuffer)
        return -ENOMEM;

    efifb_clear_screen();

    return SUCCESS;
}

static struct console console_efifb = {
    .name = __DRIVER_ID__,
    .write = con_write,
    .setup = con_setup,
    .index = -1
};

__PURE_CONSTRUCTOR__
static void console_init(void)
{
    register_console(&console_efifb);
}

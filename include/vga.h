#ifndef VGA_H
#define VGA_H

#include "types.h"

#define VGA_MEMORY 0xB8000

typedef enum {
    VGA_MODE_80x25 = 0,
    VGA_MODE_80x50 = 1,
    VGA_MODE_40x25 = 2,
} vga_mode_t;

typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

void vga_init(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_write(const char* str);
void vga_writeln(const char* str);
void vga_set_color(vga_color_t fg, vga_color_t bg);
void vga_scroll(void);
bool vga_set_mode(vga_mode_t mode);
void vga_get_resolution(uint8_t* width, uint8_t* height);
const char* vga_get_mode_name(vga_mode_t mode);

#endif


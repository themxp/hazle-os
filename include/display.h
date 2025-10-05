#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"
#include "framebuffer.h"

typedef enum {
    DISPLAY_COLOR_BLACK = 0,
    DISPLAY_COLOR_BLUE = 1,
    DISPLAY_COLOR_GREEN = 2,
    DISPLAY_COLOR_CYAN = 3,
    DISPLAY_COLOR_RED = 4,
    DISPLAY_COLOR_MAGENTA = 5,
    DISPLAY_COLOR_BROWN = 6,
    DISPLAY_COLOR_LIGHT_GREY = 7,
    DISPLAY_COLOR_DARK_GREY = 8,
    DISPLAY_COLOR_LIGHT_BLUE = 9,
    DISPLAY_COLOR_LIGHT_GREEN = 10,
    DISPLAY_COLOR_LIGHT_CYAN = 11,
    DISPLAY_COLOR_LIGHT_RED = 12,
    DISPLAY_COLOR_LIGHT_MAGENTA = 13,
    DISPLAY_COLOR_YELLOW = 14,
    DISPLAY_COLOR_WHITE = 15,
} display_color_t;

void display_init(void);
void display_clear(void);
void display_putchar(char c);
void display_write(const char* str);
void display_writeln(const char* str);
void display_set_color(display_color_t fg, display_color_t bg);
void display_scroll(void);
uint16_t display_get_columns(void);
uint16_t display_get_rows(void);

#endif


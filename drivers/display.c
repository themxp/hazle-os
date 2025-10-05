#include "display.h"
#include "framebuffer.h"
#include "font_8x16.h"
#include "vesa.h"

static const color_t color_palette[] = {
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_LIGHT_GREY,
    COLOR_DARK_GREY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE,
};

static color_t current_fg = COLOR_LIGHT_GREY;
static color_t current_bg = COLOR_BLACK;
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;
static uint16_t columns = 0;
static uint16_t rows = 0;

void display_init(void) {
    uint16_t width = vesa_get_width();
    uint16_t height = vesa_get_height();
    
    columns = width / FONT_WIDTH;
    rows = height / FONT_HEIGHT;
    
    cursor_x = 0;
    cursor_y = 0;
    current_fg = COLOR_LIGHT_GREY;
    current_bg = COLOR_BLACK;
}

void display_clear(void) {
    fb_clear(current_bg);
    cursor_x = 0;
    cursor_y = 0;
}

void display_scroll(void) {
    fb_scroll(FONT_HEIGHT, current_bg);
}

void display_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            fb_draw_char(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, ' ', current_fg, current_bg);
        }
    } else {
        fb_draw_char(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, c, current_fg, current_bg);
        cursor_x++;
    }
    
    if (cursor_x >= columns) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= rows) {
        display_scroll();
        cursor_y = rows - 1;
    }
}

void display_write(const char* str) {
    while (*str) {
        display_putchar(*str);
        str++;
    }
}

void display_writeln(const char* str) {
    display_write(str);
    display_putchar('\n');
}

void display_set_color(display_color_t fg, display_color_t bg) {
    if (fg < 16) {
        current_fg = color_palette[fg];
    }
    if (bg < 16) {
        current_bg = color_palette[bg];
    }
}

uint16_t display_get_columns(void) {
    return columns;
}

uint16_t display_get_rows(void) {
    return rows;
}


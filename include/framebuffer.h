#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "types.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} color_t;

#define COLOR_BLACK         ((color_t){0, 0, 0, 255})
#define COLOR_BLUE          ((color_t){0, 0, 170, 255})
#define COLOR_GREEN         ((color_t){0, 170, 0, 255})
#define COLOR_CYAN          ((color_t){0, 170, 170, 255})
#define COLOR_RED           ((color_t){170, 0, 0, 255})
#define COLOR_MAGENTA       ((color_t){170, 0, 170, 255})
#define COLOR_BROWN         ((color_t){170, 85, 0, 255})
#define COLOR_LIGHT_GREY    ((color_t){170, 170, 170, 255})
#define COLOR_DARK_GREY     ((color_t){85, 85, 85, 255})
#define COLOR_LIGHT_BLUE    ((color_t){85, 85, 255, 255})
#define COLOR_LIGHT_GREEN   ((color_t){85, 255, 85, 255})
#define COLOR_LIGHT_CYAN    ((color_t){85, 255, 255, 255})
#define COLOR_LIGHT_RED     ((color_t){255, 85, 85, 255})
#define COLOR_LIGHT_MAGENTA ((color_t){255, 85, 255, 255})
#define COLOR_YELLOW        ((color_t){255, 255, 85, 255})
#define COLOR_WHITE         ((color_t){255, 255, 255, 255})

void fb_init(uint32_t addr, uint16_t width, uint16_t height, uint16_t pitch, uint8_t bpp);
void fb_clear(color_t color);
void fb_putpixel(uint16_t x, uint16_t y, color_t color);
color_t fb_getpixel(uint16_t x, uint16_t y);
void fb_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_t color);
void fb_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_t color);
void fb_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, color_t color);
void fb_draw_char(uint16_t x, uint16_t y, char c, color_t fg, color_t bg);
void fb_scroll(uint16_t lines, color_t bg);
uint16_t fb_get_width(void);
uint16_t fb_get_height(void);

#endif


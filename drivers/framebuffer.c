#include "framebuffer.h"
#include "font_8x16.h"
#include "string.h"

static uint32_t* fb_addr = NULL;
static uint16_t fb_width = 0;
static uint16_t fb_height = 0;
static uint16_t fb_pitch = 0;
static uint8_t fb_bpp = 0;

void fb_init(uint32_t addr, uint16_t width, uint16_t height, uint16_t pitch, uint8_t bpp) {
    fb_addr = (uint32_t*)addr;
    fb_width = width;
    fb_height = height;
    fb_pitch = pitch;
    fb_bpp = bpp;
}

void fb_clear(color_t color) {
    uint32_t pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    
    for (uint32_t i = 0; i < (fb_width * fb_height); i++) {
        fb_addr[i] = pixel;
    }
}

void fb_putpixel(uint16_t x, uint16_t y, color_t color) {
    if (x >= fb_width || y >= fb_height) {
        return;
    }
    
    uint32_t pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    uint32_t offset = y * (fb_pitch / 4) + x;
    fb_addr[offset] = pixel;
}

color_t fb_getpixel(uint16_t x, uint16_t y) {
    if (x >= fb_width || y >= fb_height) {
        return COLOR_BLACK;
    }
    
    uint32_t offset = y * (fb_pitch / 4) + x;
    uint32_t pixel = fb_addr[offset];
    
    color_t color;
    color.b = pixel & 0xFF;
    color.g = (pixel >> 8) & 0xFF;
    color.r = (pixel >> 16) & 0xFF;
    color.a = (pixel >> 24) & 0xFF;
    
    return color;
}

void fb_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_t color) {
    for (uint16_t i = 0; i < width; i++) {
        fb_putpixel(x + i, y, color);
        fb_putpixel(x + i, y + height - 1, color);
    }
    
    for (uint16_t i = 0; i < height; i++) {
        fb_putpixel(x, y + i, color);
        fb_putpixel(x + width - 1, y + i, color);
    }
}

void fb_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_t color) {
    uint32_t pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    
    for (uint16_t j = 0; j < height; j++) {
        if (y + j >= fb_height) break;
        
        uint32_t offset = (y + j) * (fb_pitch / 4) + x;
        for (uint16_t i = 0; i < width; i++) {
            if (x + i >= fb_width) break;
            fb_addr[offset + i] = pixel;
        }
    }
}

void fb_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, color_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (dx > dy) ? (dx > -dx ? dx : -dx) : (dy > -dy ? dy : -dy);
    
    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;
    
    float x = x1;
    float y = y1;
    
    for (int i = 0; i <= steps; i++) {
        fb_putpixel((uint16_t)x, (uint16_t)y, color);
        x += x_inc;
        y += y_inc;
    }
}

void fb_draw_char(uint16_t x, uint16_t y, char c, color_t fg, color_t bg) {
    if (x + FONT_WIDTH > fb_width || y + FONT_HEIGHT > fb_height) {
        return;
    }
    
    const uint8_t* glyph = font_8x16[(uint8_t)c];
    
    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t line = glyph[row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            if (line & (0x80 >> col)) {
                fb_putpixel(x + col, y + row, fg);
            } else {
                fb_putpixel(x + col, y + row, bg);
            }
        }
    }
}

void fb_scroll(uint16_t lines, color_t bg) {
    uint32_t pixels_to_scroll = lines;
    uint32_t pixels_per_row = fb_pitch / 4;
    uint32_t total_pixels = pixels_per_row * fb_height;
    uint32_t pixels_to_move = pixels_per_row * (fb_height - pixels_to_scroll);
    
    for (uint32_t i = 0; i < pixels_to_move; i++) {
        fb_addr[i] = fb_addr[i + pixels_per_row * pixels_to_scroll];
    }
    
    uint32_t pixel = (bg.a << 24) | (bg.r << 16) | (bg.g << 8) | bg.b;
    for (uint32_t i = pixels_to_move; i < total_pixels; i++) {
        fb_addr[i] = pixel;
    }
}

uint16_t fb_get_width(void) {
    return fb_width;
}

uint16_t fb_get_height(void) {
    return fb_height;
}


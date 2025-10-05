#ifndef VESA_H
#define VESA_H

#include "types.h"

typedef struct {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed)) vbe_mode_info_t;

typedef enum {
    VESA_640x480 = 0,
    VESA_800x600 = 1,
    VESA_1024x768 = 2,
    VESA_1280x720 = 3,
    VESA_1280x1024 = 4,
    VESA_1920x1080 = 5,
} vesa_mode_t;

typedef struct {
    uint16_t mode_number;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    const char* name;
} vesa_mode_entry_t;

bool vesa_init(vesa_mode_t mode);
bool vesa_init_with_info(uint32_t fb_addr, uint16_t width, uint16_t height, uint16_t pitch, uint8_t bpp);
bool vesa_set_mode(vesa_mode_t mode);
uint32_t vesa_get_framebuffer(void);
uint16_t vesa_get_width(void);
uint16_t vesa_get_height(void);
uint16_t vesa_get_pitch(void);
uint8_t vesa_get_bpp(void);
const char* vesa_get_mode_name(vesa_mode_t mode);
vesa_mode_t vesa_get_current_mode(void);
int vesa_get_mode_count(void);
const vesa_mode_entry_t* vesa_get_mode_info(int index);

#endif


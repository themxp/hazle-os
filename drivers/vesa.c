#include "vesa.h"
#include "io.h"
#include "string.h"

static const vesa_mode_entry_t vesa_modes[] = {
    {0x101, 640, 480, 32, "640x480"},
    {0x103, 800, 600, 32, "800x600"},
    {0x105, 1024, 768, 32, "1024x768"},
    {0x107, 1280, 720, 32, "1280x720"},
    {0x109, 1280, 1024, 32, "1280x1024"},
    {0x10B, 1920, 1080, 32, "1920x1080"},
};

static uint32_t framebuffer_addr = 0;
static uint16_t screen_width = 0;
static uint16_t screen_height = 0;
static uint16_t screen_pitch = 0;
static uint8_t screen_bpp = 0;
static vesa_mode_t current_mode = VESA_1920x1080;

bool vesa_init(vesa_mode_t mode) {
    if (mode >= sizeof(vesa_modes) / sizeof(vesa_mode_entry_t)) {
        mode = VESA_1024x768;
    }
    
    current_mode = mode;
    screen_width = vesa_modes[mode].width;
    screen_height = vesa_modes[mode].height;
    screen_bpp = vesa_modes[mode].bpp;
    screen_pitch = screen_width * (screen_bpp / 8);
    
    return true;
}

bool vesa_init_with_info(uint32_t fb_addr, uint16_t width, uint16_t height, uint16_t pitch, uint8_t bpp) {
    framebuffer_addr = fb_addr;
    screen_width = width;
    screen_height = height;
    screen_pitch = pitch;
    screen_bpp = bpp;
    
    for (int i = 0; i < (int)(sizeof(vesa_modes) / sizeof(vesa_mode_entry_t)); i++) {
        if (vesa_modes[i].width == width && vesa_modes[i].height == height) {
            current_mode = (vesa_mode_t)i;
            break;
        }
    }
    
    return true;
}

bool vesa_set_mode(vesa_mode_t mode) {
    if (mode >= sizeof(vesa_modes) / sizeof(vesa_mode_entry_t)) {
        return false;
    }
    
    current_mode = mode;
    screen_width = vesa_modes[mode].width;
    screen_height = vesa_modes[mode].height;
    screen_bpp = vesa_modes[mode].bpp;
    screen_pitch = screen_width * (screen_bpp / 8);
    
    return true;
}

uint32_t vesa_get_framebuffer(void) {
    return framebuffer_addr;
}

uint16_t vesa_get_width(void) {
    return screen_width;
}

uint16_t vesa_get_height(void) {
    return screen_height;
}

uint16_t vesa_get_pitch(void) {
    return screen_pitch;
}

uint8_t vesa_get_bpp(void) {
    return screen_bpp;
}

const char* vesa_get_mode_name(vesa_mode_t mode) {
    if (mode >= sizeof(vesa_modes) / sizeof(vesa_mode_entry_t)) {
        return "Unknown";
    }
    return vesa_modes[mode].name;
}

vesa_mode_t vesa_get_current_mode(void) {
    return current_mode;
}

int vesa_get_mode_count(void) {
    return sizeof(vesa_modes) / sizeof(vesa_mode_entry_t);
}

const vesa_mode_entry_t* vesa_get_mode_info(int index) {
    if (index < 0 || index >= (int)(sizeof(vesa_modes) / sizeof(vesa_mode_entry_t))) {
        return NULL;
    }
    return &vesa_modes[index];
}


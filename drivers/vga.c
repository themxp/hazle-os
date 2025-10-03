#include "vga.h"
#include "io.h"

static uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;
static uint8_t current_color = 0x0F;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t vga_width = 80;
static uint8_t vga_height = 25;
static vga_mode_t current_mode = VGA_MODE_80x25;

static void update_cursor(void) {
    uint16_t position = cursor_y * vga_width + cursor_x;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

bool vga_set_mode(vga_mode_t mode) {
    switch (mode) {
        case VGA_MODE_80x25:
            outb(0x3D4, 0x09);
            outb(0x3D5, 0x0F);
            vga_width = 80;
            vga_height = 25;
            current_mode = mode;
            break;
            
        case VGA_MODE_80x50:
            outb(0x3D4, 0x09);
            outb(0x3D5, 0x07);
            vga_width = 80;
            vga_height = 50;
            current_mode = mode;
            break;
            
        case VGA_MODE_40x25:
            outb(0x3D4, 0x09);
            outb(0x3D5, 0x0F);
            vga_width = 40;
            vga_height = 25;
            current_mode = mode;
            break;
            
        default:
            return false;
    }
    
    cursor_x = 0;
    cursor_y = 0;
    vga_clear();
    return true;
}

void vga_get_resolution(uint8_t* width, uint8_t* height) {
    if (width) *width = vga_width;
    if (height) *height = vga_height;
}

const char* vga_get_mode_name(vga_mode_t mode) {
    switch (mode) {
        case VGA_MODE_80x25: return "80x25";
        case VGA_MODE_80x50: return "80x50";
        case VGA_MODE_40x25: return "40x25";
        default: return "Unknown";
    }
}

void vga_init(void) {
    current_color = (VGA_COLOR_BLACK << 4) | VGA_COLOR_LIGHT_GREY;
    cursor_x = 0;
    cursor_y = 0;
    vga_width = 80;
    vga_height = 25;
    current_mode = VGA_MODE_80x25;
}

void vga_clear(void) {
    uint16_t blank = ' ' | (current_color << 8);
    
    for (int i = 0; i < vga_width * vga_height; i++) {
        vga_buffer[i] = blank;
    }
    
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void vga_scroll(void) {
    uint16_t blank = ' ' | (current_color << 8);
    
    for (int i = 0; i < (vga_height - 1) * vga_width; i++) {
        vga_buffer[i] = vga_buffer[i + vga_width];
    }
    
    for (int i = (vga_height - 1) * vga_width; i < vga_height * vga_width; i++) {
        vga_buffer[i] = blank;
    }
}

void vga_putchar(char c) {
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
            uint16_t index = cursor_y * vga_width + cursor_x;
            vga_buffer[index] = ' ' | (current_color << 8);
        }
    } else {
        uint16_t index = cursor_y * vga_width + cursor_x;
        vga_buffer[index] = c | (current_color << 8);
        cursor_x++;
    }
    
    if (cursor_x >= vga_width) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= vga_height) {
        vga_scroll();
        cursor_y = vga_height - 1;
    }
    
    update_cursor();
}

void vga_write(const char* str) {
    while (*str) {
        vga_putchar(*str);
        str++;
    }
}

void vga_writeln(const char* str) {
    vga_write(str);
    vga_putchar('\n');
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    current_color = (bg << 4) | fg;
}


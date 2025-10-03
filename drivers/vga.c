#include "vga.h"
#include "io.h"

static uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;
static uint8_t current_color = 0x0F;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void update_cursor(void) {
    uint16_t position = cursor_y * VGA_WIDTH + cursor_x;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

void vga_init(void) {
    current_color = (VGA_COLOR_BLACK << 4) | VGA_COLOR_LIGHT_GREY;
    cursor_x = 0;
    cursor_y = 0;
}

void vga_clear(void) {
    uint16_t blank = ' ' | (current_color << 8);
    
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void vga_scroll(void) {
    uint16_t blank = ' ' | (current_color << 8);
    
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }
    
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
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
            uint16_t index = cursor_y * VGA_WIDTH + cursor_x;
            vga_buffer[index] = ' ' | (current_color << 8);
        }
    } else {
        uint16_t index = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[index] = c | (current_color << 8);
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        vga_scroll();
        cursor_y = VGA_HEIGHT - 1;
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


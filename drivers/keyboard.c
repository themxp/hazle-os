#include "keyboard.h"
#include "io.h"

static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

#define KEYBOARD_BUFFER_SIZE 128

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_start = 0;
static int buffer_end = 0;
static bool shift_pressed = false;

void keyboard_init(void) {
    buffer_start = 0;
    buffer_end = 0;
    shift_pressed = false;
}

void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return;
    }
    
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = false;
        return;
    }
    
    if (scancode & 0x80) {
        return;
    }
    
    char ascii = 0;
    if (scancode < sizeof(scancode_to_ascii)) {
        ascii = shift_pressed ? scancode_to_ascii_shift[scancode] : scancode_to_ascii[scancode];
    }
    
    if (ascii != 0) {
        int next = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
        if (next != buffer_start) {
            keyboard_buffer[buffer_end] = ascii;
            buffer_end = next;
        }
    }
}

bool keyboard_has_input(void) {
    return buffer_start != buffer_end;
}

char keyboard_getchar(void) {
    while (!keyboard_has_input()) {
        __asm__ volatile("hlt");
    }
    
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}


#include "kernel.h"
#include "display.h"
#include "keyboard.h"
#include "timer.h"
#include "rtc.h"
#include "speaker.h"
#include "shell.h"
#include "string.h"
#include "multiboot.h"
#include "vesa.h"
#include "framebuffer.h"
#include "heap.h"
#include "syscall.h"
#include "fat32.h"
#include "block.h"

system_info_t system_info;

void kernel_panic(const char* message) {
    disable_interrupts();
    display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_RED);
    display_clear();
    display_writeln("KERNEL PANIC!");
    display_writeln(message);
    display_writeln("System halted.");
    
    while(1) {
        __asm__ volatile("hlt");
    }
}

void kernel_main(uint32_t magic, uint32_t addr) {
    multiboot_info_t* mbi = (multiboot_info_t*)addr;
    
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        while(1) { __asm__ volatile("hlt"); }
    }
    
    uint32_t fb_addr = 0xE0000000;
    uint16_t fb_width = 1024;
    uint16_t fb_height = 768;
    uint16_t fb_pitch = 1024 * 4;
    uint8_t fb_bpp = 32;
    
    if (mbi->flags & (1 << MULTIBOOT_FLAG_FRAMEBUFFER)) {
        fb_addr = (uint32_t)mbi->framebuffer_addr;
        fb_width = mbi->framebuffer_width;
        fb_height = mbi->framebuffer_height;
        fb_pitch = mbi->framebuffer_pitch;
        fb_bpp = mbi->framebuffer_bpp;
    }
    
    fb_init(fb_addr, fb_width, fb_height, fb_pitch, fb_bpp);
    vesa_init_with_info(fb_addr, fb_width, fb_height, fb_pitch, fb_bpp);
    display_init();
    display_clear();
    
    gdt_init();
    idt_init();
    pic_init();
    timer_init();
    keyboard_init();
    rtc_init();
    speaker_init();
    syscall_init();
    heap_init();
    fat32_init();
    fat32_mount();
    
    system_info.total_memory = 128 * 1024 * 1024;
    system_info.used_memory = 2 * 1024 * 1024;
    system_info.uptime_seconds = 0;
    
    enable_interrupts();
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("================================================");
    display_write("  ");
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_write(KERNEL_NAME);
    display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display_write(" v");
    display_writeln(KERNEL_VERSION);
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("================================================");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_writeln("");
    display_writeln("Welcome to Hazle OS!");
    display_writeln("");
    display_set_color(DISPLAY_COLOR_DARK_GREY, DISPLAY_COLOR_BLACK);
    display_writeln("Repository: https://github.com/themxp/hazle-os");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_writeln("");
    display_writeln("Type 'help' to see available commands.");
    display_writeln("");
    
    shell_init();
    shell_run();
    
    while(1) {
        __asm__ volatile("hlt");
    }
}


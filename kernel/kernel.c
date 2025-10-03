#include "kernel.h"
#include "vga.h"
#include "keyboard.h"
#include "timer.h"
#include "shell.h"
#include "string.h"

system_info_t system_info;

void kernel_panic(const char* message) {
    disable_interrupts();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_clear();
    vga_writeln("KERNEL PANIC!");
    vga_writeln(message);
    vga_writeln("System halted.");
    
    while(1) {
        __asm__ volatile("hlt");
    }
}

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;
    (void)addr;
    
    vga_init();
    vga_clear();
    
    gdt_init();
    idt_init();
    pic_init();
    timer_init();
    keyboard_init();
    
    system_info.total_memory = 128 * 1024 * 1024;
    system_info.used_memory = 2 * 1024 * 1024;
    system_info.uptime_seconds = 0;
    
    enable_interrupts();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeln("========================================");
    vga_write("  ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write(KERNEL_NAME);
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write(" v");
    vga_writeln(KERNEL_VERSION);
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeln("========================================");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeln("");
    vga_writeln("Welcome to Hazle OS!");
    vga_writeln("Type 'help' to see available commands.");
    vga_writeln("");
    
    shell_init();
    shell_run();
    
    while(1) {
        __asm__ volatile("hlt");
    }
}


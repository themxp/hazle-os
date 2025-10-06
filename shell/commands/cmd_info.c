#include "commands.h"
#include "display.h"
#include "kernel.h"
#include "vesa.h"
#include "string.h"

void cmd_info(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("System Information:");
    display_writeln("===================");
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("OS Name      : ");
    display_writeln(KERNEL_NAME);
    
    display_write("Version      : ");
    display_writeln(KERNEL_VERSION);
    
    display_write("Architecture : x86 (32-bit)");
    display_putchar('\n');
    
    display_write("Repository   : ");
    display_set_color(DISPLAY_COLOR_LIGHT_BLUE, DISPLAY_COLOR_BLACK);
    display_writeln("https://github.com/mateuphinxx/hazle-os");
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("Graphics Mode: ");
    display_writeln(vesa_get_mode_name(vesa_get_current_mode()));
    
    char buffer[32];
    display_write("Resolution   : ");
    itoa(vesa_get_width(), buffer, 10);
    display_write(buffer);
    display_write("x");
    itoa(vesa_get_height(), buffer, 10);
    display_writeln(buffer);
    
    display_write("Total RAM    : ");
    itoa(system_info.total_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Used RAM     : ");
    itoa(system_info.used_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
}

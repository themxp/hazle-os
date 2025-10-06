#include "commands.h"
#include "display.h"
#include "kernel.h"
#include "string.h"

void cmd_mem(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("Memory Information:");
    display_writeln("===================");
    
    char buffer[32];
    uint32_t free_mem = system_info.total_memory - system_info.used_memory;
    uint32_t usage_percent = (system_info.used_memory * 100) / system_info.total_memory;
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("Total Memory : ");
    itoa(system_info.total_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Used Memory  : ");
    itoa(system_info.used_memory / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Free Memory  : ");
    itoa(free_mem / (1024 * 1024), buffer, 10);
    display_write(buffer);
    display_writeln(" MB");
    
    display_write("Usage        : ");
    itoa(usage_percent, buffer, 10);
    display_write(buffer);
    display_writeln("%");
}

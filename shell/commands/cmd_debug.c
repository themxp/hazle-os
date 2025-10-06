#include "commands.h"
#include "display.h"
#include "timer.h"
#include "kernel.h"
#include "io.h"
#include "string.h"

void cmd_debug(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    char buffer[32];
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("Debug Information:");
    display_writeln("==================");
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    
    uint32_t ticks = timer_get_ticks();
    display_write("Timer ticks: ");
    itoa(ticks, buffer, 10);
    display_writeln(buffer);
    
    display_write("Timer frequency: ");
    itoa(TIMER_FREQUENCY, buffer, 10);
    display_write(buffer);
    display_writeln(" Hz");
    
    display_write("Uptime: ");
    itoa(system_info.uptime_seconds, buffer, 10);
    display_write(buffer);
    display_writeln(" seconds");
    
    display_write("PIC Master mask: 0x");
    uint8_t pic_mask = inb(0x21);
    itoa(pic_mask, buffer, 16);
    display_writeln(buffer);
    
    display_write("IRQ 0 (Timer): ");
    if (pic_mask & 0x01) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("DISABLED");
    } else {
        display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
        display_writeln("ENABLED");
    }
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("IRQ 1 (Keyboard): ");
    if (pic_mask & 0x02) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("DISABLED");
    } else {
        display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
        display_writeln("ENABLED");
    }
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("Memory entropy: 0x");
    uint32_t addr_entropy = (uint32_t)&buffer;
    itoa(addr_entropy, buffer, 16);
    display_writeln(buffer);
}

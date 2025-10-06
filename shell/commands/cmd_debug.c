#include "commands.h"
#include "display.h"
#include "timer.h"
#include "string.h"

void cmd_debug(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    char buffer[32];
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("Debug Information:");
    display_writeln("==================");
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_write("Timer ticks: ");
    uint32_t ticks = timer_get_ticks();
    itoa(ticks, buffer, 10);
    display_writeln(buffer);
    
    display_write("Memory entropy: ");
    uint32_t addr_entropy = (uint32_t)&buffer;
    itoa(addr_entropy, buffer, 16);
    display_write("0x");
    display_writeln(buffer);
    
    display_write("Timer frequency: ");
    itoa(TIMER_FREQUENCY, buffer, 10);
    display_write(buffer);
    display_writeln(" Hz");
}

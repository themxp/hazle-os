#include "commands.h"
#include "display.h"
#include "timer.h"
#include "string.h"

void cmd_time(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    uint32_t ticks = timer_get_ticks();
    char buffer[32];
    
    display_write("System ticks: ");
    itoa(ticks, buffer, 10);
    display_writeln(buffer);
}

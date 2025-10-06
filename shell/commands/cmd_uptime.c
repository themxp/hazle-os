#include "commands.h"
#include "display.h"
#include "kernel.h"
#include "string.h"

void cmd_uptime(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    uint32_t seconds = system_info.uptime_seconds;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    seconds %= 60;
    minutes %= 60;
    
    char buffer[32];
    
    display_write("System uptime: ");
    itoa(hours, buffer, 10);
    display_write(buffer);
    display_write("h ");
    itoa(minutes, buffer, 10);
    display_write(buffer);
    display_write("m ");
    itoa(seconds, buffer, 10);
    display_write(buffer);
    display_writeln("s");
}

#include "commands.h"
#include "display.h"
#include "timer.h"
#include "string.h"

void cmd_random(int argc, char** argv) {
    uint32_t max = 100;
    
    if (argc >= 2) {
        max = atoi(argv[1]);
        if (max < 1) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Max must be greater than 0");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    uint32_t random = timer_get_ticks() % max;
    
    char buffer[32];
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_write("Random number: ");
    itoa(random, buffer, 10);
    display_writeln(buffer);
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

#include "commands.h"
#include "display.h"
#include "rtc.h"
#include "string.h"

void cmd_date(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    rtc_time_t time;
    rtc_read_time(&time);
    
    char buffer[32];
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_write("Current Date & Time:");
    display_putchar('\n');
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    
    itoa(time.year, buffer, 10);
    display_write(buffer);
    display_write("-");
    
    if (time.month < 10) display_write("0");
    itoa(time.month, buffer, 10);
    display_write(buffer);
    display_write("-");
    
    if (time.day < 10) display_write("0");
    itoa(time.day, buffer, 10);
    display_write(buffer);
    display_write("  ");
    
    if (time.hour < 10) display_write("0");
    itoa(time.hour, buffer, 10);
    display_write(buffer);
    display_write(":");
    
    if (time.minute < 10) display_write("0");
    itoa(time.minute, buffer, 10);
    display_write(buffer);
    display_write(":");
    
    if (time.second < 10) display_write("0");
    itoa(time.second, buffer, 10);
    display_writeln(buffer);
}

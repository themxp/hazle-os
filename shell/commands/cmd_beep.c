#include "commands.h"
#include "display.h"
#include "speaker.h"
#include "string.h"

void cmd_beep(int argc, char** argv) {
    uint32_t frequency = 440;
    uint32_t duration = 200;
    
    if (argc >= 2) {
        frequency = atoi(argv[1]);
        if (frequency < 20 || frequency > 20000) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Frequency must be between 20 and 20000 Hz");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    if (argc >= 3) {
        duration = atoi(argv[2]);
        if (duration < 1 || duration > 5000) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: Duration must be between 1 and 5000 ms");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    speaker_beep(frequency, duration);
    
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_writeln("Beep!");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

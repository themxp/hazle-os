#include "commands.h"
#include "display.h"
#include "vesa.h"
#include "string.h"

void cmd_resolution(int argc, char** argv) {
    if (argc == 1) {
        display_write("Current resolution: ");
        display_writeln(vesa_get_mode_name(vesa_get_current_mode()));
        
        display_writeln("");
        display_writeln("Available modes:");
        
        int count = vesa_get_mode_count();
        for (int i = 0; i < count; i++) {
            const vesa_mode_entry_t* mode = vesa_get_mode_info(i);
            if (mode) {
                char buffer[32];
                display_write("  ");
                itoa(i, buffer, 10);
                display_write(buffer);
                display_write(" - ");
                display_writeln(mode->name);
            }
        }
        
        display_writeln("");
        display_writeln("Usage: resolution <mode>");
        display_writeln("");
        display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
        display_writeln("Note: Resolution is set at compile time.");
        display_writeln("To change resolution:");
        display_writeln("  1. Edit boot/boot.asm (line 16-18)");
        display_writeln("  2. Edit Makefile (line 45)");
        display_writeln("  3. Run: make clean && make run");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
    display_writeln("Resolution is set at compile time.");
    display_writeln("Use 'resolution' without arguments for details.");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

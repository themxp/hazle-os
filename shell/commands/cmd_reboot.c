#include "commands.h"
#include "display.h"
#include "io.h"

void cmd_reboot(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
    display_writeln("Rebooting system...");
    
    uint8_t temp = 0x02;
    while (temp & 0x02) {
        temp = inb(0x64);
    }
    outb(0x64, 0xFE);
    
    __asm__ volatile("hlt");
}

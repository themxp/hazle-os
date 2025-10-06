#include "commands.h"
#include "display.h"
#include "timer.h"
#include "string.h"

static uint32_t random_seed = 1;

// Simple Linear Congruential Generator (LCG)
static uint32_t lcg_rand(void) {
    random_seed = (random_seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return random_seed;
}

static void init_random_seed(void) {
    uint32_t ticks = timer_get_ticks();
    
    if (ticks < 10) {
        uint32_t addr_entropy = (uint32_t)&random_seed;
        ticks = addr_entropy ^ 0xDEADBEEF;
    }
    
    random_seed = ticks ^ (ticks << 16) ^ (ticks >> 16) ^ 0x12345678;
    
    if (random_seed == 0) {
        random_seed = 0xABCDEF01;
    }
}

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
    
    static uint32_t call_count = 0;
    if (call_count == 0 || (call_count % 10 == 0)) {
        init_random_seed();
    }
    call_count++;
    
    // Generate random number using LCG
    uint32_t random_val = lcg_rand();
    uint32_t result = random_val % max;
    
    char buffer[32];
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_write("Random number: ");
    itoa(result, buffer, 10);
    display_writeln(buffer);
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

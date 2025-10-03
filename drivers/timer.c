#include "timer.h"
#include "io.h"
#include "kernel.h"

static volatile uint32_t timer_ticks = 0;

void timer_init(void) {
    uint32_t divisor = 1193180 / TIMER_FREQUENCY;
    
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
    
    timer_ticks = 0;
}

void timer_handler(void) {
    timer_ticks++;
    
    if (timer_ticks % TIMER_FREQUENCY == 0) {
        system_info.uptime_seconds++;
    }
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}


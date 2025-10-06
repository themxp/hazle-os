#include "speaker.h"
#include "io.h"
#include "timer.h"

#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43
#define SPEAKER_PORT 0x61

void speaker_init(void) {
    speaker_stop();
}

void speaker_play_tone(uint32_t frequency) {
    if (frequency < 20) {
        speaker_stop();
        return;
    }
    
    uint32_t divisor = 1193180 / frequency;
    
    outb(PIT_COMMAND, 0xB6);
    outb(PIT_CHANNEL_2, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_2, (uint8_t)((divisor >> 8) & 0xFF));
    
    uint8_t tmp = inb(SPEAKER_PORT);
    if (tmp != (tmp | 3)) {
        outb(SPEAKER_PORT, tmp | 3);
    }
}

void speaker_stop(void) {
    uint8_t tmp = inb(SPEAKER_PORT) & 0xFC;
    outb(SPEAKER_PORT, tmp);
}

void speaker_beep(uint32_t frequency, uint32_t duration_ms) {
    speaker_play_tone(frequency);
    
    uint32_t delay_count = duration_ms * 1000;
    for (uint32_t i = 0; i < delay_count; i++) {
        __asm__ volatile("nop");
    }
    
    speaker_stop();
}


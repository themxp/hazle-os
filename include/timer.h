#ifndef TIMER_H
#define TIMER_H

#include "types.h"

#define TIMER_FREQUENCY 100

void timer_init(void);
void timer_handler(void);
uint32_t timer_get_ticks(void);

extern void timer_interrupt_handler(void);

#endif


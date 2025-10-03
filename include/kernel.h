#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

#define KERNEL_VERSION "1.0.0"
#define KERNEL_NAME "Hazle OS"

void kernel_main(uint32_t magic, uint32_t addr);
void kernel_panic(const char* message);

void gdt_init(void);
void idt_init(void);
void pic_init(void);

extern void gdt_flush(void);
extern void idt_flush(void);
extern void enable_interrupts(void);
extern void disable_interrupts(void);

typedef struct {
    uint32_t total_memory;
    uint32_t used_memory;
    uint32_t uptime_seconds;
} system_info_t;

extern system_info_t system_info;

#endif


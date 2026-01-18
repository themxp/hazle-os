#ifndef HEAP_H
#define HEAP_H

#include "types.h"

#define HEAP_START 0x00400000
#define HEAP_SIZE  0x00100000

void heap_init(void);
void *heap_alloc(uint32_t size);
void heap_free(void *ptr);
void heap_reset(void);
uint32_t heap_get_used(void);

#endif

#include "heap.h"
#include "string.h"

static uint8_t *heap_start = (uint8_t *)HEAP_START;
static uint32_t heap_offset = 0;

void heap_init(void) {
    heap_offset = 0;
}

void *heap_alloc(uint32_t size) {
    if (heap_offset + size > HEAP_SIZE) {
        return NULL;
    }
    
    size = (size + 15) & ~15;
    
    void *ptr = heap_start + heap_offset;
    heap_offset += size;
    
    return ptr;
}

void heap_free(void *ptr) {
    (void)ptr;
}

void heap_reset(void) {
    heap_offset = 0;
}

uint32_t heap_get_used(void) {
    return heap_offset;
}

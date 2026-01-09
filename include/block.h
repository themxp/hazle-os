#ifndef BLOCK_H
#define BLOCK_H

#include "types.h"

#define BLOCK_SECTOR_SIZE 512

bool block_init(void);
bool block_read(uint32_t lba, uint8_t count, void* buffer);
bool block_write(uint32_t lba, uint8_t count, const void* buffer);
bool block_flush(void);
uint32_t block_get_sector_count(void);
bool block_is_available(void);

#endif

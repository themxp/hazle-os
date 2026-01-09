#include "block.h"
#include "ata.h"

static bool initialized = false;

bool block_init(void) {
    ata_init();
    initialized = ata_get_primary()->present;
    return initialized;
}

bool block_read(uint32_t lba, uint8_t count, void* buffer) {
    if (!initialized) return false;
    return ata_read_sectors(ata_get_primary(), lba, count, buffer);
}

bool block_write(uint32_t lba, uint8_t count, const void* buffer) {
    if (!initialized) return false;
    return ata_write_sectors(ata_get_primary(), lba, count, buffer);
}

bool block_flush(void) {
    if (!initialized) return false;
    return ata_flush(ata_get_primary());
}

uint32_t block_get_sector_count(void) {
    if (!initialized) return 0;
    return ata_get_primary()->sector_count;
}

bool block_is_available(void) {
    return initialized;
}

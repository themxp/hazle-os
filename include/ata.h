#ifndef ATA_H
#define ATA_H

#include "types.h"

#define ATA_PRIMARY_IO       0x1F0
#define ATA_PRIMARY_CTRL     0x3F6
#define ATA_SECONDARY_IO     0x170
#define ATA_SECONDARY_CTRL   0x376

#define ATA_REG_DATA         0x00
#define ATA_REG_ERROR        0x01
#define ATA_REG_FEATURES     0x01
#define ATA_REG_SECCOUNT     0x02
#define ATA_REG_LBA_LO       0x03
#define ATA_REG_LBA_MID      0x04
#define ATA_REG_LBA_HI       0x05
#define ATA_REG_DRIVE        0x06
#define ATA_REG_STATUS       0x07
#define ATA_REG_COMMAND      0x07
#define ATA_REG_ALTSTATUS    0x00

#define ATA_CMD_READ_PIO     0x20
#define ATA_CMD_WRITE_PIO    0x30
#define ATA_CMD_CACHE_FLUSH  0xE7
#define ATA_CMD_IDENTIFY     0xEC

#define ATA_STATUS_ERR       0x01
#define ATA_STATUS_DRQ       0x08
#define ATA_STATUS_SRV       0x10
#define ATA_STATUS_DF        0x20
#define ATA_STATUS_RDY       0x40
#define ATA_STATUS_BSY       0x80

#define ATA_DRIVE_MASTER     0xE0
#define ATA_DRIVE_SLAVE      0xF0

#define ATA_SECTOR_SIZE      512

typedef struct {
    bool present;
    bool is_master;
    uint16_t io_base;
    uint16_t ctrl_base;
    uint32_t sector_count;
    char model[41];
    char serial[21];
} ata_drive_t;

void ata_init(void);
bool ata_identify(uint16_t io_base, bool master, ata_drive_t* drive);
bool ata_read_sectors(ata_drive_t* drive, uint32_t lba, uint8_t count, void* buffer);
bool ata_write_sectors(ata_drive_t* drive, uint32_t lba, uint8_t count, const void* buffer);
bool ata_flush(ata_drive_t* drive);
ata_drive_t* ata_get_primary(void);

#endif

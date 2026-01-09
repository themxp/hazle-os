#include "ata.h"
#include "io.h"
#include "string.h"

static ata_drive_t primary_drive;

static void ata_400ns_delay(uint16_t io_base) {
    inb(io_base + ATA_REG_STATUS);
    inb(io_base + ATA_REG_STATUS);
    inb(io_base + ATA_REG_STATUS);
    inb(io_base + ATA_REG_STATUS);
}

static bool ata_poll(uint16_t io_base, bool check_error) {
    ata_400ns_delay(io_base);
    
    uint8_t status;
    int timeout = 100000;
    
    while (timeout--) {
        status = inb(io_base + ATA_REG_STATUS);
        
        if (!(status & ATA_STATUS_BSY)) {
            if (check_error) {
                if (status & ATA_STATUS_ERR) return false;
                if (status & ATA_STATUS_DF) return false;
            }
            if (status & ATA_STATUS_DRQ) return true;
            if (!(status & ATA_STATUS_BSY) && !(status & ATA_STATUS_DRQ)) {
                if (timeout < 90000) return false;
            }
        }
    }
    return false;
}

static void ata_select_drive(uint16_t io_base, bool master) {
    outb(io_base + ATA_REG_DRIVE, master ? 0xA0 : 0xB0);
    ata_400ns_delay(io_base);
}

bool ata_identify(uint16_t io_base, bool master, ata_drive_t* drive) {
    uint16_t ctrl_base = (io_base == ATA_PRIMARY_IO) ? ATA_PRIMARY_CTRL : ATA_SECONDARY_CTRL;
    
    drive->present = false;
    drive->io_base = io_base;
    drive->ctrl_base = ctrl_base;
    drive->is_master = master;
    
    ata_select_drive(io_base, master);
    
    outb(io_base + ATA_REG_SECCOUNT, 0);
    outb(io_base + ATA_REG_LBA_LO, 0);
    outb(io_base + ATA_REG_LBA_MID, 0);
    outb(io_base + ATA_REG_LBA_HI, 0);
    
    outb(io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    ata_400ns_delay(io_base);
    
    uint8_t status = inb(io_base + ATA_REG_STATUS);
    if (status == 0) {
        return false;
    }
    
    int timeout = 100000;
    while (timeout--) {
        status = inb(io_base + ATA_REG_STATUS);
        if (!(status & ATA_STATUS_BSY)) break;
    }
    if (timeout <= 0) return false;
    
    uint8_t lba_mid = inb(io_base + ATA_REG_LBA_MID);
    uint8_t lba_hi = inb(io_base + ATA_REG_LBA_HI);
    
    if (lba_mid != 0 || lba_hi != 0) {
        return false;
    }
    
    timeout = 100000;
    while (timeout--) {
        status = inb(io_base + ATA_REG_STATUS);
        if (status & ATA_STATUS_ERR) return false;
        if (status & ATA_STATUS_DRQ) break;
    }
    if (timeout <= 0) return false;
    
    uint16_t identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(io_base + ATA_REG_DATA);
    }
    
    drive->present = true;
    
    if (identify_data[83] & (1 << 10)) {
        drive->sector_count = identify_data[100] | ((uint32_t)identify_data[101] << 16);
    } else {
        drive->sector_count = identify_data[60] | ((uint32_t)identify_data[61] << 16);
    }
    
    for (int i = 0; i < 20; i++) {
        drive->model[i * 2] = (identify_data[27 + i] >> 8) & 0xFF;
        drive->model[i * 2 + 1] = identify_data[27 + i] & 0xFF;
    }
    drive->model[40] = '\0';
    
    int end = 39;
    while (end >= 0 && drive->model[end] == ' ') {
        drive->model[end--] = '\0';
    }
    
    for (int i = 0; i < 10; i++) {
        drive->serial[i * 2] = (identify_data[10 + i] >> 8) & 0xFF;
        drive->serial[i * 2 + 1] = identify_data[10 + i] & 0xFF;
    }
    drive->serial[20] = '\0';
    
    return true;
}

bool ata_read_sectors(ata_drive_t* drive, uint32_t lba, uint8_t count, void* buffer) {
    if (!drive->present) return false;
    if (count == 0) return false;
    
    ata_select_drive(drive->io_base, drive->is_master);
    
    outb(drive->io_base + ATA_REG_DRIVE, 
         (drive->is_master ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F));
    
    ata_400ns_delay(drive->io_base);
    
    outb(drive->io_base + ATA_REG_SECCOUNT, count);
    outb(drive->io_base + ATA_REG_LBA_LO, lba & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_HI, (lba >> 16) & 0xFF);
    outb(drive->io_base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    uint16_t* buf = (uint16_t*)buffer;
    
    for (int sector = 0; sector < count; sector++) {
        if (!ata_poll(drive->io_base, true)) {
            return false;
        }
        
        for (int i = 0; i < 256; i++) {
            buf[sector * 256 + i] = inw(drive->io_base + ATA_REG_DATA);
        }
    }
    
    return true;
}

bool ata_write_sectors(ata_drive_t* drive, uint32_t lba, uint8_t count, const void* buffer) {
    if (!drive->present) return false;
    if (count == 0) return false;
    
    ata_select_drive(drive->io_base, drive->is_master);
    
    outb(drive->io_base + ATA_REG_DRIVE, 
         (drive->is_master ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F));
    
    ata_400ns_delay(drive->io_base);
    
    outb(drive->io_base + ATA_REG_SECCOUNT, count);
    outb(drive->io_base + ATA_REG_LBA_LO, lba & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_HI, (lba >> 16) & 0xFF);
    outb(drive->io_base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    
    const uint16_t* buf = (const uint16_t*)buffer;
    
    for (int sector = 0; sector < count; sector++) {
        if (!ata_poll(drive->io_base, true)) {
            return false;
        }
        
        for (int i = 0; i < 256; i++) {
            outw(drive->io_base + ATA_REG_DATA, buf[sector * 256 + i]);
        }
    }
    
    return true;
}

bool ata_flush(ata_drive_t* drive) {
    if (!drive->present) return false;
    
    outb(drive->io_base + ATA_REG_DRIVE, drive->is_master ? 0xE0 : 0xF0);
    ata_400ns_delay(drive->io_base);
    outb(drive->io_base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    
    return ata_poll(drive->io_base, true);
}

void ata_init(void) {
    memset(&primary_drive, 0, sizeof(ata_drive_t));
    
    if (!ata_identify(ATA_PRIMARY_IO, true, &primary_drive)) {
        if (!ata_identify(ATA_PRIMARY_IO, false, &primary_drive)) {
            if (!ata_identify(ATA_SECONDARY_IO, true, &primary_drive)) {
                ata_identify(ATA_SECONDARY_IO, false, &primary_drive);
            }
        }
    }
}

ata_drive_t* ata_get_primary(void) {
    return &primary_drive;
}

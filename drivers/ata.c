#include "ata.h"
#include "io.h"
#include "string.h"

static ata_drive_t primary_drive;

static void ata_wait_busy(uint16_t io_base) {
    while (inb(io_base + ATA_REG_STATUS) & ATA_STATUS_BSY);
}

static void ata_wait_ready(uint16_t io_base) {
    while (!(inb(io_base + ATA_REG_STATUS) & ATA_STATUS_RDY));
}

static bool ata_wait_drq(uint16_t io_base) {
    uint8_t status;
    for (int i = 0; i < 100000; i++) {
        status = inb(io_base + ATA_REG_STATUS);
        if (status & ATA_STATUS_ERR) return false;
        if (status & ATA_STATUS_DF) return false;
        if (status & ATA_STATUS_DRQ) return true;
    }
    return false;
}

static void ata_delay(uint16_t ctrl_base) {
    for (int i = 0; i < 4; i++) {
        inb(ctrl_base + ATA_REG_ALTSTATUS);
    }
}

static void ata_select_drive(uint16_t io_base, uint16_t ctrl_base, bool master) {
    outb(io_base + ATA_REG_DRIVE, master ? ATA_DRIVE_MASTER : ATA_DRIVE_SLAVE);
    ata_delay(ctrl_base);
}

bool ata_identify(uint16_t io_base, bool master, ata_drive_t* drive) {
    uint16_t ctrl_base = (io_base == ATA_PRIMARY_IO) ? ATA_PRIMARY_CTRL : ATA_SECONDARY_CTRL;
    
    ata_select_drive(io_base, ctrl_base, master);
    
    outb(io_base + ATA_REG_SECCOUNT, 0);
    outb(io_base + ATA_REG_LBA_LO, 0);
    outb(io_base + ATA_REG_LBA_MID, 0);
    outb(io_base + ATA_REG_LBA_HI, 0);
    outb(io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    ata_delay(ctrl_base);
    
    uint8_t status = inb(io_base + ATA_REG_STATUS);
    if (status == 0) {
        drive->present = false;
        return false;
    }
    
    ata_wait_busy(io_base);
    
    if (inb(io_base + ATA_REG_LBA_MID) != 0 || inb(io_base + ATA_REG_LBA_HI) != 0) {
        drive->present = false;
        return false;
    }
    
    if (!ata_wait_drq(io_base)) {
        drive->present = false;
        return false;
    }
    
    uint16_t identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inb(io_base + ATA_REG_DATA) | (inb(io_base + ATA_REG_DATA) << 8);
    }
    
    drive->present = true;
    drive->is_master = master;
    drive->io_base = io_base;
    drive->ctrl_base = ctrl_base;
    
    if (identify_data[83] & (1 << 10)) {
        drive->sector_count = identify_data[100] | (identify_data[101] << 16);
    } else {
        drive->sector_count = identify_data[60] | (identify_data[61] << 16);
    }
    
    for (int i = 0; i < 20; i++) {
        drive->model[i * 2] = (identify_data[27 + i] >> 8) & 0xFF;
        drive->model[i * 2 + 1] = identify_data[27 + i] & 0xFF;
    }
    drive->model[40] = '\0';
    
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
    
    ata_wait_busy(drive->io_base);
    
    outb(drive->io_base + ATA_REG_DRIVE, 
         (drive->is_master ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F));
    
    ata_delay(drive->ctrl_base);
    
    outb(drive->io_base + ATA_REG_SECCOUNT, count);
    outb(drive->io_base + ATA_REG_LBA_LO, lba & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_HI, (lba >> 16) & 0xFF);
    outb(drive->io_base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    uint16_t* buf = (uint16_t*)buffer;
    
    for (int sector = 0; sector < count; sector++) {
        if (!ata_wait_drq(drive->io_base)) {
            return false;
        }
        
        for (int i = 0; i < 256; i++) {
            uint16_t data = inb(drive->io_base + ATA_REG_DATA);
            data |= (uint16_t)inb(drive->io_base + ATA_REG_DATA) << 8;
            buf[sector * 256 + i] = data;
        }
    }
    
    return true;
}

bool ata_write_sectors(ata_drive_t* drive, uint32_t lba, uint8_t count, const void* buffer) {
    if (!drive->present) return false;
    if (count == 0) return false;
    
    ata_wait_busy(drive->io_base);
    
    outb(drive->io_base + ATA_REG_DRIVE, 
         (drive->is_master ? 0xE0 : 0xF0) | ((lba >> 24) & 0x0F));
    
    ata_delay(drive->ctrl_base);
    
    outb(drive->io_base + ATA_REG_SECCOUNT, count);
    outb(drive->io_base + ATA_REG_LBA_LO, lba & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_MID, (lba >> 8) & 0xFF);
    outb(drive->io_base + ATA_REG_LBA_HI, (lba >> 16) & 0xFF);
    outb(drive->io_base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    
    const uint16_t* buf = (const uint16_t*)buffer;
    
    for (int sector = 0; sector < count; sector++) {
        if (!ata_wait_drq(drive->io_base)) {
            return false;
        }
        
        for (int i = 0; i < 256; i++) {
            uint16_t data = buf[sector * 256 + i];
            outb(drive->io_base + ATA_REG_DATA, data & 0xFF);
            outb(drive->io_base + ATA_REG_DATA, (data >> 8) & 0xFF);
        }
    }
    
    return true;
}

bool ata_flush(ata_drive_t* drive) {
    if (!drive->present) return false;
    
    outb(drive->io_base + ATA_REG_DRIVE, drive->is_master ? 0xE0 : 0xF0);
    ata_delay(drive->ctrl_base);
    outb(drive->io_base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    
    ata_wait_busy(drive->io_base);
    
    uint8_t status = inb(drive->io_base + ATA_REG_STATUS);
    return !(status & ATA_STATUS_ERR);
}

void ata_init(void) {
    memset(&primary_drive, 0, sizeof(ata_drive_t));
    
    if (!ata_identify(ATA_PRIMARY_IO, true, &primary_drive)) {
        ata_identify(ATA_PRIMARY_IO, false, &primary_drive);
    }
}

ata_drive_t* ata_get_primary(void) {
    return &primary_drive;
}

#include "fat32.h"
#include "block.h"
#include "string.h"

extern fat32_fs_t* fat32_get_fs(void);

static uint8_t sector_buffer[FAT32_SECTOR_SIZE];

bool fat32_parse_bpb(void) {
    if (!block_read(0, 1, sector_buffer)) {
        return false;
    }
    
    fat32_bpb_t* bpb = (fat32_bpb_t*)sector_buffer;
    
    if (bpb->bytes_per_sector != 512 && 
        bpb->bytes_per_sector != 1024 && 
        bpb->bytes_per_sector != 2048 && 
        bpb->bytes_per_sector != 4096) {
        return false;
    }
    
    if (bpb->sectors_per_fat_16 != 0) {
        return false;
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    
    fs->bytes_per_sector = bpb->bytes_per_sector;
    fs->sectors_per_cluster = bpb->sectors_per_cluster;
    fs->bytes_per_cluster = fs->bytes_per_sector * fs->sectors_per_cluster;
    fs->reserved_sectors = bpb->reserved_sectors;
    fs->num_fats = bpb->num_fats;
    fs->sectors_per_fat = bpb->sectors_per_fat_32;
    fs->root_cluster = bpb->root_cluster;
    
    fs->first_fat_sector = fs->reserved_sectors;
    fs->first_data_sector = fs->reserved_sectors + (fs->num_fats * fs->sectors_per_fat);
    
    if (bpb->total_sectors_32 > 0) {
        fs->total_sectors = bpb->total_sectors_32;
    } else {
        fs->total_sectors = bpb->total_sectors_16;
    }
    
    uint32_t data_sectors = fs->total_sectors - fs->first_data_sector;
    fs->total_clusters = data_sectors / fs->sectors_per_cluster;
    
    fs->current_directory = fs->root_cluster;
    strcpy(fs->current_path, "/");
    
    return true;
}

uint32_t fat32_cluster_to_lba(uint32_t cluster) {
    fat32_fs_t* fs = fat32_get_fs();
    return fs->first_data_sector + ((cluster - 2) * fs->sectors_per_cluster);
}

uint32_t fat32_lba_to_cluster(uint32_t lba) {
    fat32_fs_t* fs = fat32_get_fs();
    return ((lba - fs->first_data_sector) / fs->sectors_per_cluster) + 2;
}

bool fat32_is_eoc(uint32_t entry) {
    return entry >= FAT32_EOC_MIN;
}

bool fat32_read_cluster(uint32_t cluster, uint8_t* buffer) {
    fat32_fs_t* fs = fat32_get_fs();
    uint32_t lba = fat32_cluster_to_lba(cluster);
    for (uint32_t i = 0; i < fs->sectors_per_cluster; i++) {
        if (!block_read(lba + i, 1, buffer + (i * fs->bytes_per_sector))) {
            return false;
        }
    }
    return true;
}

bool fat32_write_cluster(uint32_t cluster, const uint8_t* buffer) {
    fat32_fs_t* fs = fat32_get_fs();
    uint32_t lba = fat32_cluster_to_lba(cluster);
    for (uint32_t i = 0; i < fs->sectors_per_cluster; i++) {
        if (!block_write(lba + i, 1, buffer + (i * fs->bytes_per_sector))) {
            return false;
        }
    }
    return true;
}

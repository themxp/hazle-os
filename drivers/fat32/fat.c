#include "fat32.h"
#include "block.h"
#include "string.h"

extern fat32_fs_t* fat32_get_fs(void);

static uint8_t fat_sector_buffer[FAT32_SECTOR_SIZE];
static uint32_t cached_fat_sector = 0xFFFFFFFF;
static bool fat_cache_dirty = false;

static bool fat_flush_cache(void) {
    fat32_fs_t* fs = fat32_get_fs();
    if (fat_cache_dirty && cached_fat_sector != 0xFFFFFFFF) {
        for (uint32_t i = 0; i < fs->num_fats; i++) {
            uint32_t fat_offset = i * fs->sectors_per_fat;
            if (!block_write(fs->first_fat_sector + fat_offset + cached_fat_sector, 1, fat_sector_buffer)) {
                return false;
            }
        }
        fat_cache_dirty = false;
    }
    return true;
}

static bool fat_load_sector(uint32_t sector) {
    if (cached_fat_sector == sector) {
        return true;
    }
    
    if (!fat_flush_cache()) {
        return false;
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    if (!block_read(fs->first_fat_sector + sector, 1, fat_sector_buffer)) {
        return false;
    }
    
    cached_fat_sector = sector;
    return true;
}

uint32_t fat32_get_fat_entry(uint32_t cluster) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_offset / fs->bytes_per_sector;
    uint32_t entry_offset = fat_offset % fs->bytes_per_sector;
    
    if (!fat_load_sector(fat_sector)) {
        return FAT32_BAD_CLUSTER;
    }
    
    uint32_t* entry = (uint32_t*)(fat_sector_buffer + entry_offset);
    return (*entry) & 0x0FFFFFFF;
}

bool fat32_set_fat_entry(uint32_t cluster, uint32_t value) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_offset / fs->bytes_per_sector;
    uint32_t entry_offset = fat_offset % fs->bytes_per_sector;
    
    if (!fat_load_sector(fat_sector)) {
        return false;
    }
    
    uint32_t* entry = (uint32_t*)(fat_sector_buffer + entry_offset);
    *entry = (*entry & 0xF0000000) | (value & 0x0FFFFFFF);
    fat_cache_dirty = true;
    
    return true;
}

uint32_t fat32_find_free_cluster(void) {
    fat32_fs_t* fs = fat32_get_fs();
    
    for (uint32_t cluster = 2; cluster < fs->total_clusters + 2; cluster++) {
        uint32_t entry = fat32_get_fat_entry(cluster);
        if (entry == FAT32_FREE_CLUSTER) {
            return cluster;
        }
    }
    return 0;
}

uint32_t fat32_allocate_cluster(void) {
    uint32_t cluster = fat32_find_free_cluster();
    if (cluster == 0) {
        return 0;
    }
    
    if (!fat32_set_fat_entry(cluster, FAT32_EOC)) {
        return 0;
    }
    
    return cluster;
}

bool fat32_extend_chain(uint32_t last_cluster, uint32_t new_cluster) {
    return fat32_set_fat_entry(last_cluster, new_cluster);
}

bool fat32_free_chain(uint32_t start_cluster) {
    uint32_t cluster = start_cluster;
    
    while (cluster >= 2 && cluster < FAT32_EOC_MIN) {
        uint32_t next = fat32_get_fat_entry(cluster);
        if (!fat32_set_fat_entry(cluster, FAT32_FREE_CLUSTER)) {
            return false;
        }
        cluster = next;
    }
    
    return fat_flush_cache();
}

uint32_t fat32_count_free_clusters(void) {
    fat32_fs_t* fs = fat32_get_fs();
    uint32_t count = 0;
    
    for (uint32_t cluster = 2; cluster < fs->total_clusters + 2; cluster++) {
        if (fat32_get_fat_entry(cluster) == FAT32_FREE_CLUSTER) {
            count++;
        }
    }
    
    return count;
}

uint32_t fat32_get_chain_length(uint32_t start_cluster) {
    uint32_t count = 0;
    uint32_t cluster = start_cluster;
    
    while (cluster >= 2 && cluster < FAT32_EOC_MIN) {
        count++;
        cluster = fat32_get_fat_entry(cluster);
    }
    
    return count;
}

bool fat32_sync_fat(void) {
    return fat_flush_cache();
}

void fat32_invalidate_fat_cache(void) {
    fat_flush_cache();
    cached_fat_sector = 0xFFFFFFFF;
}

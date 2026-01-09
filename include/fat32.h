#ifndef FAT32_H
#define FAT32_H

#include "types.h"

#define FAT32_SECTOR_SIZE    512
#define FAT32_MAX_PATH       256
#define FAT32_MAX_OPEN_FILES 8
#define FAT32_CACHE_ENTRIES  16

#define FAT32_ATTR_READ_ONLY 0x01
#define FAT32_ATTR_HIDDEN    0x02
#define FAT32_ATTR_SYSTEM    0x04
#define FAT32_ATTR_VOLUME_ID 0x08
#define FAT32_ATTR_DIRECTORY 0x10
#define FAT32_ATTR_ARCHIVE   0x20
#define FAT32_ATTR_LFN       0x0F

#define FAT32_ENTRY_FREE     0xE5
#define FAT32_ENTRY_END      0x00

#define FAT32_EOC_MIN        0x0FFFFFF8
#define FAT32_EOC            0x0FFFFFFF
#define FAT32_FREE_CLUSTER   0x00000000
#define FAT32_BAD_CLUSTER    0x0FFFFFF7

typedef struct {
    uint8_t  jump[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t sectors_per_fat_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t sectors_per_fat_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info_sector;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     fs_type[8];
} __attribute__((packed)) fat32_bpb_t;

typedef struct {
    uint32_t signature1;
    uint8_t  reserved1[480];
    uint32_t signature2;
    uint32_t free_clusters;
    uint32_t next_free_cluster;
    uint8_t  reserved2[12];
    uint32_t signature3;
} __attribute__((packed)) fat32_fsinfo_t;

typedef struct {
    char     name[11];
    uint8_t  attributes;
    uint8_t  reserved;
    uint8_t  creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t first_cluster_high;
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed)) fat32_dir_entry_t;

typedef struct {
    uint8_t  order;
    uint16_t name1[5];
    uint8_t  attributes;
    uint8_t  type;
    uint8_t  checksum;
    uint16_t name2[6];
    uint16_t zero;
    uint16_t name3[2];
} __attribute__((packed)) fat32_lfn_entry_t;

typedef struct {
    bool     mounted;
    uint32_t bytes_per_sector;
    uint32_t sectors_per_cluster;
    uint32_t bytes_per_cluster;
    uint32_t reserved_sectors;
    uint32_t num_fats;
    uint32_t sectors_per_fat;
    uint32_t root_cluster;
    uint32_t first_fat_sector;
    uint32_t first_data_sector;
    uint32_t total_clusters;
    uint32_t total_sectors;
    uint32_t current_directory;
    char     current_path[FAT32_MAX_PATH];
} fat32_fs_t;

typedef struct {
    bool     in_use;
    uint32_t first_cluster;
    uint32_t current_cluster;
    uint32_t current_sector;
    uint32_t position;
    uint32_t size;
    uint8_t  attributes;
    uint32_t parent_cluster;
    uint32_t entry_offset;
} fat32_file_t;

typedef struct {
    char     name[FAT32_MAX_PATH];
    uint32_t size;
    uint8_t  attributes;
    uint32_t first_cluster;
    uint16_t creation_date;
    uint16_t creation_time;
    uint16_t modification_date;
    uint16_t modification_time;
} fat32_file_info_t;

bool fat32_init(void);
bool fat32_mount(void);
void fat32_unmount(void);
bool fat32_is_mounted(void);

bool fat32_read_dir(uint32_t cluster, fat32_file_info_t* entries, int* count, int max_entries);
bool fat32_find_entry(uint32_t dir_cluster, const char* name, fat32_file_info_t* info);
bool fat32_change_dir(const char* path);
const char* fat32_get_current_path(void);
uint32_t fat32_get_current_dir(void);

int fat32_open(const char* path);
int fat32_read(int fd, void* buffer, uint32_t size);
int fat32_write(int fd, const void* buffer, uint32_t size);
void fat32_close(int fd);
bool fat32_seek(int fd, uint32_t position);
uint32_t fat32_tell(int fd);
uint32_t fat32_size(int fd);

bool fat32_create_file(const char* path);
bool fat32_create_dir(const char* path);
bool fat32_delete(const char* path);
bool fat32_rename(const char* old_path, const char* new_path);
bool fat32_copy(const char* src_path, const char* dst_path);

uint32_t fat32_get_free_space(void);
uint32_t fat32_get_total_space(void);
uint32_t fat32_get_cluster_size(void);

uint32_t fat32_cluster_to_lba(uint32_t cluster);
uint32_t fat32_get_fat_entry(uint32_t cluster);
bool fat32_set_fat_entry(uint32_t cluster, uint32_t value);
uint32_t fat32_find_free_cluster(void);
bool fat32_free_chain(uint32_t start_cluster);

#endif

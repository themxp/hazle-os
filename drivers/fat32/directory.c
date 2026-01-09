#include "fat32.h"
#include "block.h"
#include "string.h"

extern fat32_fs_t* fat32_get_fs(void);
extern bool fat32_read_cluster(uint32_t cluster, uint8_t* buffer);
extern bool fat32_write_cluster(uint32_t cluster, const uint8_t* buffer);
extern bool fat32_is_eoc(uint32_t entry);

static uint8_t dir_cluster_buffer[4096 * 8];

static uint8_t lfn_checksum(const uint8_t* short_name) {
    uint8_t sum = 0;
    for (int i = 0; i < 11; i++) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + short_name[i];
    }
    return sum;
}

static void extract_lfn_chars(fat32_lfn_entry_t* lfn, char* dest, int* pos) {
    for (int i = 0; i < 5 && lfn->name1[i] != 0xFFFF && lfn->name1[i] != 0; i++) {
        dest[(*pos)++] = (char)(lfn->name1[i] & 0xFF);
    }
    for (int i = 0; i < 6 && lfn->name2[i] != 0xFFFF && lfn->name2[i] != 0; i++) {
        dest[(*pos)++] = (char)(lfn->name2[i] & 0xFF);
    }
    for (int i = 0; i < 2 && lfn->name3[i] != 0xFFFF && lfn->name3[i] != 0; i++) {
        dest[(*pos)++] = (char)(lfn->name3[i] & 0xFF);
    }
}

static void short_name_to_string(const char* short_name, char* output) {
    int pos = 0;
    
    for (int i = 0; i < 8 && short_name[i] != ' '; i++) {
        output[pos++] = short_name[i];
    }
    
    if (short_name[8] != ' ') {
        output[pos++] = '.';
        for (int i = 8; i < 11 && short_name[i] != ' '; i++) {
            output[pos++] = short_name[i];
        }
    }
    
    output[pos] = '\0';
}

static void string_to_short_name(const char* input, char* short_name) {
    memset(short_name, ' ', 11);
    
    int name_len = 0;
    const char* dot = NULL;
    
    for (int i = 0; input[i]; i++) {
        if (input[i] == '.') {
            dot = &input[i];
        }
    }
    
    if (dot) {
        name_len = dot - input;
        if (name_len > 8) name_len = 8;
        
        for (int i = 0; i < name_len; i++) {
            char c = input[i];
            if (c >= 'a' && c <= 'z') c -= 32;
            short_name[i] = c;
        }
        
        int ext_len = strlen(dot + 1);
        if (ext_len > 3) ext_len = 3;
        
        for (int i = 0; i < ext_len; i++) {
            char c = dot[1 + i];
            if (c >= 'a' && c <= 'z') c -= 32;
            short_name[8 + i] = c;
        }
    } else {
        name_len = strlen(input);
        if (name_len > 8) name_len = 8;
        
        for (int i = 0; i < name_len; i++) {
            char c = input[i];
            if (c >= 'a' && c <= 'z') c -= 32;
            short_name[i] = c;
        }
    }
}

bool fat32_read_dir(uint32_t cluster, fat32_file_info_t* entries, int* count, int max_entries) {
    fat32_fs_t* fs = fat32_get_fs();
    *count = 0;
    
    char lfn_buffer[256];
    int lfn_index = 0;
    bool has_lfn = false;
    
    uint32_t current_cluster = cluster;
    
    while (current_cluster >= 2 && !fat32_is_eoc(current_cluster)) {
        if (!fat32_read_cluster(current_cluster, dir_cluster_buffer)) {
            return false;
        }
        
        int entries_per_cluster = fs->bytes_per_cluster / 32;
        
        for (int i = 0; i < entries_per_cluster && *count < max_entries; i++) {
            fat32_dir_entry_t* entry = (fat32_dir_entry_t*)(dir_cluster_buffer + i * 32);
            
            if (entry->name[0] == FAT32_ENTRY_END) {
                return true;
            }
            
            if ((uint8_t)entry->name[0] == FAT32_ENTRY_FREE) {
                continue;
            }
            
            if (entry->attributes == FAT32_ATTR_LFN) {
                fat32_lfn_entry_t* lfn = (fat32_lfn_entry_t*)entry;
                
                if (lfn->order & 0x40) {
                    memset(lfn_buffer, 0, sizeof(lfn_buffer));
                    lfn_index = (lfn->order & 0x3F) * 13;
                    has_lfn = true;
                }
                
                if (has_lfn) {
                    int base = ((lfn->order & 0x3F) - 1) * 13;
                    int pos = base;
                    extract_lfn_chars(lfn, lfn_buffer, &pos);
                }
                continue;
            }
            
            if (entry->attributes & FAT32_ATTR_VOLUME_ID) {
                has_lfn = false;
                continue;
            }
            
            fat32_file_info_t* info = &entries[*count];
            
            if (has_lfn) {
                strncpy(info->name, lfn_buffer, FAT32_MAX_PATH - 1);
                info->name[FAT32_MAX_PATH - 1] = '\0';
            } else {
                short_name_to_string(entry->name, info->name);
            }
            
            info->size = entry->file_size;
            info->attributes = entry->attributes;
            info->first_cluster = ((uint32_t)entry->first_cluster_high << 16) | entry->first_cluster_low;
            info->creation_date = entry->creation_date;
            info->creation_time = entry->creation_time;
            info->modification_date = entry->modification_date;
            info->modification_time = entry->modification_time;
            
            (*count)++;
            has_lfn = false;
        }
        
        current_cluster = fat32_get_fat_entry(current_cluster);
    }
    
    return true;
}

bool fat32_find_entry(uint32_t dir_cluster, const char* name, fat32_file_info_t* info) {
    fat32_file_info_t entries[64];
    int count;
    
    if (!fat32_read_dir(dir_cluster, entries, &count, 64)) {
        return false;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            memcpy(info, &entries[i], sizeof(fat32_file_info_t));
            return true;
        }
        
        char short_upper[13];
        char name_upper[256];
        strcpy(short_upper, entries[i].name);
        strcpy(name_upper, name);
        
        for (int j = 0; short_upper[j]; j++) {
            if (short_upper[j] >= 'a' && short_upper[j] <= 'z') {
                short_upper[j] -= 32;
            }
        }
        for (int j = 0; name_upper[j]; j++) {
            if (name_upper[j] >= 'a' && name_upper[j] <= 'z') {
                name_upper[j] -= 32;
            }
        }
        
        if (strcmp(short_upper, name_upper) == 0) {
            memcpy(info, &entries[i], sizeof(fat32_file_info_t));
            return true;
        }
    }
    
    return false;
}

bool fat32_find_free_entry(uint32_t dir_cluster, uint32_t* out_cluster, uint32_t* out_offset) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t current_cluster = dir_cluster;
    
    while (current_cluster >= 2 && !fat32_is_eoc(current_cluster)) {
        if (!fat32_read_cluster(current_cluster, dir_cluster_buffer)) {
            return false;
        }
        
        int entries_per_cluster = fs->bytes_per_cluster / 32;
        
        for (int i = 0; i < entries_per_cluster; i++) {
            fat32_dir_entry_t* entry = (fat32_dir_entry_t*)(dir_cluster_buffer + i * 32);
            
            if (entry->name[0] == FAT32_ENTRY_END || (uint8_t)entry->name[0] == FAT32_ENTRY_FREE) {
                *out_cluster = current_cluster;
                *out_offset = i * 32;
                return true;
            }
        }
        
        current_cluster = fat32_get_fat_entry(current_cluster);
    }
    
    return false;
}

bool fat32_create_entry(uint32_t dir_cluster, const char* name, uint8_t attributes, uint32_t first_cluster, uint32_t size) {
    uint32_t entry_cluster, entry_offset;
    
    if (!fat32_find_free_entry(dir_cluster, &entry_cluster, &entry_offset)) {
        return false;
    }
    
    if (!fat32_read_cluster(entry_cluster, dir_cluster_buffer)) {
        return false;
    }
    
    fat32_dir_entry_t* entry = (fat32_dir_entry_t*)(dir_cluster_buffer + entry_offset);
    memset(entry, 0, sizeof(fat32_dir_entry_t));
    
    string_to_short_name(name, entry->name);
    entry->attributes = attributes;
    entry->first_cluster_high = (first_cluster >> 16) & 0xFFFF;
    entry->first_cluster_low = first_cluster & 0xFFFF;
    entry->file_size = size;
    
    entry->creation_date = 0x4E21;
    entry->creation_time = 0x0000;
    entry->modification_date = 0x4E21;
    entry->modification_time = 0x0000;
    entry->access_date = 0x4E21;
    
    return fat32_write_cluster(entry_cluster, dir_cluster_buffer);
}

bool fat32_delete_entry(uint32_t dir_cluster, const char* name) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t current_cluster = dir_cluster;
    
    while (current_cluster >= 2 && !fat32_is_eoc(current_cluster)) {
        if (!fat32_read_cluster(current_cluster, dir_cluster_buffer)) {
            return false;
        }
        
        int entries_per_cluster = fs->bytes_per_cluster / 32;
        
        for (int i = 0; i < entries_per_cluster; i++) {
            fat32_dir_entry_t* entry = (fat32_dir_entry_t*)(dir_cluster_buffer + i * 32);
            
            if (entry->name[0] == FAT32_ENTRY_END) {
                return false;
            }
            
            if ((uint8_t)entry->name[0] == FAT32_ENTRY_FREE) {
                continue;
            }
            
            if (entry->attributes == FAT32_ATTR_LFN) {
                continue;
            }
            
            char entry_name[13];
            short_name_to_string(entry->name, entry_name);
            
            char name_upper[256];
            char entry_upper[13];
            strcpy(name_upper, name);
            strcpy(entry_upper, entry_name);
            
            for (int j = 0; name_upper[j]; j++) {
                if (name_upper[j] >= 'a' && name_upper[j] <= 'z') name_upper[j] -= 32;
            }
            for (int j = 0; entry_upper[j]; j++) {
                if (entry_upper[j] >= 'a' && entry_upper[j] <= 'z') entry_upper[j] -= 32;
            }
            
            if (strcmp(name_upper, entry_upper) == 0) {
                entry->name[0] = FAT32_ENTRY_FREE;
                return fat32_write_cluster(current_cluster, dir_cluster_buffer);
            }
        }
        
        current_cluster = fat32_get_fat_entry(current_cluster);
    }
    
    return false;
}

bool fat32_update_entry_size(uint32_t dir_cluster, const char* name, uint32_t new_size) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t current_cluster = dir_cluster;
    
    while (current_cluster >= 2 && !fat32_is_eoc(current_cluster)) {
        if (!fat32_read_cluster(current_cluster, dir_cluster_buffer)) {
            return false;
        }
        
        int entries_per_cluster = fs->bytes_per_cluster / 32;
        
        for (int i = 0; i < entries_per_cluster; i++) {
            fat32_dir_entry_t* entry = (fat32_dir_entry_t*)(dir_cluster_buffer + i * 32);
            
            if (entry->name[0] == FAT32_ENTRY_END) {
                return false;
            }
            
            if ((uint8_t)entry->name[0] == FAT32_ENTRY_FREE || entry->attributes == FAT32_ATTR_LFN) {
                continue;
            }
            
            char entry_name[13];
            short_name_to_string(entry->name, entry_name);
            
            char name_upper[256];
            char entry_upper[13];
            strcpy(name_upper, name);
            strcpy(entry_upper, entry_name);
            
            for (int j = 0; name_upper[j]; j++) {
                if (name_upper[j] >= 'a' && name_upper[j] <= 'z') name_upper[j] -= 32;
            }
            for (int j = 0; entry_upper[j]; j++) {
                if (entry_upper[j] >= 'a' && entry_upper[j] <= 'z') entry_upper[j] -= 32;
            }
            
            if (strcmp(name_upper, entry_upper) == 0) {
                entry->file_size = new_size;
                return fat32_write_cluster(current_cluster, dir_cluster_buffer);
            }
        }
        
        current_cluster = fat32_get_fat_entry(current_cluster);
    }
    
    return false;
}

bool fat32_update_entry_cluster(uint32_t dir_cluster, const char* name, uint32_t new_cluster) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t current_cluster = dir_cluster;
    
    while (current_cluster >= 2 && !fat32_is_eoc(current_cluster)) {
        if (!fat32_read_cluster(current_cluster, dir_cluster_buffer)) {
            return false;
        }
        
        int entries_per_cluster = fs->bytes_per_cluster / 32;
        
        for (int i = 0; i < entries_per_cluster; i++) {
            fat32_dir_entry_t* entry = (fat32_dir_entry_t*)(dir_cluster_buffer + i * 32);
            
            if (entry->name[0] == FAT32_ENTRY_END) {
                return false;
            }
            
            if ((uint8_t)entry->name[0] == FAT32_ENTRY_FREE || entry->attributes == FAT32_ATTR_LFN) {
                continue;
            }
            
            char entry_name[13];
            short_name_to_string(entry->name, entry_name);
            
            char name_upper[256];
            char entry_upper[13];
            strcpy(name_upper, name);
            strcpy(entry_upper, entry_name);
            
            for (int j = 0; name_upper[j]; j++) {
                if (name_upper[j] >= 'a' && name_upper[j] <= 'z') name_upper[j] -= 32;
            }
            for (int j = 0; entry_upper[j]; j++) {
                if (entry_upper[j] >= 'a' && entry_upper[j] <= 'z') entry_upper[j] -= 32;
            }
            
            if (strcmp(name_upper, entry_upper) == 0) {
                entry->first_cluster_high = (new_cluster >> 16) & 0xFFFF;
                entry->first_cluster_low = new_cluster & 0xFFFF;
                return fat32_write_cluster(current_cluster, dir_cluster_buffer);
            }
        }
        
        current_cluster = fat32_get_fat_entry(current_cluster);
    }
    
    return false;
}

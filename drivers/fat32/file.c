#include "fat32.h"
#include "block.h"
#include "string.h"

extern fat32_fs_t* fat32_get_fs(void);
extern bool fat32_read_cluster(uint32_t cluster, uint8_t* buffer);
extern bool fat32_write_cluster(uint32_t cluster, const uint8_t* buffer);
extern bool fat32_is_eoc(uint32_t entry);
extern uint32_t fat32_allocate_cluster(void);
extern bool fat32_extend_chain(uint32_t last_cluster, uint32_t new_cluster);
extern bool fat32_find_entry(uint32_t dir_cluster, const char* name, fat32_file_info_t* info);
extern bool fat32_create_entry(uint32_t dir_cluster, const char* name, uint8_t attributes, uint32_t first_cluster, uint32_t size);
extern bool fat32_delete_entry(uint32_t dir_cluster, const char* name);
extern bool fat32_update_entry_size(uint32_t dir_cluster, const char* name, uint32_t new_size);
extern bool fat32_update_entry_cluster(uint32_t dir_cluster, const char* name, uint32_t new_cluster);
extern bool fat32_update_entry(uint32_t dir_cluster, const char* name, uint32_t new_size, uint32_t new_cluster);
extern bool fat32_sync_fat(void);

static fat32_file_t open_files[FAT32_MAX_OPEN_FILES];
static uint8_t file_cluster_buffer[4096 * 8];

void fat32_init_files(void) {
    for (int i = 0; i < FAT32_MAX_OPEN_FILES; i++) {
        open_files[i].in_use = false;
    }
}

static int find_free_fd(void) {
    for (int i = 0; i < FAT32_MAX_OPEN_FILES; i++) {
        if (!open_files[i].in_use) {
            return i;
        }
    }
    return -1;
}

static bool parse_path(const char* path, uint32_t* dir_cluster, char* filename) {
    fat32_fs_t* fs = fat32_get_fs();
    
    uint32_t current = (path[0] == '/') ? fs->root_cluster : fs->current_directory;
    
    char path_copy[FAT32_MAX_PATH];
    strncpy(path_copy, path, FAT32_MAX_PATH - 1);
    path_copy[FAT32_MAX_PATH - 1] = '\0';
    
    char* last_slash = NULL;
    for (int i = strlen(path_copy) - 1; i >= 0; i--) {
        if (path_copy[i] == '/') {
            last_slash = &path_copy[i];
            break;
        }
    }
    
    if (last_slash) {
        strcpy(filename, last_slash + 1);
        *last_slash = '\0';
        
        if (strlen(path_copy) > 0 && path_copy[0] == '/') {
            char* segment = strtok(path_copy + 1, "/");
            while (segment) {
                fat32_file_info_t info;
                if (!fat32_find_entry(current, segment, &info)) {
                    return false;
                }
                if (!(info.attributes & FAT32_ATTR_DIRECTORY)) {
                    return false;
                }
                current = info.first_cluster;
                segment = strtok(NULL, "/");
            }
        } else if (strlen(path_copy) > 0) {
            char* segment = strtok(path_copy, "/");
            while (segment) {
                fat32_file_info_t info;
                if (!fat32_find_entry(current, segment, &info)) {
                    return false;
                }
                if (!(info.attributes & FAT32_ATTR_DIRECTORY)) {
                    return false;
                }
                current = info.first_cluster;
                segment = strtok(NULL, "/");
            }
        }
    } else {
        strcpy(filename, path_copy);
    }
    
    *dir_cluster = current;
    return true;
}

int fat32_open(const char* path) {
    int fd = find_free_fd();
    if (fd < 0) {
        return -1;
    }
    
    uint32_t dir_cluster;
    char filename[FAT32_MAX_PATH];
    
    if (!parse_path(path, &dir_cluster, filename)) {
        return -1;
    }
    
    fat32_file_info_t info;
    if (!fat32_find_entry(dir_cluster, filename, &info)) {
        return -1;
    }
    
    if (info.attributes & FAT32_ATTR_DIRECTORY) {
        return -1;
    }
    
    fat32_file_t* file = &open_files[fd];
    file->in_use = true;
    file->dirty = false;
    file->first_cluster = info.first_cluster;
    file->current_cluster = info.first_cluster;
    file->current_sector = 0;
    file->position = 0;
    file->size = info.size;
    file->original_size = info.size;
    file->attributes = info.attributes;
    file->parent_cluster = dir_cluster;
    strncpy(file->filename, filename, 63);
    file->filename[63] = '\0';
    
    return fd;
}

int fat32_read(int fd, void* buffer, uint32_t size) {
    if (fd < 0 || fd >= FAT32_MAX_OPEN_FILES) {
        return -1;
    }
    
    fat32_file_t* file = &open_files[fd];
    if (!file->in_use) {
        return -1;
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    
    if (file->position >= file->size) {
        return 0;
    }
    
    uint32_t bytes_remaining = file->size - file->position;
    if (size > bytes_remaining) {
        size = bytes_remaining;
    }
    
    uint8_t* out = (uint8_t*)buffer;
    uint32_t bytes_read = 0;
    
    while (bytes_read < size && file->current_cluster >= 2) {
        uint32_t fat_entry = fat32_get_fat_entry(file->current_cluster);
        
        if (!fat32_read_cluster(file->current_cluster, file_cluster_buffer)) {
            return bytes_read > 0 ? (int)bytes_read : -1;
        }
        
        uint32_t offset_in_cluster = file->position % fs->bytes_per_cluster;
        uint32_t bytes_in_cluster = fs->bytes_per_cluster - offset_in_cluster;
        uint32_t bytes_to_copy = size - bytes_read;
        
        if (bytes_to_copy > bytes_in_cluster) {
            bytes_to_copy = bytes_in_cluster;
        }
        
        memcpy(out + bytes_read, file_cluster_buffer + offset_in_cluster, bytes_to_copy);
        bytes_read += bytes_to_copy;
        file->position += bytes_to_copy;
        
        if (file->position % fs->bytes_per_cluster == 0 && bytes_read < size) {
            if (fat32_is_eoc(fat_entry)) {
                break;
            }
            file->current_cluster = fat_entry;
        }
    }
    
    return (int)bytes_read;
}

int fat32_write(int fd, const void* buffer, uint32_t size) {
    if (fd < 0 || fd >= FAT32_MAX_OPEN_FILES) {
        return -1;
    }
    
    fat32_file_t* file = &open_files[fd];
    if (!file->in_use) {
        return -1;
    }
    
    if (size == 0) {
        return 0;
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    const uint8_t* in = (const uint8_t*)buffer;
    uint32_t bytes_written = 0;
    
    if (file->first_cluster == 0) {
        uint32_t new_cluster = fat32_allocate_cluster();
        if (new_cluster == 0) {
            return -1;
        }
        file->first_cluster = new_cluster;
        file->current_cluster = new_cluster;
        file->dirty = true;
    }
    
    while (bytes_written < size && file->current_cluster >= 2) {
        if (!fat32_read_cluster(file->current_cluster, file_cluster_buffer)) {
            break;
        }
        
        uint32_t offset_in_cluster = file->position % fs->bytes_per_cluster;
        uint32_t space_in_cluster = fs->bytes_per_cluster - offset_in_cluster;
        uint32_t to_write = size - bytes_written;
        
        if (to_write > space_in_cluster) {
            to_write = space_in_cluster;
        }
        
        memcpy(file_cluster_buffer + offset_in_cluster, in + bytes_written, to_write);
        
        if (!fat32_write_cluster(file->current_cluster, file_cluster_buffer)) {
            break;
        }
        
        bytes_written += to_write;
        file->position += to_write;
        
        if (file->position > file->size) {
            file->size = file->position;
            file->dirty = true;
        }
        
        if (bytes_written < size) {
            uint32_t next = fat32_get_fat_entry(file->current_cluster);
            
            if (fat32_is_eoc(next)) {
                uint32_t new_cluster = fat32_allocate_cluster();
                if (new_cluster == 0) {
                    break;
                }
                fat32_extend_chain(file->current_cluster, new_cluster);
                file->current_cluster = new_cluster;
            } else if (next >= 2) {
                file->current_cluster = next;
            } else {
                break;
            }
        }
    }
    
    return (int)bytes_written;
}


void fat32_close(int fd) {
    if (fd < 0 || fd >= FAT32_MAX_OPEN_FILES) {
        return;
    }
    
    fat32_file_t* file = &open_files[fd];
    if (!file->in_use) {
        return;
    }
    
    if (file->dirty) {
        fat32_update_entry(file->parent_cluster, file->filename, file->size, file->first_cluster);
    }
    
    fat32_sync_fat();
    block_flush();
    
    file->in_use = false;
}

bool fat32_seek(int fd, uint32_t position) {
    if (fd < 0 || fd >= FAT32_MAX_OPEN_FILES) {
        return false;
    }
    
    fat32_file_t* file = &open_files[fd];
    if (!file->in_use) {
        return false;
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    
    file->position = position;
    file->current_cluster = file->first_cluster;
    
    uint32_t clusters_to_skip = position / fs->bytes_per_cluster;
    
    for (uint32_t i = 0; i < clusters_to_skip; i++) {
        if (file->current_cluster < 2 || fat32_is_eoc(file->current_cluster)) {
            break;
        }
        file->current_cluster = fat32_get_fat_entry(file->current_cluster);
    }
    
    return true;
}

uint32_t fat32_tell(int fd) {
    if (fd < 0 || fd >= FAT32_MAX_OPEN_FILES) {
        return 0;
    }
    
    fat32_file_t* file = &open_files[fd];
    if (!file->in_use) {
        return 0;
    }
    
    return file->position;
}

uint32_t fat32_size(int fd) {
    if (fd < 0 || fd >= FAT32_MAX_OPEN_FILES) {
        return 0;
    }
    
    fat32_file_t* file = &open_files[fd];
    if (!file->in_use) {
        return 0;
    }
    
    return file->size;
}

bool fat32_create_file(const char* path) {
    uint32_t dir_cluster;
    char filename[FAT32_MAX_PATH];
    
    if (!parse_path(path, &dir_cluster, filename)) {
        return false;
    }
    
    fat32_file_info_t info;
    if (fat32_find_entry(dir_cluster, filename, &info)) {
        return false;
    }
    
    return fat32_create_entry(dir_cluster, filename, FAT32_ATTR_ARCHIVE, 0, 0);
}

bool fat32_create_dir(const char* path) {
    uint32_t dir_cluster;
    char dirname[FAT32_MAX_PATH];
    
    if (!parse_path(path, &dir_cluster, dirname)) {
        return false;
    }
    
    fat32_file_info_t info;
    if (fat32_find_entry(dir_cluster, dirname, &info)) {
        return false;
    }
    
    uint32_t new_cluster = fat32_allocate_cluster();
    if (new_cluster == 0) {
        return false;
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    memset(file_cluster_buffer, 0, fs->bytes_per_cluster);
    
    fat32_dir_entry_t* dot = (fat32_dir_entry_t*)file_cluster_buffer;
    memcpy(dot->name, ".          ", 11);
    dot->attributes = FAT32_ATTR_DIRECTORY;
    dot->first_cluster_high = (new_cluster >> 16) & 0xFFFF;
    dot->first_cluster_low = new_cluster & 0xFFFF;
    
    fat32_dir_entry_t* dotdot = (fat32_dir_entry_t*)(file_cluster_buffer + 32);
    memcpy(dotdot->name, "..         ", 11);
    dotdot->attributes = FAT32_ATTR_DIRECTORY;
    dotdot->first_cluster_high = (dir_cluster >> 16) & 0xFFFF;
    dotdot->first_cluster_low = dir_cluster & 0xFFFF;
    
    if (!fat32_write_cluster(new_cluster, file_cluster_buffer)) {
        fat32_free_chain(new_cluster);
        return false;
    }
    
    return fat32_create_entry(dir_cluster, dirname, FAT32_ATTR_DIRECTORY, new_cluster, 0);
}

bool fat32_delete(const char* path) {
    uint32_t dir_cluster;
    char filename[FAT32_MAX_PATH];
    
    if (!parse_path(path, &dir_cluster, filename)) {
        return false;
    }
    
    fat32_file_info_t info;
    if (!fat32_find_entry(dir_cluster, filename, &info)) {
        return false;
    }
    
    if (info.first_cluster >= 2) {
        fat32_free_chain(info.first_cluster);
    }
    
    return fat32_delete_entry(dir_cluster, filename);
}

bool fat32_rename(const char* old_path, const char* new_path) {
    uint32_t old_dir_cluster, new_dir_cluster;
    char old_filename[FAT32_MAX_PATH], new_filename[FAT32_MAX_PATH];
    
    if (!parse_path(old_path, &old_dir_cluster, old_filename)) {
        return false;
    }
    
    if (!parse_path(new_path, &new_dir_cluster, new_filename)) {
        return false;
    }
    
    fat32_file_info_t info;
    if (!fat32_find_entry(old_dir_cluster, old_filename, &info)) {
        return false;
    }
    
    if (!fat32_create_entry(new_dir_cluster, new_filename, info.attributes, info.first_cluster, info.size)) {
        return false;
    }
    
    return fat32_delete_entry(old_dir_cluster, old_filename);
}

bool fat32_copy(const char* src_path, const char* dst_path) {
    int src_fd = fat32_open(src_path);
    if (src_fd < 0) {
        return false;
    }
    
    if (!fat32_create_file(dst_path)) {
        fat32_close(src_fd);
        return false;
    }
    
    int dst_fd = fat32_open(dst_path);
    if (dst_fd < 0) {
        fat32_close(src_fd);
        fat32_delete(dst_path);
        return false;
    }
    
    uint8_t copy_buffer[512];
    int bytes_read;
    
    while ((bytes_read = fat32_read(src_fd, copy_buffer, sizeof(copy_buffer))) > 0) {
        if (fat32_write(dst_fd, copy_buffer, bytes_read) != bytes_read) {
            fat32_close(src_fd);
            fat32_close(dst_fd);
            fat32_delete(dst_path);
            return false;
        }
    }
    
    fat32_close(src_fd);
    fat32_close(dst_fd);
    
    return true;
}

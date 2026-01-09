#include "fat32.h"
#include "block.h"
#include "string.h"

extern bool fat32_parse_bpb(void);
extern void fat32_init_files(void);
extern void fat32_invalidate_fat_cache(void);

static fat32_fs_t fs;

fat32_fs_t* fat32_get_fs(void) {
    return &fs;
}

bool fat32_init(void) {
    memset(&fs, 0, sizeof(fat32_fs_t));
    fat32_init_files();
    return true;
}

bool fat32_mount(void) {
    if (!block_is_available()) {
        if (!block_init()) {
            return false;
        }
    }
    
    if (!fat32_parse_bpb()) {
        return false;
    }
    
    fs.mounted = true;
    return true;
}

void fat32_unmount(void) {
    if (!fs.mounted) {
        return;
    }
    
    fat32_sync_fat();
    block_flush();
    
    fs.mounted = false;
    fat32_invalidate_fat_cache();
}

bool fat32_is_mounted(void) {
    return fs.mounted;
}

bool fat32_change_dir(const char* path) {
    if (!fs.mounted) {
        return false;
    }
    
    if (strcmp(path, "/") == 0) {
        fs.current_directory = fs.root_cluster;
        strcpy(fs.current_path, "/");
        return true;
    }
    
    if (strcmp(path, "..") == 0) {
        fat32_file_info_t info;
        if (fat32_find_entry(fs.current_directory, "..", &info)) {
            fs.current_directory = info.first_cluster;
            if (fs.current_directory == 0) {
                fs.current_directory = fs.root_cluster;
            }
            
            char* last_slash = NULL;
            for (int i = strlen(fs.current_path) - 1; i > 0; i--) {
                if (fs.current_path[i] == '/') {
                    last_slash = &fs.current_path[i];
                    break;
                }
            }
            if (last_slash && last_slash != fs.current_path) {
                *last_slash = '\0';
            } else {
                strcpy(fs.current_path, "/");
            }
            return true;
        }
        return false;
    }
    
    if (strcmp(path, ".") == 0) {
        return true;
    }
    
    uint32_t start_cluster;
    char path_copy[FAT32_MAX_PATH];
    strncpy(path_copy, path, FAT32_MAX_PATH - 1);
    path_copy[FAT32_MAX_PATH - 1] = '\0';
    
    if (path[0] == '/') {
        start_cluster = fs.root_cluster;
        strcpy(fs.current_path, "");
    } else {
        start_cluster = fs.current_directory;
    }
    
    char* segment = strtok(path_copy, "/");
    while (segment) {
        if (strlen(segment) == 0) {
            segment = strtok(NULL, "/");
            continue;
        }
        
        fat32_file_info_t info;
        if (!fat32_find_entry(start_cluster, segment, &info)) {
            return false;
        }
        
        if (!(info.attributes & FAT32_ATTR_DIRECTORY)) {
            return false;
        }
        
        start_cluster = info.first_cluster;
        if (start_cluster == 0) {
            start_cluster = fs.root_cluster;
        }
        
        if (strcmp(fs.current_path, "/") != 0) {
            int len = strlen(fs.current_path);
            if (len + strlen(segment) + 2 < FAT32_MAX_PATH) {
                fs.current_path[len] = '/';
                strcpy(fs.current_path + len + 1, segment);
            }
        } else {
            if (strlen(segment) + 2 < FAT32_MAX_PATH) {
                strcpy(fs.current_path + 1, segment);
            }
        }
        
        segment = strtok(NULL, "/");
    }
    
    fs.current_directory = start_cluster;
    
    if (strlen(fs.current_path) == 0) {
        strcpy(fs.current_path, "/");
    }
    
    return true;
}

const char* fat32_get_current_path(void) {
    return fs.current_path;
}

uint32_t fat32_get_current_dir(void) {
    return fs.current_directory;
}

uint32_t fat32_get_free_space(void) {
    extern uint32_t fat32_count_free_clusters(void);
    return fat32_count_free_clusters() * fs.bytes_per_cluster;
}

uint32_t fat32_get_total_space(void) {
    return fs.total_clusters * fs.bytes_per_cluster;
}

uint32_t fat32_get_cluster_size(void) {
    return fs.bytes_per_cluster;
}

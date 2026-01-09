#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

void cmd_ls(int argc, char** argv) {
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: No filesystem mounted");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    uint32_t dir_cluster = fat32_get_current_dir();
    
    if (argc > 1) {
        fat32_file_info_t info;
        if (fat32_find_entry(fat32_get_current_dir(), argv[1], &info)) {
            if (info.attributes & FAT32_ATTR_DIRECTORY) {
                dir_cluster = info.first_cluster;
            } else {
                display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
                display_write("Error: Not a directory: ");
                display_writeln(argv[1]);
                display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
                return;
            }
        } else {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_write("Error: Directory not found: ");
            display_writeln(argv[1]);
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    fat32_file_info_t entries[64];
    int count;
    
    if (!fat32_read_dir(dir_cluster, entries, &count, 64)) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Error: Failed to read directory");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        if (entries[i].attributes & FAT32_ATTR_DIRECTORY) {
            display_set_color(DISPLAY_COLOR_LIGHT_BLUE, DISPLAY_COLOR_BLACK);
            display_write("[DIR]  ");
        } else {
            display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
            display_write("[FILE] ");
        }
        
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        
        char size_str[16];
        if (entries[i].attributes & FAT32_ATTR_DIRECTORY) {
            strcpy(size_str, "       ");
        } else {
            itoa(entries[i].size, size_str, 10);
            int len = strlen(size_str);
            while (len < 7) {
                display_write(" ");
                len++;
            }
        }
        display_write(size_str);
        display_write("  ");
        
        if (entries[i].attributes & FAT32_ATTR_DIRECTORY) {
            display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
        } else {
            display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
        }
        display_writeln(entries[i].name);
    }
    
    display_set_color(DISPLAY_COLOR_DARK_GREY, DISPLAY_COLOR_BLACK);
    char count_str[16];
    itoa(count, count_str, 10);
    display_write(count_str);
    display_writeln(" items");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
}

#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "block.h"
#include "ata.h"
#include "string.h"

void cmd_disk(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("=== Disk Information ===");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    
    ata_drive_t* drive = ata_get_primary();
    
    if (!drive->present) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("No disk detected");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    display_write("Model: ");
    display_writeln(drive->model);
    
    display_write("Sectors: ");
    char num_str[16];
    itoa(drive->sector_count, num_str, 10);
    display_writeln(num_str);
    
    uint32_t size_mb = (drive->sector_count / 2048);
    display_write("Size: ");
    itoa(size_mb, num_str, 10);
    display_write(num_str);
    display_writeln(" MB");
    
    display_writeln("");
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("=== Filesystem Information ===");
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
            display_writeln("Filesystem: Not mounted (no FAT32 partition)");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    display_writeln("Filesystem: FAT32");
    
    uint32_t cluster_size = fat32_get_cluster_size();
    display_write("Cluster Size: ");
    itoa(cluster_size, num_str, 10);
    display_write(num_str);
    display_writeln(" bytes");
    
    uint32_t total_space = fat32_get_total_space();
    uint32_t free_space = fat32_get_free_space();
    uint32_t used_space = total_space - free_space;
    
    display_write("Total Space: ");
    itoa(total_space / (1024 * 1024), num_str, 10);
    display_write(num_str);
    display_writeln(" MB");
    
    display_write("Used Space: ");
    itoa(used_space / (1024 * 1024), num_str, 10);
    display_write(num_str);
    display_writeln(" MB");
    
    display_write("Free Space: ");
    itoa(free_space / (1024 * 1024), num_str, 10);
    display_write(num_str);
    display_writeln(" MB");
}

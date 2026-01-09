#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

extern fat32_fs_t* fat32_get_fs(void);
extern bool fat32_find_entry(uint32_t dir_cluster, const char* name, fat32_file_info_t* info);

static void print_uint32(uint32_t val) {
    char buf[12];
    int i = 10;
    buf[11] = '\0';
    if (val == 0) {
        display_write("0");
        return;
    }
    while (val > 0 && i >= 0) {
        buf[i--] = '0' + (val % 10);
        val /= 10;
    }
    display_write(&buf[i + 1]);
}

void cmd_catdbg(int argc, char** argv) {
    if (argc < 2) {
        display_writeln("Usage: catdbg <filename>");
        return;
    }
    
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_writeln("Error: No filesystem mounted");
            return;
        }
    }
    
    fat32_fs_t* fs = fat32_get_fs();
    display_write("Current dir cluster: ");
    print_uint32(fs->current_directory);
    display_writeln("");
    
    fat32_file_info_t info;
    if (!fat32_find_entry(fs->current_directory, argv[1], &info)) {
        display_writeln("Error: fat32_find_entry failed");
        return;
    }
    
    display_write("Found entry: ");
    display_writeln(info.name);
    display_write("  size: ");
    print_uint32(info.size);
    display_writeln("");
    display_write("  first_cluster: ");
    print_uint32(info.first_cluster);
    display_writeln("");
    display_write("  attributes: ");
    print_uint32(info.attributes);
    display_writeln("");
    
    if (info.first_cluster < 2) {
        display_writeln("Error: first_cluster < 2, file has no data clusters!");
        return;
    }
    
    int fd = fat32_open(argv[1]);
    if (fd < 0) {
        display_writeln("Error: fat32_open failed");
        return;
    }
    
    display_write("Opened fd: ");
    print_uint32(fd);
    display_write(", size: ");
    print_uint32(fat32_size(fd));
    display_writeln("");
    
    char buffer[129];
    int bytes_read = fat32_read(fd, buffer, 128);
    display_write("Read returned: ");
    print_uint32(bytes_read >= 0 ? bytes_read : 0);
    if (bytes_read < 0) {
        display_write(" (error)");
    }
    display_writeln("");
    
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        display_write("Content: ");
        display_writeln(buffer);
    }
    
    fat32_close(fd);
}

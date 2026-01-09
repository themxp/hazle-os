#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

extern fat32_fs_t* fat32_get_fs(void);

static void print_num(int val) {
    char buf[12];
    int neg = 0;
    if (val < 0) {
        neg = 1;
        val = -val;
    }
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
    if (neg) buf[i--] = '-';
    display_write(&buf[i + 1]);
}

void cmd_writedbg(int argc, char** argv) {
    if (argc < 3) {
        display_writeln("Usage: writedbg <filename> <text>");
        return;
    }
    
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_writeln("Mount failed");
            return;
        }
    }
    
    const char* filename = argv[1];
    
    fat32_file_info_t info;
    bool exists = fat32_find_entry(fat32_get_current_dir(), filename, &info);
    display_write("File exists: ");
    display_writeln(exists ? "yes" : "no");
    
    if (!exists) {
        bool created = fat32_create_file(filename);
        display_write("Create result: ");
        display_writeln(created ? "ok" : "FAILED");
        if (!created) return;
    }
    
    int fd = fat32_open(filename);
    display_write("Open fd: ");
    print_num(fd);
    display_writeln("");
    if (fd < 0) return;
    
    display_write("File size after open: ");
    print_num(fat32_size(fd));
    display_writeln("");
    
    char text[128];
    strncpy(text, argv[2], 120);
    text[120] = '\n';
    text[121] = '\0';
    int text_len = strlen(text);
    
    display_write("Writing ");
    print_num(text_len);
    display_write(" bytes: ");
    display_writeln(text);
    
    int written = fat32_write(fd, text, text_len);
    display_write("Write returned: ");
    print_num(written);
    display_writeln("");
    
    display_write("Calling close...");
    fat32_close(fd);
    display_writeln(" done");
    
    display_writeln("Re-checking entry:");
    if (fat32_find_entry(fat32_get_current_dir(), filename, &info)) {
        display_write("  size: ");
        print_num(info.size);
        display_writeln("");
        display_write("  first_cluster: ");
        print_num(info.first_cluster);
        display_writeln("");
    } else {
        display_writeln("  NOT FOUND!");
    }
}

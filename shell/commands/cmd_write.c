#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

void cmd_write(int argc, char** argv) {
    if (argc < 3) {
        display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
        display_writeln("Usage: write <filename> <text>");
        display_writeln("Example: write test.txt hello world");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: No filesystem mounted");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    const char* filename = argv[1];
    
    fat32_file_info_t info;
    if (!fat32_find_entry(fat32_get_current_dir(), filename, &info)) {
        if (!fat32_create_file(filename)) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_write("Error: Cannot create file: ");
            display_writeln(filename);
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    int fd = fat32_open(filename);
    if (fd < 0) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_write("Error: Cannot open file: ");
        display_writeln(filename);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    char text_buffer[512];
    text_buffer[0] = '\0';
    
    for (int i = 2; i < argc; i++) {
        if (i > 2) {
            int len = strlen(text_buffer);
            text_buffer[len] = ' ';
            text_buffer[len + 1] = '\0';
        }
        
        char* arg = argv[i];
        if (arg[0] == '"') {
            arg++;
        }
        int arglen = strlen(arg);
        if (arglen > 0 && arg[arglen - 1] == '"') {
            arg[arglen - 1] = '\0';
        }
        
        int len = strlen(text_buffer);
        strcpy(text_buffer + len, arg);
    }
    
    int text_len = strlen(text_buffer);
    text_buffer[text_len] = '\n';
    text_len++;
    
    int written = fat32_write(fd, text_buffer, text_len);
    fat32_close(fd);
    
    if (written < 0) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Error: Failed to write to file");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    }
}

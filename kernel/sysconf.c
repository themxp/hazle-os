#include "sysconf.h"
#include "fat32.h"
#include "string.h"

static sysconf_t config;

static void set_defaults(void) {
    strcpy(config.bin_path, "/usr/bin");
    strcpy(config.prompt, "hazle@os:$");
    config.default_fg = 7;
    config.default_bg = 0;
    config.history_size = 50;
    strcpy(config.version, "1.0.0");
    strcpy(config.name, "Hazle OS");
}

static void trim(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
    char *start = str;
    while (*start == ' ') start++;
    if (start != str) {
        int i = 0;
        while (start[i]) {
            str[i] = start[i];
            i++;
        }
        str[i] = '\0';
    }
}

static void parse_line(const char *line) {
    if (line[0] == '#' || line[0] == '[' || line[0] == '\0') {
        return;
    }
    
    char key[64] = {0};
    char value[64] = {0};
    
    int i = 0;
    while (line[i] && line[i] != '=' && i < 63) {
        key[i] = line[i];
        i++;
    }
    key[i] = '\0';
    
    if (line[i] != '=') return;
    i++;
    
    int j = 0;
    while (line[i] && j < 63) {
        value[j++] = line[i++];
    }
    value[j] = '\0';
    
    trim(key);
    trim(value);
    
    if (strcmp(key, "bin") == 0) {
        strncpy(config.bin_path, value, 63);
    } else if (strcmp(key, "prompt") == 0) {
        strncpy(config.prompt, value, 31);
    } else if (strcmp(key, "default_fg") == 0) {
        config.default_fg = atoi(value);
    } else if (strcmp(key, "default_bg") == 0) {
        config.default_bg = atoi(value);
    } else if (strcmp(key, "history_size") == 0) {
        config.history_size = atoi(value);
    } else if (strcmp(key, "version") == 0) {
        strncpy(config.version, value, 15);
    } else if (strcmp(key, "name") == 0) {
        strncpy(config.name, value, 31);
    }
}

void sysconf_init(void) {
    set_defaults();
}

bool sysconf_load(void) {
    set_defaults();
    
    if (!fat32_is_mounted()) {
        return false;
    }
    
    int fd = fat32_open(SYSCONF_PATH);
    if (fd < 0) {
        return false;
    }
    
    char buffer[512];
    char line[128];
    int line_pos = 0;
    int bytes;
    
    while ((bytes = fat32_read(fd, buffer, 512)) > 0) {
        for (int i = 0; i < bytes; i++) {
            if (buffer[i] == '\n' || buffer[i] == '\r') {
                if (line_pos > 0) {
                    line[line_pos] = '\0';
                    parse_line(line);
                    line_pos = 0;
                }
            } else if (line_pos < 127) {
                line[line_pos++] = buffer[i];
            }
        }
    }
    
    if (line_pos > 0) {
        line[line_pos] = '\0';
        parse_line(line);
    }
    
    fat32_close(fd);
    return true;
}

sysconf_t *sysconf_get(void) {
    return &config;
}

const char *sysconf_get_bin_path(void) {
    return config.bin_path;
}

const char *sysconf_get_prompt(void) {
    return config.prompt;
}

#include "shell.h"
#include "display.h"
#include "keyboard.h"
#include "string.h"
#include "kernel.h"
#include "exec.h"
#include "sysconf.h"
#include "fat32.h"
#include "timer.h"
#include "io.h"

static char input_buffer[SHELL_BUFFER_SIZE];
static int buffer_pos = 0;
static bool shell_running = true;

static void do_reboot(void) {
    display_writeln("rebooting...");
    outb(0x64, 0xFE);
    while (1) {
        __asm__ volatile("hlt");
    }
}

static void do_shutdown(void) {
    display_writeln("system halted.");
    disable_interrupts();
    while (1) {
        __asm__ volatile("hlt");
    }
}

static int parse_time(const char *arg) {
    if (strcmp(arg, "now") == 0) {
        return 0;
    }
    
    int len = strlen(arg);
    if (len < 2) {
        return 60;
    }
    
    char unit = arg[len - 1];
    char num_str[16];
    strncpy(num_str, arg, len - 1);
    num_str[len - 1] = '\0';
    
    int value = 0;
    for (int i = 0; num_str[i]; i++) {
        if (num_str[i] >= '0' && num_str[i] <= '9') {
            value = value * 10 + (num_str[i] - '0');
        }
    }
    
    if (value == 0) value = 1;
    
    switch (unit) {
        case 's': return value;
        case 'm': return value * 60;
        case 'h': return value * 3600;
        default:  return 60;
    }
}

static bool is_builtin(const char *cmd) {
    return strcmp(cmd, "cd") == 0 || 
           strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "reboot") == 0 ||
           strcmp(cmd, "shutdown") == 0;
}

static int run_builtin(const char *cmd, int argc, char **argv) {
    if (strcmp(cmd, "cd") == 0) {
        if (argc < 2) {
            fat32_change_dir("/");
        } else {
            if (!fat32_change_dir(argv[1])) {
                display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
                display_writeln("directory not found");
                display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
                return 1;
            }
        }
        return 0;
    }
    
    if (strcmp(cmd, "exit") == 0) {
        shell_running = false;
        return 0;
    }
    
    if (strcmp(cmd, "reboot") == 0) {
        do_reboot();
        return 0;
    }
    
    if (strcmp(cmd, "shutdown") == 0) {
        int seconds = 60;
        
        if (argc >= 2) {
            seconds = parse_time(argv[1]);
        }
        
        if (seconds == 0) {
            do_shutdown();
        } else {
            display_write("shutdown in ");
            char buf[16];
            if (seconds >= 3600) {
                itoa(seconds / 3600, buf, 10);
                display_write(buf);
                display_writeln(" hour(s)...");
            } else if (seconds >= 60) {
                itoa(seconds / 60, buf, 10);
                display_write(buf);
                display_writeln(" minute(s)...");
            } else {
                itoa(seconds, buf, 10);
                display_write(buf);
                display_writeln(" second(s)...");
            }
            
            uint32_t target = timer_get_ticks() + (seconds * 1000);
            while (timer_get_ticks() < target) {
                __asm__ volatile("hlt");
            }
            do_shutdown();
        }
        return 0;
    }
    
    return 127;
}

static char *build_path(const char *cmd) {
    static char path_buf[256];
    const char *bin_path = sysconf_get_bin_path();
    
    int i = 0;
    while (bin_path[i] && i < 200) {
        path_buf[i] = bin_path[i];
        i++;
    }
    path_buf[i++] = '/';
    
    int j = 0;
    while (cmd[j] && i < 255) {
        path_buf[i++] = cmd[j++];
    }
    path_buf[i] = '\0';
    
    return path_buf;
}

static void execute_command(char *cmd) {
    if (strlen(cmd) == 0) {
        return;
    }
    
    char *argv[MAX_ARGS];
    int argc = 0;
    
    char *token = strtok(cmd, " ");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (argc == 0) {
        return;
    }
    
    if (is_builtin(argv[0])) {
        run_builtin(argv[0], argc, argv);
        return;
    }
    
    char *path = build_path(argv[0]);
    int result = exec_run(path, argc, argv);
    
    if (result == EXIT_NOT_FOUND) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_write("command not found: ");
        display_writeln(argv[0]);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    } else if (result == EXIT_NOT_EXEC) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_write("not executable: ");
        display_writeln(argv[0]);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    }
}

void shell_init(void) {
    buffer_pos = 0;
    memset(input_buffer, 0, SHELL_BUFFER_SIZE);
    shell_running = true;
    exec_init();
    sysconf_init();
    sysconf_load();
}

void shell_print_prompt(void) {
    sysconf_t *conf = sysconf_get();
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_write("hazle");
    display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display_write("@");
    display_set_color(DISPLAY_COLOR_LIGHT_BLUE, DISPLAY_COLOR_BLACK);
    display_write("os");
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_write(":");
    display_write(fat32_get_current_path());
    display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display_write("$ ");
    display_set_color(conf->default_fg, conf->default_bg);
}

void shell_run(void) {
    shell_print_prompt();
    
    while (shell_running) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            display_putchar('\n');
            input_buffer[buffer_pos] = '\0';
            execute_command(input_buffer);
            buffer_pos = 0;
            memset(input_buffer, 0, SHELL_BUFFER_SIZE);
            if (shell_running) {
                shell_print_prompt();
            }
        } else if (c == '\b') {
            if (buffer_pos > 0) {
                buffer_pos--;
                input_buffer[buffer_pos] = '\0';
                display_putchar('\b');
            }
        } else if (buffer_pos < SHELL_BUFFER_SIZE - 1) {
            input_buffer[buffer_pos++] = c;
            display_putchar(c);
        }
    }
    
    display_writeln("shell exited");
}

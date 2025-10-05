#include "shell.h"
#include "display.h"
#include "keyboard.h"
#include "string.h"
#include "kernel.h"

static char input_buffer[SHELL_BUFFER_SIZE];
static int buffer_pos = 0;

extern command_t commands[];
extern int command_count;

void shell_init(void) {
    buffer_pos = 0;
    memset(input_buffer, 0, SHELL_BUFFER_SIZE);
    register_commands();
}

void shell_print_prompt(void) {
    display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
    display_write("hazle");
    display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display_write("@");
    display_set_color(DISPLAY_COLOR_LIGHT_BLUE, DISPLAY_COLOR_BLACK);
    display_write("os");
    display_set_color(DISPLAY_COLOR_WHITE, DISPLAY_COLOR_BLACK);
    display_write(":$ ");
}

static void execute_command(char* cmd) {
    if (strlen(cmd) == 0) {
        return;
    }
    
    char* argv[MAX_ARGS];
    int argc = 0;
    
    char* token = strtok(cmd, " ");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (argc == 0) {
        return;
    }
    
    for (int i = 0; i < command_count; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return;
        }
    }
    
    display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
    display_write("Unknown command: ");
    display_writeln(argv[0]);
    display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    display_writeln("Type 'help' for available commands.");
}

void shell_run(void) {
    shell_print_prompt();
    
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            display_putchar('\n');
            input_buffer[buffer_pos] = '\0';
            execute_command(input_buffer);
            buffer_pos = 0;
            memset(input_buffer, 0, SHELL_BUFFER_SIZE);
            shell_print_prompt();
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
}


#ifndef SHELL_H
#define SHELL_H

#include "types.h"

#define SHELL_BUFFER_SIZE 256
#define MAX_ARGS 16

void shell_init(void);
void shell_run(void);
void shell_print_prompt(void);

typedef struct {
    const char* name;
    const char* description;
    void (*handler)(int argc, char** argv);
} command_t;

void register_commands(void);

#endif


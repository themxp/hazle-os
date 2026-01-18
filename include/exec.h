#ifndef EXEC_H
#define EXEC_H

#include "types.h"

#define EXIT_SUCCESS      0
#define EXIT_FAILURE      1
#define EXIT_INVALID_ARGS 2
#define EXIT_NOT_EXEC     126
#define EXIT_NOT_FOUND    127
#define EXIT_SIGNAL_BASE  128

void exec_init(void);
int exec_run(const char *path, int argc, char **argv);
void exec_exit(int code);
int exec_get_last_status(void);
bool exec_is_running(void);

#endif

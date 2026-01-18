#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

#define SYS_EXIT        0
#define SYS_WRITE       1
#define SYS_READ        2
#define SYS_OPEN        3
#define SYS_CLOSE       4
#define SYS_GETCHAR     5
#define SYS_PUTCHAR     6
#define SYS_PUTS        7
#define SYS_READDIR     8
#define SYS_CHDIR       9
#define SYS_GETCWD      10
#define SYS_MKDIR       11
#define SYS_REMOVE      12
#define SYS_GETTIME     13
#define SYS_GETTICKS    14
#define SYS_SETCOLOR    15
#define SYS_CLEAR       16
#define SYS_BEEP        17
#define SYS_REBOOT      18
#define SYS_GETMEM      19
#define SYS_GETUPTIME   20
#define SYS_CREATE      21
#define SYS_COPY        22
#define SYS_RENAME      23
#define SYS_DISKINFO    24

#define SYSCALL_COUNT   25

void syscall_init(void);
int syscall_dispatch(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

extern void syscall_interrupt_handler(void);

#endif

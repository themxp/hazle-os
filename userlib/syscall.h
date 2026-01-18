#ifndef HAZLE_SYSCALL_H
#define HAZLE_SYSCALL_H

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

static inline int syscall0(int num) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num));
    return ret;
}

static inline int syscall1(int num, int arg1) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1));
    return ret;
}

static inline int syscall2(int num, int arg1, int arg2) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2));
    return ret;
}

static inline int syscall3(int num, int arg1, int arg2, int arg3) {
    int ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

#endif

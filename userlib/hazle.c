#include "hazle.h"
#include "syscall.h"

void exit(int code) {
    syscall1(SYS_EXIT, code);
    while (1) {}
}

int putchar(int c) {
    return syscall1(SYS_PUTCHAR, c);
}

int puts(const char *s) {
    return syscall1(SYS_PUTS, (int)s);
}

int getchar(void) {
    return syscall0(SYS_GETCHAR);
}

int open(const char *path) {
    return syscall1(SYS_OPEN, (int)path);
}

int close(int fd) {
    return syscall1(SYS_CLOSE, fd);
}

int read(int fd, void *buf, size_t size) {
    return syscall3(SYS_READ, fd, (int)buf, (int)size);
}

int write(int fd, const void *buf, size_t size) {
    return syscall3(SYS_WRITE, fd, (int)buf, (int)size);
}

int chdir(const char *path) {
    return syscall1(SYS_CHDIR, (int)path);
}

char *getcwd(char *buf, size_t size) {
    syscall2(SYS_GETCWD, (int)buf, (int)size);
    return buf;
}

int mkdir(const char *path) {
    return syscall1(SYS_MKDIR, (int)path);
}

int remove(const char *path) {
    return syscall1(SYS_REMOVE, (int)path);
}

int create(const char *path) {
    return syscall1(SYS_CREATE, (int)path);
}

int copy(const char *src, const char *dst) {
    return syscall2(SYS_COPY, (int)src, (int)dst);
}

int rename(const char *old_path, const char *new_path) {
    return syscall2(SYS_RENAME, (int)old_path, (int)new_path);
}

void setcolor(int fg, int bg) {
    syscall2(SYS_SETCOLOR, fg, bg);
}

void clear(void) {
    syscall0(SYS_CLEAR);
}

void beep(int freq, int duration) {
    syscall2(SYS_BEEP, freq, duration);
}

void reboot(void) {
    syscall0(SYS_REBOOT);
}

int getticks(void) {
    return syscall0(SYS_GETTICKS);
}

int getuptime(void) {
    return syscall0(SYS_GETUPTIME);
}

void getmem(unsigned int *total, unsigned int *used) {
    syscall2(SYS_GETMEM, (int)total, (int)used);
}

void diskinfo(unsigned int *total, unsigned int *free_space, unsigned int *cluster_size) {
    syscall3(SYS_DISKINFO, (int)total, (int)free_space, (int)cluster_size);
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strcpy(char *dest, const char *src) {
    char *orig = dest;
    while (*src) *dest++ = *src++;
    *dest = '\0';
    return orig;
}

void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    while (num--) *p++ = (unsigned char)value;
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

int atoi(const char *str) {
    int result = 0;
    int sign = 1;
    while (*str == ' ') str++;
    if (*str == '-') { sign = -1; str++; }
    else if (*str == '+') str++;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return sign * result;
}

void itoa(int value, char *str, int base) {
    char *ptr = str;
    char *ptr1 = str;
    char tmp;
    int tmp_val;
    
    if (base < 2 || base > 36) { *str = '\0'; return; }
    if (value < 0 && base == 10) { *ptr++ = '-'; ptr1++; value = -value; }
    if (value == 0) { *ptr++ = '0'; *ptr = '\0'; return; }
    
    while (value) {
        tmp_val = value % base;
        *ptr++ = (tmp_val < 10) ? (tmp_val + '0') : (tmp_val - 10 + 'a');
        value /= base;
    }
    *ptr-- = '\0';
    
    while (ptr1 < ptr) {
        tmp = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp;
    }
}

#ifndef HAZLE_H
#define HAZLE_H

typedef unsigned int size_t;

#define NULL ((void *)0)

void exit(int code);
int putchar(int c);
int puts(const char *s);
int getchar(void);
int open(const char *path);
int close(int fd);
int read(int fd, void *buf, size_t size);
int write(int fd, const void *buf, size_t size);
int chdir(const char *path);
char *getcwd(char *buf, size_t size);
int mkdir(const char *path);
int remove(const char *path);
int create(const char *path);
int copy(const char *src, const char *dst);
int rename(const char *old_path, const char *new_path);

void setcolor(int fg, int bg);
void clear(void);
void beep(int freq, int duration);
void reboot(void);

int getticks(void);
int getuptime(void);
void getmem(unsigned int *total, unsigned int *used);
void diskinfo(unsigned int *total, unsigned int *free_space, unsigned int *cluster_size);

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
void *memset(void *ptr, int value, size_t num);
void *memcpy(void *dest, const void *src, size_t n);
int atoi(const char *str);
void itoa(int value, char *str, int base);

#endif

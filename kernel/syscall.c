#include "syscall.h"
#include "exec.h"
#include "display.h"
#include "keyboard.h"
#include "timer.h"
#include "rtc.h"
#include "speaker.h"
#include "fat32.h"
#include "kernel.h"
#include "string.h"
#include "io.h"

static int last_exit_code = 0;

static int sys_exit(int code) {
    last_exit_code = code;
    exec_exit(code);
    return code;
}

static int sys_write(int fd, const char *buf, int size) {
    if (fd == 1 || fd == 2) {
        for (int i = 0; i < size; i++) {
            display_putchar(buf[i]);
        }
        return size;
    }
    return fat32_write(fd, buf, size);
}

static int sys_read(int fd, char *buf, int size) {
    if (fd == 0) {
        for (int i = 0; i < size; i++) {
            buf[i] = keyboard_getchar();
            if (buf[i] == '\n') {
                return i + 1;
            }
        }
        return size;
    }
    return fat32_read(fd, buf, size);
}

static int sys_open(const char *path) {
    return fat32_open(path);
}

static int sys_close(int fd) {
    fat32_close(fd);
    return 0;
}

static int sys_getchar(void) {
    return keyboard_getchar();
}

static int sys_putchar(int c) {
    display_putchar((char)c);
    return c;
}

static int sys_puts(const char *s) {
    display_writeln(s);
    return 0;
}

static int sys_readdir(uint32_t cluster, void *entries, int max) {
    int count = 0;
    if (cluster == 0) {
        cluster = fat32_get_current_dir();
    }
    fat32_read_dir(cluster, entries, &count, max);
    return count;
}

static int sys_chdir(const char *path) {
    return fat32_change_dir(path) ? 0 : -1;
}

static int sys_getcwd(char *buf, int size) {
    const char *cwd = fat32_get_current_path();
    strncpy(buf, cwd, size - 1);
    buf[size - 1] = '\0';
    return 0;
}

static int sys_mkdir(const char *path) {
    return fat32_create_dir(path) ? 0 : -1;
}

static int sys_remove(const char *path) {
    return fat32_delete(path) ? 0 : -1;
}

static int sys_gettime(void *time_ptr) {
    rtc_read_time((rtc_time_t *)time_ptr);
    return 0;
}

static int sys_getticks(void) {
    return timer_get_ticks();
}

static int sys_setcolor(int fg, int bg) {
    display_set_color(fg, bg);
    return 0;
}

static int sys_clear(void) {
    display_clear();
    return 0;
}

static int sys_beep(int freq, int duration) {
    speaker_beep(freq, duration);
    return 0;
}

static int sys_reboot(void) {
    outb(0x64, 0xFE);
    while (1) {
        __asm__ volatile("hlt");
    }
    return 0;
}

static int sys_getmem(uint32_t *total, uint32_t *used) {
    if (total) *total = system_info.total_memory;
    if (used) *used = system_info.used_memory;
    return 0;
}

static int sys_getuptime(void) {
    return system_info.uptime_seconds;
}

static int sys_create(const char *path) {
    return fat32_create_file(path) ? 0 : -1;
}

static int sys_copy(const char *src, const char *dst) {
    return fat32_copy(src, dst) ? 0 : -1;
}

static int sys_rename(const char *old_path, const char *new_path) {
    return fat32_rename(old_path, new_path) ? 0 : -1;
}

static int sys_diskinfo(uint32_t *total, uint32_t *free_space, uint32_t *cluster_size) {
    if (total) *total = fat32_get_total_space();
    if (free_space) *free_space = fat32_get_free_space();
    if (cluster_size) *cluster_size = fat32_get_cluster_size();
    return 0;
}

int syscall_dispatch(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    switch (num) {
        case SYS_EXIT:      return sys_exit((int)arg1);
        case SYS_WRITE:     return sys_write((int)arg1, (const char *)arg2, (int)arg3);
        case SYS_READ:      return sys_read((int)arg1, (char *)arg2, (int)arg3);
        case SYS_OPEN:      return sys_open((const char *)arg1);
        case SYS_CLOSE:     return sys_close((int)arg1);
        case SYS_GETCHAR:   return sys_getchar();
        case SYS_PUTCHAR:   return sys_putchar((int)arg1);
        case SYS_PUTS:      return sys_puts((const char *)arg1);
        case SYS_READDIR:   return sys_readdir(arg1, (void *)arg2, (int)arg3);
        case SYS_CHDIR:     return sys_chdir((const char *)arg1);
        case SYS_GETCWD:    return sys_getcwd((char *)arg1, (int)arg2);
        case SYS_MKDIR:     return sys_mkdir((const char *)arg1);
        case SYS_REMOVE:    return sys_remove((const char *)arg1);
        case SYS_GETTIME:   return sys_gettime((void *)arg1);
        case SYS_GETTICKS:  return sys_getticks();
        case SYS_SETCOLOR:  return sys_setcolor((int)arg1, (int)arg2);
        case SYS_CLEAR:     return sys_clear();
        case SYS_BEEP:      return sys_beep((int)arg1, (int)arg2);
        case SYS_REBOOT:    return sys_reboot();
        case SYS_GETMEM:    return sys_getmem((uint32_t *)arg1, (uint32_t *)arg2);
        case SYS_GETUPTIME: return sys_getuptime();
        case SYS_CREATE:    return sys_create((const char *)arg1);
        case SYS_COPY:      return sys_copy((const char *)arg1, (const char *)arg2);
        case SYS_RENAME:    return sys_rename((const char *)arg1, (const char *)arg2);
        case SYS_DISKINFO:  return sys_diskinfo((uint32_t *)arg1, (uint32_t *)arg2, (uint32_t *)arg3);
        default:            return -1;
    }
}

void syscall_init(void) {
    last_exit_code = 0;
}

#include "exec.h"
#include "elf.h"
#include "heap.h"
#include "fat32.h"
#include "string.h"

#define PROGRAM_STACK_BASE 0x00600000
#define PROGRAM_STACK_SIZE 0x10000

static int last_status = 0;

void exec_init(void) {
    heap_init();
    last_status = 0;
}

static void setup_stack_and_call(uint32_t entry, int argc, char **argv) {
    uint32_t *stack = (uint32_t *)(PROGRAM_STACK_BASE - 4);
    
    *stack-- = 0;
    
    for (int i = argc - 1; i >= 0; i--) {
        *stack-- = (uint32_t)argv[i];
    }
    
    *stack-- = (uint32_t)argc;
    
    uint32_t stack_ptr = (uint32_t)(stack + 1);
    
    __asm__ volatile(
        "mov %0, %%esp\n"
        "jmp *%1\n"
        :
        : "r"(stack_ptr), "r"(entry)
        : "memory"
    );
}

int exec_run(const char *path, int argc, char **argv) {
    if (!fat32_is_mounted()) {
        return EXIT_NOT_FOUND;
    }
    
    int fd = fat32_open(path);
    if (fd < 0) {
        return EXIT_NOT_FOUND;
    }
    
    uint32_t size = fat32_size(fd);
    if (size == 0) {
        fat32_close(fd);
        return EXIT_NOT_EXEC;
    }
    
    heap_reset();
    
    uint8_t *file_buffer = heap_alloc(size);
    if (!file_buffer) {
        fat32_close(fd);
        return EXIT_FAILURE;
    }
    
    int bytes_read = fat32_read(fd, file_buffer, size);
    fat32_close(fd);
    
    if ((uint32_t)bytes_read != size) {
        return EXIT_FAILURE;
    }
    
    if (!elf_validate(file_buffer, size)) {
        return EXIT_NOT_EXEC;
    }
    
    uint32_t entry = elf_load(file_buffer, size);
    if (entry == 0) {
        return EXIT_NOT_EXEC;
    }
    
    typedef int (*entry_func_t)(int, char**);
    entry_func_t program = (entry_func_t)entry;
    
    int result = program(argc, argv);
    
    last_status = result;
    return result;
}

void exec_exit(int code) {
    last_status = code;
}

int exec_get_last_status(void) {
    return last_status;
}

bool exec_is_running(void) {
    return false;
}

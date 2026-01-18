#ifndef ELF_H
#define ELF_H

#include "types.h"

#define ELF_MAGIC      0x464C457F

#define ELF_CLASS_32   1
#define ELF_DATA_LE    1
#define ELF_VERSION    1

#define ET_EXEC        2
#define EM_386         3

#define PT_NULL        0
#define PT_LOAD        1

#define PF_X           1
#define PF_W           2
#define PF_R           4

typedef struct {
    uint32_t magic;
    uint8_t  elf_class;
    uint8_t  endian;
    uint8_t  version;
    uint8_t  osabi;
    uint8_t  pad[8];
    uint16_t type;
    uint16_t machine;
    uint32_t version2;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed)) elf32_header_t;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} __attribute__((packed)) elf32_phdr_t;

bool elf_validate(const void *data, uint32_t size);
uint32_t elf_load(const void *data, uint32_t size);
uint32_t elf_get_entry(const void *data);

#endif

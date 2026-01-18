#include "elf.h"
#include "string.h"

bool elf_validate(const void *data, uint32_t size) {
    if (size < sizeof(elf32_header_t)) {
        return false;
    }
    
    const elf32_header_t *hdr = (const elf32_header_t *)data;
    
    if (hdr->magic != ELF_MAGIC) {
        return false;
    }
    
    if (hdr->elf_class != ELF_CLASS_32) {
        return false;
    }
    
    if (hdr->endian != ELF_DATA_LE) {
        return false;
    }
    
    if (hdr->type != ET_EXEC) {
        return false;
    }
    
    if (hdr->machine != EM_386) {
        return false;
    }
    
    return true;
}

uint32_t elf_get_entry(const void *data) {
    const elf32_header_t *hdr = (const elf32_header_t *)data;
    return hdr->entry;
}

uint32_t elf_load(const void *data, uint32_t size) {
    if (!elf_validate(data, size)) {
        return 0;
    }
    
    const elf32_header_t *hdr = (const elf32_header_t *)data;
    const uint8_t *file = (const uint8_t *)data;
    
    for (uint16_t i = 0; i < hdr->phnum; i++) {
        uint32_t phdr_offset = hdr->phoff + (i * hdr->phentsize);
        
        if (phdr_offset + sizeof(elf32_phdr_t) > size) {
            return 0;
        }
        
        const elf32_phdr_t *phdr = (const elf32_phdr_t *)(file + phdr_offset);
        
        if (phdr->type != PT_LOAD) {
            continue;
        }
        
        if (phdr->offset + phdr->filesz > size) {
            return 0;
        }
        
        uint8_t *dest = (uint8_t *)phdr->vaddr;
        const uint8_t *src = file + phdr->offset;
        
        memcpy(dest, src, phdr->filesz);
        
        if (phdr->memsz > phdr->filesz) {
            memset(dest + phdr->filesz, 0, phdr->memsz - phdr->filesz);
        }
    }
    
    return hdr->entry;
}

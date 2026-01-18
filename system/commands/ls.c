#include "../../userlib/hazle.h"
#include "../../userlib/syscall.h"

typedef struct {
    char name[256];
    unsigned int size;
    unsigned char attributes;
    unsigned int first_cluster;
    unsigned short creation_date;
    unsigned short creation_time;
    unsigned short modification_date;
    unsigned short modification_time;
} __attribute__((packed)) file_info_t;

int main(int argc, char **argv) {
    int show_all = 0;
    
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && (argv[i][1] == 'a' || argv[i][1] == 'h')) {
            show_all = 1;
        }
    }
    
    file_info_t entries[16];
    memset(entries, 0, sizeof(entries));
    
    int count = syscall3(SYS_READDIR, 0, (int)entries, 16);
    
    if (count < 0) {
        puts("failed to read directory");
        return 1;
    }
    
    if (count == 0) {
        puts("(empty directory)");
        return 0;
    }
    
    char buf[32];
    
    for (int i = 0; i < count; i++) {
        int is_dir = entries[i].attributes & 0x10;
        int is_hidden = entries[i].name[0] == '.';
        int is_system = (entries[i].name[0] == 'u' || entries[i].name[0] == 'U') &&
                        (entries[i].name[1] == 's' || entries[i].name[1] == 'S') &&
                        (entries[i].name[2] == 'r' || entries[i].name[2] == 'R');
        
        if (!show_all && (is_hidden || is_system)) {
            continue;
        }
        
        if (is_dir) {
            setcolor(11, 0);
        } else {
            setcolor(7, 0);
        }
        
        if (entries[i].name[0] != '\0') {
            char *s = entries[i].name;
            while (*s) putchar(*s++);
        } else {
            puts("(no name)");
            continue;
        }
        
        if (is_dir) {
            putchar('/');
        } else {
            putchar(' '); putchar(' ');
            itoa(entries[i].size, buf, 10);
            char *s = buf;
            while (*s) putchar(*s++);
            putchar('B');
        }
        putchar('\n');
    }
    
    setcolor(7, 0);
    return 0;
}

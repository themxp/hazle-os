# FAT32 Filesystem Implementation for Hazle OS

This document describes the FAT32 filesystem implementation for Hazle OS.

## Architecture

```
┌─────────────────────────────────────────────────┐
│                 Shell Commands                   │
│  ls, cd, cat, mkdir, touch, rm, cp, mv, disk    │
└─────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────┐
│              FAT32 Filesystem Layer              │
│  mount.c | file.c | directory.c | fat.c | bpb.c │
└─────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────┐
│             Block Device Abstraction             │
│                    block.c                       │
└─────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────┐
│               ATA/IDE Driver                     │
│                    ata.c                         │
└─────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────┐
│               Hardware (IDE Disk)                │
└─────────────────────────────────────────────────┘
```

## File Structure

```
drivers/
├── ata.c           - ATA/IDE PIO driver
├── block.c         - Block device abstraction
└── fat32/
    ├── bpb.c       - BPB parsing and cluster/LBA conversion
    ├── fat.c       - FAT table operations
    ├── directory.c - Directory parsing and LFN support
    ├── file.c      - File operations
    └── mount.c     - Mount/unmount and navigation

include/
├── ata.h           - ATA driver definitions
├── block.h         - Block device API
└── fat32.h         - FAT32 structures and API

shell/commands/
├── cmd_ls.c        - List directory
├── cmd_cd.c        - Change directory
├── cmd_cat.c       - Display file contents
├── cmd_mkdir.c     - Create directory
├── cmd_touch.c     - Create file
├── cmd_rm.c        - Delete file/directory
├── cmd_cp.c        - Copy file
├── cmd_mv.c        - Move/rename file
└── cmd_disk.c      - Disk information
```

## Shell Commands

| Command | Usage | Description |
|---------|-------|-------------|
| ls | `ls [path]` | List directory contents |
| cd | `cd <path>` | Change current directory |
| pwd | `pwd` | Print working directory |
| cat | `cat <file>` | Display file contents |
| mkdir | `mkdir <name>` | Create new directory |
| touch | `touch <file>` | Create empty file |
| rm | `rm <file>` | Delete file or directory |
| cp | `cp <src> <dst>` | Copy file |
| mv | `mv <src> <dst>` | Move or rename file |
| disk | `disk` | Display disk information |

## FAT32 On-Disk Structures

### BIOS Parameter Block (BPB)
- Located at sector 0 (boot sector)
- Contains filesystem metadata: bytes per sector, sectors per cluster, FAT count, etc.

### File Allocation Table (FAT)
- Cluster chain management (28-bit entries)
- Multiple copies for redundancy
- EOC marker: 0x0FFFFFF8 - 0x0FFFFFFF

### Directory Entries
- 32 bytes per entry
- Short name (8.3 format)
- Long File Name (LFN) support with 0x0F attribute

## Usage

### Building with Disk Support
```bash
make clean && make
make disk        # Create 64MB FAT32 disk image
make run-disk    # Run QEMU with disk attached
```

### Testing in QEMU
1. Boot Hazle OS
2. Type `disk` to see disk information
3. Type `ls` to list files
4. Type `cd <directory>` to navigate

### Creating Test Files on Host
```bash
mkdir -p mnt
sudo mount disk.img mnt
echo "Hello World" > mnt/test.txt
sudo umount mnt
```

## Limitations

- FAT32 only (no FAT12/FAT16)
- PIO mode (no DMA)
- Static buffers (no heap allocation)
- Maximum 8 open files simultaneously
- 4GB file size limit (FAT32 specification)

## References

- [Microsoft FAT32 Specification](https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/fatgen103.doc)
- [OSDev Wiki - FAT](https://wiki.osdev.org/FAT)
- [OSDev Wiki - ATA PIO](https://wiki.osdev.org/ATA_PIO_Mode)

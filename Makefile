ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld
ASMFLAGS = -f elf32

BUILD_DIR = build
ISO_DIR = isodir
BOOT_DIR = $(ISO_DIR)/boot/grub

KERNEL_SOURCES = $(wildcard kernel/*.c) $(wildcard drivers/*.c) $(wildcard drivers/fat32/*.c) $(wildcard shell/*.c) $(wildcard shell/commands/*.c) $(wildcard lib/*.c)
ASM_SOURCES = $(wildcard boot/*.asm) $(wildcard kernel/*.asm)

KERNEL_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES))
ASM_OBJECTS = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

ALL_OBJECTS = $(ASM_OBJECTS) $(KERNEL_OBJECTS)

KERNEL_BIN = $(BUILD_DIR)/hazle.bin
ISO_IMAGE = hazle-os.iso
DISK_IMAGE = disk.img

.PHONY: all clean run run-fullhd debug iso disk run-disk

all: $(ISO_IMAGE)

$(BUILD_DIR)/%.o: %.asm
	@-mkdir -p $(dir $@) 2>/dev/null || true
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c
	@-mkdir -p $(dir $@) 2>/dev/null || true
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(ALL_OBJECTS)
	@-mkdir -p $(BUILD_DIR) 2>/dev/null || true
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJECTS)

$(ISO_IMAGE): $(KERNEL_BIN)
	@-mkdir -p $(BOOT_DIR) 2>/dev/null || true
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/hazle.bin
	echo 'set timeout=0' > $(BOOT_DIR)/grub.cfg
	echo 'set default=0' >> $(BOOT_DIR)/grub.cfg
	echo 'set gfxmode=1920x1080x32' >> $(BOOT_DIR)/grub.cfg
	echo 'set gfxpayload=keep' >> $(BOOT_DIR)/grub.cfg
	echo 'insmod vbe' >> $(BOOT_DIR)/grub.cfg
	echo 'insmod vga' >> $(BOOT_DIR)/grub.cfg
	echo 'terminal_output gfxterm' >> $(BOOT_DIR)/grub.cfg
	echo 'menuentry "Hazle OS - Graphics Mode" {' >> $(BOOT_DIR)/grub.cfg
	echo '    multiboot /boot/hazle.bin' >> $(BOOT_DIR)/grub.cfg
	echo '}' >> $(BOOT_DIR)/grub.cfg
	grub2-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)

$(DISK_IMAGE):
	dd if=/dev/zero of=$(DISK_IMAGE) bs=1M count=64
	mkfs.fat -F 32 $(DISK_IMAGE)

disk: $(DISK_IMAGE)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_IMAGE) $(DISK_IMAGE)

run: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 256M -vga std

run-disk: $(ISO_IMAGE) $(DISK_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 256M -vga std -boot d -drive file=$(DISK_IMAGE),format=raw,if=ide

run-fullhd: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 256M -vga std -full-screen

debug: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 256M -vga std -s -S



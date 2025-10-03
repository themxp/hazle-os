ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -O2 -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld
ASMFLAGS = -f elf32

BUILD_DIR = build
ISO_DIR = isodir
BOOT_DIR = $(ISO_DIR)/boot/grub

KERNEL_SOURCES = $(wildcard kernel/*.c) $(wildcard drivers/*.c) $(wildcard shell/*.c) $(wildcard lib/*.c)
ASM_SOURCES = $(wildcard boot/*.asm) $(wildcard kernel/*.asm)

KERNEL_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES))
ASM_OBJECTS = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

ALL_OBJECTS = $(ASM_OBJECTS) $(KERNEL_OBJECTS)

KERNEL_BIN = $(BUILD_DIR)/hazle.bin
ISO_IMAGE = hazle-os.iso

.PHONY: all clean run iso

all: $(ISO_IMAGE)

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(ALL_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJECTS)

$(ISO_IMAGE): $(KERNEL_BIN)
	@mkdir -p $(BOOT_DIR)
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/hazle.bin
	echo 'set timeout=0' > $(BOOT_DIR)/grub.cfg
	echo 'set default=0' >> $(BOOT_DIR)/grub.cfg
	echo 'menuentry "Hazle OS" {' >> $(BOOT_DIR)/grub.cfg
	echo '    multiboot /boot/hazle.bin' >> $(BOOT_DIR)/grub.cfg
	echo '}' >> $(BOOT_DIR)/grub.cfg
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_IMAGE)

run: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 128M

debug: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -m 128M -s -S


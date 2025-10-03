# Architecture

How Hazle OS works internally.

## Overview

```
Hardware → Drivers → Kernel → Shell → User
```

Hazle OS is a simple monolithic kernel. Everything runs in kernel mode (ring 0). There's no separation between kernel and user space.

## Boot Process

1. **BIOS** initializes hardware and loads GRUB
2. **GRUB** reads multiboot header and loads kernel to 1MB
3. **boot.asm** sets up 16KB stack and calls `kernel_main()`
4. **kernel_main()** initializes:
   - VGA display driver
   - GDT (memory segmentation)
   - IDT (interrupt table)
   - PIC (interrupt controller)
   - Timer (100 Hz)
   - Keyboard driver
5. **Shell** starts and waits for input

## Memory Layout

```
0x00000000          BIOS and low memory
0x000B8000          VGA text buffer
0x00100000 (1MB)    Kernel code starts here
                    ├── .text (code)
                    ├── .data (initialized data)
                    └── .bss (uninitialized)
                    Stack (16KB, grows down)
```

No heap. No dynamic memory allocation. Everything is static.

## Components

### GDT (Global Descriptor Table)

Defines memory segments for protected mode:
- Segment 0: Null (required)
- Segment 1: Code segment (full 4GB)
- Segment 2: Data segment (full 4GB)

We use flat memory model (all segments cover full address space).

### IDT (Interrupt Descriptor Table)

Maps hardware interrupts to functions:
- INT 32 (IRQ 0): Timer - fires 100 times/second
- INT 33 (IRQ 1): Keyboard - fires on key press

### PIC (Programmable Interrupt Controller)

We remap IRQs to avoid conflict with CPU exceptions:
- IRQ 0-7 → INT 32-39
- IRQ 8-15 → INT 40-47

Only IRQ 0 (timer) and IRQ 1 (keyboard) are enabled.

### VGA Driver

Text mode display (80x25 characters):
- Memory-mapped at 0xB8000
- Each character = 2 bytes: [ASCII][Color]
- Supports 16 colors (4-bit foreground + 4-bit background)
- Auto-scrolls when full

### Keyboard Driver

PS/2 keyboard only:
- Reads scancode from port 0x60
- Converts to ASCII using lookup table
- Handles shift key
- Stores in 128-byte ring buffer

### Timer Driver

PIT (Programmable Interval Timer):
- Configured to 100 Hz (10ms interval)
- Each tick increments counter
- Used for uptime calculation

### Shell

Simple command parser:
1. Read input into buffer
2. On Enter: split by spaces
3. First word = command name
4. Look up in command table
5. Call handler function

## Interrupt Flow Example

When you press a key:

1. Keyboard controller sends scancode
2. Triggers IRQ 1 → CPU interrupt 33
3. CPU saves state, jumps to handler
4. Handler reads scancode from port 0x60
5. Converts to ASCII and stores in buffer
6. Sends EOI (End of Interrupt) to PIC
7. Returns from interrupt

## File Organization

- `boot/` - Entry point, stack setup (Assembly)
- `kernel/` - GDT, IDT, PIC, main loop
- `drivers/` - VGA, keyboard, timer
- `shell/` - Command parser and handlers
- `lib/` - String and memory functions
- `include/` - All headers

## Limitations

This is a learning OS with many limitations:

- **No filesystem**: Can't read/write files
- **No multitasking**: One thing at a time
- **No paging**: Flat memory, no protection
- **No user mode**: Everything runs in ring 0
- **No floating point**: Integer math only
- **No USB**: PS/2 keyboard only
- **No security**: Anyone can do anything

These limitations make the code simpler to understand.

## How to Extend

### Add a Command

Edit `shell/commands.c`:
```c
static void cmd_test(int argc, char** argv) {
    vga_writeln("Test command!");
}

// Add to array:
{"test", "Test command", cmd_test},
```

### Add a Driver

1. Create `drivers/mydriver.c` and `include/mydriver.h`
2. Implement `mydriver_init()`
3. Call from `kernel_main()`
4. For interrupts: register in `idt_init()`

## Learning Path

1. **Start with**: `boot/boot.asm` - how we enter C from assembly
2. **Then**: `kernel/kernel.c` - initialization sequence
3. **Study**: Individual drivers (VGA is simplest)
4. **Understand**: Interrupt handling (IDT, PIC)
5. **Explore**: Shell and commands

The code is the documentation. Read it!

## References

- [OSDev Wiki](https://wiki.osdev.org) - Comprehensive OS dev guide
- [Intel Manual](https://software.intel.com/en-us/articles/intel-sdm) - x86 architecture
- [NASM Manual](https://nasm.us/doc/) - Assembly syntax

---

This is a teaching OS. Simplicity over features. Understanding over complexity.

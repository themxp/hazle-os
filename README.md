# Hazle OS

A simple x86 operating system built from scratch for learning purposes.

## What is this?

Hazle OS is a minimal 32-bit operating system written in Assembly and C. It's designed to help you understand how operating systems work at a low level. This is **not** a production-ready OS - it's a learning tool.

**Educational purposes only. Not suitable for production use.**

## Background

This project is a continuation and improvement of a [previous attempt](https://github.com/mateuphinxx/basic-os) that didn't work out

Sometimes the best way to learn is to start over with what you've learned.

## Features

- Boots with GRUB multiboot
- 32-bit protected mode kernel
- VGA text display with multiple resolutions (80x25, 80x50, 40x25)
- 16-color support
- PS/2 keyboard input
- Timer interrupt (PIT)
- Interactive shell with 11 commands
- Clean, readable code

## Getting Started

Want to build and run? See **[QUICKSTART.md](QUICKSTART.md)** for complete setup instructions.

**TL;DR:**
```bash
# Native build (Linux/Mac/WSL)
make && make run

# Docker build (Windows/Mac/Linux)
docker-compose up build

# Run: See QUICKSTART.md for your platform
```

**Windows users:** Docker has display limitations. See **[DOCKER-WINDOWS.md](DOCKER-WINDOWS.md)** for solutions.

## Available Commands

| Command | Description |
|---------|-------------|
| `help` | List all commands |
| `clear` | Clear screen |
| `echo <text>` | Print text |
| `info` | System info |
| `time` | System ticks |
| `uptime` | Uptime |
| `mem` | Memory usage |
| `calc <a> <op> <b>` | Calculator (+, -, *, /) |
| `color <fg> <bg>` | Change colors (0-15) |
| `resolution <mode>` | Change screen resolution |
| `reboot` | Restart |

## Project Structure

```
hazle-os/
├── boot/       - Bootloader (Assembly)
├── kernel/     - Core kernel (GDT, IDT, main)
├── drivers/    - Hardware drivers (VGA, keyboard, timer)
├── shell/      - Command-line interface
├── lib/        - Utility functions (string, memory)
└── include/    - Headers
```

## How It Works

1. BIOS loads GRUB
2. GRUB loads our kernel at 1MB
3. Bootloader sets up stack and jumps to C
4. Kernel initializes hardware (GDT, IDT, drivers)
5. Shell starts and waits for commands

Want details? Read **[ARCHITECTURE.md](ARCHITECTURE.md)**.

## Adding Your Own Command

Edit `shell/commands.c`:

```c
static void cmd_hello(int argc, char** argv) {
    vga_writeln("Hello from my command!");
}

// Register in commands array:
{"hello", "My command", cmd_hello},
```

Rebuild: `make clean && make run`

## What This OS Can Do

- Display text on screen
- Read keyboard input
- Execute commands
- Track time
- Do basic calculations
- Change display colors
- Reboot

## What This OS Cannot Do

This is a **simple educational OS**. It lacks:

- File system (can't save files)
- Multitasking (one task at a time)
- Virtual memory / paging
- Memory protection
- User mode / kernel mode separation
- USB support (PS/2 only)
- Networking
- Security features

**Do NOT use this in production. It's for learning only.**

## Learning Resources

- **[QUICKSTART.md](QUICKSTART.md)** - How to build and run
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - How it works internally
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - How to contribute
- [OSDev Wiki](https://wiki.osdev.org) - OS development resources
- [Intel x86 Manual](https://software.intel.com/en-us/articles/intel-sdm) - CPU architecture

## Why Learn from Hazle OS?

- **Simple**: ~2000 lines of code, easy to understand
- **Clean**: Well-organized, modular structure
- **Educational**: Designed specifically for learning
- **Complete**: Boot to shell, everything from scratch

## Extending This OS

Want to add more? Try:
- New commands (date, random number, etc)
- RTC driver (real-time clock)
- Simple memory allocator
- More keyboard features (arrow keys, function keys)
- PC speaker driver (beep sounds)

See [OSDev Wiki](https://wiki.osdev.org) for tutorials.

## License

MIT License - free to use for learning.

## Disclaimer

This operating system is for **educational purposes only**. It is not secure, not stable, and not suitable for any production use. Use at your own risk.

---

**Have fun learning how operating systems work!**

# Quick Start Guide

How to build and run Hazle OS.

## Prerequisites

You need build tools installed. Choose your method:

### Option 1: Docker (Easiest, works on Windows/Mac/Linux)

Just install [Docker Desktop](https://www.docker.com/products/docker-desktop). That's it.

### Option 2: Native Build

**Ubuntu/Debian:**
```bash
sudo apt install nasm gcc make grub-pc-bin xorriso qemu-system-x86
```

**Arch Linux:**
```bash
sudo pacman -S nasm gcc make grub xorriso qemu
```

**macOS:**
```bash
brew install nasm i686-elf-gcc grub xorriso qemu
```

## Building the OS

### Using Docker (Recommended)

```bash
docker-compose up build
```

### Using Native Tools

```bash
make
```

Both create `hazle-os.iso` file.

## Running the OS

### Using Docker

**Important for Windows users:** Docker can't display VGA output directly. See [DOCKER-WINDOWS.md](DOCKER-WINDOWS.md) for solutions (VNC, WSL2, or native QEMU).

**For Linux/Mac:**
```bash
docker-compose up run
# Then connect to localhost:5900 with VNC viewer
```

### Using Native QEMU

```bash
make run
```

To exit QEMU: Press `Ctrl+Alt+G` to release mouse, then close window.

### Using VirtualBox

1. Create new VM (Linux, Other 32-bit)
2. Memory: 128 MB minimum
3. Add `hazle-os.iso` as optical drive
4. Boot

### Using VMware

1. Create new VM (Other Linux 32-bit)
2. Attach `hazle-os.iso` to CD/DVD
3. Boot

## First Commands to Try

```bash
help              # Show all commands
info              # System information
calc 10 + 20      # Calculator
echo Hello OS     # Print text
color 10 0        # Green text
mem               # Memory info
uptime            # System uptime
clear             # Clear screen
reboot            # Restart
```

## Troubleshooting

### Build fails: "command not found"

You're missing build tools. Install them (see Prerequisites above).

### Build fails: GCC errors

Make sure you have 32-bit compilation support:
```bash
# Ubuntu/Debian
sudo apt install gcc-multilib
```

### Black screen when running

Try rebuilding:
```bash
make clean
make
make run
```

### Docker: "permission denied"

On Linux, add your user to docker group:
```bash
sudo usermod -aG docker $USER
# Log out and back in
```

### QEMU closes immediately

The ISO might not be bootable. Check build output for errors.

## Development Workflow

1. Edit code
2. Run `make clean && make`
3. Run `make run` to test
4. Repeat

For Docker:
```bash
docker-compose up build
docker-compose up run
```

## Cleaning Build Files

```bash
make clean
```

Or with Docker:
```bash
docker-compose up clean
```

## Debug Mode

```bash
make debug
```

In another terminal:
```bash
gdb
(gdb) target remote localhost:1234
(gdb) continue
```

## What's Next?

- Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand how it works
- Modify code and experiment
- Add your own commands
- Try adding new drivers

---



# Docker on Windows

Docker on Windows has limitations with QEMU display. Here are your options:

## Problem

QEMU in Docker can't show graphical output (VGA) directly to Windows terminal. You'll see "Attaching to run-1" but no OS display.

## Solutions

### Option 1: Use VNC (Recommended for Docker)

1. **Install VNC Viewer** on Windows:
   - Download [TigerVNC](https://sourceforge.net/projects/tigervnc/) or [RealVNC](https://www.realvnc.com/en/connect/download/viewer/)

2. **Update docker-compose.yml** for VNC:

```yaml
run:
  image: hazle-os-builder
  volumes:
    - .:/workspace
  working_dir: /workspace
  command: qemu-system-i386 -cdrom hazle-os.iso -m 128M -vnc :0
  ports:
    - "5900:5900"
```

3. **Run the OS**:
```powershell
docker-compose up run
```

4. **Connect with VNC**:
   - Open VNC Viewer
   - Connect to: `localhost:5900`
   - You'll see Hazle OS!

5. **Use the keyboard** in VNC window normally

### Option 2: WSL2 (Best Experience)

1. **Install WSL2** with Ubuntu:
```powershell
wsl --install
```

2. **Inside WSL**, install tools:
```bash
sudo apt update
sudo apt install nasm gcc make grub-pc-bin xorriso qemu-system-x86
```

3. **Build and run**:
```bash
make
make run
```

4. **Keyboard works perfectly** in WSL!

### Option 3: Native QEMU on Windows

1. **Download QEMU** for Windows from [qemu.org](https://www.qemu.org/download/#windows)

2. **Build ISO with Docker**:
```powershell
docker-compose up build
```

3. **Run with native QEMU**:
```powershell
"qemu-system-i386.exe" -cdrom hazle-os.iso -m 128M
```

4. **Keyboard works** in QEMU window!

### Option 4: Test in VirtualBox/VMware

1. Build ISO:
```powershell
docker-compose up build
```

2. Create VM in VirtualBox/VMware

3. Attach `hazle-os.iso`

4. Boot and use normally

## Recommended Workflow for Windows

**For Development:**
1. Use Docker to build: `docker-compose up build`
2. Use WSL2 or native QEMU to run and test

**For Quick Testing:**
- Install QEMU on Windows and run directly

---

Docker is great for building cross-platform, but for running/testing on Windows, WSL2 or native QEMU gives better experience.


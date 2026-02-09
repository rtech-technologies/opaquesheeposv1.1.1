# OpaqueSheep OS (x86_64)

OpaqueSheep OS is a professional shell-oriented OS skeleton that keeps kernel code **high level** and clean. You write your behavior in `kmain()` using simple, Python-like helpers while the heavy lifting lives in the libraries and startup system.

For full API and APL (application) coding documentation, read the in-depth manual: **`MANUAL.man`**.

## Basics

- **Main entry:** `kernel/unice64/main.c` (Template Kernel)
- **Primary header:** `kernel/include/sys.h`
- **Boot Info:** `kernel/include/bootinfo.h` (UEFI GOP/Framebuffer data)
- **Libraries:** `kernel/libs/` (one file per library)
- **Startup system:** `kernel/stup/` (`kstup()` runs once)

## Quick Start (Automated)

### 1. Setup Environment (One-time)
Install all necessary build and emulation dependencies (Debian/Ubuntu):
```bash
make setup
```

### 2. Build OS
Compile the kernel, bootloader, and generate the bootable FAT32 image:
```bash
make
```
*Note: The Makefile automatically detects if `x86_64-elf-gcc` is present and falls back to host `gcc` if needed.*

### 3. Run OS
Launch OpaqueSheep OS in QEMU:
```bash
make run
```

## Advanced Usage

### Makefile Targets
- `make all`: (Default) Builds everything and creates `build/fat.img`.
- `make fat_img`: Specifically triggers the bootable image creation.
- `make uefi`: Builds the UEFI bootloader only.
- `make clean`: Removes all build artifacts.

### Repository Structure
- `boot/`: UEFI bootloader source.
- `kernel/include/`: Global kernel headers.
- `kernel/libs/`: Modular kernel libraries and services.
- `kernel/unice64/`: Kernel entry point and main loop.
- `build/`: Output directory for binaries and images (git-ignored).

## Where to Learn More

- **Detailed APL coding docs:** `MANUAL.man`
- **Vendor metadata:** `vendor.cfg`

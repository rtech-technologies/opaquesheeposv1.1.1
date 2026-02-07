# OpaqueSheep OS (x86_64)

OpaqueSheep OS is a professional shell-oriented OS skeleton that keeps kernel code **high level** and clean. You write your behavior in `kmain()` using simple, Python-like helpers while the heavy lifting lives in the libraries and startup system.

For full API and APL (application) coding documentation, read the in-depth manual: **`MANUAL.man`**.

## Basics

- **Main entry:** `kernel/unice64/main.c` (Template Kernel)
- **Primary header:** `kernel/include/sys.h`
- **Libraries:** `kernel/libs/` (one file per library)
- **Startup system:** `kernel/stup/` (`kstup()` runs once)

## Quick Start (Automated)

### 1. Setup Environment
Install all necessary build and emulation dependencies:
```bash
make setup
```

### 2. Build OS
Compile the kernel and UEFI bootloader:
```bash
make
```
*Note: The Makefile automatically detects if `x86_64-elf-gcc` is present and falls back to host `gcc` if needed.*

### 3. Create Boot Image
Generate the FAT32 boot image (`build/fat.img`):
```bash
make fat_img
```

### 4. Run OS
Launch OpaqueSheep OS in QEMU:
```bash
make run
```

## Advanced Usage

### Manual Build Steps
- `make all`: Builds the kernel binary.
- `make uefi`: Builds the UEFI bootloader (`BOOTX64.EFI`).
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

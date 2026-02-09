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

## 💻 Bare-Metal Deployment
To run OpaqueSheep OS on real hardware:
1. Format a USB drive as FAT32.
2. Copy the contents of `build/efi/` to the root of the USB drive.
   - The path should be `(USB):/EFI/BOOT/BOOTX64.EFI`
   - The path should be `(USB):/kernel.bin`
3. (Optional) Copy `build/startup.nsh` to the root of the USB drive for automatic booting.
4. Plug the USB into your PC and boot via UEFI mode.

Alternatively, you can write the image directly (caution: this will erase the target drive):
`sudo dd if=build/fat.img of=/dev/sdX bs=1M status=progress` (replace `/dev/sdX` with your USB device).

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

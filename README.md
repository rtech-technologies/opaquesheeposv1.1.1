# OpaqueSheep OS (x86_64)

OpaqueSheep OS is a minimal kernel skeleton designed to feel **high level** when writing your kernel logic. You write your OS behavior in `kmain()` using simple, Python-like functions (`print()`, `input()`, `fwrite()`, etc.), while the low-level startup, boot flow, and libraries live in dedicated folders.

This README is both **documentation** and a **quick-start guide** for building, running, and writing your own kernel logic.

## Philosophy: High-level main, low-level libs

- **`kernel/unice64/main.c`** is your main entry point.
- Keep it **clean and high level** (think scripting-style logic).
- All hard work belongs in **`kernel/libs/`** (each lib = one file).
- Startup initialization runs in **`kernel/stup/`** and is triggered by `kstup()`.

## Project Layout

```
boot/
  uefi/            UEFI bootloader (loads kernel.bin)

kernel/
  include/         Shared headers (sys.h is the main entry)
  libs/            Core functionality (iolib, inputlib, filelib, mem, fat)
  linker/          Linker scripts
  stup/            Startup routines / init apps
  unice64/         Kernel entry + main

vendor.cfg         Vendor config (version, flavor, etc.)
```

## Vendor Configuration (`vendor.cfg`)

The **vendor.cfg** file is a simple place to store OS metadata (version, flavor, name, etc.).
You can use it later for build tags, boot banners, or feature switches.

Example:

```
name = OpaqueSheep OS
version = 1.1.1
flavor = developer
arch = x86_64
boot = uefi
```

## Writing Your Kernel (`kmain`)

Your main file is located here:

```
kernel/unice64/main.c
```

It exposes these high-level functions from `sys.h`:

- `print(const char *message)`
- `input(void)`
- `fwrite(const void *data, size_t size, size_t count)`
- `fappend(const void *data, size_t size, size_t count)`
- `fread(void *data, size_t size, size_t count)`
- `fdelete(const char *path)`
- `dmake(const char *path)`
- `dremove(const char *path)`
- `dmove(const char *from, const char *to)`
- `memcpy_simple(...)`, `memset_simple(...)`
- `panic(const char *message)`

### How boot flow works

- `kstup()` runs once at boot to initialize your OS.
- `kmain()` is your main loop (keep it clear and readable).
- `panic()` halts the CPU (use for fatal errors).

### Example `main.c`

Below is a **high-level style kernel main** that feels like scripting:

```c
#include "sys.h"

void kstup(void) {
    print("[stup] startup init...\n");
    stup_init();
    fat_init();
}

void kmain(void) {
    print("Welcome to OpaqueSheep OS!\n");
    print("Type a command: ");

    const char *cmd = input();
    if (!cmd) {
        panic("input() failed\n");
    }

    print("You typed: ");
    print(cmd);
    print("\n");

    for (;;) {
        // Main loop
    }
}

void panic(const char *message) {
    print("PANIC: ");
    print(message);
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
```

## Example Shell (high-level style)

You can build a simple shell loop directly in `kmain()`:

```c
void kmain(void) {
    print("OpaqueSheep Shell\n");
    print("Type 'help' for commands.\n");

    for (;;) {
        print("> ");
        const char *cmd = input();
        if (!cmd) {
            continue;
        }

        if (cmd[0] == 'h') {
            print("help - list commands\n");
            print("clear - clear screen\n");
        } else if (cmd[0] == 'c') {
            print("[clear not implemented]\n");
        } else {
            print("unknown command\n");
        }
    }
}
```

## Build Instructions

### Requirements

- `gcc` or `clang`
- `nasm`
- `ld`
- `gnu-efi` development files (for UEFI)
- `qemu-system-x86_64`
- `mtools` + `dosfstools` (for FAT image creation)
- OVMF firmware files (`OVMF_CODE.fd` and `OVMF_VARS.fd`)

### Build the kernel

```
make
```

Artifacts:
- `build/kernel.elf`
- `build/kernel.bin`

### Build the UEFI bootloader

```
make uefi
```

Artifacts:
- `build/BOOTX64.EFI`

## Run in QEMU (UEFI)

Create a FAT image and copy the bootloader + kernel:

```
mkdir -p build/efi/EFI/BOOT
cp build/BOOTX64.EFI build/efi/EFI/BOOT/BOOTX64.EFI
cp build/kernel.bin build/efi/kernel.bin

dd if=/dev/zero of=build/fat.img bs=1M count=64
mkfs.fat -F 32 build/fat.img
mmd -i build/fat.img ::/EFI ::/EFI/BOOT
mcopy -i build/fat.img build/efi/EFI/BOOT/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
mcopy -i build/fat.img build/efi/kernel.bin ::/kernel.bin
```

Run in QEMU (update OVMF paths as needed):

```
qemu-system-x86_64 \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=/usr/share/OVMF/OVMF_VARS.fd \
  -drive file=build/fat.img,format=raw
```

## Notes

- The UEFI loader jumps to `kernel.bin` in long mode.
- The FAT, input, and file libraries are **stubs**—extend them as you implement drivers.
- Keep your main logic **short and high-level**, and move real work into `kernel/libs/`.

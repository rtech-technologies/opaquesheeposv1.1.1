# OpaqueSheep OS (x86_64)

OpaqueSheep OS is a professional shell-oriented OS skeleton that keeps kernel code **high level** and clean. You write your behavior in `kmain()` using simple, Python-like helpers while the heavy lifting lives in the libraries and startup system.

For full API and APL (application) coding documentation, read the in-depth manual: **`MANUAL.man`**.

## Basics

- **Main entry:** `kernel/unice64/main.c`
- **Primary header:** `kernel/include/sys.h`
- **Libraries:** `kernel/libs/` (each lib is one file)
- **Startup system:** `kernel/stup/` (`kstup()` runs once)

## Quick Start (Build + Run)

### Install requirements (Debian/Ubuntu)

```
sudo apt update
sudo apt install -y build-essential binutils nasm gnu-efi qemu-system-x86 mtools dosfstools ovmf
```

### Build

```
make
make uefi
```

Artifacts:
- `build/kernel.elf`
- `build/kernel.bin`
- `build/BOOTX64.EFI`

### Run (UEFI + QEMU)

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

```
qemu-system-x86_64 \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd \
  -drive if=pflash,format=raw,file=/usr/share/OVMF/OVMF_VARS.fd \
  -drive file=build/fat.img,format=raw
```

## Where to Learn More

- **Detailed APL coding docs:** `MANUAL.man`
- **Vendor metadata:** `vendor.cfg`

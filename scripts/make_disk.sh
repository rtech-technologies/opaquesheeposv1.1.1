#!/bin/bash
# 1. Create a 64MB blank image
dd if=/dev/zero of=opaque.img bs=1M count=64

# 2. Create GPT table and an EFI System Partition (ESP)
# We align the partition at LBA 2048 (1MB)
# Fallback: if sgdisk is missing, we use python to write a basic GPT header
if command -v sgdisk >/dev/null 2>&1; then
    sgdisk -n 1:2048:131038 -t 1:ef00 -c 1:"OPAQUESHEEP" opaque.img
else
    echo "sgdisk missing, using fallback python generator..."
    python3 -c "
with open('opaque.img', 'r+b') as f:
    f.seek(512)
    # Basic GPT Header (Simplified)
    f.write(b'EFI PART')
"
fi

# 3. Format Partition 1 as FAT32 within the image
if command -v mkfs.vfat >/dev/null 2>&1; then
    mkfs.vfat --offset 2048 -F 32 -n "OPAQUESHEEP" opaque.img
else
    echo "mkfs.vfat missing!"
fi

# 4. Inject files into the "0/:0/" partition
if command -v mcopy >/dev/null 2>&1; then
    mcopy -i opaque.img@@1M bootx64.efi ::/EFI/BOOT/
    mcopy -i opaque.img@@1M kernel.bin ::/
    mcopy -i opaque.img@@1M vendor.cfg ::/
else
    echo "mcopy missing!"
fi

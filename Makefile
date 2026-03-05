ARCH ?= x86_64
CROSS ?= x86_64-elf

# Compiler auto-detection for Debian/Ubuntu environments
CC := $(CROSS)-gcc
ifeq ($(shell command -v $(CC) 2>/dev/null),)
    $(info Cross-compiler $(CC) not found, falling back to host tools...)
    CC      := gcc
    LD      := ld
    OBJCOPY := objcopy
else
    LD      := $(CROSS)-ld
    OBJCOPY := $(CROSS)-objcopy
endif

AS := nasm
MKFS_FAT := $(shell which mkfs.fat 2>/dev/null || which /usr/sbin/mkfs.fat 2>/dev/null || which /sbin/mkfs.fat 2>/dev/null)
MMD      := $(shell which mmd 2>/dev/null || which /usr/bin/mmd 2>/dev/null)
MCOPY    := $(shell which mcopy 2>/dev/null || which /usr/bin/mcopy 2>/dev/null)
EFI_CC ?= gcc
EFI_LD ?= ld
EFI_OBJCOPY ?= objcopy
EFI_INC ?= /usr/include/efi
EFI_LIB_SEARCH := /usr/lib /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/lib/efi
EFI_CRT0 := $(firstword $(foreach dir,$(EFI_LIB_SEARCH),$(wildcard $(dir)/crt0-efi-x86_64.o)))
EFI_LDS  := $(firstword $(foreach dir,$(EFI_LIB_SEARCH),$(wildcard $(dir)/elf_x86_64_efi.lds)))

# Fallbacks if detection fails
ifeq ($(EFI_CRT0),)
    EFI_CRT0 := /usr/lib/crt0-efi-x86_64.o
endif
ifeq ($(EFI_LDS),)
    EFI_LDS := /usr/lib/elf_x86_64_efi.lds
endif

CFLAGS := -std=gnu11 -ffreestanding -fno-stack-protector -fno-stack-check -fno-pic -fno-pie -m64 -march=x86-64 -mno-red-zone -maccumulate-outgoing-args -Wall -Wextra -Ikernel/include -Ikernel/libs -Ikernel/stup
LDFLAGS := -T kernel/linker/kernel.ld -nostdlib -z max-page-size=0x1000 -z common-page-size=0x1000

BUILD_DIR := build

KERNEL_OBJS := \
	$(BUILD_DIR)/entry.o \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/iolib.o \
	$(BUILD_DIR)/inputlib.o \
	$(BUILD_DIR)/filelib.o \
	$(BUILD_DIR)/mem.o \
	$(BUILD_DIR)/fat.o \
	$(BUILD_DIR)/services.o \
	$(BUILD_DIR)/stup_init.o

all: fat_img

uefi: $(BUILD_DIR)/BOOTX64.EFI

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: kernel/unice64/entry.asm | $(BUILD_DIR)
	$(AS) -f elf64 $< -o $@

$(BUILD_DIR)/main.o: kernel/unice64/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/iolib.o: kernel/libs/iolib.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/inputlib.o: kernel/libs/inputlib.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/filelib.o: kernel/libs/filelib.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/mem.o: kernel/libs/mem.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/fat.o: kernel/libs/fat.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/services.o: kernel/libs/services.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/stup_init.o: kernel/stup/init.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -j .text -j .rodata -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		-j .rela -j .reloc --target=efi-app-x86_64 $< $@

limeline: $(BUILD_DIR)/kernel.elf
	mkdir -p $(BUILD_DIR)/limine
	cp $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/limine/
	@echo "TIMEOUT=3" > $(BUILD_DIR)/limine/limine.cfg
	@echo ":OpaqueSheep OS" >> $(BUILD_DIR)/limine/limine.cfg
	@echo "PROTOCOL=limine" >> $(BUILD_DIR)/limine/limine.cfg
	@echo "KERNEL_PATH=boot:///kernel.elf" >> $(BUILD_DIR)/limine/limine.cfg
	@echo "Limine build prepared in $(BUILD_DIR)/limine"

setup:
	sudo apt update
	sudo apt install -y build-essential binutils nasm gnu-efi qemu-system-x86 mtools dosfstools ovmf gdisk parted

fat_img: $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/BOOTX64.EFI
	@if [ -z "$(MKFS_FAT)" ]; then echo "Error: mkfs.fat not found. Run 'make setup' first."; exit 1; fi
	@if [ -z "$(MMD)" ]; then echo "Error: mmd not found. Run 'make setup' first."; exit 1; fi
	@if [ -z "$(MCOPY)" ]; then echo "Error: mcopy not found. Run 'make setup' first."; exit 1; fi
	@if [ -z "$$(which sgdisk)" ]; then echo "Error: sgdisk not found. Run 'make setup' first."; exit 1; fi
	mkdir -p $(BUILD_DIR)/efi/EFI/BOOT
	cp $(BUILD_DIR)/BOOTX64.EFI $(BUILD_DIR)/efi/EFI/BOOT/BOOTX64.EFI
	cp $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/efi/kernel.bin
	echo "\EFI\BOOT\BOOTX64.EFI" > $(BUILD_DIR)/startup.nsh
	# Create a 128MB disk image
	dd if=/dev/zero of=$(BUILD_DIR)/fat.img bs=1M count=128
	# Create GPT partition table and partitions (ESP: 64MB, Data: ~64MB)
	sgdisk -Z $(BUILD_DIR)/fat.img
	sgdisk -n 1:2048:133119 -t 1:ef00 -c 1:"EFI System Partition" $(BUILD_DIR)/fat.img
	sgdisk -n 2:133120:260000 -t 2:8300 -c 2:"OpaqueSheep Data" $(BUILD_DIR)/fat.img
	# Format and populate ESP (Partition 1)
	dd if=/dev/zero of=$(BUILD_DIR)/esp.img bs=512 count=131072
	$(MKFS_FAT) -F 32 $(BUILD_DIR)/esp.img
	MTOOLSRC=/dev/null $(MMD) -i $(BUILD_DIR)/esp.img ::/EFI ::/EFI/BOOT
	MTOOLSRC=/dev/null $(MCOPY) -i $(BUILD_DIR)/esp.img $(BUILD_DIR)/efi/EFI/BOOT/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
	MTOOLSRC=/dev/null $(MCOPY) -i $(BUILD_DIR)/esp.img $(BUILD_DIR)/efi/kernel.bin ::/kernel.bin
	MTOOLSRC=/dev/null $(MCOPY) -i $(BUILD_DIR)/esp.img $(BUILD_DIR)/startup.nsh ::/startup.nsh
	# Format Data (Partition 2)
	dd if=/dev/zero of=$(BUILD_DIR)/data.img bs=512 count=126881
	$(MKFS_FAT) -F 32 $(BUILD_DIR)/data.img
	# Stitch partitions into the disk image
	dd if=$(BUILD_DIR)/esp.img of=$(BUILD_DIR)/fat.img bs=512 seek=2048 conv=notrunc
	dd if=$(BUILD_DIR)/data.img of=$(BUILD_DIR)/fat.img bs=512 seek=133120 conv=notrunc

# OVMF Path Detection
OVMF_FD := $(firstword $(wildcard /usr/share/ovmf/OVMF.fd) \
                       $(wildcard /usr/share/qemu/OVMF.fd) \
                       $(wildcard /usr/share/OVMF/OVMF.fd))

run: fat_img
	@which qemu-system-x86_64 > /dev/null || (echo "qemu-system-x86_64 not found. run 'make setup'"; exit 1)
	@test -f "$(OVMF_FD)" || (echo "OVMF.fd not found. run 'make setup'"; exit 1)
	qemu-system-x86_64 \
		-cpu qemu64 \
		-bios $(OVMF_FD) \
		-drive file=$(BUILD_DIR)/fat.img,format=raw \
		-serial stdio

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/BOOTX64.EFI: boot/uefi/OSx2Bootmanager.c | $(BUILD_DIR)
	$(EFI_CC) -I$(EFI_INC) -I$(EFI_INC)/x86_64 -fpic -fshort-wchar -mno-red-zone \
		-ffreestanding -fno-stack-protector -DEFI_FUNCTION_WRAPPER -c $< -o $(BUILD_DIR)/boot.o
	$(EFI_LD) -nostdlib -znocombreloc -T $(EFI_LDS) -shared -Bsymbolic \
		$(EFI_CRT0) $(BUILD_DIR)/boot.o -o $(BUILD_DIR)/boot.so -L/usr/lib -lefi -lgnuefi
	$(EFI_OBJCOPY) -j .text -j .rodata -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		-j .rela -j .reloc -O efi-app-x86_64 $(BUILD_DIR)/boot.so $@

.PHONY: all clean limeline

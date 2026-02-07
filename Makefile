ARCH ?= x86_64
CROSS ?= x86_64-elf

# Compiler auto-detection for Debian/Ubuntu environments
ifeq ($(shell which $(CROSS)-gcc 2>/dev/null),)
    override CROSS :=
    CC := gcc
    LD := ld
    OBJCOPY := objcopy
else
    CC := $(CROSS)-gcc
    LD := $(CROSS)-ld
    OBJCOPY := $(CROSS)-objcopy
endif

AS := nasm
EFI_CC ?= gcc
EFI_LD ?= ld
EFI_OBJCOPY ?= objcopy
EFI_INC ?= /usr/include/efi
EFI_LIB ?= /usr/lib
EFI_CRT0 ?= $(EFI_LIB)/crt0-efi-x86_64.o
EFI_LDS ?= $(EFI_LIB)/elf_x86_64_efi.lds

CFLAGS := -std=gnu11 -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -Wall -Wextra -Ikernel/include -Ikernel/libs -Ikernel/stup
LDFLAGS := -T kernel/linker/kernel.ld -nostdlib

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

all: $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin

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
	$(OBJCOPY) -O binary $< $@

setup:
	sudo apt update
	sudo apt install -y build-essential binutils nasm gnu-efi qemu-system-x86 mtools dosfstools ovmf

fat_img: all uefi
	mkdir -p $(BUILD_DIR)/efi/EFI/BOOT
	cp $(BUILD_DIR)/BOOTX64.EFI $(BUILD_DIR)/efi/EFI/BOOT/BOOTX64.EFI
	cp $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/efi/kernel.bin
	dd if=/dev/zero of=$(BUILD_DIR)/fat.img bs=1M count=64
	mkfs.fat -F 32 $(BUILD_DIR)/fat.img
	MTOOLSRC=/dev/null mmd -i $(BUILD_DIR)/fat.img ::/EFI ::/EFI/BOOT
	MTOOLSRC=/dev/null mcopy -i $(BUILD_DIR)/fat.img $(BUILD_DIR)/efi/EFI/BOOT/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
	MTOOLSRC=/dev/null mcopy -i $(BUILD_DIR)/fat.img $(BUILD_DIR)/efi/kernel.bin ::/kernel.bin

run: fat_img
	@which qemu-system-x86_64 > /dev/null || (echo "qemu-system-x86_64 not found. run 'make setup'"; exit 1)
	@test -f /usr/share/ovmf/OVMF.fd || (echo "OVMF.fd not found. run 'make setup'"; exit 1)
	qemu-system-x86_64 \
		-bios /usr/share/ovmf/OVMF.fd \
		-drive file=$(BUILD_DIR)/fat.img,format=raw

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/BOOTX64.EFI: boot/uefi/main.c | $(BUILD_DIR)
	$(EFI_CC) -I$(EFI_INC) -I$(EFI_INC)/x86_64 -fpic -fshort-wchar -mno-red-zone \
		-fno-stack-protector -DEFI_FUNCTION_WRAPPER -c $< -o $(BUILD_DIR)/boot.o
	$(EFI_LD) -nostdlib -znocombreloc -T $(EFI_LDS) -shared -Bsymbolic \
		$(EFI_CRT0) $(BUILD_DIR)/boot.o -o $(BUILD_DIR)/boot.so
	$(EFI_OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
		-j .rela -j .reloc -O efi-app-x86_64 $(BUILD_DIR)/boot.so $@

.PHONY: all clean

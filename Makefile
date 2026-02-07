ARCH ?= x86_64
CROSS ?= x86_64-elf
CC := $(CROSS)-gcc
AS := nasm
LD := $(CROSS)-ld
OBJCOPY := $(CROSS)-objcopy
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

$(BUILD_DIR)/stup_init.o: kernel/stup/init.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

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

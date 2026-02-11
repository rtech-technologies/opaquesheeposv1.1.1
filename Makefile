CC = gcc
LD = ld
OBJCOPY = objcopy

CFLAGS = -ffreestanding -fno-stack-protector -fpic -Ikernel/include -Wall -Wextra -mno-red-zone
LDFLAGS = -nostdlib -T kernel/linker/link.ld

KERNEL_OBJS = kernel/boot.o \
              kernel/kmain.o \
              kernel/libs/services.o \
              kernel/libs/video.o \
              kernel/libs/input.o \
              kernel/libs/fat32.o \
              kernel/libs/disk.o \
              kernel/libs/interrupts.o \
              kernel/libs/path.o

all: kernel.bin bootx64.efi

kernel.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $(KERNEL_OBJS) -o kernel.bin

bootx64.efi: bootloader/main.o
	$(LD) -nostdlib -shared -Bsymbolic -o bootx64.efi bootloader/main.o

disk_img: all
	./scripts/make_disk.sh

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL_OBJS) bootloader/main.o kernel.bin bootx64.efi opaque.img

.PHONY: all clean disk_img

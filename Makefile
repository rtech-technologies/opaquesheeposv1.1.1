CC = gcc
LD = ld
OBJCOPY = objcopy

CFLAGS = -ffreestanding -fno-stack-protector -fpic -Ikernel/include -Wall -Wextra
LDFLAGS = -nostdlib -Ttext 0x100000

KERNEL_OBJS = kernel/kmain.o \
              kernel/libs/services.o \
              kernel/libs/video.o \
              kernel/libs/input.o \
              kernel/libs/fat32.o

all: kernel.bin

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

kernel.elf: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $(KERNEL_OBJS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL_OBJS) kernel.elf kernel.bin

.PHONY: all clean

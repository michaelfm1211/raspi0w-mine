# THESE CONFIGURATIONS ARE FOR THE RPI0W ONLY!
# RPI0W uses the ARM11 BCM2835 SoC, which is ARMv6 architecture.

CC = clang
LD = ld.lld
OBJCOPY = llvm-objcopy

TARGET = arm-unknown-none-gnueabihf
CFLAGS = --target=$(TARGET) -I . -Wall -Wextra -mcpu=arm1176jzf-s -ffreestanding -c
LDFLAGS = -m armelf -T linker.ld -nostdlib

SRCS = $(wildcard drivers/*.c kernel/*.c lib/*.c)
OBJS = $(SRCS:.c=.o) boot.o drivers/isrS.o

.PHONY: all
all: CFLAGS += -O2
all: kernel.img

.PHONY: debug
debug: CFLAGS += -O0 -g
debug: kernel.img

firmware/BCM43430A1.o: firmware/BCM43430A1.hcd
	$(OBJCOPY) -I binary -O elf32-littlearm $< $@

boot.o: boot.S
	$(CC) $(CFLAGS) -o $@ $^

drivers/isrS.o: drivers/isr.S
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

kernel.elf: $(OBJS) firmware/BCM43430A1.o boot.o
	$(LD) $(LDFLAGS) -o kernel.elf $^

kernel.img: kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.img

.PHONY: clean
clean:
	rm -rf $(OBJS) boot.o kernel.elf kernel.img

.PHONY: qemu
qemu: kernel.img
	qemu-system-arm -M raspi0 -kernel kernel.elf -serial null -serial stdio

TARGET = arm-unknown-non-gnueabihf
CFLAGS = --target=$(TARGET) -I.. -Wall -Wextra -mcpu=arm1176jzf-s -ffreestanding -c

CC=clang
LD=ld.lld
OBJCOPY=llvm-objcopy
PYTHON=python

all: test.img

test.img: sym.o test.o
	$(LD) -T linker.ld -o test.elf test.o sym.o
	$(OBJCOPY) -O binary test.elf test.img

sym.S:
	$(PYTHON) copy-global-symbols.py kernel.elf sym.S

sym.o: sym.S
	# $(CC) $(CFLAGS) sym.S -o sym.o
	$(CC) --target=$(TARGET) -c -o sym.o sym.S

test.o: test.c
	$(CC) $(CFLAGS) test.c -o test.o

.PHONY: clean
clean:
	rm -rf sym.S sym.o test.o test.elf test.img

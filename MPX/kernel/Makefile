#
# Makefile for MPX/Linux

AS      = nasm
CC      = clang
CFLAGS  = --target=i386-elf -Wall -Wextra -Werror -ffreestanding -g -c
ifeq ($(shell uname), Darwin)
LD	= i686-elf-ld
else
LD      = ld
endif
LDFLAGS = -melf_i386 -g
ASFLAGS = -f elf -g

OBJFILES =\
core/gdt.o\
core/idt.o\
core/interrupts.o\
core/io.o\
core/irq.o\
core/kmain.o\
core/serial.o\
core/system.o\
core/tables.o\
mem/paging.o\
mem/heap.o\
r2/pcb.o

.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../include -I../ -o $@ $<

all: kernel.o

kernel.o: $(OBJFILES)
	$(LD) $(LDFLAGS) -r -o kernel.o $(OBJFILES)

clean:
	rm -f $(OBJFILES)

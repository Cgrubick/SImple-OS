#
# Makefile for MPX/Linux libs

AS      = nasm
CC      = clang
CFLAGS  = --target=i386-elf -Wall -Wextra -Werror -ffreestanding -g -c
ifeq ($(shell uname), Darwin)
LD	= i686-elf-ld
else
LD      = ld
endif
LDFLAGS = -melf_i386
ASFLAGS = -f elf -g

OBJFILES =\
string.o

.c.s:
	$(CC) $(CFLAGS) -S -o $@ $<
.s.o:
	$(AS) $(ASFLAGS) -c -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../include -o $@ $<

all: lib.o

lib.o: $(OBJFILES)
	$(LD) $(LDFLAGS) -r -o lib.o $(OBJFILES)

clean:
	rm -f $(OBJFILES)

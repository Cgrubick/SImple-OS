#
# Makefile for MPX Modules

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

# add new files as you create them.
# I'll start you off with just mpx_supt.o
OBJFILES =\
mpx_supt.o \
r1/com_hand.o \
r1/date.o \
r1/help.o \
r3/procsr3.o \
r3/usercom.o \
r4/com_hand_proc.o \
r4/infinite.o \
r5/mcb.o
.c.s:
	$(CC) $(CFLAGS) -S -o $@ $<
.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../include -o $@ $<

all: modules.o

modules.o: $(OBJFILES)
	$(LD) $(LDFLAGS) -r -o modules.o $(OBJFILES)

clean:
	rm -f modules.o $(OBJFILES)

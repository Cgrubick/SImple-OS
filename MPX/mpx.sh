#!/bin/sh

set -e

if ! [ -f kernel.bin ]; then
	make kernel.bin
fi

exec qemu-system-i386 -nographic -kernel kernel.bin -s "$@"

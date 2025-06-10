#
# Copyright (C) 2025 Sylas Hollander.
# PURPOSE: Makefile for Apple TV bare-metal programs
# SPDX-License-Identifier: MIT
#

# If the generation of headers is parallelized, they will likely not exist or be corrupted by the time Clang gets to them
.NOTPARALLEL: kernel_bin.h initramfs_bin.h

# Check what OS we're running. Should work on Linux and macOS.
OSTYPE = $(shell uname)

# Target defs for Linux cross compiler.
TARGET = i386-apple-darwin8

# Definitions for compiler
CC := clang

# Definitions for linker
ifeq ($(OSTYPE),Linux)
	LD := /opt/cross/bin/$(TARGET)-ld
else
	LD := ld
endif

# Flags for mach-o linker
LDFLAGS := 	-static \
           	-segalign 0x1000 \
           	-segaddr __TEXT 0x00100000 \
           	-sectalign __TEXT __text 0x1000 \
           	-sectalign __DATA __common 0x1000 \
           	-sectalign __DATA __bss 0x1000 \

DEFINES :=
ifdef INITRAMFS
	DEFINES += -DINITRAMFS
endif

CFLAGS := -Wall -Werror -nostdlib -fno-stack-protector -fno-builtin -O0 -std=gnu11 --target=$(TARGET) -Iinclude $(DEFINES)

OBJS := start.o atvlib.o baselibc_string.o cons.o tinyprintf.o debug.o linux.o e820.o

all: kernel_bin.h initramfs_bin.h mach_kernel

# Add kernel to executable
kernel_bin.h: $(KERNEL)
ifdef KERNEL
	xxd -n kernel_bin -i $< > $@
else
	$(error No kernel file specified. Specify one by appending KERNEL=/path/to/kernel)
endif
initramfs_bin.h: $(INITRAMFS)
ifdef INITRAMFS
	xxd -n initramfs_bin -i $< > $@
else
	$(warning No initramfs/initrd file specified. Specify one by appending INITRAMFS=/path/to/initramfs if you want.)
endif

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mach_kernel: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
# Note: header generation must run before compilation

clean:
	rm -f initramfs_bin.h kernel_bin.h *.o mach_kernel
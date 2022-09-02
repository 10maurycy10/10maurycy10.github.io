---
title: "AVR build system"
date: 2022-08-21T09:30:21-07:00
tags: ["avr", "programming"]
draft: true
---

# Generic makefile

I wanted to have a maintainable way to build all my AVR projects.

I ended up writing a Makefile that can be ``include``ed from each program.

This allows a two line makefile to compile a project.

```make
# Example Makefile for single file project
OBJ=blink.c.o
include ../genericMakefile
```

# Features

- Target microcontroler configurable from command line or environment. ``make flash TARGET=atmega103``

- Programmer configurable in the same way with ``PROGRAMER`` variable.

- Define ``F_CPU``, and allow setting from command line or project level makefile

- Show flash and ram usage

# CLI

```sh
make something.c.o # Compile the object file for a given 
make all.elf # Compile and link project
make clean # Remove generated binary files
make size # Show rom usage
make flash # Flash the program onto the rom (might require root)
make avrdude # Open avrdude cli (might require root)
make erase # Erase chip w/out flashing anything
```

# Other examples

Include custom fuse settings in flash.

```make
OBJ=test.c.o
FLASHOP= -U lfuse:w:0x22:m
include ../genericMakefile
```

Include prebuilt library.

```make
OBJ=test.c.o
LIBS=lib.so
include ../genericMakefile
```

Compile assembly and link alongside c

```make
OBJ=testasm.asm.o test.c.o
include ../genericMakefile
```

Override default ``F_CPU``

```make
F_CPU?=test.c.o
include ../genericMakefile
```

# Prerequisites

- GNU make
- avr-gcc (avr-libc)
- avrdude

## Installation on arch linux

```
pacman -S make avr-gcc avr-libc avrdude
```

## Installation on Debian based systems

```
apt install make avr-gcc avr-libc avrdude
```

# Code

[Makefile](genericMakefile)





---
title: "One bit computer: Instruction set tweaks"
date: 2023-09-17T11:41:06-07:00
tags: ["onebit"]
draft: false
---

I have decided to make a few small changes to the instruction set, to make things more usable.

The first change is to make the SKZ instruction use the data bus, not the accumulator[^0]. 
This makes it much more versatile, because it can be used in the middle of a computation, without modifying the accumulator.
As the accumulator is memory mapped, the old behavior can still be achieved.

Another change is with the NOP instruction, becuase it is not used for instruction skipping, it can be used for something else.
For now, I will just have it strobe a flag that can control additional logic.

|Opcode|Name|Function|
|------|----|--------|
|0000 0|NOP |Strobes the NOP flag.| 
|0001 1|LD  |Load the bus into the accumulator.|
|0010 2|LDC |Load the complement of the bus into the accumulator.|
|0011 3|STO |Writes the accumulator on the the bus|. 
|0100 4|STOC|Writes the complement of the accumulator on the bus.|
|0101 5|OEN |Loads the OEN register from the bus.|
|0110 6|ADD |Adds the accumulator to the bus, using the carry register to store the carry.|
|0111 7|XNOR|XNORs the accumulator with the bus.|
|1000 8|XOR |XORs the accumulator with the bus.|
|1001 9|AND |ANDs the accumulator with the bus.|
|1010 A|OR  |ORs the accumulator with the bus.|
|1011 B|ONE |Forces a one into the accumulator and a zero into the carry.|
|1100 C|TWO |Forces a zero into the accumulator and a one into the carry.|
|1101 D|JMP |Strobes the jump flag.|
|1110 E|SKZ |Skips the next instruction if the data bus is zero.|
|1111 F|RET |Skips the next instruction and strobes the return flag.|

Implementing these require only very minor changes to the CPU:

![The modified schematic diagram](new.svg)

The rest of this post is just general documentation about the processor for easy reference:

# General architecture:

The computer uses a Harvard architecture, with separate memory for instructions and data.
This avoids the need for separate fetch and execute cycles, and allows universal one instruction-per-cycle execution
The program counter is separate from the CPU registers, and can be any size needed, but jumps only support 16 bit destinations.

# Clocking:

There are 2 clock phases, during phase 1, the instruction and argument are read and executed.
During phase 2, the registers are updated and memory is written to.

# Instruction format:

Instructions are 16 bits, split up like this:

|Bits (inclusive)|Function|
|---|---|
|bit 0 (LSB) to bit 9:|Address offset, 10 bits|
|bit 10:|Register select bit|
|bit 11:|Zero page bit|
|bit 12 to 15 (MSB):|Opcode, 4 bits|

# Addressing:

Each instruction has a single operand, located in memory.

Depending on the register select bit, the value in one of two 15-bit index registers is ORed with the address offset part of the instruction, unless the zero page bit is one.
If the zero page bit is one, the address offset is directly used as the memory address.
The index register not selected by the register select bit will be mapped into memory starting at address 0x1.

|Start|End (inclusive)|Mapping
|-|-|-|
|0x0|0x0|Value of the accumulator register, read only|
|0x0|0x0|Halt flag, stops execution if a one is written, write only|
|0x1|0xf|The currently *unselected* index register (MSB first)|
|0x10|0x1f|Jump destination (MSB first)|
|0x20|0x7fff|Memory, will include IO at some point|

# Opcodes:

There are 3 1-bit user visible registers: the accumulator (ACC), carry flag (CARRY), and Output ENable (OEN).

## 0000 0x0 NOP
Registers effected: None

No operation, strobes the NOP flag during clock phase 2.

## 0001 0x1 LD
Registers effected: ACC

Loads the argument into the accumulator.

## 0010 0x2 LDC
Registers effected: ACC

Loads the complement of the argument into the accumulator.

## 0011 0x3 STO
Registers effected: None

Stores the accumulator to memory address of the argument if OEN is one.
If OEN is zero, this does nothing.

## 0100 0x4 STOC
Registers effected: None

Stores the complement accumulator to memory address of the argument if OEN is one.
If OEN is zero, this does nothing.

## 0101 0x5 OEN
Registers effected: OEN

Loads the argument into the OEN register

## 0110 0x6 ADD
Registers effected: ACC CARRY

Adds the carry flag, accumulator and argument together, storing the sum into the accumulator and the carry into the carry flag.

## 0111 0x7 XNOR
Registers effected: ACC

Performs the XNOR operation between the argument and the accumulator, storing the result in the accumulator.

## 1000 0x8 XOR
Registers effected: ACC

Performs the XOR operation between the argument and the accumulator, storing the result in the accumulator.

## 1001 0x9 AND
Registers effected: ACC

Performs the AND operation between the argument and the accumulator, storing the result in the accumulator.

## 1010 0xA OR
Registers effected: ACC

Performs the OR operation between the argument and the accumulator, storing the result in the accumulator.

## 1011 0xB ONE
Registers effected: ACC CARRY

Sets the accumulator to one and carry flag to zero.

## 1100 0xC TWO
Registers effected: ACC CARRY

Sets the accumulator to zero and carry flag to one.

## 1101 0xD JMP
Registers effected: None

Sets the jump flag during phase 1 and 2, should result in the jump destination being loaded into the program counter.
This should result in the next instruction being the instruction pointed to by the jump destination. 

## 1110 0xE SKZ
Registers effected: None

If the argument is zero, the next instruction will be skipped and have no effect.

## 1111 0xF RET
Registers effected: None

Unconditionally sets the return flag during phase 1 and 2, the next instruction will be skipped and have no effect.
This should reset the program counter to zero.

[^0]: The "skip if accumulator is zero" behavior comes from the [MC14500](http://www.bitsavers.org/components/motorola/14500/MC14500B_Industrial_Control_Unit_Handbook_1977.pdf), the chip that inspired the one bit computer.

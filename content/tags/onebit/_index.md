---
title: "One bit computer overview"
---

I am working on building a computer with no ICs.

The goal is to build a computer using the simplest components available, and where it is possible follow data through an entire computation.
Ideally, it would be built from relays, but they are very expensive and too slow for any real time applications.
Transistors offer a nice balance between cost, speed and complexity. 
While you can not see how a transistor operates just by looking at it, the function is fairly simple, and so is the actual physics behind them.

To minimize the amount of logic required for a functional computer, it will use a one bit data bus, ALU and registers. 
This also allows the most efficient use of RAM. (Most computers use a whole 8 bits for a boolean value)

Unfortunately, the addresses and program counter have to be bigger then one bit. (Building a counter is a lot simpler then an multi-bit adder)
The instructions will also have to be a lot bigger to fit both an opcode and operand address.

# Design Requirements:

I want the computer to be able to run simple games like Snake and Tetris, as well as a do simple mathematics like addition, multiplication, square roots, logs, etc.
4kB of address space should be more then enough for these applications and can always be left mostly empty.
It should also be constructed from just a few types of components: NPN transistors, diodes, resistors, LEDs and switches. (and a few capacitors for the clock)

It should also be reasonably compact, a functional system should fit in 1 m² with all the components visible.
A full 32 thousand bits of ram (4 kB) probably won't fit, I am totally fine with stacking memory boards to have enough for games and stuff.

<!--
- Supports conditonal branching, booliean arithmatic and addition.
- At least 8000ish 8 bit additions per second.
- At least 1KB of address space, with indirect addressing support
- Minimal and modular, a functional system should be able to fit in 1 m^2 with all components visable, but I also want it to be easly expandable.
- Indicator lights along the datapath, on registers and between sections of logic.
- Should support single stepping.
- Fully discrete component construction, including RAM. (Program memory may be a flash chip for longer programs)
-->

# General Architecture:

To avoid a separate fetch and execute stage, the computer will use a Harvard style architecture with separate program ROM and data RAM.
This also has the advantage that ROM is much, much simpler then RAM.
There are 2 clock phases, during phase 1, the program counter updates, the instruction is read from ROM, the operand is fetched and the new register state is computed.
During phase 2, the registers update, memory is written to, and the next value for the program counter is computed.

Instructions will be 16 bits with 4 opcode bits, a zero page bit, a register select bit and 10 address offset bits:

|Range (inclusive)|Use|
|-|-|
|Bit 0 (LSB) to bit 9|Address offset (10 bits)|
|Bit 10|Register select|
|Bit 11|Zero page bit|
|Bit 12 to 15 (MSB)|Opcode|

# CPU:

The CPU itself has a one bit ALU, accumulator register and carry flag.
There are 2 more registers, Skip and OEN (Output ENable) that are used for conditional execution.
Skip is used to skip single instructions, and OEN can disable writing to memory to perform larger conditionals without jumping.

The registers are implemented as a chain of 2 d-latches, with the first one saving the result from the ALU, and the second being the input to the ALU.
During phase 1, the first one stores the result, and during phase 2, the second one copies the data in the first.
The exception is the Skip register, which has the clock phases reversed, so that it updates during phase 1, and stays constant during phase 2.
The Skip register is used to gate the phase 2 clock to registers, memory, as well as jump circuitry, so it has to stay constant during phase 2.

There are 16 instructions, based off of the [MC14500 ICU chip.](http://www.bitsavers.org/components/motorola/14500/MC14500B_Industrial_Control_Unit_Handbook_1977.pdf)
Unlike the MC14500, my computer will include arithmetic instructions, which add quite a bit of complexity (full adder, carry register and control logic) but add a lot of utility. (The MC14500 takes 12 cycles to add a bit with carry!)

|Opcode|Name|Function|
|------|----|--------|
|0000 0|NOP |Strobes the NOP flag.|
|0001 1|LD  |Load the bus into the accumulator.|
|0010 2|LDC |Load the complement of the bus into the accumulator.|
|0011 3|STO |Writes the accumulator on the the bus|.
|0100 4|STOC|Writes the complement of the accumulator on the bus.|
|0101 5|OEN |Loads the OEN register from the bus.|
|0110 6|ADD |Adds the accumulator, carry register and the bus, using the carry register to store the carry.|
|0111 7|XNOR|XNORs the accumulator with the bus.|
|1000 8|XOR |XORs the accumulator with the bus.|
|1001 9|AND |ANDs the accumulator with the bus.|
|1010 A|OR  |ORs the accumulator with the bus.|
|1011 B|ONE |Forces a one into the accumulator and a zero into the carry.|
|1100 C|TWO |Forces a zero into the accumulator and a one into the carry.|
|1101 D|JMP |Strobes the jump flag, see the "program counter" section.|
|1110 E|SKZ |Skips the next instruction if the data bus is zero.|
|1111 F|RET |Skips the next instruction and strobes the return flag.|

If OEN is set to zero, write operations are skipped (STO/STOC), but computation continues normally.

Some of these instructions don't have a function, like NOP and RET, and can be used as spare instruction for expansion.

This CPU can be implemnted in very few gates, and in practice will be even fewer as the gates can often be combined together when building them from transistors:

![The whole CPU in logic gates.](gates.svg)

# Memory/Addressing:

There are 2, 15-bit, memory mapped index registers, each can address 4kB of memory (yes, 4kB, each address only has *one* bit).
They are used for cheap offset addressing, where an OR gate is used instead of an adder. 

The index register used for an instruction is selected by the register select bit.
If the zero page bit is 0, the value in the selected index register register is ORed with the address offset in the instruction to determine the memory address.
If the zero page bit is 1, the address offset is used directly as the memory address.

The memory map currently looks like this:

|Start|End (inclusive)|Use|
|-|-|-|
|0x0|0x0|Value of the accumulator register, read only|
|0x0|0x0|Halt flag, stops execution if a one is written, write only|
|0x1|0xf|The currently unselected index register (MSB first)|
|0x10|0x1f|Jump destination (MSB first)|
|0x20|0x7fff|Main memory, will include IO at some point|

The reason the currently *unselected* index register is mapped is to prevent writing to an index register using indirect addressing causing memory corruption when the address changes during a write.

# Program counter:

The program counter is the address into the program ROM of the current instruction, and counts up by 1 every cycle at the start of phase 1.
It will be implemented using 2 chained latches for every bit, with the first one updating during phase 2, and the second during phase 1.
During normal operation, the lest significant one always toggles, and every other only toggles if all the bits less significant then it it are ones

The exception to this is the JMP instruction, if the Jump flag is one, and Skip is zero, the the next value will instead be computed from the operand to the instruction:
If the zero page bit is 0, the Jump Destination register (really just a special 16 bits in memory) is ORed with the offset to determine the new program counter value.
If the zero page bit is 1, the program counter will simply be set to the offset part of the instruction.

# Progress updates:

Below is a every post I have made about the computer, in reverse chronological order:

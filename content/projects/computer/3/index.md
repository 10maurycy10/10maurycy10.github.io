---
title: "One bit computer: Addressing considerations"
date: 2023-09-06T18:14:07-07:00
tags: ["onebit"]
draft: false
---

I handwaved addressing in the [first post](../1), but it is very important, and will have a huge impact on how usable the final computer is.
Because the opcodes contain no addressing information, addressing must be done with extra bits in the instruction[^0].

The simplest option is direct addressing, where the extra bits directly control the address.
For example, a 12 bit instruction can be spit into a 4-bit opcode and a 8-bit address, providing 256 bits (32 bytes) of address space:

```text
Instruction: 0110 0000 0010
             |  | |       |
     Opcode -+--+ |       |
     Address------+-------+	
```
The main problem with this is that the address is fixed, and can *not* be changed at runtime.
This makes many things, like arrays and pointers impossible to implement. [^3]

An alternative is PDP-8 style relative addressing, where addresses are either an absolute address, or relative an address in the index register.
This requires adding an extra bit to the instruction to indicate if the address is relative (0) or absolute (1).
Becuase the CPU has no large registers, the index register has to be memory mapped to a low numbered address, so that it can always be accessed with absolute addressing.

```text
Instruction: 0110 1 000 0100
             |  | | |      |
   Opcode ---+--+ | |      |
   Zero page bit -+ |      |
   Address ---------+------+
```

This system has a number of desirable features:

- A section of memory at the start (low memory), in the example the first 128 bits (16 bytes), is always accessible, and can be used as additional registers.
- Accessing structures and fixed indexes into an array is very fast and easy, without requiring incrementing or decrementing any registers.

Because most algorithms involve multiple pieces of data, multiple index registers would avoid pointlessly copying data and addresses.
There are a few ways of doing this, but the simplest (and possibly best) one is to add an extra bit to the instruction to select what index register to use.
This also allows both registers to share a memory address, by having the register select bit control which one is mapped into memory.
Ideally when one register is selected, the *other one* should be mapped into memory.
This prevents the current index register from being written with a relative address to which would result in unpredictable and broken behavior[^7].

Another option is to have the select bit memory mapped, allowing shorter instructions, but would make copying one register to another, a fairly common operation quite slow.[^5]

The main problem with relative addressing is that adders are complex, and a 13 bit adder would be required to meet the design minimum of 1kB of address space,
The situation for adders is slightly better with the improved XOR design[^2], they are still very complex and large.

Instead of adding, another option is to use the index register as the lower (most significant) part of the address in the instruction as the upper (lest significant) part.
The problem with this is that it is super inflexible. If the top 4 bits come from the instruction, the smallest value you can indirectly address is a byte, and the largest value is a byte.
If you need to read multi-byte values, you have to constantly increment the index register.

A much better method is to instead OR the address from the instruction with the index register.
If part of the index register is left as zeros, that section can be overridden by the instruction.
This is not as flexible as a proper adder, because it is restricted to self aligned, power-of-2 sized memory regions, but this should be fine for most uses.[^4]

The plan for the 1 bit computer is to use this scheme, with 2 16-bit[^8] index registers occupying 32 bits of the lower address space.
The total instruction size will be 13 bits, 4 of which are the opcode, 1 is the zero page bit, 1 is the index register select bit, leaving 7 bits for the address.
The provides 128 bits of lower memory (16 bytes), 1 of which will be the accumulator, 32 are occupied by the index and jump destination registers, leaving 95 bits (11.8 bytes) of usable low memory.

|First bit|Last bit|Length (bits)|Use|
|-|-|-|-|
|0x0000|0x0000|0x1 1|Accumulator (Read only)|
|0x0000|0x0000|0x1 1|Halt flag, stops the clock when set (Write only)|
|0x0001|0x0010|0xd 16|The currently unselected index register|
|0x0011|0x0020|0xd 16|Jump destination|
|0x0021|0x007f|0x5f 95|General purpose low memory/Extra registers|
|0x0080|0xffff|0xff80 65408|High memory, should include memory mapped IO|

[^8]: Just 10 bits have to be built to meet the design specs, but expandablity is not a problem

[^0]: I guess I accidentally made an [orthogonal instruction set](https://en.wikipedia.org/wiki/Orthogonal_instruction_set), which according to Wikipedia are quite rare for RISC computers, but I suppose that claim is based off more complex addressing modes and instructions with multiple arguments.

[^1]: The reason this is not 8 is that the accumulator should be mapped into memory, to be able to set the OEN register after a reset, and allow easily inverting the accumulator with the LDC instruction.

[^2]: Detailed at the bottom of the [last post](../2)

[^3]: 
	Impossible without self-modifying code that is.
	Self modifying code would require the program to execute from RAM, which requires a bunch of additional hardware, like a shift register and sequencing logic to read the instruction in from memory 1 bit at a time.
	This would also significantly slow down execution as every instruction would eat up a load of cycles before it even gets to execute.

[^4]:
	To be fair, this limitation exists in many modern processors, which often impose a speed penalty or even crash if you for example, try to read a 64 bit value who's address is not a multiple of 64.

[^5]:
	This is what it takes to copy one register with a memory mapped select bit to another, repeated multiple times for every bit:
	```asm
	; Copy data from register 0 to register one
	; tmp is a temporary location in low memory
	;
	; Select register one, mapping 0 into memory
	ONE
	STO reg_sel
	; Store a bit from register 0
	LD index
	STO tmp
	; Select register 0, mapping zero into memory
	ONE
	STOC reg_sel
	; Write that bit to register 1
	LD tmp
	STO index
	```
 
[^7]: That is, unless the index registers are built with  edge triggered latches, that are twice as expensive as normal memory.

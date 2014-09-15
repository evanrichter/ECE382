Simple Calculator Lab
===

Objectives & Purpose
---
This lab's objective is to give me practice in the assembly programming skills I have learned in class. Specific objectives include using the instruction set, addressing modes, conditional jumps, status register flags, assembler directives, and the assembly process effectively. The program I am implementing will take a series of operands and operations and store the results. It is a basic calculator that needs to implement add, subtract, clear, and end for minimum credit. For more credit, it will implement an upper- and lower-bound for overflow values, and a speedy multiply function.

Preliminary Design
---
![](./images/lab1_prelim.bmp "Preliminary Design")


Software Flow-Chart / Algorithms
---
In this section, I discuss the general flow of my basic calculator program.

### Setup Initial State

Storing operation identifiers and a test string into ROM with labels makes later code more readable.
```Assembly
plus:	.equ	0x11
minus:	.equ	0x22
times:	.equ	0x33
clear:	.equ	0x44
end:	.equ	0x55

input:	.byte	0x22, 0x11, 0x22, 0x55
```

After the normal boilerplate instructions, `main` initializes `R5` to point to the first byte in the test string, clears `R6` for use as the accumulator, and points `R7` to the first available space in RAM to store results: `0x0200`. Since the first value is always assumed to be a value, not an operation, `main` jumps to that command unconditionally.
```Assembly
main:
	mov.w   #__STACK_END,SP			; BOILERPLATE	Initialize stackpointer
	mov.w   #WDTPW|WDTHOLD,&WDTCTL 	; BOILERPLATE	Stop watchdog timer

	mov.w	#0x00, R6
	mov.w	#0x0200, R7
	mov.w	#input, R5

	jmp 	enternum

```

Debugging
---

Testing
---

Implementation plan <<insert photo of attack plan>>
Discuss code with code blocks

```Assembly
plus:	.equ	0x11
minus:	.equ	0x22
times:	.equ	0x33
clear:	.equ	0x44
end:	.equ	0x55
```

Debugging and testing
---

Conclusion & lessons learned
---

Documentation
---
I referenced Taylor Bodin's [lab 1 notebook](https://github.com/taylorbodin/ECE382_Lab1/blob/master/README.md) to see general formatting of the `readme.md` markdown file, and the overall flow of the lab report.

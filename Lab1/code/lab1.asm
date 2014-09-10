;-------------------------------------------------------------------------------
;	Name:			Evan Richter
;	Term:			Fall 2014
;	MCU:			MSP430G2553
;	Assignment:		3
;	Date:			10 September 2014
;	Documentation:
;-------------------------------------------------------------------------------
	.cdecls C,LIST,"msp430.h"	; BOILERPLATE	Include device header file
 	.text						; BOILERPLATE	Assemble into program memory
	.retain						; BOILERPLATE	Override ELF conditional linking and retain current section
	.retainrefs					; BOILERPLATE	Retain any sections that have references to current section
	.global main				; BOILERPLATE	Project -> Properties and select the following in the pop-up
								; Build -> Linker -> Advanced -> Symbol Management
								;    enter main into the Specify program entry point... text box
plus:	.equ	0x11
minus:	.equ	0x22
times:	.equ	0x33
clear:	.equ	0x44
end:	.equ	0x55

input:	.byte	0x21, 0x22, 0x01,  0x11,  0x19,  0x44,  0x15,  0x22,  0x05,  0x55
;-------------------------------------------------------------------------------
;          		main: steps through input string with a case switch
;
;				R5: array pointer to unused byte
;				R6: accumulator
;				R7: memory pointer to available byte
;-------------------------------------------------------------------------------
main:
	mov.w   #__STACK_END,SP			; BOILERPLATE	Initialize stackpointer
	mov.w   #WDTPW|WDTHOLD,&WDTCTL 	; BOILERPLATE	Stop watchdog timer

	mov.b	#0x00, R6
	mov.w	#0x0200, R7
	mov.w	#input, R5

checkend:
	cmp.b	#end, 0(R5)
	jnz		checkplus
	jmp		trapcpu

checkplus:
	cmp.b	#plus, 0(R5)
	jnz		checkminus
	inc.w	R5
	add.b	@R5+, R6
	jmp		save

checkminus:
	cmp.b	#minus, 0(R5)
	jnz		checktimes
	inc.w	R5
	sub.b	@R5+, R6
	jmp		save

checktimes:
	cmp.b	#times, 0(R5)
	jnz		checkclear
	inc.w	R5

	jmp		save

checkclear:
	cmp.b	#clear, 0(R5)
	jnz		enternum
	mov.b	#0x00, 0(R7)
	inc.w	R7
	inc.w	R5
	mov.b	R5, R6
	jmp		checkend

enternum:
	mov.b	@R5+, R6
	jmp		checkend


trapcpu:
	jmp		trapcpu


save:
	mov.b	R6, 0(R7)
	inc.w	R7
	jmp		checkend

;-------------------------------------------------------------------------------
;           System Initialization
;-------------------------------------------------------------------------------
	.global __STACK_END				; BOILERPLATE
	.sect 	.stack					; BOILERPLATE
	.sect   ".reset"				; BOILERPLATE		MSP430 RESET Vector
	.short  main					; BOILERPLATE

;-------------------------------------------------------------------------------
;	Name:			Evan Richter
;	Term:			Fall 2014
;	MCU:			MSP430G2553
;	Assignment:		Lab 2
;	Date:			16 September 2014
;	Documentation:
;	Functionality:
;-------------------------------------------------------------------------------
	.cdecls C,LIST,"msp430.h"	; BOILERPLATE	Include device header file
 	.text						; BOILERPLATE	Assemble into program memory
	.retain						; BOILERPLATE	Override ELF conditional linking and retain current section
	.retainrefs					; BOILERPLATE	Retain any sections that have references to current section
	.global main				; BOILERPLATE	Project -> Properties and select the following in the pop-up
								; Build -> Linker -> Advanced -> Symbol Management
								;    enter main into the Specify program entry point... text box

key:	.byte	0xac
crypt:	.byte	0xef,0xc3,0xc2,0xcb,0xde,0xcd,0xd8,0xd9,0xc0,0xcd,0xd8,0xc5,0xc3,0xc2,0xdf,0x8d,0x8c,0x8c,0xf5,0xc3,0xd9,0x8c,0xc8,0xc9,0xcf,0xde,0xd5,0xdc,0xd8,0xc9,0xc8,0x8c,0xd8,0xc4,0xc9,0x8c,0xe9,0xef,0xe9,0x9f,0x94,0x9e,0x8c,0xc4,0xc5,0xc8,0xc8,0xc9,0xc2,0x8c,0xc1,0xc9,0xdf,0xdf,0xcd,0xcb,0xc9,0x8c,0xcd,0xc2,0xc8,0x8c,0xcd,0xcf,0xc4,0xc5,0xc9,0xda,0xc9,0xc8,0x8c,0xde,0xc9,0xdd,0xd9,0xc5,0xde,0xc9,0xc8,0x8c,0xca,0xd9,0xc2,0xcf,0xd8,0xc5,0xc3,0xc2,0xcd,0xc0,0xc5,0xd8,0xd5,0x8f
;-------------------------------------------------------------------------------
;          		main: sets up registers 12-15 and places a value on the stack for
;					  use by decryptMessage
;
;-------------------------------------------------------------------------------
main:
	mov.w   #__STACK_END,SP			; BOILERPLATE	Initialize stackpointer
	mov.w   #WDTPW|WDTHOLD,&WDTCTL 	; BOILERPLATE	Stop watchdog timer

	;set up registers
	mov.w	#crypt, R12
	mov.w	#0x0200, R13
	mov.b	#0x5e, R14
	mov.w	#key, R15
	push.b	#0x01
	call	#decryptMessage

trapcpu:
	jmp		trapcpu


;-------------------------------------------------------------------------------
;Subroutine Name:	decryptMessage
;Author:			Evan Richter
;Function: Decrypts a string of bytes and stores the result in memory.  Accepts
;           the address of the encrypted message, address of the key, and address
;           of the decrypted message (pass-by-reference).  Accepts the length of
;           the message by value.  Uses the decryptCharacter subroutine to decrypt
;           each byte of the message.  Stores theresults to the decrypted message
;           location.
;Inputs:	R12-15: msgStart*, ramStart*, msgLength, key*, and keyLength on the stack
;Outputs:	decrypted string to RAM
;Registers destroyed: none
;-------------------------------------------------------------------------------

decryptMessage:
	push.w	R10
	mov.b	2(SP), R10
	push.w	R7
	push.w	R8
	push.w	R12
	push.w	R13
	push.b	R14
	push.w	R15
	push.b	R10


checkEndString:
	cmp.b	#0, R14			; msgLength == 0 ?
	jeq		endString
	mov.b	@R12+, R8
	cmp.b	#1, R10			; keyLength == 1 ?
	jne		endKey
	pop.b	R10				; restore keyLength
	pop.w	R15				; restore key*
	push.w	R15
	push.b	R10
endKey:
	mov.b	@R15, R9		; put key in register for decrypt
	dec.b	R10
	dec.b	R14
	call	#decryptByte
	mov.b	R8, 0(R13)
	inc.w	R13
	jmp		checkEndString
endString:
	;string done

	pop.b	R10
	pop.w	R15
	pop.w	R14
	pop.w	R13
	pop.w	R12
	pop.w	R8
	pop.w	R7
	pop.w	R10
	ret

;-------------------------------------------------------------------------------
;Subroutine Name:	decryptByte
;Author:			Evan Richter
;Function: Decrypts a byte of data by XORing it with a key byte.  Returns the
;           decrypted byte in the same register the encrypted byte was passed in.
;           Expects both the encrypted data and key to be passed by value.
;Inputs:	R8-R9, cryptByte, key
;Outputs:	R8, plainByte
;Registers destroyed:	R8
;-------------------------------------------------------------------------------

decryptByte:
	xor.b	R9, R8
	ret

;-------------------------------------------------------------------------------
;           System Initialization
;-------------------------------------------------------------------------------
	.global __STACK_END				; BOILERPLATE
	.sect 	.stack					; BOILERPLATE
	.sect   ".reset"				; BOILERPLATE		MSP430 RESET Vector
	.short  main					; BOILERPLATE

;-------------------------------------------------------------------------------
;	Name:			Evan Richter
;	Term:			Fall 2014
;	MCU:			MSP430G2553
;	Assignment:		Lab 2
;	Date:			16 September 2014
;	Documentation:	none
;	Functionality:	B functionality
;-------------------------------------------------------------------------------
	.cdecls C,LIST,"msp430.h"	; BOILERPLATE	Include device header file
 	.text						; BOILERPLATE	Assemble into program memory
	.retain						; BOILERPLATE	Override ELF conditional linking and retain current section
	.retainrefs					; BOILERPLATE	Retain any sections that have references to current section
	.global main				; BOILERPLATE	Project -> Properties and select the following in the pop-up
								; Build -> Linker -> Advanced -> Symbol Management
								;    enter main into the Specify program entry point... text box

key:	.byte	0xac,0xdf,0x23
keyLen:	.byte	0x03
crypt:	.byte	0xf8,0xb7,0x46,0x8c,0xb2,0x46,0xdf,0xac,0x42,0xcb,0xba,0x03,0xc7,0xba,0x5a,0x8c,0xb3,0x46,0xc2,0xb8,0x57,0xc4,0xff,0x4a,0xdf,0xff,0x12,0x9a,0xff,0x41,0xc5,0xab,0x50,0x82,0xff,0x03,0xe5,0xab,0x03,0xc3,0xb1,0x4f,0xd5,0xff,0x40,0xc3,0xb1,0x57,0xcd,0xb6,0x4d,0xdf,0xff,0x4f,0xc9,0xab,0x57,0xc9,0xad,0x50,0x80,0xff,0x53,0xc9,0xad,0x4a,0xc3,0xbb,0x50,0x80,0xff,0x42,0xc2,0xbb,0x03,0xdf,0xaf,0x42,0xcf,0xba,0x50,0x8f
msgLen:	.byte	0x52
;-------------------------------------------------------------------------------
;          		main: sets up registers 12-15 and places a value on the stack for
;					  use by decryptMessage
;
;-------------------------------------------------------------------------------
main:
	mov.w   #__STACK_END,SP			; BOILERPLATE	Initialize stackpointer
	mov.w   #WDTPW|WDTHOLD,&WDTCTL 	; BOILERPLATE	Stop watchdog timer

	;set up registers
	mov.w	#keyLen, R11	; keyLength
	mov.b	@R11, R11
	mov.w	#crypt, R12		; msgStart*
	mov.w	#0x0200, R13	; ramStart*
	mov.w	#msgLen, R14	; msgLength
	mov.b	@R14, R14
	mov.w	#key, R15		; key*
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
;Inputs:	R11: keyLength
;			R12: msgStart*
;			R13: ramStart*
;			R14: msgLength
;			R15: key*
;Outputs:	decrypted string to RAM
;Registers destroyed: none
;-------------------------------------------------------------------------------

decryptMessage:
	push.w	R8
	push.w	R9
	push.w	R11
	push.w	R12
	push.w	R13
	push.w	R14
	push.w	R15

checkEndString:
	cmp.w	#0, R14			; msgLength == 0 ?
	jeq		endString
	mov.b	@R12+, R8
	cmp.w	#0, R11			; keyLength == 0 ?
	jne		endKey
	mov.w	#keyLen, R11	; restore keyLength
	mov.b	@R11, R11
	mov.w	#key, R15		; restore key*

endKey:
	mov.b	@R15+, R9		; put key in register for decrypt
	dec.w	R11				; keyLength used as counter
	dec.w	R14				; msgLength used as counter
	call	#decryptByte
	mov.b	R8, 0(R13)
	inc.w	R13
	jmp		checkEndString
endString:
	;string done

	pop.w	R15
	pop.w	R14
	pop.w	R13
	pop.w	R12
	pop.w	R11
	pop.w	R9
	pop.w	R8
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

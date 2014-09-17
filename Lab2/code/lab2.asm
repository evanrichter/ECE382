;-------------------------------------------------------------------------------
;	Name:			Evan Richter
;	Term:			Fall 2014
;	MCU:			MSP430G2553
;	Assignment:		Lab 2
;	Date:			16 September 2014
;	Documentation:	none
;	Functionality:	A Functionality
;					"Fast. Neat. Average. Friendly. Good. Good."
;-------------------------------------------------------------------------------
	.cdecls C,LIST,"msp430.h"	; BOILERPLATE	Include device header file
 	.text						; BOILERPLATE	Assemble into program memory
	.retain						; BOILERPLATE	Override ELF conditional linking and retain current section
	.retainrefs					; BOILERPLATE	Retain any sections that have references to current section
	.global main				; BOILERPLATE	Project -> Properties and select the following in the pop-up
								; Build -> Linker -> Advanced -> Symbol Management
								;    enter main into the Specify program entry point... text box

key:	.byte	0x73, 0xBe
keyLen:	.byte	0x02
crypt:	.byte	0x35,0xdf,0x00,0xca,0x5d,0x9e,0x3d,0xdb,0x12,0xca,0x5d,0x9e,0x32,0xc8,0x16,0xcc,0x12,0xd9,0x16,0x90,0x53,0xf8,0x01,0xd7,0x16,0xd0,0x17,0xd2,0x0a,0x90,0x53,0xf9,0x1c,0xd1,0x17,0x90,0x53,0xf9,0x1c,0xd1,0x17,0x90
msgLen:	.byte	0x2a
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

	mov.w	#0x0260, R15	; will store keys in RAM temporarily
	mov.w	#0x0000, 0(R15)	; start with key 0x0000

	mov.w	#0x03, R9		; R9 holds most number of spaces found in any plain text
	mov.w	#0x00, R10		; R10 holds number of spaces found in the plain text
							; the ' ' or 0x20 character is most common in an English string

; will iterate over many keys, saving keys that had valid chars, and more than 3 spaces
	jmp		countSpaces

rankKey:
	cmp		#0x04, R10	; R10 >= 4?
	jl		foundGood	; if yes, a key that produced many spaces was found
	jmp		nextKey		; good key not found, try next key

foundGood:
	mov.b	0(R15), 2(R15)		; store any better keys after this one in RAM
	mov.b	1(R15), 3(R15)
	incd.w	R15
	jmp		nextKey

nextKey:
	inc.b	1(R15)
	jnc		nooverflow
	inc.b	0(R15)
nooverflow:
	jc		testedAll			; only called if larger byte just carried over
								; keys from 0x0000 to 0xFFFF have been tested!
	jmp		countSpaces

countSpaces:
	mov.w	#0x00, R10			; clear ' ' count
	call	#decryptMessage
	cmp.w	#0xFFFF, R10		; did key return bad chars?
	jne		foundGood
	jmp		nextKey


testedAll:
	; after running the code, I had a list of possible keys with only one element
	; 0x73be
	mov.w	#key, R15		; substitute the best key found back in
	call	#decryptMessage
	jmp		trapcpu

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
	push.w	R12
	push.w	R13
	push.w	R14
	push.w	R15
	push.w	R11

checkEndString:
	cmp.w	#0, R14			; msgLength == 0 ?
	jeq		endString
	mov.b	@R12+, R8
	cmp.w	#0, R11			; keyLength == 0 ?
	jne		endKey
	pop.w	R11				; restore keyLength
	pop.w	R15				; restore key*
	push.w	R15
	push.w	R11

endKey:
	mov.b	@R15+, R9		; put key in register for decrypt
	dec.w	R11				; keyLength used as counter
	dec.w	R14				; msgLength used as counter
	call	#decryptByte
	mov.b	R8, 0(R13)
	inc.w	R13
	cmp.b	#0x20, R8		; check if ' '
	jeq		goodChar
	cmp.b	#0x2e, R8		; check if '.'
	jeq		goodChar
	cmp.b	#0x41, R8		; R8 < 0x41 ?
	jl		badChar
	cmp.b	#0x5b, R8		; R8 >= 0x5b ?
	jge		checkMid		; if yes, check if between ASCII upper and lower case letters
	jmp		goodChar
checkMid:
	cmp.b	#0x61, R8		; R8 < 0x61 ?
	jl		badChar
	cmp.b	#0x7b, R8		; R8 >= 0x7b ?
	jge		badChar
	jmp		goodChar
goodChar:
	jmp		checkEndString
badChar:
	mov.w	#0xFFFF, R10	; main will check if R10 = xFFFF
	jmp		endString
endString:
	;string done

	pop.w	R11
	pop.w	R15
	pop.w	R14
	pop.w	R13
	pop.w	R12
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

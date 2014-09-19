Subroutines & Cryptography Lab
===

Objectives & Purpose
---
The objective for this lab is to practice assembly code programming skills, specifically writing and working with subroutines. Two subroutines are required: one will implement pass-by-value and one will use pass-by-reference. The problem is to decrypt a string of cipher text that had been encoded with XOR. My three tasks are to decrypt cipher text of arbitrary length with a key:

1. of length 1 byte
2. of length multi-byte
3. that is unknown

The plain text will be in ASCII format.

Preliminary Design 
---
![](./images/lab2_prelim.jpg)

The handwritten visualization above was enough for me to write up a pseudocode implementation of B funtionality. Using this work, I went straight to implementing the B functionality in assembly.

```
const char [] cryptText = 0x11, 0x22, 0x34 ...

void main() {
	//setup initial values
	traverseString();
}

char decryptByte(cryptByte R8, key R9) {
	xor.b R9, R8
	return plainByte R8
}

void traverseString(msgStart* R11, ramStart* R12, msgLength R13, key* R14, keyLength R15) {
	push R8, R9, R11, R12, R13, R14, R15
	
	for (; R13 > 0; R13-- R15--) {
		mov.b @R11+, R8
		if (R15 == 0) {
			pop R15
			pop R14
			push R14
			push R15
			mov.b @R14+, R9
		}
		decryptByte(R8, R9)
		mov.b R9, 0(R12)
		inc.w R12
	}
	
	pop R15, R14, R13, R12, R11, R9, R8
	return
}
```

Software Flow-Chart / Algorithms
---


Debugging & Testing
---
### Method
When debugging my program...

Conclusion & Lessons Learned
---



Documentation
---
none

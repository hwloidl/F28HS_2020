@ From: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0040d/Bgbbfgia.html
@ Example 5.9: strcpy
@ An example for LDR r0,= pseuod-instruction and a loop over bytes
	
.text
.global _start
_start:
		BL		print_strs					@ print contents of srcstr and dststr
		
		LDR		R1, =srcstr					@ Pointer to first string
		LDR		R0, =dststr					@ Pointer to second string
		BL		strcopy						@ Call subroutine to do copy.

		BL		print_strs					@ print contents of srcstr and dststr

exit:		MOV 		R7, #1
		SWI 		0

@ stop:		MOV		R0, #0x18					@ angel_SWIreason_ReportException
@ 		LDR		R1, =0x20026					@ ADP_Stopped_ApplicationExit
@		SWI 		0x123456					@ Angel semihosting ARM SWI

@ ============================================	
strcopy:	
		LDRB		R2, [R1]					@ Load byte
		STRB		R2, [R0]					@ Store byte
		ADD             R1, #1						@  and update address.
		ADD             R0, #1						@  and update address.
		CMP		R2, #0						@ Check for zero terminator.
		BNE		strcopy						@ Keep going if not.
		BX              LR						@ Return
	        @ MOV		PC,LR						@ Return
@ ============================================	

print:
		MOV 		R7, #4
		MOV 		R0, #1
		MOV 		R2, #maxsize
		MOV 		R1, R5
		SWI 		0
		BX 		LR

print_strs:									@ print contents of srcstr and dststr
		LDR 		R5, =srcstr					@ Pointer to first string
		MOV 		R7, #4
		MOV 		R0, #1
		MOV 		R2, #maxsize
		MOV 		R1, R5
		SWI 		0
		LDR 		R5, =dststr					@ Pointer to second string
		MOV 		R7, #4
		MOV 		R0, #1
		MOV 		R2, #maxsize
		MOV 		R1, R5
		SWI 		0
		BX		LR
	
.data
	.equ maxsize, 32
srcstr:
	.asciz	"First string: source\n"
	.org srcstr+maxsize
@ srclen:
@ 	.zero (maxsize+srcstr-$)

dststr:
	.asciz	"Second string: destination\n"
	.org dststr+maxsize



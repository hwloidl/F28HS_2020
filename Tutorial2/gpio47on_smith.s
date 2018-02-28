@@ GPIO PIN access via memory mapping file to Controller
@@ From: http://www.brucesmith.info/resources-assembly-language-raspbian/
@@ "Raspberry Pi Assembly Language", Bruce Smith
@@	
@@ This example sets GPI0 47 (the ACT LED) (number is hard-coded into the assembler code)
@@
@@ Compile:  gcc -o gpio47on_smith gpio47on_smith.s
@@ Run:	     sudo ./gpio47on_smith
@@ -----------------------------------------------------------------------------
	
	.global main

main:
	SUB	SP, SP, #16		@ Create 16 bytes storage
	LDR	R0, .addr_file		@ get GPIO Controller addr
	LDR	R1, .flags		@ set flag permissions
	BL	open			@ call to get file handle

	STR	R0, [SP, #12]		@ File handle number
	LDR	R3, [SP, #12]		@ Get File handle
	STR	R3, [SP, #0]		@ Store file handle on top stack
	LDR	R3, .gpiobase		@ get GPIO_Base address
	STR	R3, [SP, #4]		@ store on SP+4
	MOV	R0, #0			@ R0=0
	MOV	R1, #4096		@ R1=page
	MOV	R2, #3			@ R2=3
	MOV	R3, #1			@ R3=1 (stdouts)
	BL	mmap			@ call libc fct for mmap

	STR	R0, [SP, #16]		@ store virtual mem addr
	LDR	R3, [SP, #16]		@ get virtual mem addr
fctsel:	
	ADD	R3, R3, #16		@ add 16 for block 4 (GPFSEL4)
	LDR	R2, [SP, #16]		@ get virtual mem addr
	ADD	R2, R2, #16		@ add 16 for block 4 (GPFSEL4)
	LDR	R2, [R2, #0]		@ load R2 with value at R2
	BIC	R2, R2, #0b111<<21	@ Bitwise clear of three bits
	STR	R2, [R3, #0]		@ Store result in Register [set input]
	LDR	R3, [SP, #16]		@ Get virtual mem address
	ADD	R3, R3, #16		@ Add 16 for block 4 (GPFSEL4)
	LDR	R2, [SP, #16]		@ Get virtual mem addr
	ADD	R2, R2, #16		@ add 16 for block 4 (GPFSEL4)
	LDR	R2, [R2, #0]		@ Load R2 with value at R2
	ORR	R2, R2, #1<<21		@ Set bit....
	STR	R2, [R3, #0]		@ ...and make output
on:	
	LDR	R3, [SP, #16]		@ get virt mem addr
	ADD	R3, R3, #32		@ add 32 to offset to set register GPSET1
	MOV	R4, #1			@ get 1
	MOV	R2, R4, LSL#15		@ Shift by pin number
	STR	R2, [R3, #0]		@ write to memory
	LDR	R0, [SP, #12]		@ get file handle
	BL	close			@ close file

	ADD	SP, SP, #16		@ restore SP
	
	MOV R7, #1
	SWI 0


.addr_file:	.word	.file
.flags:		.word	1576962
@.gpiobase:	.word	0x20200000	@ GPIO_Base for Pi 1
.gpiobase:	.word	0x3F200000	@ GPIO_Base for Pi 2

.data
.file:	.ascii "/dev/mem\000"

	
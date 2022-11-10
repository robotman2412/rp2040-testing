	.cpu cortex-m0plus
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.data
	.align	2
	.type	lmao, %object
	.size	lmao, 4
lmao:
	.word	0xdeadbeef
	.text
	.global	dot
	.global	dot.LPIC1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	dot, %function
dot:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	add	r7, sp, #0
	ldr	r2, .L3
.LPIC0:
	add	r2, pc
	ldr	r3, .L3+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	movs	r0, r3
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L4:
	.align	2
.L3:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC0+4)
	.word	lol(GOT)
	.size	dot, .-dot

	.cpu cortex-m0plus
	.file	"main.S"

	.global __ptrtab

	.text
	.align	1
	.global	test
	.arch armv6s-m
	.syntax unified
	.code	16
	.thumb_func
	.type	test, %function
test:
	@ Preserve some stuff.
	push {lr}
	push {r7}
	
	ldr r7, =__ptrtab+8
	ldr r7, [r7]
	mov r12, r7
	
	pop {r7}
	blx r12
	
	pop	{pc}
	
	.size	test, .-test

@---------------------------------------------------------------------------------
	.section ".init"
@---------------------------------------------------------------------------------
	.global _start
	.align	4
	.arm
_start:
	stmfd  SP!, {R0-R3,R12,LR}     @save registers
	mrs	r12, cpsr
	stmfd  SP!, {R12}     @save cpsr
	
	ldr	r1,=0x36333333
	mcr	p15, 0, r1, c5, c0, 2
	
	.word	0xBF8DA5ED		@ Magic number to identify this region
	.word	0x00000000
	.word	0xFFFFFFFF
	
	ldr	r1,=0x06333333
	mcr	p15, 0, r1, c5, c0, 2
	
	ldmfd   sp!, {r12}	@ load cpsr
	msr	cpsr, r0		@ restore cpsr
	ldmfd  SP!, {R0-R3,R12,LR}     @load registers
	.word	0xBF8DA5EE		@ Magic number to identify this region
@---------------------------------------------------------------------------------
	@.align
	@.pool
	@.end
@---------------------------------------------------------------------------------

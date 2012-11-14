@---------------------------------------------------------------------------------
	.section ".init"
@---------------------------------------------------------------------------------
	.global _start
	.align	4
	.arm
_start:

	stmfd  SP!, {R10,r11}     @save registers 2 used			@1S+2N (9I)
	mrs	r10, cpsr @read it										@1S
	orr	r11, r10, #0x80 @no irq									@1S
	msr	cpsr, r11 @set it										@1S
	ldr r11, = 0x027C2000										@1S+1N+1I
	stmfd  r11!, {R0-R3,R12,LR,SP}     @save registers			@6S+2N
	
	
	.word	0xBF8DA5EC		@ Magic number to identify this region
	.word	0x00000000
	
	mov SP,r11													@1S
	
	ldr	r11,=0x36333333											@1S+1N+1I
	mcr	p15, 0, r11, c5, c0, 2									@1N+bI+1C @b is unknown
	
	
	.word	0xBF8DA5ED		@ Magic number to identify this region
	.word	0x00000000
	
	
	BIC r11,r11,#0x30000000 @ldr	r11, =0x06333333				@1S
	mcr	p15, 0, r11, c5, c0, 2									@1N+bI+1C @b is unknown
	
	ldmfd  SP!, {R0-R3,R12,LR,SP}     @load registers				@7S+1N+1I
	msr	cpsr, R10		@ restore cpsr							@1S
	
	ldmfd  SP!, {R10,r11} @load registers 2 used				@1S+2N (9I)
	
	.word	0xBF8DA5EE		@ Magic number to identify this region
@---------------------------------------------------------------------------------
	@.align
	@.pool
	@.end
@---------------------------------------------------------------------------------

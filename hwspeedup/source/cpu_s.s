	.text


	.align 4
	.code 32
	.arm
	
	.global cpuSetramreg
cpuSetramreg:
	mcr	p15, 0, r0, c6, c7, 0
	bx lr
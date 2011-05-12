	.text

	.equ REG_IME, 0x04000208

	.align 4
	.code 32
	.arm
	
	.global puSetGbaIWRAM
puSetGbaIWRAM:
	ldr	r0,=( (0b01110 << 1) | 0x03000000 | 1)	
	mcr	p15, 0, r0, c6, c2, 0
	bx lr
	
	.global cpu_GetMemPrem
cpu_GetMemPrem:
	mrc p15, 0, r0, c5, c0, 2
	bx lr

	.global puSetMemPerm
puSetMemPerm:
	mcr p15, 0, r0, c5, c0, 2
	bx lr
	
	.global pu_Enable
pu_Enable:
 	mrc	p15,0,r0,c1,c0,0
 	orr	r0,r0,#1
	mcr	p15,0,r0,c1,c0,0
	bx lr
	
	.global cpu_SetCP15Cnt
cpu_SetCP15Cnt:
	mcr p15, 0, r0, c1, c0, 0
	bx lr
			
	.global cpu_GetCP15Cnt
cpu_GetCP15Cnt:
	mrc p15, 0, r0, c1, c0, 0
	bx lr		
 
	.global cpu_ArmJump
cpu_ArmJump:
	bic r0, r0, #1
	bx r0

	.global cpu_GetCP15Cnt
	
cpu_GbaSaveRegs:

	.global cpuGetCPSR
cpuGetCPSR:
	mrs r0, cpsr
	bx lr

	.global pu_SetRegion
pu_SetRegion:
	ldr	r2, =_puSetRegion_jmp
	add	r2, r0, lsl #3
	bx		r2
_puSetRegion_jmp:
	mcr	p15, 0, r1, c6, c0, 0
	bx		lr
	mcr	p15, 0, r1, c6, c1, 0
	bx		lr
	mcr	p15, 0, r1, c6, c2, 0
	bx		lr
	mcr	p15, 0, r1, c6, c3, 0
	bx		lr
	mcr	p15, 0, r1, c6, c4, 0
	bx		lr
	mcr	p15, 0, r1, c6, c5, 0
	bx		lr
	mcr	p15, 0, r1, c6, c6, 0
	bx		lr
	mcr	p15, 0, r1, c6, c7, 0
	bx		lr

	.global pu_SetDataPermissions
pu_SetDataPermissions:

	mcr	p15, 0, r0, c5, c0, 2
	bx		lr

	.global pu_SetCodePermissions
pu_SetCodePermissions:
	mcr	p15, 0, r0, c5, c0, 3
	bx		lr

	.global pu_SetDataCachability
pu_SetDataCachability:
	mcr	p15, 0, r0, c2, c0, 0
	bx		lr

	.global pu_SetCodeCachability
pu_SetCodeCachability:
	mcr	p15, 0, r0, c2, c0, 1
	bx		lr

	.global pu_GetWriteBufferability 
pu_GetWriteBufferability:
	mcr	p15, 0, r0, c3, c0, 0
	bx		lr







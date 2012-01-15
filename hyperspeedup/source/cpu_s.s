	.text

	.equ REG_IME, 0x04000208

	.align 4
	.code 32
	.arm
	
	.global cpuSetCPSR
cpuSetCPSR:
	msr cpsr, r0
	bx lr
	
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
@      reg[13].I = 0x03007F00;

@      reg[R13_IRQ].I = 0x03007FA0;
@      reg[R13_SVC].I = 0x03007FE0;

	bic r0, r0, #1
	bx r0

	.global cpu_ArmJumpforstackinit
cpu_ArmJumpforstackinit:
@      reg[13].I = 0x03007F00;

@      reg[R13_IRQ].I = 0x03007FA0;
@      reg[R13_SVC].I = 0x03007FE0;

	mov	r2, #0x12		@ Switch to IRQ Mode
	msr	cpsr, r2
	ldr	sp, =0x03007FA0		@ Set IRQ stack

	mov	r2, #0x13		@ Switch to SVC Mode
	msr	cpsr, r2
	ldr	sp, =0x03007FE0		@ Set SVC stack
	

	mov	r2, #0x1F		@ Switch to System Mode
	msr	cpsr, r2
	ldr sp,=0x03007F00
	
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




@---------------------------------------------------------------------------------
	.global swiIntrWaitc
@---------------------------------------------------------------------------------
swiIntrWaitc:
@---------------------------------------------------------------------------------

	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4
	bx	lr

@---------------------------------------------------------------------------------
	.global testcode
@---------------------------------------------------------------------------------
testcode:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
	.global testasm
@---------------------------------------------------------------------------------
testasm:
@---------------------------------------------------------------------------------

	mov r0, #0
	mov r1, #0
	mov r2, #0
	mov r3, #0
	mov r4, #0
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	mov r9, #0
	mov r10, #0
	@stmia	r0!, {r1-r15}
	@BKPT 0x2D0 @silent debug call
	@stmia	r0!, {r1-r15}
	bx	lr


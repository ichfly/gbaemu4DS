	.text

	.equ REG_IME, 0x04000208

	.align 4
	.code 32
	.arm
	
	.global readbankedsp
readbankedsp:
	push {r1-r4}
	
	
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r0, r0, #0x1F
	
	
	cmp r0,#0x10 @ichfly user is system
	moveq r0,#0x1F
	
	orr	r4, r4, r0
	msr	cpsr, r4	@ hop, c'est fait
	
	mov r0,sp
	
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"
	
	@swi 0x2D0000
	
	pop {r1-r4}
	
	bx lr
	
	.global readbankedlr
readbankedlr:
	push {r1-r4}
	
	
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r0, r0, #0x1F
	
	
	cmp r0,#0x10 @ichfly user is system
	moveq r0,#0x1F
	
	orr	r4, r4, r0
	msr	cpsr, r4	@ hop, c'est fait
	
	mov r0,lr
	
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"
	
	
	pop {r1-r4}
	bx lr
	
	
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

	.global cpuGetSPSR
cpuGetSPSR:
	mrs r0, spsr
	bx lr
	
	
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




@swicode

@---------------------------------------------------------------------------------
	.global ichflyswiWaitForVBlank
@---------------------------------------------------------------------------------
ichflyswiWaitForVBlank:
@---------------------------------------------------------------------------------
	mov	r0, #1
	mov	r1, #1
	mov	r2, #0
@---------------------------------------------------------------------------------
	.global ichflyswiIntrWait
@---------------------------------------------------------------------------------
ichflyswiIntrWait:
@---------------------------------------------------------------------------------

	push {lr}
	cmp	r0, #0
	blne	testirq

wait_irq:
	@swi	#(6<<16) @ichfly my code
	
	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4	
	
	@ichfly einschub
	
	mrs	r2, cpsr
	bic	r3, r2, #0xC0
	
	
	
	@push regs
	push {r0-r3}
	sub r0,sp,#4*17 @+1 res you know
	ldr r1,=exRegs
	mov r2,#4*16
	BLX memcpy
	pop {r0-r3}
	
	sub sp,sp,#0x58
	ldr	r12, =SPtoloadswi	@save old stack
	str sp, [r12]
	add sp,sp,#0x58
	
	mov r12,sp
	
	msr	cpsr,r3 @irq
	msr cpsr,r2
	
	mov sp,r12
	ldr	r12,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r12, c5, c0, 2	

	@pop regs
	push {r0-r3}
	sub r1,sp,#4*17 @+1 res you know
	ldr r0,=exRegs
	mov r2,#4*16
	BLX memcpy
	pop {r0-r3}



	@ichfly my code end
	
	bl	testirq
	beq	wait_irq
	pop {lr}
	bx	lr

testirq:
	mov	r12, #0x4000000
	strb	r12, [r12,#0x208]
	ldr	r3, [r12,#-8]
	ands	r0, r1,	r3
	eorne	r3, r3,	r0
	strne	r3, [r12,#-8]
	mov	r0, #1
	strb	r0, [r12,#0x208]
	bx	lr



@---------------------------------------------------------------------------------
	.global ichflyswiHalt
@---------------------------------------------------------------------------------
ichflyswiHalt:
@---------------------------------------------------------------------------------

	push {r12}
	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4
	pop {r12}
	bx	lr
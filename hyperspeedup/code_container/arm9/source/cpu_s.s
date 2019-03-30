.arch	armv4t
.cpu arm7tdmi
.section	.itcm,"ax",%progbits

.equ REG_IME, 0x04000208

.align 4
.code 32
.arm
	
__sp_undef	=	__dtcm_top - 0x100;	@ichfly @ 1.792 Byte

	.global readbankedsp
   .type   readbankedsp STT_FUNC
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
	
	pop {r1-r4}
	
	bx lr
	
	.global readbankedlr
   .type   readbankedlr STT_FUNC
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
   .type   cpuSetCPSR STT_FUNC
cpuSetCPSR:
	msr cpsr, r0
	bx lr
	
	.global puSetGbaIWRAM
   .type   puSetGbaIWRAM STT_FUNC
puSetGbaIWRAM:
	ldr	r0,=( (0b01110 << 1) | 0x03000000 | 1)	
	mcr	p15, 0, r0, c6, c2, 0
	bx lr
	
	.global cpu_GetMemPrem
   .type   cpu_GetMemPrem STT_FUNC

cpu_GetMemPrem:
	mrc p15, 0, r0, c5, c0, 2
	bx lr

	.global puSetMemPerm
   .type   puSetMemPerm STT_FUNC

puSetMemPerm:
	mcr p15, 0, r0, c5, c0, 2
	bx lr
	
	.global pu_Enable
   .type   pu_Enable STT_FUNC

pu_Enable:
 	mrc	p15,0,r0,c1,c0,0
 	orr	r0,r0,#1
	mcr	p15,0,r0,c1,c0,0
	bx lr
	
	.global cpu_SetCP15Cnt
   .type   cpu_SetCP15Cnt STT_FUNC

cpu_SetCP15Cnt:
	mcr p15, 0, r0, c1, c0, 0
	bx lr
			
	.global cpu_GetCP15Cnt
   .type   cpu_GetCP15Cnt STT_FUNC
cpu_GetCP15Cnt:
	mrc p15, 0, r0, c1, c0, 0
	bx lr		
 
	.global cpu_ArmJump
   .type   cpu_ArmJump STT_FUNC

cpu_ArmJump:
@      reg[13].I = 0x03007F00;

@      reg[R13_IRQ].I = 0x03007FA0;
@      reg[R13_SVC].I = 0x03007FE0;

	bic r0, r0, #1
	bx r0

	.global cpu_ArmJumpforstackinit
   .type   cpu_ArmJumpforstackinit STT_FUNC

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
   .type   cpu_GetCP15Cnt STT_FUNC
	
cpu_GbaSaveRegs:

	.global cpuGetSPSR
   .type   cpuGetSPSR STT_FUNC
cpuGetSPSR:
	mrs r0, spsr
	bx lr
	
	
	.global cpuGetCPSR
   .type   cpuGetCPSR STT_FUNC
cpuGetCPSR:
	mrs r0, cpsr
	bx lr
	
	.global pu_SetRegion
   .type   pu_SetRegion STT_FUNC
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
   .type   pu_SetDataPermissions STT_FUNC

pu_SetDataPermissions:

	mcr	p15, 0, r0, c5, c0, 2
	bx		lr

	.global pu_SetCodePermissions
   .type   pu_SetCodePermissions STT_FUNC

pu_SetCodePermissions:
	mcr	p15, 0, r0, c5, c0, 3
	bx		lr

	.global pu_SetDataCachability
   .type   pu_SetDataCachability STT_FUNC

pu_SetDataCachability:
	mcr	p15, 0, r0, c2, c0, 0
	bx		lr

	.global pu_SetCodeCachability
   .type   pu_SetCodeCachability STT_FUNC

pu_SetCodeCachability:
	mcr	p15, 0, r0, c2, c0, 1
	bx		lr

	.global pu_GetWriteBufferability 
   .type   pu_GetWriteBufferability STT_FUNC

pu_GetWriteBufferability:
	mcr	p15, 0, r0, c3, c0, 0
	bx		lr




@swicode

@---------------------------------------------------------------------------------
	.global ichflyswiWaitForVBlank
   .type   ichflyswiWaitForVBlank STT_FUNC

@---------------------------------------------------------------------------------
ichflyswiWaitForVBlank:
@---------------------------------------------------------------------------------
	mov	r0, #1
	mov	r1, #1
	mov	r2, #0
@---------------------------------------------------------------------------------
	.global ichflyswiIntrWait
   .type   ichflyswiIntrWait STT_FUNC

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
	mov lr,pc
	BL memcpy
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
	mov lr,pc
	BL memcpy
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
   .type   ichflyswiHalt STT_FUNC

@---------------------------------------------------------------------------------
ichflyswiHalt:
@---------------------------------------------------------------------------------

	push {r12}
	mov r12 ,#0
	mcr p15,0,r12,c7,c0,4
	pop {r12}
	bx	lr
	



@coto: sleep mode protection for gba
@---------------------------------------------------------------------------------
	.global backup_mpu_setprot
	.type   backup_mpu_setprot STT_FUNC
@---------------------------------------------------------------------------------
backup_mpu_setprot:

	push {r6,r7}
	@read MPU mode (should be NDS)
	ldr	r6, =MPUPERMBACKUPSET_SWI		@MPU current Protection Settings for Data Region are backd
	mrc	p15, 0, r7, c5, c0, 2      
	str	r7, [r6]
	pop {r6,r7}
	bx lr
	
	
@-----------------------------------
	.global restore_mpu_setprot
	.type   restore_mpu_setprot STT_FUNC
@---------------------------------------------------------------------------------
restore_mpu_setprot:

	push {r6,r7}
	@restore MPU mode (should be NDS anyway)
	ldr	r6, =MPUPERMBACKUPSET_SWI  @MPU current Protection Settings for Data Region are restored
	ldr	r7, [r6]
	mcr	p15, 0, r7, c5, c0, 2	
	pop {r6,r7}
	
	bx lr

	
	
@---------------------------------------------------------------------------------
	.global resettostartup
	.type   resettostartup STT_FUNC

@---------------------------------------------------------------------------------
resettostartup:
@---------------------------------------------------------------------------------
	B main
	

@---------------------------------------------------------------------------------
	.global copyMode_5
	.type   copyMode_5 STT_FUNC
@---------------------------------------------------------------------------------
copyMode_5: @r0 = src r1 =tar
@---------------------------------------------------------------------------------
	push {r4-r11,lr}
	
	mov r2 , #0x80000000
	add r2 ,r2, #0x8000

	mov r3 , #0x8 @8 times 8*4*9
loop:

	LDMIA r0!, {r4-r12,r14}
	orr	r4, r4, r2
	orr	r5, r5, r2
	orr	r6, r6, r2
	orr	r7, r7, r2
	orr	r8, r8, r2
	orr	r9, r9, r2
	orr	r10, r10, r2
	orr	r11, r11, r2
	orr	r12, r12, r2
	orr	r14, r14, r2	
	STMIA r1!, {r4-r12,r14}
	
	subs r3, #1
	BNE loop
	
	LDMIA r0!, {r4-r12} @end with one missing times 8*4
	orr	r4, r4, r2
	orr	r5, r5, r2
	orr	r6, r6, r2
	orr	r7, r7, r2
	orr	r8, r8, r2
	orr	r9, r9, r2
	orr	r10, r10, r2
	orr	r11, r11, r2
	orr	r12, r12, r2	
	STMIA r1!, {r4-r12}
	
	pop {r4-r11,pc}
	
	
@---------------------------------------------------------------------------------
	.global copyMode_3
	.type   copyMode_3 STT_FUNC
@---------------------------------------------------------------------------------
copyMode_3: @r0 = src r1 =tar
@---------------------------------------------------------------------------------
	push {r4-r11,lr}
	
	mov r2 , #0x80000000
	add r2 ,r2, #0x8000

	mov r3 , #13 @13 times
loop2:

	LDMIA r0!, {r4-r12,r14}
	orr	r4, r4, r2
	orr	r5, r5, r2
	orr	r6, r6, r2
	orr	r7, r7, r2
	orr	r8, r8, r2
	orr	r9, r9, r2
	orr	r10, r10, r2
	orr	r11, r11, r2
	orr	r12, r12, r2
	orr	r14, r14, r2	
	STMIA r1!, {r4-r12,r14}
	
	subs r3, #1
	BNE loop2
	
	LDMIA r0!, {r4-r6} @end with one missing
	orr	r4, r4, r2
	orr	r5, r5, r2
	orr	r6, r6, r2
	STMIA r1!, {r4-r6}
	
	pop {r4-r11,pc}
	
	


@
@			Copyright (C) 2017  Coto
@This program is free software; you can redistribute it and/or modify
@it under the terms of the GNU General Public License as published by
@the Free Software Foundation; either version 2 of the License, or
@(at your option) any later version.

@This program is distributed in the hope that it will be useful, but
@WITHOUT ANY WARRANTY; without even the implied warranty of
@MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
@General Public License for more details.

@You should have received a copy of the GNU General Public License
@along with this program; if not, write to the Free Software
@Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
@USA
@


.arm
.code 32

.global 	IRQWait
.type   	IRQWait STT_FUNC
IRQWait:

	stmdb sp!, {r0-r7, lr}
	mov r3,r0

	@r0 = IE/dummy / r1 = IRQtowaitfor / r2 = IF /r3 = reentrant / r4 = dummy/BIOSflags / r5 = IME saved / r6 = 0x04000000 /r7 = BIOSflags addr
	mov r6,#0x04000000
	ldr r5,[r6,#0x208]	@IME saved
	mov r0,#1
	str r0,[r6,#0x208]	@force IME = 1
	
waitirq:
	add	r7, r6, #0x210
	ldmia r7, {r0,r2}	@r0 = IE / r2 = IF
	
	@BIOS Flag
	#ifdef ARM7
	ldr	r7, =__irq_flags
	#endif
	#ifdef ARM9
	ldr	r7, =__irq_flags
	#endif
	ldr r4,[r7]
	
	and r2,r2,r0			@IF & IE
	orr r2,r2,r4			@orr swi flags
	
	@sleep if interrupt to wait for (r1) hasnt happened.
	tst r2,r1
	
	#ifdef ARM7
	@4000301h - NDS7 - Halt function (HALTCNT) - Low Power Mode Control (R/W)
	ldrne	r2, =0x04000301
	movne 	r3,#(2<<6)			@2=Halt
	strne 	r3,[r2]
	#endif
	
	#ifdef ARM9
	@NDS9	-	Halt function	CP15
	movne 	r0,#0
	mcrne 	p15,0,r0,c7,c0,4				@low power mode: waitforIrq CP15
	#endif
	
	bne waitirq		@retry until r1 happens
	
	orr r4,r4,r1	@add in bios flags
	str r4,[r7]
	
	str r5,[r6,#0x208]	@restore IME
exitirq:
	ldmia sp!, {r0-r7, lr}
	
bx r14

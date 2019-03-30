/*---------------------------------------------------------------------------------

	Copyright (C) 2005
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.


---------------------------------------------------------------------------------*/
.arch	armv4t
.cpu arm7tdmi

#include "ichflysettings.h"


@new stacks todo mix some stacks
__sp_undef	=	__dtcm_top - 0x100;	@ichfly @ 1.792 Byte
__sp_svc	=	__sp_undef - 0x700;	@ichfly @ 4.096 Byte
__sp_irq	=	__sp_svc  - 0x1000; @ichfly @ 1.024 Byte each @also in interruptDispatcher.s

.section .itcm,"ax",%progbits

.extern	irqTable
.code 32


.global	IntrMain, __cpsr_mask
.type   IntrMain STT_FUNC
@---------------------------------------------------------------------------------
IntrMain:
@---------------------------------------------------------------------------------
	mov	r12, #0x4000000		@ REG_BASE
		
	add	r12, r12, #0x210
	ldmia	r12, {r1,r2}
	ands	r1, r1, r2
	ldr	r2, =irqTable


@---------------------------------------------------------------------------------
findIRQ:
@---------------------------------------------------------------------------------
	ldr r0, [r2, #4]
	cmp	r0, #0
	beq	no_handler
	ands	r0, r0, r1
	bne	jump_intr
	add	r2, r2, #8
	b	findIRQ

@---------------------------------------------------------------------------------
no_handler:
@---------------------------------------------------------------------------------
#ifndef gba_handel_IRQ_correct
	str	r1, [r12, #4]	@ IF Clear
	@help the emu
	
	ldr r0, =IF
	ldrh r2, [r0]
	orr r1,r1,r2
	strh r1, [r0]
	ldr r0, =ioMem
	ldr r0,[r0]
	add	r0, r0, #0x200
	add	r0, r0, #0x2 @don't like todo
	strh r1,[r0]
#endif
		
	mov	pc,lr

@---------------------------------------------------------------------------------
jump_intr:
@---------------------------------------------------------------------------------
	ldr	r1, [r2]		@ user IRQ handler address
	cmp	r1, #0
	bne	got_handler
	mov	r1, r0
	b	no_handler
@---------------------------------------------------------------------------------
got_handler:
@---------------------------------------------------------------------------------
#ifndef advanced_irq_check
	str	r0, [r12, #4]	@ IF Clear
#endif


	mrs	r12, spsr


#ifdef gba_handel_IRQ_correct

	@ldr r0,=SPtoload
	@ldr r0,[r0]
	ldr r0,=__sp_irq
	
	stmfd	r0!, {r12,SP,lr}	@ {spsr,SP, lr_irq}


	mov SP,r0
	
	mov lr,pc
	bx	r1
	
exitichfly:

	ldmfd   SP, {r0,SP,lr}	@ {spsr,SP, lr_irq}

	
#else
	stmfd	sp!, {r12,lr}	@ {spsr, lr_irq}

	@leave irq mode

	mov r2,lr
	
	mrs	r12, cpsr
	push {r12}
	mov r0,sp

	mov	r3, r12
	ldr	r12, =SPtemp
	ldr sp,[r12]
	bic	r3, r3, #0xdf		@ \__
	orr	r3, r3, #0x11		@ /  --> Enable IRQ & FIQ. Set CPU mode to Fiq. @so the pointer don't swap
	msr	cpsr,r3

	mov sp,r0
	
	mov lr,r2
	
	adr	lr, IntrRet
	bx	r1

@---------------------------------------------------------------------------------
IntrRet:
@---------------------------------------------------------------------------------

	mov r0,sp
	
	mrs	r2, cpsr
	mov	r3, r2
	bic	r3, r3, #0xdf		@ \__
	orr	r3, r3, #0xd2		@ /  --> Disable IRQ & FIQ. Set CPU mode to IRQ. @so the pointer don't swap
	msr	cpsr,r3
	
	ldr r12,=SPtemp
	str sp,[r12]@save new stack size change
	
	mov sp,r0


	
	
	
	pop	{r2}
	msr	cpsr, r2
exitichfly:

	ldmfd   sp!, {r0,lr}	@ {spsr, lr_irq}

#endif



	msr	spsr, r0		@ restore spsr
	
	mov	pc,lr

	.pool
	.end

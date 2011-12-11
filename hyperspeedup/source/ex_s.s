	@MRS SP, CPSR
	@ORR SP, SP, #0xC0 @don't need that no FIRQ 
	@MSR CPSR_fsxc, SP
	@LDR SP, =0x27FFD9C
	@ADD SP, SP, #1 @dont know why this is not working
	@STMDB SP!, {r12,lr}
	@MRS lr,spsr
	@MRC P15, 0 ,r12, c1,c0,0
	@STMDB SP!, {r12,lr}
	@BIC r12, r12, #0x1
	@MCR P15,0,r12,c1,c0,0
	@BIC r12,sp, #0x1
	@LDR r12, [r12, #0x10]
	@cmp r12, #0x0 @speedup
	@ldr r12, =exMain
	@BLX r12
	@LDMIA SP!, {r12,lr}







	.section	.itcm,"ax",%progbits

	.equ REG_BASE,	0x04000000
	.equ REG_IME,	0x04000208

	.align 4
	.code 32
	.arm



__sp_svc	=	__dtcm_top - 0x100;
__sp_irq	=	__sp_svc - 0x100;
__sp_usr	=	__sp_irq - 0x100;
__sp_exc = __sp_usr - 0x100;


@========== Exception code ====================
	
	.section	.vectors,"ax",%progbits
	
		.global irqhandler2
irqhandler2:
b	inter_Reset
b	inter_undefined
b	inter_swi
b	inter_fetch
b	inter_data
b	inter_res
b	inter_irq
b	inter_fast
b	inter_res2


inter_irq:
	STMDB SP!, {R0-R3,R12,LR}
	MRC P15, 0 ,r0, c9,c1,0
	Mov r0, r0, LSR #0xC
	Mov r0, r0, LSL #0xC
	ADD r0,r0, #0x4000
	ADD lr,pc,#0
	LDR pc, [r0, #-0x4]
	LDMIA SP!, {R0-R3,R12,LR}
	SUBS pc, lr, #0x4
	
inter_swi:



	@ change the PU to nds mode
	ldr	SP,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2
	ldr	SP, =exRegs
	
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	
	@ save the registres 0->12
	stmia	SP, {r0-r12}
	
	@ jump into the personal handler
	ldr	r1, =exHandlerswi
	ldr	r1, [r1]
	
	
	ldr	sp, =__sp_exc	@ use the new stack
	
	#mov lr,pc @ichfly change back if possible
	#bx r1
	blx	r1 @ichfly change back if possible
	
	@ restore the registres 0->12
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
	
	ldr	lr, [lr, #(15 * 4)] 
	
	@add lr,lr,#4
	
	@subs    pc, lr, #4
	
	subs    pc, lr, #0 @ichfly this is not working	
	
inter_data:
inter_undefined:
inter_fetch:
inter_res:
inter_fast:
inter_res2:
inter_Reset: @all debug we say all is debug
	
	
	@ change the PU to nds mode
	ldr	SP,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2


@R0,R1,R4,R8 change

	
	@ichfly my code
	
	ldr	SP, =exRegs

	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)


	
	@ save the registres 0->12
	stmia	SP!, {r0-r12}
	

	
	
	MRS r1,spsr	
	ldr r0, =BIOSDBG_SPSR

	
	mov r2,SP

	str	r1, [r0]	@ charge le SPSR
	@ change the mode  @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r1, #0x1F
	
	
	cmp r1,#0x10 @ichfly user is system
	moveq r1,#0x1F
	
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	
	stmia r2, {r13-r14} @save the registrers	@ on sauvegarde les registres bankés (r13 et r14)
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"




	@ jump into the personal handler @ on appelle l'handler perso
	ldr	r12, =exHandler
	ldr	r12, [r12]
	
	ldr	sp, =__sp_exc	@ use the new stack

	#mov lr,pc @ichfly change back if possible
	
	#bx r12
	blx	r12 @ichfly change back if possible
      
	
	@ need a better solution 
	mrc	p15, 0, r0, c5, c0, 2
	ldr	r1, =_exMain_tmpPu
	
	str	r0, [r1]
	
	
	
	ldr	r0,=0x33333333
	mcr	p15, 0, r0, c5, c0, 2
	

	@ restore SPSR @ on restaure les bankés
	ldr r0, =BIOSDBG_SPSR
	ldr	r1, [r0]	@ charge le SPSR
	MSR spsr,r1
	ldr	r0, =(exRegs + 13 * 4)
	cmp r1,#0x10 @ichfly user is system
	moveq r1,#0x1F
	@change mode to the saved mode @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r1, #0x1F
	
	cmp r1,#0x10 @ichfly user is system
	moveq r1,#0x1F
	
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	ldmia r0, {r13-r14}	@restor r13 and r14  @ on restaure les registres bankés (r13 et r14). /!\ N'allons pas croire qu'on les a restauré dans notre contexte: en effet, on a changé de mode là !
	msr	cpsr, r3	@chagne to mode "normal"@ on revient au mode "normal"
	

	ldr	r12, =_exMain_tmpPu
	ldr	SP, [r12]
	
	mcr	p15, 0, SP, c5, c0, 2	
	
	@restore r0-r12 easy
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
			
	@restore PU from the handler @ restaure la protection du PU, comme voulue par l'handler perso
	

	
	
	@my speedup code
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #4
	
	
	.section	.dtcm,"ax",%progbits

		.global BIOSDBG_SPSR
BIOSDBG_SPSR:
	.word 0

_exMain_tmpPu:
	.word 0
			
	.global exHandler
exHandler:
	.word	0
	
	.global exHandlerswi
exHandlerswi:
	.word	0
			
	.global exPuProtection
exPuProtection:
	.word 0
	
	.global exRegs
exRegs:
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
		
	.pool
	.end

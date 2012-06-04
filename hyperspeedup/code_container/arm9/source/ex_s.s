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


#include "ichflysettings.h"



	.section	.itcm,"ax",%progbits

	.equ REG_BASE,	0x04000000
	.equ REG_IME,	0x04000208

	.align 4
	.code 32
	.arm



@__sp_svc	=	__dtcm_top - 0x100;   @ichfly old
@__sp_irq	=	__sp_svc - 0x100;
@__sp_usr	=	__sp_irq - 0x100;
@__sp_exc = __sp_usr - 0x100;

@new stacks todo mix some stacks @ichfly also old
@__sp_undef	=	__dtcm_top - 0x100;	@ichfly @ 1.792 Byte
@__sp_svc	=	__sp_undef - 0x700;	@ichfly @ 2.048 Byte
@__sp_data	=	__sp_svc   - 0x800; @ichfly @ 2.048 Byte
@__sp_irq	=	__sp_data  - 0x800; @ichfly @ 2.048 Byte each

@new stacks todo mix some stacks
__sp_undef	=	__dtcm_top - 0x100;	@ichfly @ 1.792 Byte
__sp_svc	=	__sp_undef - 0x700;	@ichfly @ 4.096 Byte
__sp_irq	=	__sp_svc  - 0x1000; @ichfly @ 1.024 Byte each

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

somethingfailed:

inter_Reset:
inter_res:
inter_fast:
inter_res2:
	str sp,[pc, #0x10]
	str lr,[pc, #0x10]
	ldr sp, =failcpphandler
	ldr lr, =exHandler
	str sp,[lr]
	b dointerwtf
	
	
.global savedsp
savedsp:
	.word 0
.global savedlr
savedlr:
	.word 0

_exMain_tmpPuplain:
	.word 0
.global SPtoload
SPtoload:
	.word __sp_irq
	
.global SPtemp
SPtemp: @lol not realy
	.word 0

#ifdef gba_handel_IRQ_correct

inter_irq:
	stmfd  SP!, {R0-R3,R12,LR} @save registers to SP_irq

	MRC P15, 0 ,r0, c9,c1,0 @get addr
	Mov r0, r0, LSR #0xC
	Mov r0, r0, LSL #0xC
	ADD r0,r0, #0x4000
	
	mrc	p15, 0, r2, c5, c0, 2 @set pu
	ldr	r1,=0x33333333
	mcr	p15, 0, r1, c5, c0, 2
	ldr	r1, =_exMain_tmpPuplain
	str	r2, [r1]
	

	
	ADD lr,pc,#0  @jump
	LDR pc, [r0, #-0x4]
	
	mov	r12, #0x4000000		@ REG_BASE
	ldr	r0, [r12, #0x214]	@get IF
	
	
	
	ldr	r1, =_exMain_tmpPuplain @set pu back
	ldr	r2, [r1] @ichfly
	mcr	p15, 0, r2, c5, c0, 2
	
		

	
	ldr	r2, =anytimejmpfilter
	ldr r2, [r2]
	ands r0,r0,r2 @ anytimejmpfilter und IF
	BNE	got_over_gba_handler


	LDMIA SP!, {R0-R3,R12,LR} @exit
	SUBS pc, lr, #0x4
	
	
got_over_gba_handler:

	ldr	r1, =0x03333333          @set pu
	mcr	p15, 0, r1, c5, c0, 2


nop @need this nops
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop

	@original from gba
#ifdef checkclearaddr

	ldr    R1,=0x03008000
	mov    R0,#0x4000000       @ptr+4 to 03FFFFFC (mirror of 03007FFC)
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R1, #-0x4]      @jump to [03FFFFFC] USER handler

#else

	mov    R0,#0x4000000       @ptr+4 to 03FFFFFC (mirror of 03007FFC)
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R0, #-0x4]      @jump to [03FFFFFC] USER handler
#endif
	
	ldr	r1, =_exMain_tmpPuplain @set pu back @ichfly einschub
	ldr	r2, [r1] @ichfly
	mcr	p15, 0, r2, c5, c0, 2	

	  
	ldmfd  SP!, {R0-R3,R12,LR} @restore registers from SP_irq  
	subs   PC,LR, #0x4         @return from IRQ (PC=LR-4, CPSR=SPSR)

#else
inter_irq:
	str sp,[pc, #-0xC] @ichfly sizechange
	ldr sp,=SPtoload
	ldr sp,[sp]
	STMDB SP!, {R0-R4,R12,LR}
	ldr r0,=SPtoload
	sub r1,sp,#0x3E4@ ichfly irq stack size 0x400
	str r1,[r0]

	MRC P15, 0 ,r0, c9,c1,0
	Mov r0, r0, LSR #0xC
	Mov r0, r0, LSL #0xC
	ADD r0,r0, #0x4000
	
	mrc	p15, 0, r2, c5, c0, 2 @ ichfly
	ldr	r1,=0x33333333
	mcr	p15, 0, r1, c5, c0, 2
	ldr	r1, =_exMain_tmpPuplain
	str	r2, [r1] @ichfly
		
	@BL IntrMain
	
	mov	r12, #0x4000000		@ REG_BASE
	ldr	r4, [r12, #0x214]		
	
	ADD lr,pc,#0
	LDR pc, [r0, #-0x4]
	
	ldr	r1, =_exMain_tmpPuplain
	ldr	r2, [r1] @ichfly
	

	

	mcr	p15, 0, r2, c5, c0, 2
	
	
	ldr	r2, =IME
	ldrh r2, [r2]
	cmp r2,#1
	BNE	noIME
		

	
	ldr	r2, =anytimejmpfilter
	ldr r2, [r2]
	ands r4,r4,r2 @ anytimejmpfilter und IF
	@ldr    R0,[pc,#-0x400]
	@cmp r4,#0 @the s flag in and
	BNE	got_over_gba_handler


noIME:

	LDMIA SP!, {R0-R4,R12,LR} @exit
	
	@ichfly marker todoooooooooooooooooooooooooo!
	str sp,[pc, #-0x90]  @ichfly sizechange
	
	
	
	ldr sp,[pc, #-0x90]  @ichfly sizechange
	SUBS pc, lr, #0x4
	
got_over_gba_handler:

	@nop @test
	@nop
	@nop

	ldr	r1, =0x03333333
	
	mcr	p15, 0, r1, c5, c0, 2
	
	LDMIA SP!, {R0-R4,R12,LR} @exit
	str sp,[pc, #-0xA8]  @ichfly sizechange
	ldr sp,[pc, #-0xA8]  @ichfly sizechange


nop @need this nops
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop


	@original from gba
	stmfd  SP!, {R0-R3,R12,LR} @save registers to SP_irq
#ifdef checkclearaddr

	ldr    R1,=0x03008000
	mov    R0,#0x4000000       @ptr+4 to 03FFFFFC (mirror of 03007FFC)
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R1, #-0x4]      @jump to [03FFFFFC] USER handler

#else

	mov    R0,#0x4000000       @ptr+4 to 03FFFFFC (mirror of 03007FFC)
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R0, #-0x4]      @jump to [03FFFFFC] USER handler
#endif
	
	ldr	r1, =_exMain_tmpPuplain @ichfly einschub
	ldr	r2, [r1] @ichfly
	
	
	mcr	p15, 0, r2, c5, c0, 2	

	  
	ldmfd  SP!, {R0-R3,R12,LR} @restore registers from SP_irq  
	subs   PC,LR, #0x4         @return from IRQ (PC=LR-4, CPSR=SPSR)
#endif

.global SPtoloadswi
SPtoloadswi:
	.word __sp_svc
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
	
	
	ldr	sp, =SPtoloadswi	@ use the new stack
	ldr sp, [sp]
	
	@mov lr,pc @ichfly change back if possible
	@bx r1
	blx	r1 @ichfly change back if possible
	
	
	ldr	r1, =SPtoloadswi	@save old stack
	str sp, [r1]
	
	@ restore the registres 0->12
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
	
	ldr	lr, [lr, #(15 * 4)] 
	
	@add lr,lr,#4
	
	@subs    pc, lr, #4
	
	subs    pc, lr, #0 @ichfly this is not working	
	




inter_fetch: @ break function todo

	subs    lr, lr, #0x8000000
	ldr		sp,=rom
	ldr		sp,[sp]
	add		lr,lr,sp
	subs    pc, lr, #4





inter_undefined:

	@ change the PU to nds mode
	ldr	SP,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2
	ldr	SP, =exRegs
	
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	
	@ save the registres 0->12
	stmia	SP, {r0-r12}
	
	@ jump into the personal handler
	ldr	r1, =exHandlerundifined
	ldr	r1, [r1]
	
	
	ldr	sp, =__sp_undef	@ use the new stack
	
	blx	r1 @ichfly change back if possible
	
	@ restore the registres 0->12
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
	
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #4 @ichfly this is not working	










dointerwtf:

inter_data:
	
	@ change the PU to nds mode
	ldr	SP,=0x33333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2

	
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
	
	ldr	sp, =__sp_undef	@ use the new stack

	blx	r12
      
	
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
	cmp r1,#0x10 @ichfly user is system  @todo need that?
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
	
	.global exHandlerundifined
exHandlerundifined:
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

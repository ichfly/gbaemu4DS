.arch	armv4t
.cpu arm7tdmi

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
__sp_irq	=	__sp_svc  - 0x1000; @ichfly @ 1.024 Byte each @also in interruptDispatcher.s

@========== Exception code ====================
	
	.section	.vectors,"ax",%progbits
	
		.global irqhandler2
irqhandler2:
b	inter_Reset + 0x1000000
b	inter_undefined + 0x1000000
b	inter_swi + 0x1000000
b	inter_fetch + 0x1000000
b	inter_data + 0x1000000
b	inter_res + 0x1000000
b	inter_irq + 0x1000000
b	inter_fast + 0x1000000
b	inter_res2 + 0x1000000

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
	stmfd  SP!, {R0-R3,R12,LR}     @save registers to SP_irq
	
	mrc	p15, 0, r2, c5, c0, 2      @set pu
	ldr	r1,=0x36333333
	mcr	p15, 0, r1, c5, c0, 2
	ldr	r1, =_exMain_tmpPuplain
	str	r2, [r1]
	

	
	BL IntrMain
	
	mov	r0, #0x4000000		@ REG_BASE @ptr+4 to 03FFFFFC (mirror of 03007FFC) from gbabios
	ldr	r1, [r0, #0x214]	@get IF
	
		

	
	ldr	r2, =anytimejmpfilter
	ldr r2, [r2]
	ands r1,r1,r2 @ anytimejmpfilter und IF
	BEQ	irqexitdirect
	
	
gba_handler:

	ldr	r1, =0x06333333          @set pu
	mcr	p15, 0, r1, c5, c0, 2

#ifdef checkclearaddr

	ldr    R1,=0x03008000
	@mov    R0,#0x4000000       @ptr+4 to 03FFFFFC (mirror of 03007FFC)
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R1, #-0x4]      @jump to [03FFFFFC] USER handler

#else

	@mov    R0,#0x4000000	   @ REG_BASE @ptr+4 to 03FFFFFC (mirror of 03007FFC) already done
	add    LR,PC,#0            @retadr for USER handler
	ldr    PC,[R0, #-0x4]      @jump to [03FFFFFC] USER handler
#endif
	
	ldr	r1, =_exMain_tmpPuplain @set pu back @ichfly einschub
	ldr	r2, [r1] @ichfly
	mcr	p15, 0, r2, c5, c0, 2	

	ldmfd  SP!, {R0-R3,R12,LR} @restore registers from SP_irq  
	subs   PC,LR, #0x4         @return from IRQ (PC=LR-4, CPSR=SPSR)
	
	

irqexitdirect:
	
	ldr	r1, =_exMain_tmpPuplain  @set pu back
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
	ldr	r1,=0x36333333
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

	ldr	r1, =0x06333333
	
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
	ldr	SP,=0x36333333	@ see cpumg.cpp for meanings protections
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
	
	mov lr,pc @ichfly change back if possible , coto: change back was possible
	bx r1
	
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
	ldr	SP,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, SP, c5, c0, 2
	ldr	SP, =exRegs
	
	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)
	
	@ save the registres 0->12
	stmia	SP, {r0-r12}
	
	@ jump into the personal handler
	ldr	r1, =exHandlerundifined
	ldr	r1, [r1]
	
	
	ldr	sp, =__sp_undef	@ use the new stack
	
	mov lr,pc
	bx	r1 @ichfly change back if possible , coto: change back was possible
	
	@ restore the registres 0->12
	ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
	
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #4 @ichfly this is not working	










dointerwtf:

inter_data:
	

	
	ldr	SP, =exRegs

	str	lr, [SP, #(15 * 4)]	@ save r15 (lr is r15)


	
	@ save the registres 0->12
	stmia	SP!, {r0-r12}
	
	@ change the PU to nds mode
	ldr	r7,=0x36333333	@ see cpumg.cpp for meanings protections
	mcr	p15, 0, r7, c5, c0, 2

	

	MRS r5,spsr

	
	mov r6,SP
#ifndef directcpu
	ldr r0, =BIOSDBG_SPSR
	str	r1, [r0]	@ charge le SPSR
#endif

	@ change the mode  @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r5, #0x1F
	
	
	cmp r1,#0x10 @ichfly user is system
	moveq r1,#0x1F
	
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	
	stmia r6, {r13-r14} @save the registrers	@ on sauvegarde les registres bankés (r13 et r14)
	msr	cpsr, r3	@ back to normal mode @ on revient au mode "normal"



#ifdef directcpu
	
#ifndef gba_handel_IRQ_correct
	BIC r5,r5,#0x80
#endif
	
	@ldr r1,=exRegs
	sub r1,SP,#13 * 4
	
	lsls r2,r5, #0x1A
	BMI itisTHUMB
	
itisARM:

	ldr r0, [LR, #-0x8]
	ldr	sp, =__sp_undef	@ use the new stack
	BL emuInstrARM @ is emuInstrARM(u32 opcode, s32 *R)
	B exitdirectcpu
itisTHUMB:
	ldrh r0, [LR,#-0x8]
	sub LR, #0x2
	str LR, [r1, #15*4]
	ldr	sp, =__sp_undef	@ use the new stack
	BL emuInstrTHUMB @ is emuInstrTHUMB(u16 opcode, s32 *R)
exitdirectcpu:
#else

	ldr	sp, =__sp_undef	@ use the new stack

	@ jump into the personal handler @ on appelle l'handler perso
	ldr	r12, =exHandler
	ldr	r12, [r12]


	blx	r12
	
#endif      

	

	@ restore SPSR @ on restaure les bankés
#ifndef directcpu
	ldr r0, =BIOSDBG_SPSR
	ldr	r5, [r0]	@ charge le SPSR
#endif
	MSR spsr,r5

	@change mode to the saved mode @ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r5, r5, #0x1F
	
	cmp r5,#0x10 @ichfly user is system
	moveq r5,#0x1F	
	
	orr	r4, r4, r5
	msr	cpsr, r4	@ hop, c'est fait
	ldmia r6, {r13-r14}	@restor r13 and r14  @ on restaure les registres bankés (r13 et r14). /!\ N'allons pas croire qu'on les a restauré dans notre contexte: en effet, on a changé de mode là !
	msr	cpsr, r3	@chagne to mode "normal"@ on revient au mode "normal"
	



	
	BIC SP,r7,#0x30000000 @ldr	SP, =0x06333333
	mcr	p15, 0, SP, c5, c0, 2



	@restore r0-r12 easy
	sub lr,r6,#13 * 4 @ldr	lr, =exRegs
	ldmia	lr, {r0-r12}
			
	@restore PU from the handler @ restaure la protection du PU, comme voulue par l'handler perso
	

	
	
	@my speedup code
	ldr	lr, [lr, #(15 * 4)] 
	
	subs    pc, lr, #4
	
	
.section	.dtcm,"ax",%progbits

.global MPUPERMBACKUPSET_SWI	@swi mpu save sleep mode
MPUPERMBACKUPSET_SWI:
	.word 0x00000000

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

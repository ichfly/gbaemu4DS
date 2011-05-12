	.section	.itcm,"ax",%progbits

	.equ REG_BASE,	0x04000000
	.equ REG_IME,	0x04000208

	.align 4
	.code 32
	.arm

@========== Exception code ====================

	.global exMain
exMain:
	@ réactive le PU en mode "libre"
	ldr	r12,=0x36333333	@ définition des protections
	mcr	p15, 0, r12, c5, c0, 2
	mrc	p15, 0, r12, c1, c0, 0	@ activation
	orr	r12, r12, #1
	mcr	p15, 0, r12, c1, c0, 0
	
	@ sauvegarde les registres 0->11 (pas bankés)
	ldr	r12, =exRegs
	stmia	r12, {r0-r11}
	
	@ on sauvegarde l'état des irq, puis on les bloque
	ldr	r0, =REG_BASE
	ldr	r1, [r0, #(REG_IME - REG_BASE)]
	str	r0, [r0, #(REG_IME - REG_BASE)]
	ldr	sp, =__sp_exc	@ définit une nouvelle stack
	stmfd	sp!, {r1}	@ sauvegarde IME

	@ maintenant, on va récuperer tous les registres pré-exception "spéciaux" (bankés, r12, et le pc)
	ldr 	r0, =0x027FFD90

	ldmia	r0, {r1, r2, r3}	@ récupere les infos du bios, dans l'ordre: le SPSR, R12 et le PC (R15)
	str	r3, [r12, #(15 * 4)]	@ enregistre r15
	str	r2, [r12, #(12 * 4)]	@ enregistre r12

	ldr r0, =(exRegs + 13 * 4)
	@ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r1, #0x1F
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	stmia r0, {r13-r14}	@ on sauvegarde les registres bankés (r13 et r14)
	msr	cpsr, r3	@ on revient au mode "normal"

	@ on appelle l'handler perso
	ldr	r12, =exHandler
	ldr	r12, [r12]
	blxne	r12
	
	@ sauvegarde l'état de protection du PU, puis le mets en mode "libre". on fait ca, parce ce qu'on présume que l'handler perso a remis un statut au pu pour après l'exception. mais nous, on a encore besoin de l'accès total (par exemple on touche à IME, au dtcm, etc)
	mrc	p15, 0, r0, c5, c0, 2
	ldr	r1, =_exMain_tmpPu
	str	r0, [r1]
	ldr	r0,=0x36333333
	mcr	p15, 0, r0, c5, c0, 2
	
	@ ok, maintenant y va falloir remettre tous les registres depuis notre buffer vers les "vrais" (enfin, les pré-exception quoi...)
	ldr	r0, =0x027FFD90

	@ on commence par les registres "spéciaux": r12 et le pc
	ldr	r1, =exRegs
	ldr	r12, [r1, #(12 * 4)]
	str	r12, [r0, #4]	@ on enregistre R12 dans les infos pour le BIOS (0x027FFD94)
	
	ldr	r12, [r1, #(15 * 4)]
	str	r12, [r0, #8]	@ pareil pour R15 (0x027FFD98)

	@ on restaure les bankés
	ldr	r1, [r0]	@ charge le SPSR
	ldr	r0, =(exRegs + 13 * 4)
	@ on change de mode (on se mets dans le mode qui était avant l'exception)
	mrs	r3, cpsr
	bic	r4, r3, #0x1F
	and	r1, r1, #0x1F
	orr	r4, r4, r1
	msr	cpsr, r4	@ hop, c'est fait
	ldmia r0, {r13-r14}	@ on restaure les registres bankés (r13 et r14). /!\ N'allons pas croire qu'on les a restauré dans notre contexte: en effet, on a changé de mode là !
	msr	cpsr, r3	@ on revient au mode "normal"
	
	@ on restaure le statut initial des irq
	ldr	r0, =REG_IME
	ldmfd	sp!, {r1}
	str	r1, [r0]
	
	@ on restaure les autres registres simples (r0->r11). /!\ Ceux là sont dans notre contexte !
	ldr	r12, =exRegs
	ldmia	r12, {r0-r11}
			
	@ restaure la protection du PU, comme voulue par l'handler perso
	ldr	r12, =_exMain_tmpPu
	ldr	r12, [r12]
	mcr	p15, 0, r12, c5, c0, 2
	
	ldr	sp, =0x027FFD8D	@ on restaure la pile du bios (addresse harcodée parce que le bios est con)
	
	@ on retourne au programme (code désassemblé du bios. normalement, on serait censé revenir au bios avec un "mov pc, lr" mais y me fait chier...)
	ldmia   sp!, {r12, lr}
	mcr     p15, 0, r12, c1, c0, 0
	msr     spsr_fsxc, lr
	ldmia   sp!, {r12, lr}
	subs    pc, lr, #4

_exMain_tmpPu:
	.word 0
			
	.global exHandler
exHandler:
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

asmCPUWriteHalfWordpu: @ in r2 addr r3 data can use r9-r12


	mov	r0, r3, lsr #24
	@mov	r2, r0
	cmp	r3, #14
	ldrls	pc, [pc, r3, asl #2]
	mov pc,lr
.L43:
	.fill 4 , 4 , .fallout
	.word	.caseUpdateRegister
	.word	.fallout
	.word	.fallout
	.word	.fallout
	.word	.caseROM
	.word	.caseROM
	.word	.fallout
	.word	.fallout
	.word	.fallout
	.word	.caseEEPROM
	.word	.caseFlash
.caseFlash:
	ldr	r10, .L52
	ldrb	r9, [r10, #0]	@ zero_extendqisi2
	ldr	r10, .L52+4
	ldrb	r11, [r10, #0]	@ zero_extendqisi2
	ldr	r10, .L52+8
	orr	r0, r11, r9
	ldrb	r1, [r10, #0]	@ zero_extendqisi2
	eor	r1, r1, #1
	orrs	r0, r0, r1
	moveq	pc,lr
	ldr	r9, .L52+12
	and	r1, r3, #255
	ldr	r9, [r9, #0]
	mov	r0, r2
	blx	r9
.LVL28:
.LDL1:
.L38:
.fallout:
	mov pc,lr
.LVL29:
.caseEEPROM:

	ldr	r9, .L52+16
	ldrb	r9, [r9, #0]	@ zero_extendqisi2
	cmp	r3, #0
	moveq pc,lr
	and	r1, r3, #255
	b	_Z11eepromWritejh
.LVL31:
.caseROM:

	ldr	r9, .L52+20
	bic	r10, r2, #2
.LVL32:
	cmp	r10, r9
	@mov	r0, r2
	beq	.dortcWrite
	add	r10, r10, #4
	cmp	r10, r9
	beq	.dortcWrite
	
	b	_Z13agbPrintWritejt
.LVL34:
.caseUpdateRegister:
	ldr	r9, .L52+24
	cmp	r9, r2
	movhi	pc,lr
	ldr	r0, .L52+28
.LVL35:
	and	r0, r2, r0
	mov r1,r3
	b	_Z17CPUUpdateRegisterjt
.LVL36:
.dortcWrite:
	mov r0, r2
	mov r1, r3
	b	_Z8rtcWritejt
.LVL37:
.L53:
	.align	4
.L52:
	.word	cpuFlashEnabled
	.word	cpuSramEnabled
	.word	eepromInUse
	.word	cpuSaveGameFunc
	.word	cpuEEPROMEnabled
	.word	0x80000C4
	.word	0x40003FF
	.word	0x3FE
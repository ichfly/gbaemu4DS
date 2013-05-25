asmCPUWriteBytepu: @ in r2 addr r3 data can use r9-r12


	mov	r9, r2, lsr #24 @r3
	cmp	r9, #13
	beq	.caseEEPROM
	cmp	r9, #14
	beq	.caseFlash
	cmp	r9, #4
	movne pc,lr
	@mov	r12, r1
	ldr	r9, .L21
	cmp	r2, r9
	movhi	pc,lr

	mov	r0, r2, asl #22
	mov	r0, r0, lsr #22
	sub	r1, r0, #0x90
	cmp	r1, #0xF
	bhi	.CPUUpdate
	.loc 1 812 0 discriminator 1
	mov	r10, #0x4000000
	mov	r11, #0x184
	ldrh	r1, [r10, r11]
	tst	r1, #2
	bne	.L15
	orr	r3, r1, #40000000
	orr	r9, r3, r9, asl #12
	mov	r3, #0x4000000
	str	r9, [r3, #0x188]
	mov pc , lr
.caseFlash:
	ldr	r9, .L21+4
	ldr	r9, [r3, #0]
	cmp	r9, #5
	moveq	pc,lr
	ldr	r9, .L21+8
	ldrb	r10, [r3, #0]	@ zero_extendqisi2
	ldr	r9, .L21+12
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r9, .L21+16
	orr	r2, r10, r2
	ldrb	r9, [r9, #0]	@ zero_extendqisi2
	eor	r9, r9, #1
	orrs	r9, r10, r9
	moveq pc,lr

	ldr	r3, .L21+20
	ldr	r3, [r3, #0]
	bx	r3
	
.caseEEPROM:
	ldr	r2, .L21+24
	ldrb	r2, [r2, #0]	@ zero_extendqisi2
	cmp	r2, #0
	moveq pc, lr
	
	mov r1 ,r3  @ichfly
	
	b	_Z11eepromWritejh

.CPUUpdate:
	ldr	r0, .L21+28
	tst	r2, #1
	and	r0, r2, r0
	ldr	r1, .L21+32
	beq	.L8

	ldrb	r1, [r0, r1]	@ zero_extendqisi2
	orr	r1, r1, r3, asl #8
	b	_Z17CPUUpdateRegisterjt
	
.L8:
	ldrh	r1, [r0, r1]
	bic	r1, r1, #255
	orr	r1, r3, r1
	b	_Z17CPUUpdateRegisterjt
.LVL12:
.L22:
	.align	4
.L21:
	.word	0x40003FF
	.word	saveType
	.word	cpuFlashEnabled
	.word	cpuSramEnabled
	.word	eepromInUse
	.word	cpuSaveGameFunc
	.word	cpuEEPROMEnabled
	.word	0x3FE
	.word	ioMem
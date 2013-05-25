asmCPUWriteMemorypu: @ in r2 addr r3 data can use r9-r12


	
	mov	r9, r2, lsr #24
	cmp	r9, #13
	beq	.caEEPROMa
	cmp	r9, #14
	beq	.othersava
	cmp	r9, #4
	movne pc, lr
	
.URa:

	ldr	r9, .L34
	cmp	r2, r9
	movhi	pc, lr
	
	mov	r10, r3
	
	and	r9, r2, #0x3FC
	mov	r3, r3, asl #16
	mov	r0, r9
	mov	r1, r3, lsr #16
	bl	_Z17CPUUpdateRegisterjt
	add	r0, r9, #2
	mov	r1, r10, lsr #16
	b	_Z17CPUUpdateRegisterjt
.othersava:
	ldr	r9, .L34+4
	ldrb	r1, [r9, #0]	@ zero_extendqisi2
	ldr	r9, .L34+8
	ldrb	r10, [r9, #0]	@ zero_extendqisi2
	ldr	r9, .L34+12
	orr	r10, r1, r10
	ldrb	r9, [r9, #0]	@ zero_extendqisi2
	eor	r3, r9, #1
	orrs	r9, r10, r9
	moveq	pc, lr

	ldr	r9, .L34+16
	and	r1, r3, #255
	mov r0, r2
	ldr	r9, [r9, #0]
	bx	r9
.caEEPROMa:
	ldr	r8, .L34+20
	ldrb	r8, [r8, #0]	@ zero_extendqisi2
	cmp	r8, #0
	moveq	pc, lr

	and	r1, r3, #0xFF @no one cares here only one bit used
	b	_Z11eepromWritejh


	.align	4
.L34:
	.word	0x40003FF
	.word	cpuFlashEnabled
	.word	cpuSramEnabled
	.word	eepromInUse
	.word	cpuSaveGameFunc
	.word	cpuEEPROMEnabled
	
	
	
	
	
	
	
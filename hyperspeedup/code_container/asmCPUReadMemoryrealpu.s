asmReadMemoryrealpu: @ in r2 addr out r3 data can use r9-r12


_ZL19CPUReadMemoryrealpuj:

	mov	r3, r2, lsr #24
	@mov	r4, r0

	cmp	r3, #14
	ldrls	pc, [pc, r3, asl #2]
	b	.fallout
.L121:
	.fill 4 , 4 , .fallout
	.word	.caseIO
	.word	.fallout
	.word	.fallout
	.word	.fallout
	.word	.caserom
	.word	.caserom
	.word	.caserom
	.word	.caserom
	.word	.caserom
	.word	.caseEEPROM
	.word	.caseFlash
.caserom:
	.loc 1 175 0
	bic	r3, r2, #0xFE000000
	ldr	r2, .L151 @romSize
	ldr	r2, [r2, #0]
	bic	r3, r3, #3
	cmp	r3, r2
	bhi	.overrom
	.loc 1 194 0
	ldr	r2, .L151+4
	ldr	r2, [r2, #0]
.LVL105:
.L148:
	ldr	r3, [r9, r3]
.LVL106:
.fallout:
	ands	r10, r2, #3
	movne	r10, r10, asl #3
	movne	r3, r3, ror r10
	mov pc,lr
.LVL110:
.caseFlash:
.LBB76:
	ldr	r3, .L151+8
	ldrb	r1, [r3, #0]	@ zero_extendqisi2
	ldr	r3, .L151+12
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	orrs	r3, r1, r3
	beq	.fallout
	mov r0, r2
	bl	_Z9flashReadj
.LVL111:
	mov	r2, r0
	b	.fallout
.LVL112:
.caseEEPROM:
	ldr	r3, .L151+16
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.fallout
	mov r0, r2
	bl	_Z10eepromReadj
	mov r3, r0
.LVL113:
.caseIO:
	ldr	r1, .L151+28 @0x4000200
	bic	r0, r2, #2
	cmp	r0, r1
	bne	.endIFupdate

	mov	r0, #0x4000000
	mov	r1, #0x214
	ldrh	r1, [r0, r1]
	ldr	r0, .L151+32 @IF
	ldr	r9, .L151+24 @ioMem
	strh	r1, [r0, #0]	@ movhi
	ldr	r0, .L151+36 @0x202
	strh	r1, [r9, r0]	@ movhi
	@b .stdIOread

	and	r3, r2, #0x3FC
	b	.L148
	
.endIFupdate:
	@relock
	add	r3, r2, #0xFC000000
	sub	r1, r3, #0x100
	cmp	r1, #0x10
	bhi	.endtimerupdate
	
	BIC	r1, r2, #0x3
	ldrh	r9, [r1, #0]
	ldr	r9, .L151+24   @ioMem
	and	r3, r2, #0x3FC
	mov	r1, r1, asr #1
	strh	r1, [r3, r9]	@ movhi
	@b .stdIOread
	and	r3, r2, #0x3FC
	b	.L148
	
.endtimerupdate:
	sub	r3, r3, #4
	cmp	r3, #3
	bls	.updateVCsub
	ldr	r3, .L151+40 @0x40003FF
	cmp	r4, r3
	bhi	.fallout
	b	.stdIOread
.overrom:
	ldr	r3, .L151+44 @ichflyfilestreamsize
	ldr	r3, [r2, #0]
	cmp	r3, r2
	movcs	r2, #0
	bcs	.fallout
.LBB69:
.LBB70:
	mov	r3, r2, lsr #9
	ldr	r2, .L151+48 @sectortabel
	ldr	r1, [r2, #0]
	and	r5, r2, #0x1FC
	ldr	r2, [r1, r3, asl #3]
	cmp	r2, #0
	beq	.rommnachladen
	ldr	r2, [r2, r5]
.LVL122:
	b	.fallout
.LVL123:
.updateVCsub:
.LBE70:
.LBE69:
.LBB72:
.LBB73:
	ldr	r0, .L151+52 @DISPSTAT
	mov	r1, #0x4000000
	ldrh	r5, [r1, #6]
	ldrh	r3, [r0, #0]
	ldrh	r9, [r1, #4]
	ldr	r1, .L151+56 @VCountdstogba
	and	r9, r9, #3
	ldrb	r1, [r1, r5]	@ zero_extendqisi2
	bic	r3, r3, #7
	orr	r3, r9, r3
	cmp	r1, r3, lsr #8
	ldr	r9, .L151+24 @ioMem
	ldr	r5, .L151+60 @VCOUNT
	@strh	r3, [r0, #0]	@ movhi @ichfly haha
	orreq	r3, r3, #4
	strh	r1, [r5, #0]	@ movhi
	strh	r1, [r9, #6]	@ movhi
	@streqh	r3, [r0, #0]	@ movhi @ichfly haha
	str		r3, [r0, #0]
	strh	r3, [r9, #4]	@ movhi
	and	r3, r2, #0x3FC
	b	.L148
.LVL129:
.stdIOread:
.LBE73:
.LBE72:
	ldr	r1, .L151+64 @ioReadable
	and	r3, r2, #0x3FC
	@ldrb	r0, [r1, r3]	@ zero_extendqisi2 @ichfly can't use
	@cmp	r0, #0
	@beq	.fallout
	add	r1, r1, r3
	ldrb	r9, [r1, #2]	@ zero_extendqisi2
	cmp	r9, #0
	ldr	r9, .L151+24  @ioMem
	bne	.L148
	ldrh	r3, [r3, r9]
.LVL131:
	b	.fallout
.LVL132:
.rommnachladen:
.LBB74:
.LBB71:
	add	r0, r1, r3, asl #3
	ldr	r10, .L151+68
	ldr	r9, .L151+72
	ldr	r11, [r10, #0]
	ldr	r12, .L151+76
	ldr	r11, [r9, r11, asl #2]
	ldr	r12, [r12, #0]
	str	r2, [r1, r11, asl #2]
	ldr	r2, [r10, #0]
	mov	r11, r3, asl #1
	add	r12, r12, r2, asl #9
	str	r11, [r9, r2, asl #2]
	str	r12, [r1, r3, asl #3]
	ldr	r3, .L151+80
	mov	r2, r12
	ldr	r3, [r3, #0]
	ldr	r0, [r0, #4]
	mov	r1, #1
	blx	r3
	ldr	r3, [r6, #0]
	cmp	r3, #254
	addne	r3, r3, #1
	moveq	r3, #0
	str	r3, [r10, #0]
	ldr	r2, [r7, r5]
	b	.fallout
.L152:
	.align	4
.L151:
	.word	romSize @0
	.word	rom
	.word	cpuSramEnabled
	.word	cpuFlashEnabled
	.word	cpuEEPROMEnabled @0x10
	.word	0x40003FC
	.word	ioMem
	.word	0x4000200
	.word	IF              @0x20
	.word	0x202
	.word	0x40003FF
	.word	ichflyfilestreamsize
	.word	sectortabel     @0x30
	.word	DISPSTAT
	.word	VCountdstogba
	.word	VCOUNT
	.word	ioReadable      @0x40
	.word	current_pointer
	.word	allocedfild
	.word	greatownfilebuffer
	.word	readSectorslocked @0x50
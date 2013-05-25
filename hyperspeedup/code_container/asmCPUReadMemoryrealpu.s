asmReadMemoryrealpu: @ in r2 addr out r3 data can use r9-r12


_ZL19CPUReadMemoryrealpuj:

	mov	r3, r2, lsr #24
	mov	r4, r0

	cmp	r3, #14
	ldrls	pc, [pc, r3, asl #2]
	b	.fallout
.L121:
	.fill 4 , 4 , .fallout
	.word	.L117
	.word	.fallout
	.word	.fallout
	.word	.fallout
	.word	.L118
	.word	.L118
	.word	.L118
	.word	.L118
	.word	.L118
	.word	.L119
	.word	.L120
.L118:
	.loc 1 175 0
	ldr	r2, .L151
	bic	r3, r0, #-33554432
	ldr	r2, [r2, #0]
	bic	r3, r3, #3
	cmp	r3, r2
	bhi	.L149
	.loc 1 194 0
	ldr	r2, .L151+4
	ldr	r2, [r2, #0]
.LVL105:
.L148:
	ldr	r2, [r2, r3]
.LVL106:
.fallout:
.LBB67:
	ands	r3, r2, #3
	movne	r9, r3, asl #3
	movne	r3, r9, ror r2
.LVL109:
.L134:
.LBE68:
.LBE67:
.LBE75:
	mov pc,lr
.LVL110:
.L120:
.LBB76:
	.loc 1 212 0
	ldr	r3, .L151+8
	ldrb	r1, [r3, #0]	@ zero_extendqisi2
	ldr	r3, .L151+12
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	orrs	r3, r1, r3
	beq	.fallout
	.loc 1 214 0
	bl	_Z9flashReadj
.LVL111:
	mov	r2, r0
	b	.L134
.LVL112:
.L119:
	.loc 1 204 0
	ldr	r3, .L151+16
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.fallout
.LBE76:
	.loc 1 262 0
	ldmfd	sp!, {r3, r4, r5, r6, r7, lr}
.LBB77:
	.loc 1 206 0
	b	_Z10eepromReadj
.LVL113:
.L117:
	.loc 1 138 0
	add	r3, r0, #-67108864
	sub	r1, r3, #256
	cmp	r1, #16
	bhi	.L122
	.loc 1 146 0
	ldr	r1, .L151+20
	and	r1, r0, r1
	ldrh	ip, [r1, #0]
	ldr	r1, .L151+24
	.loc 1 140 0
	and	r0, r0, #1020
.LVL114:
	.loc 1 146 0
	mov	ip, ip, asr #1
	strh	ip, [r0, r1]	@ movhi
.L122:
	.loc 1 150 0
	ldr	r1, .L151+28
	bic	r0, r4, #2
	cmp	r0, r1
	bne	.L123
	.loc 1 152 0
	mov	r0, #67108864
	mov	r1, #532
	ldrh	r1, [r0, r1]
	ldr	r0, .L151+32
	.loc 1 153 0
	ldr	ip, .L151+24
.LVL115:
	.loc 1 152 0
	strh	r1, [r0, #0]	@ movhi
	.loc 1 153 0
	ldr	r0, .L151+36
	strh	r1, [ip, r0]	@ movhi
.L123:
	.loc 1 158 0
	sub	r3, r3, #4
	cmp	r3, #3
	bls	.L150
	.loc 1 162 0
	ldr	r3, .L151+40
	cmp	r4, r3
	bhi	.fallout
	b	.L126
.LVfallout:
.L149:
	.loc 1 180 0
	ldr	r2, .L151+44
	ldr	r2, [r2, #0]
	cmp	r3, r2
	.loc 1 189 0
	movcs	r2, #0
	.loc 1 180 0
	bcs	.fallout
.LVL117:
.LBB69:
.LBB70:
	.loc 2 96 0
	ldr	r2, .L151+48
	.loc 2 94 0
	mov	r3, r3, lsr #9
.LVL118:
	.loc 2 96 0
	ldr	r1, [r2, #0]
	.loc 2 93 0
	and	r5, r0, #508
.LVL119:
	.loc 2 96 0
	ldr	r2, [r1, r3, asl #3]
.LVL120:
	add	r0, r1, r3, asl #3
.LVL121:
	.loc 2 98 0
	cmp	r2, #0
	beq	.L130
	ldr	r2, [r2, r5]
.LVL122:
	b	.fallout
.LVL123:
.L150:
.LBE70:
.LBE69:
.LBB72:
.LBB73:
	.loc 1 96 0
	ldr	r0, .L151+52
	.loc 1 78 0
	mov	r1, #67108864
.LVL124:
	ldrh	r5, [r1, #6]
.LVL125:
	.loc 1 96 0
	ldrh	r3, [r0, #0]
	.loc 1 79 0
	ldrh	ip, [r1, #4]
.LVL126:
	.loc 1 83 0
	ldr	r1, .L151+56
	.loc 1 99 0
	and	ip, ip, #3
.LVL127:
	.loc 1 83 0
	ldrb	r1, [r1, r5]	@ zero_extendqisi2
	.loc 1 96 0
	bic	r3, r3, #7
	.loc 1 99 0
	orr	r3, ip, r3
	.loc 1 103 0
	cmp	r1, r3, lsr #8
	.loc 1 102 0
	ldr	ip, .L151+24
	.loc 1 83 0
	ldr	r5, .L151+60
.LVL128:
	.loc 1 99 0
	strh	r3, [r0, #0]	@ movhi
	.loc 1 105 0
	orreq	r3, r3, #4
	.loc 1 83 0
	strh	r1, [r5, #0]	@ movhi
	.loc 1 102 0
	strh	r1, [ip, #6]	@ movhi
	.loc 1 105 0
	streqh	r3, [r0, #0]	@ movhi
	.loc 1 111 0
	strh	r3, [ip, #4]	@ movhi
.LVL129:
.L126:
.LBE73:
.LBE72:
	.loc 1 162 0 discriminator 1
	ldr	r1, .L151+64
.LVL130:
	and	r3, r4, #1020
	ldrb	r0, [r1, r3]	@ zero_extendqisi2
	cmp	r0, #0
	beq	.fallout
	.loc 1 163 0
	add	r1, r1, r3
	ldrb	r2, [r1, #2]	@ zero_extendqisi2
	cmp	r2, #0
	.loc 1 164 0
	ldr	r2, .L151+24
	.loc 1 163 0
	bne	.L148
	.loc 1 166 0
	ldrh	r2, [r3, r2]
.LVL131:
	b	.fallout
.LVL132:
.L130:
.LBB74:
.LBB71:
	.loc 2 100 0
	ldr	r6, .L151+68
	ldr	ip, .L151+72
	ldr	lr, [r6, #0]
	.loc 2 103 0
	ldr	r7, .L151+76
	.loc 2 100 0
	ldr	lr, [ip, lr, asl #2]
	.loc 2 103 0
	ldr	r7, [r7, #0]
	.loc 2 100 0
	str	r2, [r1, lr, asl #2]
	.loc 2 102 0
	ldr	r2, [r6, #0]
.LVL133:
	mov	lr, r3, asl #1
	.loc 2 103 0
	add	r7, r7, r2, asl #9
.LVL134:
	.loc 2 102 0
	str	lr, [ip, r2, asl #2]
	.loc 2 104 0
	str	r7, [r1, r3, asl #3]
	.loc 2 106 0
	ldr	r3, .L151+80
.LVL135:
	mov	r2, r7
	ldr	r3, [r3, #0]
	ldr	r0, [r0, #4]
	mov	r1, #1
	blx	r3
.LVL136:
	.loc 2 110 0
	ldr	r3, [r6, #0]
	cmp	r3, #254
	addne	r3, r3, #1
	moveq	r3, #0
	str	r3, [r6, #0]
	.loc 2 113 0
	ldr	r2, [r7, r5]
	b	.fallout
.L152:
	.align	2
.L151:
	.word	romSize
	.word	rom
	.word	cpuSramEnabled
	.word	cpuFlashEnabled
	.word	cpuEEPROMEnabled
	.word	67109884
	.word	ioMem
	.word	67109376
	.word	IF
	.word	514
	.word	67109887
	.word	ichflyfilestreamsize
	.word	sectortabel
	.word	DISPSTAT
	.word	VCountdstogba
	.word	VCOUNT
	.word	ioReadable
	.word	current_pointer
	.word	allocedfild
	.word	greatownfilebuffer
	.word	readSectorslocked
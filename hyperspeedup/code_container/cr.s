@_Z13emuInstrTHUMBtPi: @can use all regs execptioon r5/r6/r7 @ in r4 regs r8 asmcode

	
	ldr	pc, [pc, r0, asr #6] @ichfly my way lol because auto aline
	nop
	.fill 80 , 4 , .interrorme @.SPACE   80*4 @sub	r3, r2, #80 @todo do better interrorme

	.word	.STRRdRsRn
	.word	.STRRdRsRn
	.word	.STRHRdRsRn
	.word	.STRHRdRsRn
	.word	.STRBRdRsRn
	.word	.STRBRdRsRn
	.word	.LDSBRdRsRn
	.word	.LDSBRdRsRn
	.word	.LDRRdRsRn
	.word	.LDRRdRsRn
	.word	.LDRHRdRsRn
	.word	.LDRHRdRsRn
	.word	.LDRBRdRsRn
	.word	.LDRBRdRsRn
	.word	.LDSHRdRsRn
	.word	.LDSHRdRsRn
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.STRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.LDRRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.STRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.LDRBRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.STRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.LDRHRdRsImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.STRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.LDRR0R7SPImm
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.L5487
	.word	.L5488
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.L5489
	.word	.L5490
	.word	.interrorme
	.word	.interrorme
	.word	.L5491
	.word	.L5491
	.word	.L5491
	.word	.L5491
	.word	.L5491
	.word	.L5491
	.word	.L5491
	.word	.L5491
	.word	.L5492
	.word	.L5492
	.word	.L5492
	.word	.L5492
	.word	.L5492
	.word	.L5492
	.word	.L5492
	.word	.L5492

	
	
	
.STRRdRsRn:

	and	r8, r0, #7 @ichfly
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r0, [r4, r2]
	ldr	r3, [r4, r3]
	add	r2, r0, r3
	ldr	r3, [r4, r8, asl #2] 
	b	asmCPUWriteMemorypu
	
.STRHRdRsRn:

	and	r8, r0, #7 @ichfly
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	mov r8,r8, asl #2
	ldr	r0, [r4, r2]
	ldr	r3, [r4, r3]
	ldrh	r2, [r4, r8]
	add	r3, r0, r3
	b	_ZL18CPUWriteHalfWordpujt
	
.STRBRdRsRn:

	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r0, [r4, r2]
	ldr	r3, [r4, r3]
	ldrb	r3, [r4, r8, asl #2]	@ zero_extendqisi2
	add	r2, r0, r3
	b	_ZL14CPUWriteBytepujh
	
.LDSBRdRsRn:

	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r2, r0, r3
	bl	_ZL17CPUReadByteSignedrealpuj
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back
	
.LDRRdRsRn:

	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r2, r0, r3
	bl	asmCPUReadMemoryreal
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back
	
.LDRHRdRsRn:

	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r2, r0, r3
	bl	_ZL21CPUReadHalfWordrealpuj
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back
	
.LDRBRdRsRn:
	
	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r2, r0, r3
	bl	_ZL17CPUReadByterealpuj
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back

.LDSHRdRsRn:

	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r2, r0, r3
	bl	_ZL21CPUReadHalfWordSignedrealpuj
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back
	
.STRRdRsImm:

	and	r8, r0, #7
	mov	r3, r0, asr #1
	and	r3, r3, #7*4
	mov	r0, r0, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	ldr	r3, [r4, r8, asl #2]
	add	r2, r0, r3
	b	asmCPUWriteMemorypu
	
.LDRRdRsImm:

	and	r8, r0, #7
	mov	r3, r0, asr #1
	and	r3, r3, #7*4
	mov	r0, r0, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	add	r2, r0, r3
	bl	asmCPUReadMemoryreal
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back
	
.STRBRdRsImm:

	and	r8, r0, #7
	mov	r3, r0, asr #1
	and	r3, r3, #7*4
	mov	r0, r0, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	ldrb	r3, [r4, r8, asl #2]
	add	r2, r0, r3
	b	_ZL14CPUWriteBytepujh

.LDRBRdRsImm:

	and	r8, r0, #7
	mov	r3, r0, asr #1
	and	r3, r3, #7*4
	mov	r0, r0, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	add	r2, r0, r3
	bl	_ZL17CPUReadByterealpuj
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back

.STRHRdRsImm:
	
	and	r8, r0, #7
	mov	r3, r0, asr #1
	and	r3, r3, #7*4
	mov	r0, r0, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	ldrh	r3, [r4, r8, asl #2]
	add	r2, r0, r3
	b	_ZL18CPUWriteHalfWordpujt

.LDRHRdRsImm:

	and	r8, r0, #7
	mov	r3, r0, asr #1
	and	r3, r3, #7*4
	mov	r0, r0, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	add	r2, r0, r3
	bl	_ZL21CPUReadHalfWordrealpuj
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back

.STRR0R7SPImm:
	mov	r2, r0, asr #6
	ldr	r8, [r4, #52]
	and	r2, r2, #7*4
	and	r0, r0, #0xFF
	ldr	r3, [r4, r2]
	add	r2, r8, r0, asl #2
	b	asmCPUWriteMemorypu

.LDRR0R7SPImm:
	ldr	r3, [r4, #52]
	and	r0, r0, #255
	add	r2, r3, r0, asl #2
	and	r8, r2, #7
	bl	asmCPUReadMemoryreal
	str	r3, [r4, r8, asl #2]
	mov pc,lr @ jmp back
	
	
	
	


	
	
	
	
	
.interrorme:
	ldr	r3, [r8, #60]
	ldr	r0, .L5467
	ldrh	r1, [r3, #-6]
	bl	iprintf
	mov	r2, #0
	mov	r3, #67108864
	str	r2, [r3, #520]
	bl	_Z9debugDumpv
.L5466:
	b	.L5466
	
.align	2
.L5467:
	.word	.LC1
.LC1:
	.ascii	"Undefined THUMB instruction %04x\012\000"
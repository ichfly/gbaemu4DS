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
	.word	.PUSHRlist
	.word	.interrorme @.L5488 @no need there as there is no use about this
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme @.L5489 @no need there as there is no use about this
	.word	.interrorme @.L5490 @no need there as there is no use about this
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme @.L5491 @no need there as there is no use about this
	.word	.interrorme @.L5491 @no need there as there is no use about this
	.word	.interrorme @.L5491 @no need there as there is no use about this
	.word	.interrorme @.L5491 @no need there as there is no use about this
	.word	.interrorme @.L5491
	.word	.interrorme @.L5491
	.word	.interrorme @.L5491
	.word	.interrorme @.L5491
	.word	.interrorme @.L5492 @no need there as there is no use about this
	.word	.interrorme @.L5492
	.word	.interrorme @.L5492
	.word	.interrorme @.L5492
	.word	.interrorme @.L5492
	.word	.interrorme @.L5492
	.word	.interrorme @.L5492
	.word	.interrorme @.L5492

	
	
	
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
	
	
	
	
.PUSHRlist:

	ldr	r2, .L5756
	and	r3, r8, #255
	ldrb	r3, [r2, r3]	@ zero_extendqisi2
	ldr	r7, [r1, #52]
	
	tst	r8, #1
	
	sub	r7, r7, r3, asl #2

	bic	r6, r7, #3

	bne	.L5748
.backjmp1:
	tst	r4, #2
	bne	.L5749
.backjmp2:
	tst	r4, #4
	bne	.L5750
.backjmp3:
	tst	r4, #8
	bne	.L5751
.backjmp4:
	tst	r4, #16
	bne	.L5752
.backjmp5:
	tst	r4, #32
	bne	.L5753
.backjmp6:
	tst	r4, #64
	bne	.L5754
.backjmp7:
	tst	r4, #128
	beq	.PUSHRlistexit
	mov	r2, r7
	ldr	r3, [r5, #0x1C]
	bl	_ZL16CPUWriteMemorypujj

.PUSHRlistexit:
	str	r7, [r5, #52]
	mov pc,lr

	

.L5748:
	mov	r2, r6
	ldr	r3, [r1, #0]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp1

.L5749:
	mov	r2, r6
	ldr	r3, [r1, #4]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp2
.L5750:
	mov	r2, r6
	ldr	r3, [r1, #8]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp3
.L5751:
	mov	r2, r6
	ldr	r3, [r1, #0xC]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp4
.L5752:
	mov	r2, r6
	ldr	r3, [r1, #0x10]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp5
.L5753:
	mov	r2, r6
	ldr	r3, [r1, #0x14]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp6
.L5754:
	mov	r2, r6
	ldr	r3, [r1, #0x18]
	bl	_ZL16CPUWriteMemorypujj
	add	r6, r6, #4
	b	.backjmp7



.L5756:
	.word	cpuBitsSet



	
	
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
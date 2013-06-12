@_Z13emuInstrTHUMBtPi: @can use all regs execptioon r5/r6/r7 @ in r4 regs r8 asmcode

	
	ldr	pc, [pc, r8, asr #6] @ichfly my way lol because auto aline
	nop
	.fill 80,4   @,.interrorme @todo do better interrorme

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
	.word	.PUSHRlistLR
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.interrorme
	.word	.POPRlist
	.word	.POPRlistPC
	.word	.interrorme
	.word	.interrorme
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.STMR07Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist
	.word	.LDMR0R7Rlist

	
	
	
.STRRdRsRn:

	and	r8, r0, #7 @ichfly
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r0, [r4, r2]
	ldr	r3, [r4, r3]
	add	r0, r0, r3
	ldr	r1, [r4, r8, asl #2] 
	bl	_Z19asmCPUWriteMemorypujj
	b .exitthumbhandel
	
.STRHRdRsRn:

	and	r8, r0, #7 @ichfly
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	mov r8,r8, asl #2
	ldr	r0, [r4, r2]
	ldr	r3, [r4, r3]
	add	r0, r0, r3
	ldrh r1, [r4, r8]
	bl	_Z21asmCPUWriteHalfWordpujt
	b .exitthumbhandel
	
.STRBRdRsRn:

	and	r8, r0, #7
	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r0, [r4, r2]
	ldr	r3, [r4, r3]
	ldrb	r1, [r4, r8, asl #2]	@ zero_extendqisi2
	add	r0, r0, r3
	bl	_Z17asmCPUWriteBytepujh
	b .exitthumbhandel
	
.LDSBRdRsRn:

	mov	r2, r0, asr #4
	mov	r3, r0, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z17asmCPUWriteBytepujh
	
	lsl	r0, r0, #24 @make signed
	ASR	r0, r0, #24
	
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back
	
.LDRRdRsRn:

	mov	r2, r8, asr #4
	mov	r3, r8, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z22asmCPUReadMemoryrealpuj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back
	
.LDRHRdRsRn:

	mov	r2, r8, asr #4
	mov	r3, r8, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z24asmCPUReadHalfWordrealpuj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back
	
.LDRBRdRsRn:
	
	mov	r2, r8, asr #4
	mov	r3, r8, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z20asmCPUReadByterealpuj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back

.LDSHRdRsRn:

	mov	r2, r8, asr #4
	mov	r3, r8, asr #1
	and	r2, r2, #7*4
	and	r3, r3, #7*4
	ldr	r3, [r4, r3]
	ldr	r0, [r4, r2]
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z30asmCPUReadHalfWordrealpuSignedj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back
	
.STRRdRsImm:

	mov	r3, r8, asr #1
	and	r3, r3, #7*4
	mov	r0, r8, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	and	r8, r8, #7
	ldr	r1, [r4, r8, asl #2]
	add	r0, r0, r3
	bl	_Z19asmCPUWriteMemorypujj
	b .exitthumbhandel
	
.LDRRdRsImm:

	mov	r3, r8, asr #1
	and	r3, r3, #7*4
	mov	r0, r8, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z22asmCPUReadMemoryrealpuj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back
	
.STRBRdRsImm:

	mov	r3, r8, asr #1
	and	r3, r3, #7*4
	mov	r0, r8, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	and	r8, r8, #7
	ldrb	r1, [r4, r8, asl #2]
	add	r0, r0, r3
	bl	_Z17asmCPUWriteBytepujh
	b .exitthumbhandel

.LDRBRdRsImm:

	mov	r3, r8, asr #1
	and	r3, r3, #7*4
	mov	r0, r8, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z20asmCPUReadByterealpuj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back

.STRHRdRsImm:
	
	mov	r3, r8, asr #1
	and	r3, r3, #7*4
	mov	r0, r8, lsr #4
	and	r8, r8, #7
	mov r8, r8, asl #2
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	ldrh	r1, [r4, r8]
	add	r0, r0, r3
	bl	_Z21asmCPUWriteHalfWordpujt
	b .exitthumbhandel

.LDRHRdRsImm:

	mov	r3, r8, asr #1
	and	r3, r3, #7*4
	mov	r0, r8, lsr #4
	ldr	r3, [r4, r3]
	and	r0, r0, #0x7C
	add	r0, r0, r3
	and	r8, r8, #7
	bl	_Z24asmCPUReadHalfWordrealpuj
	str	r0, [r4, r8, asl #2]
	b .exitthumbhandel @ jmp back

.STRR0R7SPImm:
	and	r0, r8, #0xFF
	mov	r2, r8, asr #6
	ldr	r8, [r4, #52]
	and	r2, r2, #0x1C
	ldr	r1, [r4, r2]
	add	r0, r8, r0, asl #2
	bl	_Z19asmCPUWriteMemorypujj
	b .exitthumbhandel

.LDRR0R7SPImm:
	ldr	r3, [r4, #52]
	and	r0, r8, #0xFF
	mov	r8, r8, asl #6
	add	r0, r3, r0, asl #2
	and	r8, r8, #0x1C
	bl	_Z22asmCPUReadMemoryrealpuj
	str	r0, [r4, r8]
	b .exitthumbhandel @ jmp back
	
	
	
	
.PUSHRlist:

	ldr	r2, .L5756
	and	r3, r8, #0xFF
	ldrb	r3, [r2, r3]	@ zero_extendqisi2
	ldr	r9, [r4, #0xD*4]
	
	tst	r8, #1
	
	sub	r9, r9, r3, asl #2

	bic	r10, r9, #3

	bne	.L5748
.backjmp1:
	tst	r8, #2
	bne	.L5749
.backjmp2:
	tst	r8, #4
	bne	.L5750
.backjmp3:
	tst	r8, #8
	bne	.L5751
.backjmp4:
	tst	r8, #16
	bne	.L5752
.backjmp5:
	tst	r8, #32
	bne	.L5753
.backjmp6:
	tst	r8, #64
	bne	.L5754
.backjmp7:
	tst	r8, #128
	beq	.PUSHRlistexit
	mov	r0, r9
	ldr	r1, [r4, #0x1C]
	bl	_Z19asmCPUWriteMemorypujj

.PUSHRlistexit:
	str	r10, [r4, #0x34]
	b .exitthumbhandel


.L5748:
	mov	r0, r10
	ldr	r1, [r4, #0]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp1

.L5749:
	mov	r0, r10
	ldr	r1, [r4, #4]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp2
.L5750:
	mov	r0, r10
	ldr	r1, [r4, #8]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp3
.L5751:
	mov	r0, r10
	ldr	r1, [r4, #0xC]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp4
.L5752:
	mov	r0, r10
	ldr	r1, [r4, #0x10]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp5
.L5753:
	mov	r0, r10
	ldr	r1, [r4, #0x14]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp6
.L5754:
	mov	r0, r10
	ldr	r1, [r4, #0x18]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.backjmp7





.PUSHRlistLR:
	ldr	r2, .L5756
	ldr	r9, [r1, #52]
	and	r3, r8, #0xFF
	ldrb	r3, [r2, r3]	@ zero_extendqisi2
	sub	r9, r9, #4
	sub	r9, r9, r3, asl #2
	tst	r8, #1
	bic	r10, r9, #3
	bne	.L5740
.L5505:
	tst	r8, #2
	bne	.L5741
.L5506:
	tst	r8, #4
	bne	.L5742
.L5507:
	tst	r8, #8
	bne	.L5743
.L5508:
	tst	r8, #16
	bne	.L5744
.L5509:
	tst	r8, #32
	bne	.L5745
.L5510:
	tst	r8, #64
	bne	.L5746
.L5511:
	tst	r8, #128
	bne	.L5747
.L5512:
	tst	r8, #256
	beq	.L5710
	mov	r0, r10
	ldr	r1, [r4, #56]
	bl	_Z19asmCPUWriteMemorypujj
.L5710:
	str	r9, [r4, #52]
	b .exitthumbhandel


.L5740:
	mov	r0, r10
	ldr	r1, [r4, #0]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5505
.L5741:
	mov	r0, r10
	ldr	r1, [r4, #4]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5506

.L5742:
	mov	r0, r10
	ldr	r1, [r4, #8]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5507

.L5743:
	mov	r0, r10
	ldr	r1, [r4, #12]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5508

.L5744:
	mov	r0, r10
	ldr	r1, [r4, #16]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5509

.L5745:
	mov	r0, r10
	ldr	r1, [r4, #20]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5510

.L5746:
	mov	r0, r10
	ldr	r1, [r4, #24]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5511
	
.L5747:
	mov	r0, r10
	ldr	r1, [r4, #28]
	bl	_Z19asmCPUWriteMemorypujj
	add	r10, r10, #4
	b	.L5512



.POPRlistPC:
	ldr	r3, [r4, #52]
	ldr	r1, .L5756
	and	r2, r0, #255
	ldrb	r9, [r1, r2]	@ zero_extendqisi2
	tst	r0, #1
	bic	r10, r3, #3
	add	r9, r3, r9, asl #2
	bne	.L5733
.L5514:
	tst	r4, #2
	bne	.L5734
.L5515:
	tst	r4, #4
	bne	.L5735
.L5516:
	tst	r4, #8
	bne	.L5736
.L5517:
	tst	r4, #16
	bne	.L5737
.L5518:
	tst	r4, #32
	bne	.L5738
.L5519:
	tst	r4, #64
	bne	.L5739
.L5520:
	tst	r4, #128
	beq	.L5710
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	str	r0, [r4, #28]
@.L5710: @commentarry already exist
	str	r9, [r4, #52]
	b .exitthumbhandel
	

.L5733:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #0]
	b	.L5514
.L5734:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #4]
	b	.L5515

.L5735:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #8]
	b	.L5516

.L5736:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #12]
	b	.L5517

.L5737:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #16]
	b	.L5518

.L5738:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #20]
	b	.L5519

.L5739:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #24]
	b	.L5520


.POPRlist:
	ldr	r3, [r4, #52]
	ldr	r1, .L5756
	and	r2, r8, #0xFF
	ldrb	r9, [r1, r2]	@ zero_extendqisi2
	tst	r8, #1
	bic	r10, r3, #3
	add	r9, r3, r9, asl #2
	bne	.La5733
.La5514:
	tst	r8, #2
	bne	.La5734
.La5515:
	tst	r8, #4
	bne	.La5735
.La5516:
	tst	r8, #8
	bne	.La5736
.La5517:
	tst	r8, #16
	bne	.La5737
.La5518:
	tst	r8, #32
	bne	.La5738
.La5519:
	tst	r8, #64
	bne	.La5739
.La5520:
	tst	r4, #128
	beq	.La5529
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #28]
.La5529:
	mov	r0, r6
	bl	_Z22asmCPUReadMemoryrealpuj
	orr	r0, r0, #1 @thumb you know
	add	r0, r0, #4
	str	r0, [r4, #60]
.LVL4421:
@.L5710: @commentarry already exist
	str	r9, [r4, #52]
	b .exitthumbhandel
	

.La5733:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #0]
	b	.La5514
.La5734:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #4]
	b	.La5515

.La5735:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #8]
	b	.La5516

.La5736:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #12]
	b	.La5517

.La5737:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #16]
	b	.La5518

.La5738:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #20]
	b	.La5519

.La5739:
	mov	r0, r10
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r10, r10, #4
	str	r0, [r4, #24]
	b	.La5520




.STMR07Rlist:
	and	r9, r8, #7
	ldr	r1, .L5756
	ldr	r3, [r4, r9, asl #2]
	and	r2, r8, #0xFF
	ldrb	r11, [r1, r2]	@ zero_extendqisi2
	bic	r12, r3, #3
	add	r11, r3, r11, asl #2
	tst	r8, #1
	bne	.L5719
.L5530:
	tst	r8, #2
	bne	.L5720
.L5531:
	tst	r8, #4
	bne	.L5721
.L5533:
	tst	r8, #8
	bne	.L5722
.L5535:
	tst	r8, #16
	bne	.L5723
.L5537:
	tst	r8, #32
	bne	.L5724
.L5539:
	tst	r8, #64
	bne	.L5725
.L5541:
	tst	r8, #128
	beq .exitthumbhandel
	mov	r0, r12
	ldr	r1, [r4, #28]
	bl	_Z19asmCPUWriteMemorypujj
	streq	r11, [r4, r9, asl #2]
	b .exitthumbhandel


.L5719:
	mov	r0, r12
	ldr	r1, [r4, #0]
	bl	_Z19asmCPUWriteMemorypujj
	add	r12, r12, #4
	str	r11, [r4, r9, asl #2]
	b	.L5530

.L5720:
	mov	r0, r12
	ldr	r1, [r4, #4]
	bl	_Z19asmCPUWriteMemorypujj
	str	r11, [r4, r9, asl #2]
	add	r12, r12, #4
	b	.L5531

.L5721:
	mov	r0, r12
	ldr	r1, [r4, #8]
	bl	_Z19asmCPUWriteMemorypujj
	str	r11, [r4, r9, asl #2]
	add	r12, r12, #4
	b	.L5533

.L5722:
	mov	r0, r12
	ldr	r1, [r4, #12]
	bl	_Z19asmCPUWriteMemorypujj
	str	r11, [r4, r9, asl #2]
	add	r12, r12, #4
	b	.L5535

.L5723:
	mov	r0, r12
	ldr	r1, [r4, #16]
	bl	_Z19asmCPUWriteMemorypujj
	str	r11, [r4, r9, asl #2]
	add	r12, r12, #4
	b	.L5537

.L5724:
	mov	r0, r12
	ldr	r1, [r4, #20]
	bl	_Z19asmCPUWriteMemorypujj
	str	r11, [r4, r9, asl #2]
	add	r12, r12, #4
	b	.L5539

.L5725:
	mov	r0, r12
	ldr	r1, [r4, #24]
	bl	_Z19asmCPUWriteMemorypujj
	str	r11, [r4, r9, asl #2]
	add	r12, r12, #4
	b	.L5541




.LDMR0R7Rlist:
	and	r11, r8, #7
	ldr	r9, [r4, r11, asl #2]
	ldr	r2, .L5756
	and	r3, r8, #0xFF
	tst	r8, #1
	bic	r12, r9, #3
	ldrb	r10, [r2, r3]	@ zero_extendqisi2
	bne	.L5711
.L5549:
	tst	r8, #2
	bne	.L5712
.L5550:
	tst	r8, #4
	bne	.L5713
.L5551:
	tst	r8, #8
	bne	.L5714
.L5552:
	tst	r8, #16
	bne	.L5715
.L5553:
	tst	r8, #32
	bne	.L5716
.L5554:
	tst	r8, #64
	bne	.L5717
.L5555:
	tst	r8, #128
	bne	.L5718
.L5556:
	mov	r8, r8, asr r11
	tst	r8, #1
	addeq	r9, r9, r10, asl #2
	streq	r9, [r4, r11, asl #2]
	b .exitthumbhandel



.L5711:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #0]
	b	.L5549
.L5712:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #4]
	b	.L5550

.L5713:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #8]
	b	.L5551

.L5714:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #12]
	b	.L5552

.L5715:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #16]
	b	.L5553

.L5716:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #20]
	b	.L5554

.L5717:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	add	r12, r12, #4
	str	r0, [r4, #24]
	b	.L5555

.L5718:
	mov	r0, r12
	bl	_Z22asmCPUReadMemoryrealpuj
	str	r0, [r4, #28]
	b	.L5556



.L5756:
	.word	cpuBitsSet



	
	
.interrorme:
	ldr	r3, [r4, #0x3C]
	ldr	r0, .L5467
	ldrh	r1, [r3, #-6]
	bl	iprintf
	mov	r2, #0
	mov	r3, #0x4000000
	str	r2, [r3, #0x208]
	bl	_Z9debugDumpv
.L5466:
	b	.L5466
	
.align	4
.L5467:
	.word	.LC1
.LC1:
	.ascii	"Unimplemented THUMB instruction %04x\012\000"
	
.align	4

.exitthumbhandel:
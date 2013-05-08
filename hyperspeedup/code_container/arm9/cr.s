@---------------------------------------------------------------------------------
	.align	4
	.arm
	.global _io_dldi_stub
@---------------------------------------------------------------------------------


@_Z13emuInstrTHUMBtPi: @ can use all regs execptioon r5/r6/r7
	@mov	r2, r0, asr #8  @ old way
	@stmfd	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
	@sub	r3, r2, #80
	@mov	r4, r0
	@mov	r5, r1
	@cmp	r3, #127
	@ldrls	pc, [pc, r3, asl #2]
	@b	.L5470
	
	ldr	pc, [pc, r0, asr #6] @ichfly my way lol because auto aline
	nop
.space   #80* @sub	r3, r2, #80 @todo do better
.L5493:
	.word	.STRRd_(Rs_Rn)
	.word	.STRRd_(Rs_Rn)
	.word	.STRHRd_(Rs_Rn)
	.word	.STRHRd_(Rs_Rn)
	.word	.STRBRd_(Rs_Rn)
	.word	.STRBRd_(Rs_Rn)
	.word	.LDSBRd_(Rs_Rn)
	.word	.L5474
	.word	.L5475
	.word	.L5475
	.word	.L5476
	.word	.L5476
	.word	.L5477
	.word	.L5477
	.word	.L5478
	.word	.L5478
	.word	.L5479
	.word	.L5479
	.word	.L5479
	.word	.L5479
	.word	.L5479
	.word	.L5479
	.word	.L5479
	.word	.L5479
	.word	.L5480
	.word	.L5480
	.word	.L5480
	.word	.L5480
	.word	.L5480
	.word	.L5480
	.word	.L5480
	.word	.L5480
	.word	.L5481
	.word	.L5481
	.word	.L5481
	.word	.L5481
	.word	.L5481
	.word	.L5481
	.word	.L5481
	.word	.L5481
	.word	.L5482
	.word	.L5482
	.word	.L5482
	.word	.L5482
	.word	.L5482
	.word	.L5482
	.word	.L5482
	.word	.L5482
	.word	.L5483
	.word	.L5483
	.word	.L5483
	.word	.L5483
	.word	.L5483
	.word	.L5483
	.word	.L5483
	.word	.L5483
	.word	.L5484
	.word	.L5484
	.word	.L5484
	.word	.L5484
	.word	.L5484
	.word	.L5484
	.word	.L5484
	.word	.L5484
	.word	.L5485
	.word	.L5485
	.word	.L5485
	.word	.L5485
	.word	.L5485
	.word	.L5485
	.word	.L5485
	.word	.L5485
	.word	.L5486
	.word	.L5486
	.word	.L5486
	.word	.L5486
	.word	.L5486
	.word	.L5486
	.word	.L5486
	.word	.L5486
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5487
	.word	.L5488
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5470
	.word	.L5489
	.word	.L5490
	.word	.L5470
	.word	.L5470
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

	
	
	
.STRRd_(Rs_Rn):

	and	r4, r0, #7 @ichfly
	mov	r2, r0, asr #6
	mov	r3, r0, asr #3
	and	r2, r2, #7
	and	r3, r3, #7
	ldr	r0, [r1, r2] @auto aline
	ldr	r3, [r1, r3] @auto aline
	ldr	r1, [r1, r4] @auto aline 
	add	r0, r0, r3
	b	_ZL16CPUWriteMemorypujj
	
.STRHRd_(Rs_Rn):

	and	r4, r0, #7 @ichfly
	mov	r2, r0, asr #6
	mov	r3, r0, asr #3
	and	r2, r2, #7
	and	r3, r3, #7
	@ldr	r0, [r1, r2] @auto aline
	@ldr	r3, [r1, r3] @auto aline
	ldrh	r1, [r1, r4, asl #2]
	add	r0, r0, r3
	b	_ZL18CPUWriteHalfWordpujt
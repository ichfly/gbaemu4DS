

#include "ichflysettings.h"

#ifdef asmspeedup
	.align	4
	.global	CPUWriteMemorypu
    .type   CPUWriteMemorypu STT_FUNC
CPUWriteMemorypu:
	stmfd	sp!, {r3, r4, r5, lr}
	mov	r4, r1
	mov	r3, r0, lsr #24
	cmp	r3, #4
	bne	.unklike
	
	
	ldr	r3, .L130
	cmp	r0, r3
	ldmhifd	sp!, {r3, r4, r5, pc}
	and	r5, r0, #0x3FC
	mov	r1, r1, asl #16
	mov	r0, r5
	mov	r1, r1, lsr #16
	bl	_Z17CPUUpdateRegisterjt
	add	r0, r5, #2
	mov	r1, r4, lsr #16
	ldmfd	sp!, {r3, r4, r5, lr}
	b	_Z17CPUUpdateRegisterjt
	
	
	
	
.unklike:
	cmp	r3, #13
	beq	.eeprom
	cmp	r3, #14
	ldmnefd	sp!, {r3, r4, r5, pc}
.Flash:
	ldr	r3, .L130+4
	ldrb	r1, [r3, #0]	@ zero_extendqisi2
	ldr	r3, .L130+8
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldr	r3, .L130+12
	orr	r2, r1, r2
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	eor	r3, r3, #1
	orrs	r3, r2, r3
	ldmeqfd	sp!, {r3, r4, r5, pc}
	ldr	r3, .L130+16
	and	r1, r4, #255
	ldr	r3, [r3, #0]
	blx	r3
	ldmfd	sp!, {r3, r4, r5, pc}
.eeprom:
	ldr	r3, .L130+20
	ldrb	r3, [r3, #0]	@ zero_extendqisi2
	cmp	r3, #0
	ldmeqfd	sp!, {r3, r4, r5, pc}
	and	r1, r1, #255
	ldmfd	sp!, {r3, r4, r5, lr}
	b	_Z11eepromWritejh
	.align	4
.L130:
	.word	0x40003FF           @0
	.word	cpuFlashEnabled		@4
	.word	cpuSramEnabled      @8
	.word	eepromInUse         @12
	.word	cpuSaveGameFunc     @16
	.word	cpuEEPROMEnabled    @20
	
#endif
	
	
	
	
	
	
	
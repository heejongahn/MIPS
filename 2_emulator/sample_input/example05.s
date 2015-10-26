	.data
data1:	.word	3
	.word	4096
	.word	0xf4c2a
data2:	.word	2147483646
data3:	.word	2147483647
	.text
main:
	la	$15, data2
	addiu	$12, $12, -100
	addiu	$14, $14, -1
	addiu	$13, $13, 0xffff
	addu	$2, $4, $5
	addu	$2, $6, $7
	subu	$9, $3, $2
cs311:
	and	$11, $11, $0
	addiu	$10, $10, 0x1
	or	$6, $6, $0
	jal	cs312
cs312:
	lw	$6, 4($15)
	sll	$7, $6, 2
	sw	$7, 8($15)
	srl	$5, $4, 2
	nor	$5, $5, $0
	sltiu	$9, $10, -4
	beq	$9, $0, cs313
	jr	$31
cs313:
	lui	$17, 0x1000
	lw	$18, 0($17)
	sltu	$4, $2, $3
	bne	$4, $0, cs314
	j	cs311
cs314:
	ori	$16, $16, 0xf0f0

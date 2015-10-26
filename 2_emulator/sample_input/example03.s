	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.word	0x12341
	.text
main:
	lui	$3, 0x1000
	lw	$5, 0($3)
	lw	$8, 4($3)
	lw	$9, 8($3)
	lw	$10, 12($3)
	addiu	$5, $5, 24
	addiu	$6, $0, 124
	addu	$7, $5, $6
	sw	$5, 16($3)
	sw	$6, 20($3)
	sw	$7, 24($3)
	addiu	$3, $3, 12
	lw	$12, -4($3)
	lw	$13, -8($3)
	lw	$14, -12($3)

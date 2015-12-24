	.data
data1:	.word	10
data2:	.word	20
	.text
main:
	la	$24, data1
	la	$25, data2
	lw	$8, 0($24)
	lw	$9, 0($25)
	addiu	$21, $0, 1
loop1:
	addiu	$4, $4, 1
	addu	$10, $10, $8
	and	$11, $10, $8
	andi	$12, $10, 0xf0f0
	sltiu	$22, $4, 10
	beq	$22, $0, loop2
	j	loop1
loop2:
	addiu	$5, $5, 1
	nor	$13, $13, $9
	or	$14, $13, $9
	sll	$15, $13, 2
	srl	$16, $13, 1
	sltu	$23, $5, $9
	bne	$23, $21, tail
	jal	loop2
tail:
	subu	$17, $5, $4
	sw	$10, 4($25)
	sw	$11, 8($25)
	sw	$12, 12($25)
	sw	$13, 16($25)
	sw	$14, 20($25)
	sw	$15, 24($25)
	sw	$16, 28($25)

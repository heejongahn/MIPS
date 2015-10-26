	.data
	.text
main:
	sw	$a0, 0($sp)
	sltiu	$t0, $a0, 1
	beq	$t0, $zero, L1
	addiu	$v0, $zero, 1
L1:
	addiu	$a0, $a0, -1
	jal	main
	lw	$a0, 0($sp)
	lw	$ra, 4($sp)
	addu	$v0, $a0, $v0

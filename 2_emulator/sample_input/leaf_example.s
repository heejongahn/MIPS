	.data
	.text
main:
	addu	$t0, $a0, $a1
	addu	$t1, $a2, $a3
	subu	$s0, $t0, $t1
	addu	$v0, $s0, $zero

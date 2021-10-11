.data

.text
main:
	li	$t0, 2
	li	$t1, 3
	li	$t2, 5
	mult	$t1, $t2
	mflo	$t2
	add	$t0, $t0, $t2
	li	$t1, 8
	li	$t2, 3
	div	$t1, $t2
	mflo	$t2
	sub	$t0, $t0, $t2
	li	$v0, 1
	add	$a0, $t0, $zero
	syscall

li $v0, 10
syscall

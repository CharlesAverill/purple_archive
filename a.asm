.data

.text
main:
	ori	$t0, $zero, 2
	ori	$t1, $zero, 3
	ori	$t2, $zero, 5
	mult	$t1, $t2
	mflo	$t2
	add	$t0, $t0, $t2
	ori	$t1, $zero, 8
	ori	$t2, $zero, 3
	div	$t1, $t2
	mflo	$t2
	sub	$t0, $t0, $t2
	ori	$v0, $zero, 1
	add	$a0, $t0, $zero
	syscall

	ori $v0, $zero, 10
	syscall

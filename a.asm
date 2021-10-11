.data
.text
main:
	ori	$t0, $zero, 2
	ori	$t1, $zero, 3
	mult	$t0, $t1
	mflo	$t1
	ori	$t0, $zero, 5
	add	$t1, $t1, $t0
	ori	$t0, $zero, 6
	ori	$t2, $zero, 3
	div	$t0, $t2
	mflo	$t2
	add	$t1, $t1, $t2
	ori	$t0, $zero, 1
	add	$t1, $t1, $t0
	ori	$v0, $zero, 1
	add	$a0, $t1, $zero
	syscall
	ori	$t0, $zero, 1
	ori	$t1, $zero, 3
	add	$t0, $t0, $t1
	ori	$v0, $zero, 1
	add	$a0, $t0, $zero
	syscall
exit:
	ori $v0, $zero, 10
	syscall

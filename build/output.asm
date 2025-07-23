.data
	yo_mama: .float -15.000000
	suggo_my_eggos: .word -36
	x: .word 7
	test: .word 8
	p: .word 9
	suiiiiii: .float 2323125.000000
	type: .asciiz "hello, world!"
	res: .asciiz "test 123"

.text
.globl main
main:
	li $v0, 2
	lwc1 $f12, yo_mama
	syscall
	li $v0, 4
	la $a0, type
	syscall
	jal fart
	li $v0, 10
	syscall
solve:
	li $v0, 7
	jr $ra
ligma:
	l.s $f0, suiiiiii
	jr $ra
fart:
	li $v0, 4
	la $a0, res
	syscall

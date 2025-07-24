.data
	yo_mama: .float 2323294.000000
	suggo_my_eggos: .word -36
	sixety_nine: .float -6.900000
	x: .word 7
	test: .word 8
	p: .word 9
	suiiiiii: .float 2323294.000000
	type: .asciiz "hello, world!"
	res: .asciiz "test 123"
	rando_float: .float 1.120000

.text
.globl main
main:
	li $v0, 2
	lwc1 $f12, yo_mama
	syscall
	li $v0, 4
	la $a0, type
	syscall
	lwc1 $f0, rando_float
	jal fart
	li $v0, 2
	lwc1 $f12, rando_float
	syscall
	li $v0, 10
	syscall
solve:
	li $v0, 7
	jr $ra
ligma:
	l.s $f1, suiiiiii
	jr $ra
fart:
	li $v0, 4
	la $a0, res
	syscall

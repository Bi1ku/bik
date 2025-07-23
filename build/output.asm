.data
	NWLRBBMQBHCDARZO: .asciiz "hello, world!"
	WKKYHIDDQSCDXRJM: .asciiz "yoooo!"
	OWFRXSJYBLDBEFSA: .float 1.100000

.text
.globl main
main:
	li $v0, 4
	la $a0, NWLRBBMQBHCDARZO
	syscall
	li $a0, 1
	li $a1, 2
	jal x
	li $v0, 10
	syscall
x:
	li $v0, 4
	la $a0, WKKYHIDDQSCDXRJM
	syscall
	jr $ra

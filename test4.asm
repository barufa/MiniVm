read %r0
push %r0
read %r0
push %r0
read %r0
push %r0
read %r0
push %r0
call func
print %r0
hlt
func:
	pop %r3
	pop %r0
	mov %r0, %r2
	pop %r0
	mov %r0, %r1
	pop %r0
	mul %r0, %r1
	pop %r0
	mul %r0, %r2
	sub %r1, %r2
	push %r3
	mov %r2, %r0
ret

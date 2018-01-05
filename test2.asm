read %r0
mov $1, %r1
mov $32, %r3
loop:
	mov %r1, %r2
	mul $2, %r1
	and %r0, %r2
	cmp $0, %r2
	jmpe sum
cont:
	cmp $1, %r1
jmpl loop
mov %r1, %r2
and %r0, %r2
cmp $0, %r2
jmpe sumul
contul:
print %r3
hlt
sum:	
	sub $1, %r3
	jmp cont
sumul:
	sub $1, %r3
	jmp contul
hlt

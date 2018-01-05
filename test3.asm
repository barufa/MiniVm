read %r0
sw %r0, $600
read %r0
sw %r0, $601
read %r0
sw %r0, $602
read %r0
sw %r0, $603
read %r0
sw %r0, $604
read %r0
sw %r0, $605
push $6
push $600
pop %r0
pop %r1
mov $0, %r2
loop:
	lw %r0, %r3
	add $1, %r0
	add %r3, %r2
	sub $1, %r1
	cmp $0, %r1
jmpl loop
print %r2
hlt

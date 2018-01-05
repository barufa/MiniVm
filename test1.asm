read %r0
cmp %r0, $0
jmpl neg
print %r0
hlt
neg:
mul $-1, %r0
print %r0
hlt

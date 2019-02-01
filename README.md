# MiniVm
La máquina posee una única memoria y su propio lenguaje para ejecutar programas dentro de esta. Este lenguaje consta inicialmente de 17 instrucciones (NOP, MOV, LW, SW, PUSH, POP, PRINT, READ, ADD, SUB, MUL, DIV, CMP, JMP, JMPE, JMPL y HLT) las cuales trabajan
con operandos de 4 bytes.

## Como usar
Para ejecutar algún código en la mini-vm este debe ser pasado en un archivo de extensión asm como argumento del programa a través de la terminal. Dentro de la Mini-VM las funciones yyparse() y processLabels() se encargan de convertir el código en instrucciones ejecutables por la Mini-VM, que son almacenadas en un arreglo code de tipo struct Instruction (una estructura que almacena la
instrucción con sus operandos) para poder realizar las operaciones. Una vez hecho esto se imprime el código en pantalla y se llama a la función run() para ejecutarlo. La función run() consiste en un bucle que ejecuta instrucción por instrucción hasta llegar a la instrucción HTL que denota el fin del programa.

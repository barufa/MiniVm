#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "machine.h"
#include "parser.tab.h"
#define SETF(F) (machine.reg[FLAGS]|=(1<<(F)))
#define CLRF(F) (machine.reg[FLAGS]&=(~(1<<(F))))
#define ISON(F) (machine.reg[FLAGS]&(1<<(F)))

int count;
struct Instruction code[512];
const char *regname[REGS] = { "\%zero", "\%pc", "\%sp", "\%r0","\%r1","\%r2","\%r3", "\%flags"};
struct Machine machine;

int leer(){
	int n;
	scanf("%d",&n);
	return n;
}

int reader(struct Operand v){
	switch (v.type){
		case REG:
			return machine.reg[v.val];
		case MEM:
			return machine.memory[v.val];
		case IMM:
			return v.val;
		case LABELOP:
			return v.val;
		default:
			printf("Error read operand\n");
			abort();
            break;
    }
    return v.val;
}

int writer(struct Operand v,int val){
	switch (v.type){
		case REG:
			machine.reg[v.val]=val;
            break;
		case MEM:
			machine.memory[v.val]=val;
            break;
		default:
			printf("Error write operand\n");
            abort();
            break;
	}
	return val;
}

int reg(const char* r) {
  r++;
  if (!strcmp(r,"zero")) return 0;
  if (!strcmp(r,"pc")) return 1;
  if (!strcmp(r,"sp")) return 2;
  if (!strcmp(r,"r0")) return 3;
  if (!strcmp(r,"r1")) return 4;
  if (!strcmp(r,"r2")) return 5;
  if (!strcmp(r,"r3")) return 6;
  if (!strcmp(r,"flags")) return 7;
  printf("Unkown Register %s\n",r);
  abort();
}
void dumpMachine() {
  int i;
  printf("**************** Machine state at PC=%d **************\n", machine.reg[PC]);
  for (i=0;i<REGS;i++) 
    if (strlen(regname[i])==0) continue; 
    else printf("%s\t\t= \t\t%d \t\t%x\n",regname[i],machine.reg[i],machine.reg[i]);
  printf("******************************************************\n");
}

void runIns(struct Instruction i) {
	switch(i.op){
		case NOP:
			break;
		case MOV:
			writer(i.dst,reader(i.src));
			break;
		case SW:
			machine.memory[reader(i.dst)]=reader(i.src);
			break;
		case LW:
			writer(i.dst,machine.memory[reader(i.src)]);
			break;
		case PUSH:
			machine.memory[machine.reg[SP]]=reader(i.src);
			machine.reg[SP]--;
			break;
		case POP:
			machine.reg[SP]++;
			writer(i.src,machine.memory[machine.reg[SP]]);
			break;
		case PRINT:
			printf("%d\n",reader(i.src));
			break;
		case READ:
			writer(i.src,leer());
			break;
		case ADD:
			if(!writer(i.dst,reader(i.dst)+reader(i.src)))SETF(ZERO_BIT_FLAGS);
			else CLRF(ZERO_BIT_FLAGS);
			break;
		case SUB:
			if(!writer(i.dst,reader(i.dst)-reader(i.src)))SETF(ZERO_BIT_FLAGS);
			else CLRF(ZERO_BIT_FLAGS);
			break;
		case MUL:
			if(!writer(i.dst,reader(i.dst)*reader(i.src)))SETF(ZERO_BIT_FLAGS);
			else CLRF(ZERO_BIT_FLAGS);
			break;
		case DIV:
			if(reader(i.src)==0){
				printf("DIVISION POR 0!!!\n");
				abort();
			}
			if(!writer(i.dst,reader(i.dst)/reader(i.src)))SETF(ZERO_BIT_FLAGS);
			else CLRF(ZERO_BIT_FLAGS);
			break;
		case CMP:
			if(reader(i.dst)==reader(i.src))SETF(EQUAL_BIT_FLAGS);
			else CLRF(EQUAL_BIT_FLAGS);
			if(reader(i.src)<reader(i.dst))SETF(LOWER_BIT_FLAGS);
			else CLRF(LOWER_BIT_FLAGS);
			break;
		case CALL:
			machine.memory[machine.reg[SP]]=machine.reg[PC];
			machine.reg[SP]--;
			machine.reg[PC]=reader(i.src)-1;
			break;
		case RET:
			machine.reg[SP]++;
			machine.reg[PC]=machine.memory[machine.reg[SP]];
			break;
		case AND:
			if(!writer(i.dst,(reader(i.src)&reader(i.dst))))SETF(ZERO_BIT_FLAGS);
			else CLRF(ZERO_BIT_FLAGS);
			break;
		default:
		printf("Instruction not implemented\n");
		abort();
  }
}

void run(struct Instruction *code) {
  machine.reg[PC] = 0;
  machine.reg[SP] = MEM_SIZE; 
  struct Instruction i;
  while (code[machine.reg[PC]].op!=HLT){
    i=code[machine.reg[PC]]; 
    if(i.op!=JMP && i.op!=JMPE && i.op!=JMPL){
		runIns(code[machine.reg[PC]]);
		machine.reg[PC]++;
	}else
		if(i.op==JMP || (i.op==JMPE && ISON(EQUAL_BIT_FLAGS)) || (i.op==JMPL && ISON(LOWER_BIT_FLAGS)))
			machine.reg[PC]=reader(i.src);
		else
			machine.reg[PC]++;
  }
}

void processLabels() {
  int i,j;
  for (i=0;i<count;i++) {
    if (code[i].op==LABEL) {
      for (j=0;j<count;j++) {
        if (code[j].op == JMP || code[j].op== JMPE || code[j].op==JMPL || code[j].op==CALL){
          if(code[j].src.lab && strcmp(code[j].src.lab,code[i].src.lab)==0) {
            code[j].src.type=IMM;
            code[j].src.val=i;
            code[j].src.lab=NULL;
          }
        }
      }
      for (j=i;j<count-1;j++) {
        code[j]=code[j+1];
      }
      count--;
    }
  }
  for (j=0;j<count;j++) {
    if (code[j].op == JMP || code[j].op== JMPE || code[j].op==JMPL || code[j].op==CALL) {
      if (code[j].src.lab) {
        printf("Jump to unkown label %s\n",code[j].src.lab);
        exit(-1);
      }
    }
  }
}
void printOperand(struct Operand s) {
  switch(s.type){
    case IMM:
      printf("$%d", s.val);
      break;
    case REG:
      printf("%s", regname[s.val]);
      break;
    case MEM:
      printf("%d", s.val);
      break;
	default:
		break;
  } 

}
void printInstr(struct Instruction i) {
  switch (i.op) {
    case NOP:
      printf("NOP\n");
      break;
    case HLT:
      printf("HLT\n");
      break;
    case MOV:
      printf("MOV ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case ADD:
      printf("ADD ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case MUL:
      printf("MUL ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case SUB:
      printf("SUB ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case DIV:
      printf("DIV ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break; 
    case LW:
      printf("LW ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case SW:
      printf("SW ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case JMPL:
      printf("JMPL ");
      printOperand(i.src);
      if (i.src.lab)
        printf("%s",i.src.lab);
      printf("\n");
      break;
    case JMPE:
      printf("JMPE ");
      printOperand(i.src);
      if (i.src.lab)
        printf("%s",i.src.lab);
      printf("\n");
      break;
    case JMP:
      printf("JMP ");
      printOperand(i.src);
      if (i.src.lab)
        printf("%s",i.src.lab);
      printf("\n");
      break;
    case LABEL:
      printf("LABEL %s",i.src.lab);
      printf("\n");
      break;
    case CMP:
      printf("CMP ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    case PRINT:
      printf("PRINT ");
      printOperand(i.src);
      printf("\n");
      break;
    case PUSH:
      printf("PUSH ");
      printOperand(i.src);
      printf("\n");
      break;
    case POP:
      printf("POP ");
      printOperand(i.src);
      printf("\n");
      break;
    case READ:
      printf("READ ");
      printOperand(i.src);
      printf("\n");
      break;
    case CALL:
      printf("CALL ");
      printOperand(i.src);
      if (i.src.lab)
        printf("%s",i.src.lab);
      printf("\n");
      break;
    case RET:
      printf("RET ");
      printf("\n");
      break;
    case AND:
      printf("AND ");
      printOperand(i.src);
      printf(",");
      printOperand(i.dst);
      printf("\n");
      break;
    default:
      printf("Instrucction not printed");
      printf("\n");
  }
}	
int main(int argc, char **argv){
	const int copystdin=dup(0);
	int i,archivo;
	if(argc!=2){
		printf("Uso: ./machine test.asm\n");
		return -1;
	}
	fflush(stdin);
	archivo=open(argv[1],O_RDWR,777);
	close(0);dup(archivo);close(archivo);
	yyparse();
	processLabels();
	printf("Running the following code\n");
	for(i=0;i<count;i++){
		printf("%d: ",i);
		printInstr(code[i]);
	}
	printf("***************\n");
	fflush(0);
	dup2(copystdin,0);close(copystdin);
	run(code);
	return 0;
}

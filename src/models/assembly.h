#ifndef DECOMPILER_ASS_H
#define DECOMPILER_ASS_H

#include "../includes/types.h"
#include "x86.h"



typedef enum { 
    OP_TYPE_NONE=0,
    OP_TYPE_REG=1, 
    OP_TYPE_IMM=2, 
    OP_TYPE_PTR=3, 
    OP_TYPE_FP=4 
    } OperandType1;

typedef enum { 
    
    OP_TYPE_NUM=5,    //$x
    OP_TYPE_LABEL=6,  //$.LCx
    OP_TYPE_SBR=7,     //stack base reg %ebp %rbp
    OP_TYPE_STR=8,     //stack top reg %esp %rsp
    OP_TYPE_SBP=9,     //stack base ptr (%ebp) (%rbp)
    OP_TYPE_STP=10,     //stack top ptr (%esp) (%rsp)
    OP_TYPE_ARG=11
    } OperandType2;

typedef enum  {
    INS_GRP_NONE=0,
    INS_GRP_MOV=1, 
    INS_GRP_ADD=2, 
    INS_GRP_SUB=3, 
    INS_GRP_MULTU=4,
    INS_GRP_MULT=5,
    INS_GRP_DIVU=6,
    INS_GRP_DIV=7,
    INS_GRP_CJMP=8,
    INS_GRP_JMP=9,
    INS_GRP_TEST=10,
    INS_GRP_CSET=11,
    INS_GRP_CALL=12,
    INS_GRP_CMP=13,
    INS_GRP_LEA=14,
    INS_GRP_INC=15,
    INS_GRP_PUSH=16,
    INS_GRP_POP=17
  } InstrGroup;


typedef struct {
 
  int offset;
  int scale;

} X86ptr;

typedef union  {
        char       reg[10]; //eax, esi, edi, etc
        char       imm[10];  //if IMM the actual value
        double     fp;
      
} X86Value;


typedef struct {

  char         *op_string;  
  OperandType1  type1;  //REG, IMM, MEM
  OperandType2  type2;  //NUM, LABEL, etc
  uint8_t      size;
  X86Value     value;
  X86ptr       ptr;
       
} Operand;

typedef struct {
    
 ListElement      listElement;
 x86_insn          mnem_id;
 InstrGroup       grpid;
 uint8_t          size;
 char              *mnemonic;
 Operand           operands[6];
 uint8_t           opcount; //number of operands in the instruction
 
} Instruction;




typedef struct _basicBlock {
  ListElement              listElement;
  char                     *label;
  char                     *thenLabel;
  struct _basicBlock       *thenBB;
  char                     *elseLabel;
  struct _basicBlock       *elseBB;
  char                     *leftOp;
  char                     *rightOp;
  char                     *compOperator;
  int                      pos;              //position in cfg tree
  struct _basicBlock       *head;            //head node of the interval it belongs to
  bool                     visited;	
  bool                     merged;            // if merged with some other block during CFA
  List                     Predecessors;      //List of Predecessors
  List                     Instructions;        //List of X86 Instructions
  List                     ClikeInsL;        //List of C like Instructions
  
} BasicBlock;



typedef struct {
  ListElement      listElement;
  BasicBlock       *bbptr;

} Predecessor;



typedef struct {
  ListElement      listElement;
  char             *funcName;
  List             funcBBlist;

} FuncBlock;




typedef struct {
  ListElement      listElement;
  char             *label;
  char             *string;

} StringBlock;

//map instruction string to an id and size
typedef struct  {
  const char* instr_name;
  x86_insn mnem_id;
  uint8_t size;
  InstrGroup grpid;
} instr_map;

#endif

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
    OP_TYPE_STP=10     //stack top ptr (%esp) (%rsp)
  
    } OperandType2;

typedef enum  {
    INS_GRP_NONE=0,
    INS_GRP_MOV=1, 
    INS_GRP_ADD=2, 
    INS_GRP_SUB=3, 
    INS_GRP_MULT=4,
    INS_GRP_CJMP=5,
    INS_GRP_JMP=6,
    INS_GRP_TEST=7,
    INS_GRP_CSET=8,
    INS_GRP_CALL=8,
    INS_GRP_CMP=9 
  } InstrGroup;

typedef enum  { NOCOND, IF, IFELSE } CONDITIONAL;

typedef struct {
 
  char offset[10];
  char scale[10];

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
 //unsigned short    flags;  //COND, JMP, CALL, RET , etc
 char              *mnemonic;
 Operand           operands[6];
 uint8_t           opcount; //number of operands in the instruction
 bool              trap;  //a trap instruction or not
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
  CONDITIONAL              condtype;
  bool                     visited;	
  List                     Predecessors;      //List of Predecessors
  List                     Instructions;        //List of Instructions
  
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

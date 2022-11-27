#ifndef DECOMPILER_ASS_H
#define DECOMPILER_ASS_H

#include "../includes/types.h"
#include "x86.h"



typedef enum { 
    OP_TYPE_NONE=0,
    OP_TYPE_REG=1, 
    OP_TYPE_IMM=2, 
    OP_TYPE_MEM=3, 
    OP_TYPE_FP=4 
    } OperandType;
    
typedef enum  {
    INS_FLAG_CFLOW    = 0x001,
    INS_FLAG_COND     = 0x002,
    INS_FLAG_INDIRECT = 0x004,
    INS_FLAG_JMP      = 0x008,
    INS_FLAG_CALL     = 0x010,
    INS_FLAG_RET      = 0x020,
    INS_FLAG_NOP      = 0x040
  } InstructionFlags;

typedef enum  { NOCOND, IF, IFELSE } CONDITIONAL;


typedef union  {
    
        char       reg[10];  //eax, esi, edi, etc
        char       imm[10];  //if IMM the actual value
        double     fp;
        int    offset;   //if MEM the offset from the pointer
} X86Value;


typedef struct {

  char         *op_string;  
  OperandType  type;  //REG, IMM, MEM, etc
  uint8_t      size;
  X86Value     value;
       
} Operand;

typedef struct {
    
 ListElement      listElement;
 x86_insn          mnem_id;
 uint8_t          size;
 unsigned short    flags;  //COND, JMP, CALL, RET , etc
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
} instr_map;

#endif

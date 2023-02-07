#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"


#define FOREVER for(;;)
#define MAX_CHARS_IN_LINE 200

int glob_veriable = 2;
extern const instr_map instr_table[];
extern int instr_table_length;

extern bool is64bits;

void trim(char * s) {      
    //remove trailing and leading spaces
    //TODO remove spaces between brackets

    char * p = s;       //input string pointer
    int l = strlen(p);

    while(isspace(p[l - 1])) //replacing the spaces at the end with zero
            p[--l] = 0;
            
    while(* p && isspace(* p)) //moving pointer until no space is found
            ++p, --l;

    memmove(s, p, l + 1); //overrite input string 
}   



void print_instructions(List *Instructions) {

    List_reset(Instructions);
    for (int i = 0; i < Instructions->numItems; i++) {
      Instruction *ins = (Instruction *)List_getNextElement(Instructions);
      printf("   Instruction with %d operands\n",ins->opcount);
      printf("       mnemonic-> %s\n",ins->mnemonic);
      printf("       mnem_id -> %d\n",ins->mnem_id);  
      printf("       size    -> %d\n",ins->size);
      printf("       grpid   -> %d\n",ins->grpid);
     
      for (int i=0;i<ins->opcount;i++) {
           printf("           operand-> %s\n",ins->operands[i].op_string);
           if (ins->operands[i].type1 == OP_TYPE_IMM)
           printf("                imm value-> %s\n",ins->operands[i].value.imm);
           if (ins->operands[i].type1 == OP_TYPE_PTR) {
           printf("                offset   -> %d\n",ins->operands[i].ptr.offset);
           printf("                type2    -> %d\n",ins->operands[i].type2);
           }
      }
      
    }
}

void print_func_block(FuncBlock *funcBlockp){

   List *funcBBlistP = &(funcBlockp->funcBBlist);
   List_reset(funcBBlistP);

   for (int i = 0; i < funcBBlistP->numItems; i++) {

     BasicBlock *block = (BasicBlock *)List_getNextElement(funcBBlistP);
     printf("   basic block with label -> %s\n",block->label);
    // printf("      leftOp -> %s\n",block->leftOp);
    // printf("      rightOp -> %s\n",block->rightOp);
     printf("      cmpOperator-> %s\n",block->compOperator);
     printf("      thenLabel-> %s\n",block->thenLabel);
     printf("      elseLabel-> %s\n",block->elseLabel);
     print_instructions(&(block->Instructions));

   }
   
}

void print_blocks(List *funcBlocksP) {

    List_reset(funcBlocksP);     //point to the first block
   // printf("number of progblocks->%d\n", funcBlocksP->numItems);
     for (int i = 0; i < funcBlocksP->numItems; i++) {

      FuncBlock *block = (FuncBlock *)List_getNextElement(funcBlocksP);

      printf("Function block ->%s\n",block->funcName);
    //  if (block->type == FUNC)
        print_func_block(block);

    }
}

void print_string_blocks(List *stringBlocksP) {

    List_reset(stringBlocksP);     //point to the first block
   // printf("number of progblocks->%d\n", funcBlocksP->numItems);
     for (int i = 0; i < stringBlocksP->numItems; i++) {

      StringBlock *block = (StringBlock *)List_getNextElement(stringBlocksP);

      printf("String block label ->%s string->%s\n",block->label, block->string);
   

    }
}


char *get_temp_label() {

  static int index=0;
  char buf[3], tl[8];

    index++;
    itoa(index,buf,10); //convert int(base10) index to string and store it indo buff
    strcpy(tl,".TL");

    strcat(tl,buf); //append buf string to tl
  return strdup(tl);

}

char *convertTo64reg(char *reg) {
  if (!strncmp(reg,"%eax",4)) {/*printf("converted to rax\n");*/return("%rax");}
  else if (!strncmp(reg,"%esi",4)) {/*printf("converted to rsi\n");*/return("%rsi");}
  else if (!strncmp(reg,"%edi",4)) {/*printf("converted to rdi\n");*/return("%rdi");}
  else if (!strncmp(reg,"%edx",4)) {/*printf("converted to rdx\n");*/return("%rdx");}
  else return reg;
}

char *convertTo4(char *reg) {
   if (!strncmp(reg,"%ax",3)) {printf("converted to eax\n");return("%eax");}
   else if (!strncmp(reg,"%al",3)) {printf("converted to eax\n");return("%eax");}
   else if (!strncmp(reg,"%bx",3)) {printf("converted to ebx\n");return("%ebx");}
   else if (!strncmp(reg,"%bl",3)) {printf("converted to ebx\n");return("%ebx");}
   else return reg;
}

int process_instruction_details(Instruction *ins) {

   // int init, ret, jmp, cflow, cond, call, nop, only_nop, priv, trap;
    bool found = false;

    for (int i=0; i<instr_table_length; i++) {
         
        if (!strcmp(ins->mnemonic, instr_table[i].instr_name)) {
            ins->mnem_id = instr_table[i].mnem_id;
            ins->grpid = instr_table[i].grpid;
            ins->size = instr_table[i].size;
            found=true;
            break;
        }
          
    };

    if (!found) {
      //check if it is a conditional set instruction (not defined in table)
      if (!strncmp(ins->mnemonic,"set",3)) {
        ins->grpid = INS_GRP_CSET;
        //ins->flags |= INS_FLAG_CSET;
        return 1;
      } else {
        printf("invalid instruction ->%s\n",ins->mnemonic);
        return 0;
      }
    }

  
   return 1;
}

void process_operand(char *op_string, Operand *op) {

    char *firstpos;
    char  offsets[20]; memset(offsets,0,20);
    op->op_string = strdup(op_string);

    //determine if operand is Immediate, Register or Memory (pointer to a memory location)
    if (op_string[0] == '$') {
      //operand is of the form $x or $.LCx
      op->type1 = OP_TYPE_IMM;
      strcpy(op->value.imm, op_string+1);  
      if (op_string[1] == '.') 
        op->type2 = OP_TYPE_LABEL; 
      else 
        op->type2 = OP_TYPE_NUM;

    } else if (op_string[0] == '%') {
        //operand is of the form %reg
        op->type1 = OP_TYPE_REG;
        if (strlen(op_string)<4) 
          op_string = strdup(convertTo4(op_string)); //if %ax -> %eax
        if (is64bits) 
          strcpy(op->value.reg, convertTo64reg(op_string));
        else 
          strcpy(op->value.reg, op_string);
        if (!strcmp(op_string+2, "bp"))  
          op->type2 = OP_TYPE_SBR; //stack base reg
        if (!strcmp(op_string+2, "sp"))  
          op->type2 = OP_TYPE_STR; //stack top reg
        if (is64bits) { 
          if ((!strcmp(op_string+2, "di")) || (!strcmp(op_string+2, "si")))
            op->type2 = OP_TYPE_ARG; } //x86-64 used as args to functions
       
   } else if ( (firstpos=strchr(op_string ,'(')) && (strchr(firstpos+1,')')) ) { 
        //operand is of the form (%reg) or offset(%reg) where %reg is the base reg  -> reg+offset 
        //TODO it can also be segment:offset(%reg1,$reg2,scale)  or offset(base,index,scale) -> base+index*scale+offset
        // don't handle segment
        // offset is optional
        // scale is optional. defualt is 1
        // base is optional eg offset(,index,scale)
        op->type1 = OP_TYPE_PTR;
        strcpy(op->value.reg, firstpos+1); // firstpos = %
        op->value.reg[strlen(op->value.reg)-1] = 0; //remove ')' from reg name
        strncpy(offsets, op_string, strlen(op_string) - strlen(firstpos));
 
        //if (strlen(offsets)) strcpy(op->ptr.offset, offsets); 
        if (strlen(offsets)) op->ptr.offset = atoi(offsets); 
        if (is64bits) strcpy(op->value.reg, convertTo64reg(op->value.reg)); 
        if (!strcmp(op->value.reg+2, "bp"))  op->type2 = OP_TYPE_SBP; 
        if (!strcmp(op->value.reg+2, "sp"))  op->type2 = OP_TYPE_STP; //pointer to top of stack
        

    } else if (op_string[0] == '.') {
        //for example jbe .L3 operand is .L3
        //this will be processed by link blocks procedure
        

    } 
    else
        printf("invalid operand ->%s\n",op_string);
    

}

int process_instruction(char *inst, Instruction **instP) {

    char *currentWord;

    printf("  processing inst-> %s\n",inst);
    Instruction *instructionP;
  
    // allocate memory for intruction structure and initialize fields to 0
    instructionP = (Instruction *)calloc(1,sizeof(Instruction));
    

    currentWord = strtok(inst, " \t");        //tokenise instruction with space char
    int index = 0;
    /*Run over instruction line (word-by-word)*/
    while (currentWord != NULL)
    {
            //printf("      word-> %s <-\n",currentWord);
            if(currentWord[strlen(currentWord)-1] ==',' )  
                    currentWord[strlen(currentWord)-1] =0;      
            
            //first word in instruction's line is the mnemonic
            if (index == 0) {
                 instructionP->mnemonic = strdup(currentWord);
                 if (!process_instruction_details(instructionP)) {
                  free(instructionP);
                  return 0;
                 };
            }

            //following words are the operands
            if (index == 1){  
              //breaks if instruction is a call to  a function with 2 parms due to space in between
              //copy only the func name up to (        
                if (instructionP->grpid == INS_GRP_CALL) {
                   char *pos = strchr(currentWord ,'('); 
                   char func[20]; 
                   if (pos) strncpy(func, currentWord, strlen(currentWord)-strlen(pos));
                  
                   instructionP->operands[0].op_string= (pos ? strdup(func) : strdup(currentWord));
                   index++;
                   break;
                }

                process_operand(currentWord, &instructionP->operands[0]);                  
               
            }
            if (index == 2) {
                process_operand(currentWord, &instructionP->operands[1]);
               
            }
            if (index == 3) {
                process_operand(currentWord, &instructionP->operands[2]);
                
            }


            index++;
           
            currentWord = strtok(NULL, " \t");
            //continue;
    }
    
    instructionP->opcount = index-1;
    *instP = instructionP;
    return 1;
   // List_pushElement_back( Instructions, instructionP); 
    
}

/*
void process_string_constants(List *blocks) {
//update the value of an IMM operand with the contents of constant strings
// for example for  movl  $.LC0, %edi  find and update the value of the first operand with LC0 string contents

printf("processing string constants\n");
    BasicBlock *targetBlock=0;

    List_reset(blocks);     //point to the first block

     for (int i = 0; i < blocks->numItems; i++) {

       BasicBlock *block = (BasicBlock *)List_getNextElement(blocks);
       List *instructions = &(block->Instructions);
       List_reset(instructions);

         for (int i = 0; i < instructions->numItems; i++) {
           
            targetBlock=0;
            Instruction *ins = (Instruction *)List_getNextElement(instructions);
            if ((ins->operands[0].type == OP_TYPE_IMM) && (ins->operands[0].value.imm[0]=='.')){
                
                ListElement *current = blocks->current;  //save current pointer
                targetBlock = find_block(ins->operands[0].value.imm, blocks);
                blocks->current = current;  //restore current pointer
                if (targetBlock) {
                    Instruction *tempins = (Instruction *)(targetBlock->Instructions.head);
                    strcpy(ins->operands[0].value.imm, tempins->operands[0].op_string);
                    printf("process strings: found target block with string ->%s<-\n", ins->operands[0].value.imm);
                }
            }
        }
      

    }

   

}
*/

void parse_assembly(FILE* fpointer, List *funcBlocks, List *stringBlocks) {

    
    //ProgBlock *progblockP;
    FuncBlock *funcblockP = 0;
    StringBlock *stringblockP;
    BasicBlock *basicblockP;
    Instruction *instp;
    bool seen_unc_jump = false;

    char currentLine[MAX_CHARS_IN_LINE];
    char testline[MAX_CHARS_IN_LINE];

    printf("instructions table loded. size ->%d\n", instr_table_length);
    

    FOREVER     //endles loop
    {
        /*Hold the current line in file*/
        fgets(currentLine, MAX_CHARS_IN_LINE, fpointer); 
        currentLine[strlen(currentLine)-1]=0; 

        //remove leading spaces
        trim(currentLine);

        /*Check if we get to the end of the file*/
        if(feof(fpointer))
            break;

        // check if current line is the beginning of a string label a function or function label
        if (currentLine[strlen(currentLine)-1]== ':') { 

            currentLine[strlen(currentLine)-1] = 0 ; //remove : from end of label

            if (!strncmp(currentLine,".LC",3)) {  // a string label

              /* create a new string block for the new string label*/
                stringblockP = (StringBlock *)malloc(sizeof(StringBlock)); //creating memory space 
                stringblockP->label = strdup(currentLine);
                
                //get next line where the string constant exists
                fgets(currentLine, MAX_CHARS_IN_LINE, fpointer); 
                currentLine[strlen(currentLine)-1]=0; 
                stringblockP->string = strdup(strchr(strstr(currentLine,".string"), ' ')+1);
                if (stringblockP->string) 
                  printf("found string label %s with string ->%s\n",stringblockP->label,stringblockP->string);
                else 
                  printf("string for label %s not found\n", stringblockP->label);

                // add new string block to the list
                List_pushElement_back( stringBlocks, stringblockP); 

                continue;
            }

            if (currentLine[0]!='.') {  //a function name is found

                printf("start of new function found-> %s\n",currentLine);
                   
                //create a new func block 
                funcblockP = (FuncBlock *)calloc(1,sizeof(FuncBlock)); //creating memory space
                funcblockP->funcName = strdup(currentLine);

                //add current func block to the func blocks list
                List_pushElement_back( funcBlocks, funcblockP); 
                
                //create a list to hold the basic blocks of the new function
                List_new(&(funcblockP->funcBBlist));
                
                //create a new basic block for the instructions just after the function definition
                basicblockP = (BasicBlock *)calloc(1,sizeof(BasicBlock)); //creating memory space
                basicblockP->label = get_temp_label();

                //add current basic block to the func basic blocks list
                List_pushElement_back( &(funcblockP->funcBBlist), basicblockP); 

                //create a list to hold the instructions in the basic block
                List_new(&(basicblockP->Instructions));
                List_new(&(basicblockP->Predecessors));
                // go and get the instructions of the current basic block
                continue;

            } else if (currentLine[0]=='.') {  //a label within a function is found
               
                printf("a label within the current function found-> %s\n",currentLine);

                // the current bb will flow through to this new bb unless there was a jmp instr at the end of this bb
                if(!seen_unc_jump) 
                  basicblockP->thenLabel = strdup(currentLine);

                seen_unc_jump=false;

                //create a new basic block for the instructions of the current label
                basicblockP = (BasicBlock *)calloc(1,sizeof(BasicBlock)); //creating memory space
                basicblockP->label =strdup(currentLine);

                //add current basic block to the func basic blocks list
                List_pushElement_back( &(funcblockP->funcBBlist), basicblockP); 

                //create a list to hold the instructions in the basic block
                List_new(&(basicblockP->Instructions));
                List_new(&(basicblockP->Predecessors));

                // go and get the instructions of the current basic block
                continue;
            } 
        } 

        /*if we come here it means that the line is an instruction. 
          process it and add it to the current basic block of the current func block */
        
         // instp will have a pointer to a new instruction block after processing
         if (!process_instruction(currentLine, &instp)) continue;  //skip the instruction if something went wrong

         if (instp->grpid == INS_GRP_CJMP ) { //for example-> jle .L2

           printf("processing conditional jump instruction->%s\n",instp->mnemonic);
           basicblockP->thenLabel = strdup(instp->operands[0].op_string);  //etc .L2
           // create a temp label for else instructions bb to be created
           char *templabel = get_temp_label();
           basicblockP->elseLabel = templabel;  //etc .TL3
           basicblockP->compOperator = strdup(instp->mnemonic);  //etc jle

           //pop the last instruction from current basic block. this must be the compare instruction before the
           //existing cond jump instruction
           /*
           instp = (Instruction *)List_popElement_last(&(basicblockP->Instructions));
        
            basicblockP->leftOp = strdup(instp->operands[0].op_string);
            if (instp->opcount==2)
               basicblockP->rightOp = strdup(instp->operands[1].op_string);
            */

            //create a new basic block for the else instructions 
            basicblockP = (BasicBlock *)calloc(1,sizeof(BasicBlock)); //creating memory space
            basicblockP->label = templabel;
              
            //add the new basic block to the func basic blocks list
            List_pushElement_back( &(funcblockP->funcBBlist), basicblockP); 
            //create a list to hold the instructions in the basic block
            List_new(&(basicblockP->Instructions));
            List_new(&(basicblockP->Predecessors));

         } else if (instp->grpid == INS_GRP_JMP ) {
              
            //for example -> jmp .L3
            //if exists, this can only be the last instruction of the current bb
            printf("processing jump instruction->%s\n",instp->mnemonic);
            basicblockP->thenLabel = strdup(instp->operands[0].op_string); //etc .L3
            seen_unc_jump = true;

         } else
        
              List_pushElement_back( &(basicblockP->Instructions), instp);
        
    }

   //process_target_blocks(funcBlocks);

   //process_string_constants(funcBlocks);
 
   
}

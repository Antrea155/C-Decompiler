#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"
#include "models/dflow.h"

void display_symbols(void);
extern bool is64bits;

List *stack;
#define push( listElement ) _List_pushElement( stack, (ListElement *)(listElement) )
#define pop()  (Symbol *)List_popElement( stack )
#define top()  (Symbol *)List_getHead( stack )

//int glob_veriable=5;

 List *globalSymbols;
 List *funcSymTable;
 List *curFuncSymbs;
 List *stringLabels;

 bool seenOnce;

//return "local"(-offset) if offset < 0 and "par"offset otherwise
char  *genUniqName( int offset )
{
    char *name = calloc(15,sizeof(char));
    char buf[5];

    memset(buf,0,5);
    itoa(abs(offset),buf,10);

    if (offset<0) {
        strcpy(name,"local_");
        strcat(name, buf);
    } else {
        strcpy(name,"par_");
        strcat(name, buf);
    }
    //printf("uniq->%s\n",name);
	return name;
}

void handle_pars(char *funcName, FuncSymBlock *fsb) {

    char funcn[15]; memset(funcn,0,15);
    //char parms[50]; memset(parms,0,50);
    char *pos = strchr(funcName ,'('); 
    if (!pos) fsb->fname = strdup(funcName) ;
    else {
        strncpy(funcn, funcName, strlen(funcName)-strlen(pos));
        fsb->fname = strdup(funcn);
        //save the par types in (partype1, parype2,...)
        
       
        char *parms = strdup(pos+1);
        parms[strlen(parms)-1]=0;
        //printf("parms->%s\n",parms);
        char *parm = strtok(parms, ",");        
        int index = 0;
       /*Run over instruction line (word-by-word)*/
        while (parm != NULL) {
          if (parm[0]==' ') parm=parm+1;
          fsb->parmtypes[index] = strdup(parm);
         // printf("parm->%s<-\n",fsb->parmtypes[index]);
          index++;
          parm = strtok(NULL, ",");
        }
    }
}

char *get_stringLabel(char *label) {

      ListElement *current = stringLabels->current;
      List_reset(stringLabels);     //point to the first block
    
      for (int i = 0; i < stringLabels->numItems; i++)  {

        StringBlock *sb = (StringBlock *)List_getNextElement(stringLabels);
        if (!strcmp(label,sb->label)) {
             stringLabels->current = current;
             printf("   found string %s for label %s\n",sb->string, label);
             return sb->string;
        }

      }
       stringLabels->current = current;
       printf ("  string label %s not found in stringBlocks\n",label);
}

char *get_Coper(char *insmnem) {

    //get the C operator from a conditional jump or set x86 instruction
    // ja, jg, seta, setg, je, jz, jge, jne, etc
    char *a = strchr(insmnem,'a');
    char *g = strchr(insmnem,'g');

    char *e = strchr(insmnem,'e');
    char *z = strchr(insmnem,'z');

    char *b = strchr(insmnem,'b');
    char *l = strchr(insmnem,'l');

    char *n = strchr(insmnem,'n');

    if ((g && e) || (a && e)) return (">=");
    else if ( (b && e) || (l && e)) return ("<=");
    else if ( (n && e) || (n && z)) return ("!=");
    else if (a || g) return (">");
    else if (e || z) return ("==");
    else if (b || l) return ("<");
    else return ("nop");

}

void add_toClikeIns(char *clins, BasicBlock *bb) {

    ClikeIns *cins = (ClikeIns *)calloc(1, sizeof(ClikeIns));
    cins->cins = strdup(clins);
    List_pushElement_back(&(bb->ClikeInsL),cins);
}

Symbol *new_symbol(char *index1, int index2 ) {
    
    Symbol *sym = (Symbol *)calloc(1,sizeof(Symbol));
    if (index1) sym->index1 = strdup(index1);
    if (index2) sym->index2 = index2;
    
    
    return sym;

}

Symbol *cpy_sym(Symbol *symb1 ) {
    
    Symbol *sym = (Symbol *)calloc(1,sizeof(Symbol));
    memcpy(sym,symb1,sizeof(Symbol));
    
    return sym;

}

void add_REGsymbol(char *index, int size) {
    
    Symbol *sym = (Symbol *)calloc(1,sizeof(Symbol));
    sym->index1 = strdup(index);
    sym->name= strdup(index);
    sym->value = strdup(index);
    sym->nameSet = true;
    sym->size = size;
    List_pushElement_back(globalSymbols,sym);

}


void add_symbol(Symbol *sym) {
    
    if (sym->index1) {
      sym->name = strdup(sym->index1);
      sym->value = strdup(sym->index1);
      sym->nameSet = true;
      List_pushElement_back(globalSymbols,sym);
    }
    else if (sym->index2) {
       List_pushElement_back(curFuncSymbs,sym);
    } else 
       printf("add_symbol-> index not specified\n");

}

void init_REGS_symbs() {

    add_REGsymbol("%eax", 4);
    add_REGsymbol("%ebx", 4 );
    add_REGsymbol("%ecx", 4 );
    add_REGsymbol("%edx", 4 );
    add_REGsymbol("%esp", 4 );
    add_REGsymbol("%ebp", 4 );
    add_REGsymbol("%edi", 4 );
    add_REGsymbol("%esi", 4 );

    add_REGsymbol("%rax", 8 );
    add_REGsymbol("%rbx", 8 );
    add_REGsymbol("%rcx", 8 );
    add_REGsymbol("%rdx", 8 );
    add_REGsymbol("%rsp", 8 );
    add_REGsymbol("%rbp", 8 );
    add_REGsymbol("%rdi", 8 );
    add_REGsymbol("%rsi", 8 );
   

}

Symbol *get_symbAt(char *index1, int index2) {

   if (index1) {
     
      ListElement *current = globalSymbols->current;
      List_reset(globalSymbols);     //point to the first block
    
      for (int i = 0; i < globalSymbols->numItems; i++)  {

        Symbol *sym = (Symbol *)List_getNextElement(globalSymbols);
        if (!strcmp(index1, sym->index1)) { 
            globalSymbols->current = current;
            if (!sym->nameSet) {
                sym->name = strdup(index1);
                sym->nameSet = true;
                if (!sym->value) sym->value = strdup(index1);
            }
            return (sym); }

       }
     
       globalSymbols->current = current;
       printf("get_symbAt->symbol not found %s\n",index1);  
       return NULL;

   } else if (index2) {

        ListElement *current = curFuncSymbs->current;
        List_reset(curFuncSymbs);     //point to the first block
    
        for (int i = 0; i < curFuncSymbs->numItems; i++)  {

            Symbol *sym = (Symbol *)List_getNextElement(curFuncSymbs);
            if (index2 == sym->index2) {
                curFuncSymbs->current = current;
                return (sym); 
            } 
        }
     
       //not found. add it
       curFuncSymbs->current = current;
       Symbol *newSym= new_symbol(0,index2);
       newSym->name = genUniqName(index2);
       newSym->value = strdup(newSym->name);
       //TO DO check maxLocaOffset of FuncSymBlock
       add_symbol(newSym);
     
       printf("get_symbAt->symbol not found. added %d\n",index2);  
       return newSym;

   } else {
     printf("get_symbAt->an index must be given\n");
     return NULL;
   }

   
}

void upd_symbAt(char *index1, int index2, Symbol *newSym) {

   if (index1) {
     
      ListElement *current = globalSymbols->current;
      List_reset(globalSymbols);     //point to the first block
    
      for (int i = 0; i < globalSymbols->numItems; i++)  {

        Symbol *sym = (Symbol *)List_getNextElement(globalSymbols);
        if (!strcmp(index1, sym->index1)) { 

            globalSymbols->current = current; 
            if (newSym->name) sym->name = strdup(newSym->name);
            if (newSym->reference) sym->reference = newSym->reference;
            if (newSym->size) sym->size = newSym->size;
            if (newSym->value) sym->value = newSym->value;
            return;
            }

       }
     
       globalSymbols->current = current;
       printf("symbol not found %s\n",index1);  
       return NULL;

   } else if (index2) {

   } else {
     printf("an index must be given\n");
     return NULL;
   }

   
}

void analyze_inst(Instruction *ins, BasicBlock *bb){
    printf("    analyzing Instruction->%s\n",ins->mnemonic);
    char temp[50];
    int insGrpId = ins->grpid;
    int op1type1 = ins->operands[0].type1;
    int op1type2 = ins->operands[0].type2;
    int op2type1 = ins->operands[1].type1;
    int op2type2 = ins->operands[1].type2;

    Symbol *symbol, *reg, *reg2, *var;

   if (insGrpId == INS_GRP_PUSH) { // push callee func parms to stack. for x86-32

        if (op1type2 == OP_TYPE_SBR); //ignore pushing the stack base reg
        else if ((op1type1 == OP_TYPE_REG) && seenOnce) {
            reg = cpy_sym(get_symbAt(ins->operands[0].value.reg,0));
            push(reg);
        } else if ((op1type2 == OP_TYPE_SBP) && seenOnce) {
            var = cpy_sym(get_symbAt(0,ins->operands[0].ptr.offset));
            push(var);
        } else if ((op1type2 == OP_TYPE_LABEL) && seenOnce) {
            char *string = get_stringLabel(ins->operands[0].value.imm);
            symbol = (Symbol *)calloc(1,sizeof(Symbol));
            symbol->value = strdup(string);
            push(symbol);
        } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_CALL) {

        memset(temp,0,30);
        strcpy(temp,ins->operands[0].op_string);
        strcat(temp,"(");

        List_reset(stack);     printf("parms in stack->%d\n",stack->numItems);

        for (int i = 0; i < (stack->numItems)-1; i++) {
            symbol = pop();
            strcat(temp,symbol->value); strcat(temp,",");
        }
        symbol = pop();
        strcat(temp,symbol->value); strcat(temp,")");

        reg = (is64bits) ? get_symbAt("%rax",0): get_symbAt("%eax",0);
        reg->value = strdup(temp);
        printf("call->%s\n",reg->value);
        List_destroy(stack);
        //TODO before pop save in callees funcsymblock


   } else if (insGrpId == INS_GRP_MOV) {

        if ((op1type1 == OP_TYPE_REG) && (op2type1 == OP_TYPE_REG)) {
            if ((op2type2 == OP_TYPE_ARG) && (strlen(ins->mnemonic) == 4)) { //x86-64 places pars for callee in edi, esi,..
              reg = cpy_sym(get_symbAt(ins->operands[0].value.reg,0));
              push(reg); printf("pushed in stack\n");
            } else {
              reg = get_symbAt(ins->operands[0].value.reg,0);
              upd_symbAt(ins->operands[1].value.reg,0,reg);
            }

        } else if ((op1type2 == OP_TYPE_LABEL) && (op2type2 == OP_TYPE_ARG)) { //x86-64 places pars for callee in edi, esi,..
            char *string = get_stringLabel(ins->operands[0].value.imm);
            symbol = (Symbol *)calloc(1,sizeof(Symbol));
            symbol->value = strdup(string);
            push(symbol);

        } else if ((op1type1 == OP_TYPE_REG) && (op2type2 == OP_TYPE_LABEL)) {
              printf("  ->>instruction not analyzed<<---\n");

        } else if ((op1type2 == OP_TYPE_NUM) && (op2type2 == OP_TYPE_SBP)) {
            var = get_symbAt(0,ins->operands[1].ptr.offset);
            printf("Ins->%s = %s;\n",var->name,ins->operands[0].value.imm);
            sprintf(temp, "%s = %s; ",var->name,ins->operands[0].value.imm);
            add_toClikeIns(temp, bb);

        } else if ((op1type2 == OP_TYPE_NUM) && (op2type1 == OP_TYPE_REG)) {
            reg = get_symbAt(ins->operands[1].value.reg,0);
            reg->value = strdup(ins->operands[0].value.imm); //TODO if rax could be the return value
            if (!strncmp(reg->index1,"%rax",4) ) printf("rax->%s\n",reg->value);
            if (!strncmp(reg->index1,"%eax",4) ) printf("eax->%s\n",reg->value);
 
        } else if ((op1type2 == OP_TYPE_SBP) && (op2type1 == OP_TYPE_REG)) {
            var = get_symbAt(0,ins->operands[0].ptr.offset);
            reg = get_symbAt(ins->operands[1].value.reg,0);
            reg->reference = var->reference;
            reg->value = strdup(var->value);
        
        } else if ((op1type1 == OP_TYPE_REG) && (op2type2 == OP_TYPE_SBP)) {
            reg = get_symbAt(ins->operands[0].value.reg,0);
            var = get_symbAt(0,ins->operands[1].ptr.offset);
            var->reference = reg->reference;
            //in x86-64 linux edi, esi, edx, ecx are used to stored passed parameters to a function. change var name to par_
            if (op1type2 == OP_TYPE_ARG) {
                var->name = genUniqName(abs(ins->operands[1].ptr.offset));
                var->value = strdup(var->name);
            }
            printf("Ins2->%s = %s;\n",var->name,reg->value);
            sprintf(temp, "%s = %s; ",var->name,reg->value);
            if (reg->value[0]!='%') add_toClikeIns(temp, bb);

        } else if ((op1type1 == OP_TYPE_REG) && (op2type2 == OP_TYPE_SBP)) {
            reg = get_symbAt(ins->operands[0].value.reg,0);
            var = get_symbAt(0,ins->operands[1].ptr.offset);
            printf("Ins->%s = %s;\n",var->name,reg->value);
            sprintf(temp,"%s = %s; ",var->name,reg->value);
            if (reg->value[0]!='%') add_toClikeIns(temp, bb);

        } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_ADD) {

        if ((op1type2 == OP_TYPE_NUM) && (op2type2 == OP_TYPE_SBP)) {
            var = get_symbAt(0,ins->operands[1].ptr.offset);
            printf("Ins->%s+= %s;\n",var->name,ins->operands[0].value.imm);
            sprintf(temp,"%s+= %s; ",var->name,ins->operands[0].value.imm);
            add_toClikeIns(temp, bb);
        } else if ((op1type1 == OP_TYPE_REG) && (op2type2 == OP_TYPE_SBP)) {
            reg = get_symbAt(ins->operands[0].value.reg,0);
            var = get_symbAt(0,ins->operands[1].ptr.offset);
            printf("Ins->%s+= %s;\n",var->name,reg->value);
            sprintf(temp,"%s+= %s; ",var->name,reg->value);
            add_toClikeIns(temp, bb);
        } else if ((op1type1 == OP_TYPE_REG) && (op2type1 == OP_TYPE_PTR)) {
            reg = get_symbAt(ins->operands[0].value.reg,0);
            var = get_symbAt(ins->operands[1].value.reg,0)->reference;
            printf("Ins->%s+= %s;\n",var->name,reg->value);
            sprintf(temp, "%s+= %s; ",var->name,reg->value);
            add_toClikeIns(temp, bb);
        } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_SUB) {

        if ((op1type2 == OP_TYPE_NUM) && (op2type2 == OP_TYPE_STR)) {
            if (seenOnce == false) {seenOnce = true; printf("seenOnce is true\n");}
        } else if (op2type2 == OP_TYPE_STR) ; //ignore mem allocation for stack 
        else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_CMP) {

       if ((op1type2 == OP_TYPE_SBP) && (op2type1 == OP_TYPE_REG)) {
         var = get_symbAt(0,ins->operands[0].ptr.offset);
         reg = get_symbAt(ins->operands[1].value.reg,0);
         bb->leftOp = strdup(reg->value);
         bb->rightOp = strdup(var->name);
         printf("left->%s, right->%s\n",bb->leftOp,bb->rightOp);

       } else if ((op1type1 == OP_TYPE_REG) && (op2type1 == OP_TYPE_REG)) {
         reg = get_symbAt(ins->operands[0].value.reg,0);
         reg2 = get_symbAt(ins->operands[1].value.reg,0);
         bb->leftOp = strdup(reg->value);
         bb->rightOp = strdup(reg2->value);
         printf("left->%s, right->%s\n",bb->leftOp,bb->rightOp);

       } else if ((op1type1 == OP_TYPE_REG) && (op2type2 == OP_TYPE_SBP)) {
         reg = get_symbAt(ins->operands[0].value.reg,0);
         var = get_symbAt(0,ins->operands[1].ptr.offset);
         bb->leftOp = strdup(var->name);
         bb->rightOp = strdup(reg->value);
         printf("left->%s, right->%s\n",bb->leftOp,bb->rightOp);

       } else if ((op1type2 == OP_TYPE_NUM) && (op2type2 == OP_TYPE_SBP)) {
         var = get_symbAt(0,ins->operands[1].ptr.offset);
         bb->leftOp = strdup(var->name);
         bb->rightOp = strdup(ins->operands[0].value.imm);
         printf("left->%s, right->%s\n",bb->leftOp,bb->rightOp);

       } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_MULT) {

      if ((op1type2 == OP_TYPE_SBP) && (op2type1 == OP_TYPE_REG)) {
         var = get_symbAt(0,ins->operands[0].ptr.offset);
         reg = get_symbAt(ins->operands[1].value.reg,0);
         sprintf(temp,"(%s)*%s",reg->value,var->name);
         reg->value = strdup(temp);
         printf("reg->%s\n",reg->value);

      } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_DIV) {
    
      if (op1type1 == OP_TYPE_REG)  { 
        reg = get_symbAt(ins->operands[0].value.reg,0);
        reg2 = (ins->size == 8 ) ? get_symbAt("%rdx",0) : get_symbAt("%edx",0);
        symbol = (ins->size == 8 ) ? get_symbAt("%rax",0) : get_symbAt("%eax",0);
        sprintf(temp,"(%s)%%%s",symbol->value,reg->value);
        reg2->value = strdup(temp);
        (ins->size == 8 ) ? printf("rdx->%s\n",reg2->value) : printf("edx->%s\n",reg2->value);
        sprintf(temp,"(%s)/%s",symbol->value,reg->value);
        symbol->value = strdup(temp);
        (ins->size == 8 ) ? printf("rax->%s\n",symbol->value) : printf("eax->%s\n",symbol->value);

      } else if (op1type2 == OP_TYPE_SBP)  {
        reg = (is64bits) ? get_symbAt("%rax",0) : get_symbAt("%eax",0); 
        var = get_symbAt(ins->operands[0].value.reg,0);
        sprintf(temp,"(%s)/%s",reg->value,var->name);
        //TODO change edx value
        reg->value = strdup(temp);
        printf("reg->%s\n",reg->value);
      } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_LEA) {

      if ((op1type2 == OP_TYPE_SBP) && (op2type1 == OP_TYPE_REG)) {
         var = get_symbAt(0,ins->operands[0].ptr.offset);
         reg = get_symbAt(ins->operands[1].value.reg,0);
         reg->reference = var;
         sprintf(temp,"&%s",var->name);
         reg->value = strdup(temp);
         printf("reg->%s\n",reg->value);
       } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_INC) {

      if (op1type1 == OP_TYPE_PTR) {
        symbol = get_symbAt(ins->operands[0].value.reg,0)->reference;
        printf("ins->++%s;\n",symbol->name);
        sprintf(temp,"++%s; ",symbol->name);
        add_toClikeIns(temp, bb);
       } else printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_TEST) {

       if ((op1type1 == OP_TYPE_REG) && (op2type1 == OP_TYPE_REG)) {
         reg = get_symbAt(ins->operands[0].value.reg,0);
         reg2 = get_symbAt(ins->operands[1].value.reg,0);
         if (!strcmp(reg->index1, reg2->index1)) bb->leftOp = strdup(reg2->value);
         else {
            sprintf(temp, "((%s)&%s)",reg2->value,reg2->value);
            bb->leftOp = strdup(temp);
         }
         bb->rightOp = strdup("0");
         printf("left->%s, right->%s\n",bb->leftOp,bb->rightOp);
       } else  printf("  ->>instruction not analyzed<<---\n");

   } else if (insGrpId == INS_GRP_CSET) {

       if (op1type1 == OP_TYPE_REG)  {
         reg = get_symbAt(ins->operands[0].value.reg,0);
         char *Coper = get_Coper(ins->mnemonic);
         sprintf(temp,"(%s%s%s)",bb->leftOp,Coper,bb->rightOp);
         reg->value = strdup(temp);
         printf("reg->%s\n",reg->value);

       } else  printf("  ->>instruction not analyzed<<---\n");

   } else
     printf("  ->>instruction not analyzed<<---\n");

}



void data_flow(List *funcBlocks, List *stringBlocks) {

   
    stringLabels = stringBlocks;
    stack = List_new(NULL);  //create the list for stack emulation
    globalSymbols = List_new(NULL);
    funcSymTable = List_new(NULL);

    init_REGS_symbs();


    List_reset(funcBlocks);     //point to the first funcblock

     for (int i = 0; i < funcBlocks->numItems; i++) {

       FuncBlock *funcblock = (FuncBlock *)List_getNextElement(funcBlocks);
       
       seenOnce = false;
       FuncSymBlock *funcSymBlock = (FuncSymBlock *)calloc(1, sizeof(FuncSymBlock));
       //remove pars from funcname and add them to the FuncSymblock in an array of 5
      
       handle_pars(funcblock->funcName,funcSymBlock); 
       
       printf("\n\ndata flow analysis for function -> %s\n",funcSymBlock->fname);
       List_pushElement_back(funcSymTable, funcSymBlock );
       curFuncSymbs = List_new(&funcSymBlock->funcsymbols); 
      
      //TODO do I need this?
      /*
       add_symbol(new_symbol(funcSymBlock->fname,0));  //type= LONG, size=0, val="" add to global sym list
       Symbol *tmpSym = (Symbol *)calloc(1,sizeof(Symbol)); 
       upd_symbAt(funcSymBlock->fname,0,tmpSym);
       free(tmpSym);  
      */
       //analyze the instructions in each basic block
       List *BBlist = &(funcblock->funcBBlist);
       List_reset(BBlist);

       for (int i = 0; i < BBlist->numItems; i++) {
       
          BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);
          List *instructions = &(bb->Instructions);
          List_reset(instructions);
          printf("\nanalyzing basic block->%s\n",bb->label);

          List_new(&bb->ClikeInsL);  //list to hold the new C like instructions for the block

          for (int i = 0; i < instructions->numItems; i++) {

             Instruction *inst = (Instruction *)List_getNextElement(instructions);
             analyze_inst(inst,bb);
             
          }

       }
       //display_symbols();  
       //TODO set return value
       Symbol *eax =  (is64bits) ? get_symbAt("%rax",0) : get_symbAt("%eax",0);
       funcSymBlock->retValue = strdup(eax->value);

     }

  


}


 void display_dfins(List *funcblocks) {

    
    List_reset(funcblocks);     //point to the first block

     for (int i = 0; i < funcblocks->numItems; i++) {

       FuncBlock *block = (FuncBlock *)List_getNextElement(funcblocks);
       printf("\nresults of flow analysis of BBs in function -> %s\n",block->funcName);

       List *BBlist = &(block->funcBBlist);
       List_reset(BBlist);

       for (int i = 0; i < BBlist->numItems; i++) {
           
              
            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);
            printf(" c like ins for basic block -> %s\n ins-> ",bb->label);

            List *clikeinsL = &(bb->ClikeInsL);
            List_reset(clikeinsL);
            for (int i = 0; i < clikeinsL->numItems; i++) {
               ClikeIns *cins= (ClikeIns *)List_getNextElement(clikeinsL);
               printf("%s ",cins->cins);
            }
            
            printf("\n leftOp->%s\n",bb->leftOp);
            printf(" rightOp->%s\n\n",bb->rightOp);
      

        }
      
    }
   
 }

//return the definition of a function
//funcname(param list)
//this will be called by the control flow analysis module

char *getFuncDefinition(char *funcname) {

  char *funcdef = calloc(1,40);
  List_reset(funcSymTable);

 // memset(funcdef, 0, 40);

  for (int i=0; i<funcSymTable->numItems; i++) {

    FuncSymBlock *funcsymblock = (FuncSymBlock *)List_getNextElement(funcSymTable);

    if (!strcmp(funcname, funcsymblock->fname)) {

      List *funcsymbols = &(funcsymblock->funcsymbols);
      List_reset(funcsymbols);
      strcpy(funcdef, funcname);
      strcat(funcdef, "(");
   
      int parm=0;
      for (int j=0; j<funcsymbols->numItems; j++) {
        Symbol *symbol = (Symbol *)List_getNextElement(funcsymbols);
        if (!strncmp(symbol->name, "par", 3)) {
          strcat(funcdef,symbol->name );
          strcat(funcdef,",");
          parm++;
        }
      }

      if (parm>0) funcdef[strlen(funcdef)-1]=0;  //delete the last ,
      strcat(funcdef,"){\n\n");
      break;
    }
  }

  if (strlen(funcdef)==0)
     printf("funcdef-> func %s not found in symbols table\n");
  
  return funcdef;

}



char *getFuncReturn(char *funcname) {

  char *funcret = calloc(1,40);
  List_reset(funcSymTable);

  for (int i=0; i<funcSymTable->numItems; i++) {

    FuncSymBlock *funcsymblock = (FuncSymBlock *)List_getNextElement(funcSymTable);

    if (!strcmp(funcname, funcsymblock->fname)) {
       strcpy(funcret, "return ");
       strcat(funcret, funcsymblock->retValue);
       strcat(funcret,"\n}\n\n");
       break;
    }
  }

  if (strlen(funcret)==0)
     printf("funcRet-> func %s not found in symbols table\n");
  
  return funcret;

}


void display_symbols() {

      ListElement *current = globalSymbols->current;
      List_reset(globalSymbols);     //point to the first block
    
      printf("GLOBAL symbols\n");
      for (int i = 0; i < globalSymbols->numItems; i++)  {

        Symbol *sym = (Symbol *)List_getNextElement(globalSymbols);
        printf("index1->%s\n",sym->index1);
        if (sym->name) printf("     name->%s\n",sym->name);
        if (sym->value) printf("    value->%s\n",sym->value);
      }

       globalSymbols->current = current;
       
       current = curFuncSymbs->current;
       List_reset(curFuncSymbs);     //point to the first block
       printf("CURRENT FUNC  symbols\n");
       for (int i = 0; i < curFuncSymbs->numItems; i++)  {

        Symbol *sym = (Symbol *)List_getNextElement(curFuncSymbs);
        printf("index2->%d\n",sym->index2);
        if (sym->name) printf("     name->%s\n",sym->name);
        if (sym->value) printf("    value->%s\n",sym->value);
      }

      curFuncSymbs->current = current;

}
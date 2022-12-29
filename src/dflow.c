#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"
#include "models/dflow.h"

List *stack;
#define push( listElement ) _List_pushElement( stack, (ListElement *)(listElement) )
#define pop()  (Symbol *)List_popElement( stack )
#define top()  (Symbol *)List_getHead( stack )

//int glob_veriable=5;

 List *globalSymbols;
 List *funcSymTable;
 List *curFuncSymbs;

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
	return name;
}

void display_symbols() {

      ListElement *current = globalSymbols->current;
      List_reset(globalSymbols);     //point to the first block
    
      printf("GLOBAL symbols\n");
      for (int i = 0; i < globalSymbols->numItems; i++)  {

        Symbol *sym = (Symbol *)List_getNextElement(globalSymbols);
        printf("index1->%s\n",sym->index1);
        if (sym->name) ("name->%s\n",sym->name);
        if (sym->value) ("value->%s\n",sym->value);
      }

       globalSymbols->current = current;
       
       current = curFuncSymbs->current;
       List_reset(curFuncSymbs);     //point to the first block
       printf("CURRENT FUNC  symbols\n");
       for (int i = 0; i < curFuncSymbs->numItems; i++)  {

        Symbol *sym = (Symbol *)List_getNextElement(curFuncSymbs);
        printf("index1->%s\n",sym->index1);
        if (sym->name) ("name->%s\n",sym->name);
        if (sym->value) ("value->%s\n",sym->value);
      }

      curFuncSymbs->current = current;

}

void handle_pars(char *funcName, FuncSymBlock *fsb) {

    char funcn[15]; memset(funcn,0,15);
    char *pos = strchr(funcName ,'('); 
    if (!pos) fsb->fname = strdup(funcName) ;
    else {
        strncpy(funcn, funcName, strlen(funcName)-strlen(pos));
        fsb->fname = strdup(funcn);
    }
}

Symbol *new_symbol(char *index1, int index2 ) {
    
    Symbol *sym = (Symbol *)calloc(1,sizeof(Symbol));
    if (index1) sym->index1 = strdup(index1);
    if (index2) sym->index2 = index2;
    sym->type = LONG;
    
    return sym;

}

void add_REGsymbol(char *index, int size) {
    
    Symbol *sym = (Symbol *)calloc(1,sizeof(Symbol));
    sym->index1 = strdup(index);
    sym->name= strdup(index);
    sym->value = strdup(index);
    sym->nameSet = true;
    sym->type = REG;
    sym->size = size;
    sym->isReg = true;
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
     
       //not found.
       globalSymbols->current = current;
       Symbol *newSym= new_symbol(0,index2);
       newSym->name = genUniqName(index2);
       newSym->value = strdup(newSym->name);
       //TO DO check maxLocaOffset of FuncSymBlock
       add_symbol(newSym);
     
       printf("get_symbAt->symbol not found. added %s\n",index2);  
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
            if (newSym->seen) sym->seen = newSym->seen;
            if (newSym->size) sym->size = newSym->size;
            if (newSym->type) sym->type = newSym->type;
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

void data_flow(List *funcBlocks, List *stringBlocks) {

   

    stack = List_new(NULL);  //create the list for stack emulation
    globalSymbols = List_new(NULL);
    funcSymTable = List_new(NULL);

    init_REGS_symbs();

        BasicBlock *targetBlock=0;

    List_reset(funcBlocks);     //point to the first funcblock

     for (int i = 0; i < funcBlocks->numItems; i++) {

       FuncBlock *funcblock = (FuncBlock *)List_getNextElement(funcBlocks);
       //char *funcName = strdup(funcblock->funcName);
       
       seenOnce = false;
       FuncSymBlock *funcSymBlock = (FuncSymBlock *)calloc(1, sizeof(FuncSymBlock));
       //TO DO remove pars from funcname and add them to the FuncSymblock in an array of 5
       handle_pars(funcblock->funcName,funcSymBlock);
       //funcSymBlock->fname = funcName;
       printf("\n\ndata flow analysis for function -> %s\n",funcSymBlock->fname);
       List_pushElement_back(funcSymTable, funcSymBlock );
       curFuncSymbs = List_new(&funcSymBlock->funcsymbols); 
      
       add_symbol(new_symbol(funcSymBlock->fname,0));  //type= LONG, size=0, val="" add to global sym list
       Symbol *tmpSym = (Symbol *)calloc(1,sizeof(Symbol));
       tmpSym->type = FUNC;  
       upd_symbAt(funcSymBlock->fname,0,tmpSym);
       free(tmpSym);
       //List_pushElement_back(, funcSymBlock );

       List *BBlist = &(funcblock->funcBBlist);
       List_reset(BBlist);

         for (int i = 0; i < BBlist->numItems; i++) {
         }


     }

     display_symbols();
  
   /*
    Symbol *sym = (Symbol *)calloc(1,sizeof(Symbol));
    sym->name  = strdup("symbname");
    push(sym);  printf("stck size->%d\n", stack->numItems);
    sym=pop();  printf("stck size->%d\n", stack->numItems);
    printf("pop->%s\n", sym->name);

    sym= get_symbAt("%rax",0);
    printf("symbat->%s\n",sym->name);

    Symbol *sym2 = (Symbol *)calloc(1,sizeof(Symbol));
    sym2->name = strdup("newname");
    upd_symbAt("%rax",0,sym2);
    sym= get_symbAt(0,0);
    printf("symbat->%s\n",sym->name);
   */


}
/*
int main() {
   // int glob_veriable=2;
    data_flow(NULL, NULL);
}*/
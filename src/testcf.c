#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"


/*
 a program to test the control flow analysis module
 a number of basic blocks are created, labeled B1 - B15, with no instructions and linked in accordance with fig 2 in 
 article "structuring decompiled graphs" https://link.springer.com/content/pdf/10.1007%2F3-540-61053-7_55.pdf
 the control flow module is then called and must verify that:
   the test CFG is firstly divided to correct number of intervals and then T2 transformations are
   applied to these intervals  until the limit Graph is reached
*/

void control_flow(List *funcBlocks);
void display_successors(List *funcblocks);
void display_predecessors(List *funcBlocksP);

List *funcBlocks;         //list of pointers to Blocks
  
List *BBlist;
int glob_veriable = 2;

BasicBlock *findbb( char *label) {
    
    List_reset(BBlist);     //point to the first block
    
    for (int i = 0; i < BBlist->numItems; i++)  {

      BasicBlock *block = (BasicBlock *)List_getNextElement(BBlist);
      if (!strcmp(label, block->label)) return (block);

    }

    printf("target block %s not found\n",label);
    return 0;

}

void addbb(char *label) {

       
        //create a new basic block for the instructions of the current label
    BasicBlock *basicblockP = (BasicBlock *)calloc(1,sizeof(BasicBlock)); //creating memory space
    basicblockP->label =strdup(label);
    //add current basic block to the func basic blocks list
    List_pushElement_back( BBlist, basicblockP); 
        //create a list to hold the predecessors in the basic block
    List_new(&(basicblockP->Instructions));
    List_new(&(basicblockP->Predecessors));
}

void linkbb(char *label, char *thenlabel, char *elselabel) {

    BasicBlock *bb = findbb(label);

    BasicBlock *thenbb = findbb(thenlabel);
    bb->thenBB = thenbb;

    List *predecessors = &(thenbb->Predecessors);
    Predecessor *bbptr = (Predecessor *)malloc(sizeof(Predecessor));
    bbptr->bbptr = bb;
    List_pushElement_back(predecessors,bbptr);

    if (elselabel) {
        BasicBlock *elsebb = findbb(elselabel);
        bb->elseBB = elsebb;
        List *predecessors = &(elsebb->Predecessors);
        Predecessor *bbptr = (Predecessor *)malloc(sizeof(Predecessor));
        bbptr->bbptr = bb;
        List_pushElement_back(predecessors,bbptr);
    }
}

int main() {

    funcBlocks = List_new(NULL);  //create the list of function blocks

        //create a new func block 
    FuncBlock *funcblockP = (FuncBlock *)calloc(1,sizeof(FuncBlock)); //creating memory space
    funcblockP->funcName = strdup("testFunc");

    
    //add current func block to the func blocks list
    List_pushElement_back( funcBlocks, funcblockP); 
    
    //create a list to hold the basic blocks of the new function
    BBlist = &(funcblockP->funcBBlist);
    List_new(BBlist);

    addbb("B1");
    addbb("B2");
    addbb("B3");
    addbb("B4");
    addbb("B5");
    addbb("B6");
    addbb("B7");
    addbb("B8");
    addbb("B9");
    addbb("B10");
    addbb("B11");
    addbb("B12");
    addbb("B13");
    addbb("B14");
    addbb("B15");
    printf("finished adding basic blocks\n");

    linkbb("B1","B5","B2"); 
    linkbb("B2","B4","B3");
    linkbb("B3","B5",0);
    linkbb("B4","B5",0);
    linkbb("B5","B6",0);
    linkbb("B6","B12","B7");
    linkbb("B7","B9","B8");
    linkbb("B8","B10","B9");
    linkbb("B9","B10",0);
    linkbb("B10","B11",0);
    //linkbb("B11","B12","B7");
    linkbb("B12","B13",0);
    linkbb("B13","B14",0);
    linkbb("B14","B13","B15");
    linkbb("B15","B6",0);

    printf("finished linking basic blocks\n");

    display_successors(funcBlocks);
    display_predecessors(funcBlocks);

    control_flow(funcBlocks);

     display_successors(funcBlocks);
    display_predecessors(funcBlocks);
}



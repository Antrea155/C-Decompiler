#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"
#include "models/cflow.h"

#define MAX_BBs 30
//List *funcBlocksList;
void display_BBs_seq(List *funcblocks);
List *curBBlist;

void printf_interval(IntervalBlock *interval) {

    printf("Interval head ->%d\n",interval->ihead->pos);
    List *bbsIninterval = &(interval->BBsInInterval);
     ListElement *current = bbsIninterval->current;
     List_reset(bbsIninterval);     //point to the first block
    printf("BBs in interval -> ");
      for (int i = 0; i < bbsIninterval->numItems; i++)  {

        BBnode *bb = (BBnode *)List_getNextElement(bbsIninterval);
        printf("%d ",bb->bbptr->pos);

      }  
             bbsIninterval->current = current;
            // printf("   found string %s for label %s\n",sb->string, label);
      
    printf("\n");
     

}

void number_bbs(BasicBlock *bb, int *n) {

    if ((bb) && (!bb->visited)) {
      bb->visited = true;
      number_bbs(bb->thenBB, n);
      number_bbs(bb->elseBB, n);
      bb->pos = *n;  
      *n = *n-1; 
    }

}

BasicBlock *get_bb_in_cfg(int pos, List *BBlist) {

      ListElement *current = BBlist->current;
      List_reset(BBlist);     //point to the first block
    
      for (int i = 0; i < BBlist->numItems; i++)  {

        BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);

        if (bb->pos == pos) {  
             BBlist->current = current;
            // printf("   found string %s for label %s\n",sb->string, label);
             return bb;
        }

      }
       BBlist->current = current;
       printf ("  bb with pos %d not found in bblist\n",pos);
       return NULL;
} 


bool exists(int pos, List *IntervalList) {

  ListElement *current = IntervalList->current;
  
  List_reset(IntervalList);
  for (int i=0;i<IntervalList->numItems;i++) {
    BBnode *intervalNode = (BBnode *)List_getNextElement(IntervalList);
    if (intervalNode->bbptr->pos == pos ) {
       IntervalList->current = current;
       return true; //printf("exists\n");
    }
  }

   IntervalList->current = current; //printf("not exists\n");
   return false;
  
}

bool includesAll(List *IntervalList, List *predecesorsList) {

  ListElement *current = predecesorsList->current;
 
  List_reset(predecesorsList);
  for (int i=0;i<predecesorsList->numItems;i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(predecesorsList);
    if (!exists(pred->bbptr->pos, IntervalList )) {
       predecesorsList->current = current; 
       return false;
    }
  }

   predecesorsList->current = current; //printf("inlclude\n");
   return true;
  
}

bool delete_from_inprocess(BBnode *bb, List *inprocessList) {

   ListElement *current = inprocessList->current;
 // printf("find pos->%d\n",bb->bbptr->pos);
   List_reset(inprocessList);
   for (int i=0;i<inprocessList->numItems;i++) {
    
    BBnode *procNode = (BBnode *)List_getNextElement(inprocessList);
   // printf("proclist->%d\n", procNode->bbptr->pos);
    if (procNode->bbptr->pos == bb->bbptr->pos ) {  
       //if we are about to delete the current element then adjust the current pointer  
       //so that it points to the previous element
       if (current == (ListElement *)procNode) {
         if (current->prev == NULL) current = NULL;
         else current = current->prev; 
       }
       List_remove(inprocessList, procNode);
       inprocessList->current = current;
       return true;
    }
  }

   inprocessList->current = current;
   return false;

}

void findIntervals() {

  bool visited[MAX_BBs] = {false};
  List *inprocessList = List_new(NULL);

  BasicBlock *bb = (BasicBlock *)List_getHead(curBBlist);
  visited[bb->pos] = true;
  BBnode *newBBforProcess = (BBnode *)calloc(1,sizeof(BBnode));
  newBBforProcess->bbptr = bb;
  List_pushElement_back(inprocessList,newBBforProcess); //printf("added in process entry ->%d\n",newBBforProcess->bbptr->pos);
//int counter=0;

  while ((inprocessList->numItems > 0) ) {

    IntervalBlock *curInterval = (IntervalBlock *)calloc(1, sizeof(IntervalBlock));
    curInterval->ihead = ((BBnode *)List_getHead(inprocessList))->bbptr;// printf("assigned head ->%d\n",curInterval->ihead->pos);
    List_new(&(curInterval->BBsInInterval));
    bool added = true;

    while (added) {  

        added = false;
        List_reset(inprocessList);
        //int items_in_processing_list = inprocessList->numItems;
        for (int i=0;i<inprocessList->numItems;i++) { //printf("in for i->%d items->%d\n",i,items_in_processing_list);

            BBnode *bbinProcess = (BBnode *)List_getNextElement(inprocessList); //printf("got bb->%d\n",bbinProcess->bbptr->pos);
            if ((curInterval->ihead == bbinProcess->bbptr) || 
                      (includesAll(&(curInterval->BBsInInterval), &(bbinProcess->bbptr->Predecessors)) )) {

                BBnode *aBB = (BBnode *)calloc(1,sizeof(BBnode));
                aBB->bbptr = bbinProcess->bbptr;
                List_pushElement_back(&(curInterval->BBsInInterval),aBB); // printf("added in interval ->%d\n",aBB->bbptr->pos);
                bbinProcess->bbptr->head = curInterval->ihead;

                if ((bbinProcess->bbptr->elseBB) && (!visited[bbinProcess->bbptr->elseBB->pos]) ) {
                    visited[bbinProcess->bbptr->elseBB->pos] = true;
                    newBBforProcess = (BBnode *)calloc(1,sizeof(BBnode));
                    newBBforProcess->bbptr = bbinProcess->bbptr->elseBB;
                    List_pushElement_back(inprocessList,newBBforProcess); //printf("added in process else ->%d\n",newBBforProcess->bbptr->pos);
                    added = true;
                }
                if ((bbinProcess->bbptr->thenBB) && (!visited[bbinProcess->bbptr->thenBB->pos]) ) {
                    visited[bbinProcess->bbptr->thenBB->pos] = true;
                    newBBforProcess = (BBnode *)calloc(1,sizeof(BBnode));
                    newBBforProcess->bbptr = bbinProcess->bbptr->thenBB;
                    List_pushElement_back(inprocessList,newBBforProcess); //printf("added in process then ->%d\n",newBBforProcess->bbptr->pos);
                    added = true;
                }

               
                delete_from_inprocess(bbinProcess, inprocessList);
               // if (!delete_from_inprocess(bbinProcess, inprocessList)) printf("NOT DEeleted\n"); else printf("deleted->%d\n",bbinProcess->bbptr->pos);
            }
        }
        
    } 
     printf("\nINTERVAL\n");
     printf_interval(curInterval);
     printf("END INTERVAL\n");
  } 

}

void control_flow(List *funcBlocks) {

 

   List_reset(funcBlocks);     //point to the first funcblock

   for (int i = 0; i < funcBlocks->numItems; i++) {

       FuncBlock *funcblock = (FuncBlock *)List_getNextElement(funcBlocks);

       List *BBlist = &(funcblock->funcBBlist);
       List_reset(BBlist);

       BasicBlock *firstbb = (BasicBlock *)List_getHead(BBlist);
       int n = BBlist->numItems;
       //assign a number to each bb in accordance with its position in CFG
       number_bbs(firstbb, &n);
//display_BBs_seq(funcBlocks);
       curBBlist = BBlist;
       findIntervals();
   }

}

void display_BBs_seq(List *funcblocks) {

    
    List_reset(funcblocks);     //point to the first block

     for (int i = 0; i < funcblocks->numItems; i++) {

       FuncBlock *block = (FuncBlock *)List_getNextElement(funcblocks);
       printf("\n\nsequence of BBs in LinkList for function -> %s\n",block->funcName);

       List *BBlist = &(block->funcBBlist);
       List_reset(BBlist);

         for (int i = 0; i < BBlist->numItems; i++) {
            
            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);
            printf("[%d %s], ",bb->pos, bb->label);

         }

         printf("\n\nsequence of BBs in CFG for function -> %s\n",block->funcName);

         for (int i = 0; i < BBlist->numItems; i++) {
            
            BasicBlock *bb = (BasicBlock *)get_bb_in_cfg(i+1, BBlist);
            printf("[%d %s], ",bb->pos, bb->label);

         }
         
    }
   
    }
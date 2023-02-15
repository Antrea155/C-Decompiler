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
void display_BBs_seq();
void print_interval(IntervalBlock *interval);
void display_allfuncIntervals();

List *curBBlist;
List *curAllfuncIntervals;


void number_bbs(BasicBlock *bb, int *n) {

    if ((bb) && (!bb->visited)) {
      bb->visited = true;
      number_bbs(bb->elseBB, n);
      number_bbs(bb->thenBB, n);
      bb->pos = *n;  
      *n = *n-1; 
    }

}

void set_all_not_visited() {

     List_reset(curBBlist);

      for (int i = 0; i < curBBlist->numItems; i++)  {

        BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);

        bb->visited = false;

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
    
    BBnode *processingNode = (BBnode *)List_getNextElement(inprocessList);
   // printf("proclist->%d\n", processingNode->bbptr->pos);
    if (processingNode->bbptr->pos == bb->bbptr->pos ) {  
       //if we are about to delete the current element then adjust the current pointer  
       //so that it points to the previous element
       if (current == (ListElement *)processingNode) {
         if (current->prev == NULL) current = NULL;
         else current = current->prev; 
       }
       List_remove(inprocessList, processingNode);
       inprocessList->current = current;
       return true;
    }
  }

   inprocessList->current = current;
   return false;

}

int noOfBBchildren(BasicBlock *bb) {

    return ( bb->thenBB ? 1 : 0) + (bb->elseBB ? 1 : 0);
}

bool canApplyT2(BasicBlock *bb) {

    if (noOfBBchildren(bb) == 1) {
         if ((bb != bb->thenBB) && (bb->thenBB->Predecessors.numItems == 1) && (noOfBBchildren(bb->thenBB)<=1) )
           return true;
    }
    return false;
}

// return intervalBlock that bb belongs to
IntervalBlock *get_IntervalBlock(BasicBlock *bb) {

    List_reset(curAllfuncIntervals);
    for (int i = 0; i < curAllfuncIntervals->numItems; i++)  {

      IntervalBlock *interval = (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
      if (interval->ihead->pos == bb->head->pos)
        return interval;
    }
    printf("interval for bb %d not found\n",bb->pos);

}



void findIntervals() {

 // bool visited[MAX_BBs] = {false};
  set_all_not_visited();
  List *inprocessList = List_new(NULL);

  //get the first BB from the BBlist of the function and add it to the processing list
  BasicBlock *bb = (BasicBlock *)List_getHead(curBBlist);
 // visited[bb->pos] = true;
  bb->visited = true;
  BBnode *newBBforProcess = (BBnode *)calloc(1,sizeof(BBnode));
  newBBforProcess->bbptr = bb;
  List_pushElement_back(inprocessList,newBBforProcess); //printf("added in process entry ->%d\n",newBBforProcess->bbptr->pos);


  while ((inprocessList->numItems > 0) ) {

    //while there are unprocessed BBs create a new interval with head BB the first BB in the unprocessed list
    IntervalBlock *curInterval = (IntervalBlock *)calloc(1, sizeof(IntervalBlock));
    curInterval->ihead = ((BBnode *)List_getHead(inprocessList))->bbptr;// printf("assigned head ->%d\n",curInterval->ihead->pos);
    List_new(&(curInterval->BBsInInterval));
    bool added = true;

    while (added) {  

        //exit the loop and create a new interval if no more BBs could be added to the current interval. 
        //if a BB is added to the current interval then the BB is deleted from the 
        //unprocessed list and its successors are added to it
        //a BB is added to the current interval if it is the head BB or all its predecessors are included in the
        //current interval list of BBs

        added = false;
        List_reset(inprocessList);
        //int items_in_processing_list = inprocessList->numItems;
        for (int i=0;i<inprocessList->numItems;i++) { //printf("in for i->%d items->%d\n",i,items_in_processing_list);

            BBnode *bbinProcess = (BBnode *)List_getNextElement(inprocessList); //printf("got bb->%d\n",bbinProcess->bbptr->pos);
            if ((curInterval->ihead == bbinProcess->bbptr) || 
                      (includesAll(&(curInterval->BBsInInterval), &(bbinProcess->bbptr->Predecessors)) )) {

                bbinProcess->bbptr->head = curInterval->ihead;
                BBnode *aBB = (BBnode *)calloc(1,sizeof(BBnode));
                aBB->bbptr = bbinProcess->bbptr;
                List_pushElement_back(&(curInterval->BBsInInterval),aBB); // printf("added in interval ->%d\n",aBB->bbptr->pos);
          
                if ((bbinProcess->bbptr->elseBB) && (!bbinProcess->bbptr->elseBB->visited) ) {
                    bbinProcess->bbptr->elseBB->visited = true;
                    newBBforProcess = (BBnode *)calloc(1,sizeof(BBnode));
                    newBBforProcess->bbptr = bbinProcess->bbptr->elseBB;
                    List_pushElement_back(inprocessList,newBBforProcess); //printf("added in process else ->%d\n",newBBforProcess->bbptr->pos);
                    added = true;
                }

                if ((bbinProcess->bbptr->thenBB) && (!bbinProcess->bbptr->thenBB->visited) ) {
                    bbinProcess->bbptr->thenBB->visited = true;
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
     //printf("\nINTERVAL\n");
    // print_interval(curInterval);
     //printf("END INTERVAL\n");
     // add current interval to all intervals
     List_pushElement_back(curAllfuncIntervals, curInterval);
  } 

}

void control_flow(List *funcBlocks) {

 
   
   List_reset(funcBlocks);     //point to the first funcblock

   for (int i = 0; i < funcBlocks->numItems; i++) {

       FuncBlock *funcblock = (FuncBlock *)List_getNextElement(funcBlocks);
       printf("flow control analysis for function ->%s\n",funcblock->funcName);

       List *BBlist = &(funcblock->funcBBlist);
       curBBlist = BBlist;
       List_reset(curBBlist);

       BasicBlock *firstbb = (BasicBlock *)List_getHead(curBBlist);
       int n = curBBlist->numItems;
       //assign a number to each bb in accordance with its position in CFG
       number_bbs(firstbb, &n);
       display_BBs_seq();
       
       curAllfuncIntervals = List_new(NULL); //will hold all intervals for the function
       findIntervals();

       display_allfuncIntervals();
   }

}

void display_BBs_seq() {

    
       List_reset(curBBlist);

        printf("\n\nsequence of BBs in Link List\n");

         for (int i = 0; i < curBBlist->numItems; i++) {
            
            BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);
            printf("[%d %s], ",bb->pos, bb->label);

         }

         printf("\n\nsequence of BBs in CFG\n");

         for (int i = 0; i < curBBlist->numItems; i++) {
            
            BasicBlock *bb = (BasicBlock *)get_bb_in_cfg(i+1, curBBlist);
            printf("[%d %s], ",bb->pos, bb->label);

         }
         
    
   
 }

 void print_interval(IntervalBlock *interval) {

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
          
      
    printf("\n");
     

}

void display_allfuncIntervals() {

   ListElement *current = curAllfuncIntervals->current;
   List_reset(curAllfuncIntervals);

   printf("\n\nAll intervals found\n");
   for (int i = 0; i < curAllfuncIntervals->numItems; i++)  {

       IntervalBlock *interval = (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
       print_interval(interval);

    }  
    curAllfuncIntervals->current = current;


}
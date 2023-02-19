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
     // number_bbs(bb->elseBB, n);
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


 bool belongs(int pos, List *IntervalList) {

  ListElement *current = IntervalList->current;
  
  List_reset(IntervalList);
  for (int i=0;i<IntervalList->numItems;i++) {
    BBnode *intervalNode = (BBnode *)List_getNextElement(IntervalList);
    if (intervalNode->bbptr->pos == pos ) {
       IntervalList->current = current;
       return true; 
    }
  }

   IntervalList->current = current; 
   return false;
  
}

bool includesAll(List *IntervalList, List *predecesorsList) {

  ListElement *current = predecesorsList->current;
 
  List_reset(predecesorsList);
  for (int i=0;i<predecesorsList->numItems;i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(predecesorsList);
    if (!belongs(pred->bbptr->pos, IntervalList )) {
       predecesorsList->current = current; 
       return false;
    }
  }

   predecesorsList->current = current; //printf("inlclude\n");
   return true;
  
}


int noOfBBchildren(BasicBlock *bb) {

    return ( bb->thenBB ? 1 : 0) + (bb->elseBB ? 1 : 0);
}

bool canApplyT2(BasicBlock *bb) {

    if ((noOfBBchildren(bb) == 1)) {
         if ( !(bb->thenBB->merged) && (bb != bb->thenBB) && (bb->thenBB->Predecessors.numItems == 1) &&
           (noOfBBchildren(bb->thenBB)<=1) )
           return true;
    }
    return false;
}

// return intervalBlock that bb belongs to
IntervalBlock *get_IntervalBlock(BasicBlock *bb) {

    ListElement *current = curAllfuncIntervals->current;
    List_reset(curAllfuncIntervals);
    for (int i = 0; i < curAllfuncIntervals->numItems; i++)  {

      IntervalBlock *interval = (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
      if (interval->ihead->pos == bb->head->pos) {
        curAllfuncIntervals->current = current;
        return interval;
      }
    }
    printf("interval for bb %d not found\n",bb->pos);
    curAllfuncIntervals->current = current;
    return NULL;
}



void delete_from_Interval(List *interval, BasicBlock *bb) {

    
    ListElement *current = interval->current;
    List_reset(interval);

    for (int i=0; i<interval->numItems; i++ ) {

      BBnode *aBB = (BBnode *)List_getNextElement(interval);
      if (aBB->bbptr->pos == bb->pos) {
        
       //if we are about to delete the current element then adjust the current pointer  
       //so that it points to the previous element
       if (current == (ListElement *)aBB) {
         if (current->prev == NULL) current = NULL;
         else current = current->prev; 
        }
        List_remove(interval, aBB);
        interval->current = current;
        return;
      }
      
    }

   printf("bb %d could not be removed from interval\n", bb->pos);
}

void mergeLinks(BasicBlock *bb, BasicBlock *childbb){


}

void mergebb(BasicBlock *bb) {

    printf ("bb [%s %d] will be merged with [%s %d]\n", bb->label, bb->pos, bb->thenBB->label, bb->thenBB->pos);
    bb->thenBB->merged = true;
    //mergebbInstructions
    //mergeLinks

}


bool ifcond(BasicBlock *bb, List *interval) {

   if (!belongs(bb->thenBB->pos, interval) && !belongs(bb->elseBB->pos,interval))
      return false;

   if (bb->thenBB->thenBB == bb->elseBB)
     { //printf("found cond 1\n"); 
     return true;}
   else if (bb->elseBB->thenBB == bb->thenBB)
     {//printf("found cond 2\n");
     return true; }

   return false;

}

bool ifElsecond(BasicBlock *bb, List *interval) {

   if (!belongs(bb->thenBB->pos, interval) && !belongs(bb->elseBB->pos,interval))
      return false;

   if (bb->thenBB->thenBB == bb->elseBB->thenBB)
     {//printf("found ifelse cond\n");
     return true; }

   return false;

}

void mergeCond(BasicBlock *bb , int cond) {

    printf("will merge conditional bb [%s %d]\n", bb->label, bb->pos);
    while (canApplyT2(bb->thenBB)) mergebb(bb->thenBB); //TODO do I Need this?
    while (canApplyT2(bb->elseBB)) mergebb(bb->elseBB);
    //mergeifElse
    if (cond == 2) {
      bb->thenBB->merged = true;
      bb->elseBB->merged = true;
    } else 
       bb->thenBB->merged = true;
    //mergeif

}

void check_for_conditionals(List *interval) {

  bool found = true;

  ListElement *current = interval->current;

  while (found) { 

    found = false;
    List_reset(interval);
    for (int i=0; i<interval->numItems; i++) {

       BasicBlock *bb = ((BBnode *)List_getPrevElement(interval))->bbptr;
       
       if ((noOfBBchildren(bb) == 2) && !(bb->thenBB->merged)) { 
          
          if (ifcond(bb, interval) ) { printf("if cond at %d\n",bb->pos);
             mergeCond( bb, 1 );
             found = true;
          } else if (ifElsecond(bb, interval)) {  printf("ifelse cond at %d\n",bb->pos);
             mergeCond( bb, 2 );
             found = true;
          }
       }
    }
  }
  interval->current = current; 

}


IntervalBlock *can_be_added_to_interval(List *curAllfuncIntervals, BasicBlock *bb){

    ListElement *current = curAllfuncIntervals->current;
    List_reset(curAllfuncIntervals);

    for (int i=0; i<curAllfuncIntervals->numItems; i++ ) {

      IntervalBlock *iblock = (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
      if  (includesAll(&(iblock->BBsInInterval), &(bb->Predecessors)) ) {
        curAllfuncIntervals->current = current;
        return iblock;
      } 
    }

    curAllfuncIntervals->current = current;
    return NULL;

}


void findIntervals() {

  IntervalBlock *anIntervalBlock;

  printf("\n\ncreating intervals\n");

   for (int i = 1; i <= curBBlist->numItems; i++) {

       BasicBlock *bbinProcess = (BasicBlock *)get_bb_in_cfg(i, curBBlist);
       if (bbinProcess->merged) continue; //ignore any bb that has  been merged with its parent

        printf("processing [%d %s]\n",bbinProcess->pos, bbinProcess->label);
       
       if ((anIntervalBlock = can_be_added_to_interval(curAllfuncIntervals, bbinProcess))) {

         bbinProcess->head = anIntervalBlock->ihead;
         BBnode *aBB = (BBnode *)calloc(1,sizeof(BBnode));
         aBB->bbptr = bbinProcess;
         List_pushElement_back(&(anIntervalBlock->BBsInInterval),aBB);  //printf("added in interval ->%d\n",aBB->bbptr->pos);

       } else {

           IntervalBlock *curInterval = (IntervalBlock *)calloc(1, sizeof(IntervalBlock));
           List_new(&(curInterval->BBsInInterval));
           curInterval->ihead = bbinProcess;
           bbinProcess->head = curInterval->ihead;
           BBnode *aBB = (BBnode *)calloc(1,sizeof(BBnode));
           aBB->bbptr = bbinProcess;
           List_pushElement_back(&(curInterval->BBsInInterval),aBB);  //printf("added in new interval ->%d\n",aBB->bbptr->pos);
           List_pushElement_back(curAllfuncIntervals, curInterval);
       }
      

    }
   

}


void performT2() {

  

  printf("\n\nperfroming T2 transformation\n");

   for (int i=curBBlist->numItems; i>0; i--) {

       BasicBlock *bb = (BasicBlock *)get_bb_in_cfg(i, curBBlist);
       if (bb->merged) continue;

       printf("processing [%d %s]\n",bb->pos, bb->label);
       
       if (canApplyT2(bb)) {

         BasicBlock *thenBB = bb->thenBB;
         IntervalBlock *thenBBintervalb = get_IntervalBlock(thenBB);
         //add thenBB to the interval's latchNodes if it does not exist

         //delete thenBB from its interval
         delete_from_Interval(&(thenBBintervalb->BBsInInterval), thenBB);
         //merge bb with thenBB
         mergebb(bb);
         //delete thenBBinterval if no more bbs in it
         if (List_is_empty(&(thenBBintervalb->BBsInInterval)))
           List_remove(curAllfuncIntervals,thenBBintervalb );
       }


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
       

    //loop until just one interval remains
       curAllfuncIntervals = List_new(NULL); //will hold all intervals for the function

       findIntervals();
  
  
       display_allfuncIntervals();

       //for each interval ??? really?
       performT2();
     
       //for each interval ??? really?
        //check for loops 
        //find nodes for loops
        //set type of each loop
        //check for conditionals and reduce

        //perform T2 ??
        //reduce loops
        for (int i = 0; i<curAllfuncIntervals->numItems; i++) {
            List *interval = &(((IntervalBlock *)List_getNextElement(curAllfuncIntervals))->BBsInInterval);
            check_for_conditionals(interval);
        }
       
       performT2();
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
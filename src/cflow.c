#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"
#include "models/cflow.h"

extern List *stack;
#define push( listElement ) _List_pushElement( stack, (ListElement *)(listElement) )
#define pop()  (BBnode *)List_popElement( stack )
#define top()  (BBnode *)List_getHead( stack )

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

BBnode *newBBnode(BBnode *bbn) {

  BBnode *newBB = (BBnode *)calloc(1, sizeof(BBnode));
  newBB->bbptr = bbn->bbptr;
  return newBB;
}

BBnode *newBBnode2(BasicBlock *bb) {

  BBnode *newBB = (BBnode *)calloc(1, sizeof(BBnode));
  newBB->bbptr = bb;
  return newBB;
}

void set_all_not_visited() {

     List_reset(curBBlist);

      for (int i = 0; i < curBBlist->numItems; i++)  {

        BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);

        bb->visited = false;

      }

}

void remove_predecessor(BasicBlock *bb, BasicBlock *fromBB) {

  List *predecessors = &(fromBB->Predecessors);
  ListElement *current = predecessors->current;

  List_reset(predecessors);
  for (int i=0; i<predecessors->numItems;i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(predecessors);
    if (pred->bbptr->pos == bb->pos) {
       List_remove(predecessors, pred);
       return;
    }
  }
  printf("%d not found in predecessors\n",bb->pos);
}

void add_predecessor(BasicBlock *bb, BasicBlock *toBB) {

  List *predecessors = &(toBB->Predecessors);

  Predecessor *newPred = (Predecessor *)calloc(1, sizeof(Predecessor));
  newPred->bbptr = bb;
  List_pushElement_back(predecessors,newPred);
  
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


bool belongs(int pos, List *aList) {

  ListElement *current = aList->current;
  
  List_reset(aList);
  for (int i=0;i<aList->numItems;i++) {
    BBnode *intervalNode = (BBnode *)List_getNextElement(aList);
    if (intervalNode->bbptr->pos == pos ) {
       aList->current = current;
       return true; 
    }
  }

   aList->current = current; 
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

List *get_intersection(IntervalBlock *intervalblock) {

  // get the intersection set between interval nodes and predecessors of interval's head
  List *ihead_predecessors = &(intervalblock->ihead->Predecessors);
  List *intervalnodes = &(intervalblock->BBsInInterval);

  ListElement *current = ihead_predecessors->current;
 
  List_reset(ihead_predecessors);
  List_new(&(intervalblock->latchNodes));

  for (int i=0;i<ihead_predecessors->numItems;i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(ihead_predecessors);
    if (belongs(pred->bbptr->pos, intervalnodes )) {
     
       List_pushElement_back(&(intervalblock->latchNodes),newBBnode((BBnode *)pred));
       
    }
  }

   ihead_predecessors->current = current; //printf("inlclude\n");
   return &(intervalblock->latchNodes);


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

void  adjust_pred(BasicBlock *bb, BasicBlock *achild, BasicBlock *otherChild, int cond){

  List *predecesorsOfchild = &(achild->Predecessors);
 
  List *predecessorsParent = &(bb->Predecessors);
 
 

  ListElement *current = predecesorsOfchild->current;
 
  List_reset(predecesorsOfchild);

  for (int i=0;i<predecesorsOfchild->numItems;i++) {

    Predecessor *pred = (Predecessor *)List_getNextElement(predecesorsOfchild);

    if ((pred->bbptr->pos != bb->pos) && 
         (( (cond !=3 ) && (pred->bbptr->pos != otherChild->pos) ) || (cond == 3))
         ) {
      printf("adjusting pred ->%d of achild ->%d\n", pred->bbptr->pos, achild->pos);
      if (pred->bbptr->thenBB == achild) pred->bbptr->thenBB = bb;
      else if (pred->bbptr->elseBB == achild) pred->bbptr->elseBB = bb;
      else printf("could not adjust pred\n");
      if (!belongs(pred->bbptr->pos, predecessorsParent))
         add_predecessor(pred->bbptr, bb);
    }
    
    }
  

   predecesorsOfchild->current = current; //printf("inlclude\n");
   

}

void mergeT2(BasicBlock *bb, BasicBlock *thenchild) {

    
      printf ("bb [%s %d] will be merged with [%s %d]\n", bb->label, bb->pos, thenchild->label, thenchild->pos);
   
  
   
   if (thenchild->thenBB) {
      printf("merge case 4\n");
      thenchild->merged = true;
      bb->thenBB = thenchild->thenBB;
      remove_predecessor(thenchild,bb->thenBB);
      add_predecessor(bb,thenchild->thenBB);

    }  else {
        printf("merge case 5\n");
        thenchild->merged = true;
       bb->thenBB = 0;
    }

  //  if (bb->thenBB == bb->elseBB) 
   //  bb->elseBB = 0;
    
    if ((bb->elseBB) && (!bb->thenBB))
         {bb->thenBB = bb->elseBB ; bb->elseBB = 0;}
    //add  bb to predecessors of grandchildrens 
    // remove child from predecessor of grandchildren
    //mergebbInstructions
    //mergeLinks
    if (bb->thenBB == bb) { printf("pointing to itself\n");
      bb->thenBB = bb->elseBB;
      //erase bb from predecsessors of bb
      remove_predecessor(bb,bb);
    }
}


int ifcond(BasicBlock *bb, List *interval) {

   if (!belongs(bb->thenBB->pos, interval) && !belongs(bb->elseBB->pos,interval))
      return 0;

   if (bb->elseBB == bb->thenBB)
     return 3;
   else if (bb->thenBB->thenBB == bb->elseBB)
     return 1;
   else if (bb->elseBB->thenBB == bb->thenBB)
     return 2; 

   return 0;

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

   // printf("will merge conditional bb [%s %d]\n", bb->label, bb->pos);
   // while (canApplyT2(bb->thenBB)) mergebb(bb->thenBB); //TODO do I Need this?
  //  while (canApplyT2(bb->elseBB)) mergebb(bb->elseBB);
    BasicBlock *thenchild = bb->thenBB;
    BasicBlock *elsechild = bb->elseBB;

     if (cond == 1) {
     // mergebb(bb, bb->thenBB, bb->elseBB);// bb->thenBB->merged = true;
     // mergebb(bb, bb->elseBB); //->elseBB->merged = true;
      //mergeifElse -> blockmerge(bb,bb->thenbb,bb->elsebb)

      //  merge if (leftop op right op) with {then instructions}
      // merge with else instructions
        printf("mergec case 1 ifcond\n");
        thenchild->merged = true;
        elsechild->merged = true;

        bb->thenBB = bb->elseBB;
        bb->elseBB = 0;
        if (elsechild->thenBB) {
          bb->thenBB = thenchild->thenBB;
          remove_predecessor(elsechild,elsechild->thenBB);
          add_predecessor(bb,elsechild->thenBB);
        }
        else {
        
        bb->thenBB = 0;
        
        }
     } else if (cond == 2) {
     // mergebb(bb, bb->thenBB, bb->elseBB);// bb->thenBB->merged = true;
     // mergebb(bb, bb->elseBB); //->elseBB->merged = true;
      //mergeifElse -> blockmerge(bb,bb->thenbb,bb->elsebb)

      // reverse operator and merge with else instructions -> if (leftop !op rightop) {else intructions}
      // merge with then instructions
        printf("mergec case 2 ifcond\n");
        thenchild->merged = true;
        elsechild->merged = true;

        bb->elseBB =0;
        if (thenchild->thenBB) {
          bb->thenBB = thenchild->thenBB;
          remove_predecessor(thenchild,thenchild->thenBB);
          add_predecessor(bb,thenchild->thenBB);
        }
        else {
        
        bb->thenBB = 0;
        
        }
    } else if (cond == 3) { //bb then and else points to same child
        printf("mergec case 3 ifcond\n");
        thenchild->merged = true;
        elsechild->merged = true;
        //  merge if (leftop op right op) with {then instructions}
        bb->elseBB =0;
        if (thenchild->thenBB) {
          bb->thenBB = thenchild->thenBB;
          remove_predecessor(thenchild,thenchild->thenBB);
          add_predecessor(bb,thenchild->thenBB);
        }
        else {
        
        bb->thenBB = 0;
        
        }

    } else {

      printf("mergec case 4 ifelse\n");
      //merge if (leftop op rightop)  {then instructions}
      //merge else {else intructions}
      thenchild->merged = true;
      elsechild->merged = true;
      
       bb->thenBB = thenchild->thenBB;
       bb->elseBB = 0;
       // else of children?
       remove_predecessor(thenchild,thenchild->thenBB);
       remove_predecessor(elsechild, thenchild->thenBB);
       add_predecessor(bb, thenchild->thenBB);
       
    }
    
    //if a predx of a child is not bb or the other child make predx point to bb
    //and add predx to bb's preds if not exists
    if (cond !=3) {
     adjust_pred(bb, thenchild, elsechild, cond);
     adjust_pred(bb, elsechild, thenchild, cond);
    }
    else 
     adjust_pred(bb,thenchild,0, cond);

     if ((bb->elseBB) && (!bb->thenBB))
         {bb->thenBB = bb->elseBB ; bb->elseBB = 0;}
    

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
          int type;
          if ((type = ifcond(bb, interval)) ) { 
             mergeCond( bb, type );
           //  found = true;
          } else if (ifElsecond(bb, interval)) {  
             mergeCond( bb, 4 );
          //   found = true;
          }
       }
    }
  }
  interval->current = current; 

}

void merge_loop(IntervalBlock *intervalblock) {

  //erase loophead's NEXT from the interval
  delete_from_Interval(&(intervalblock->BBsInInterval), intervalblock->loophead->thenBB);
 

  BasicBlock *bb = intervalblock->loophead;
  BasicBlock *thenchild = intervalblock->loophead->thenBB;

  printf("will merge loop bb [%s %d]\n", bb->label, bb->pos);

  if (thenchild->thenBB == bb) {
    thenchild->merged = true;
    bb->thenBB = 0;

    remove_predecessor(thenchild,bb);
    if (thenchild->elseBB) {
      bb->thenBB = thenchild->elseBB;
      remove_predecessor(thenchild,thenchild->elseBB);
      add_predecessor(bb,thenchild->elseBB);
    }


    } else 
      printf("this is not a loop !!!\n");

    adjust_pred(bb, thenchild, 0, 3);

   if ((bb->elseBB) && (!bb->thenBB))
         {bb->thenBB = bb->elseBB ; bb->elseBB = 0;}

}

bool check_for_loops(IntervalBlock *intervalb) {

  // a loop exists in the interval if the intersection set between the interval nodes and
  // the predecessors of the interval's head is not empty
  // this intersection set are the latchnodes of the loop

  List *latchnodes = get_intersection(intervalb);
  
  if (List_is_empty(latchnodes)) return false;

  //a loop exists. find its nodes
  intervalb->loophead = intervalb->ihead;
  List *loopnodes = List_new(&(intervalb->loopNodes));
  
  for (int i=0; i<latchnodes->numItems; i++) { 

    List_destroy(stack);
    BBnode *latchnode = (BBnode *)List_getNextElement(latchnodes);
    if (!belongs(latchnode->bbptr->pos,loopnodes)) { 
      List_pushElement_back(loopnodes,newBBnode(latchnode)); //printf("added %d\n",latchnodet->bbptr->pos);
      push(newBBnode(latchnode));
    }
    
    while (!List_is_empty(stack)) { 

       BasicBlock *bb = ((BBnode *)top())->bbptr;
       pop();
       List *predecessors = &(bb->Predecessors); 
       List_reset(predecessors);
       for (int j=0; j<predecessors->numItems; j++) { 
          BBnode *pred = (BBnode *)List_getNextElement(predecessors);  //BBnode and Predecessor are of the same type
          
          if (!belongs(pred->bbptr->pos,loopnodes)) {         
            List_pushElement_back(loopnodes,newBBnode(pred)); //printf("added %d\n",loopnodet->bbptr->pos);
            push(newBBnode(pred));  
          }             
        }

    } 

  } 

  printf("loop found in interval with loop head ->[%s %d]\n",intervalb->ihead->label, intervalb->loophead->pos);
  printf("loop nodes found ->%d\n", loopnodes->numItems);
  return true;
    

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

        //printf("processing [%d %s]\n",bbinProcess->pos, bbinProcess->label);
     
       if ((anIntervalBlock = can_be_added_to_interval(curAllfuncIntervals, bbinProcess))) {

         bbinProcess->head = anIntervalBlock->ihead;
         List_pushElement_back(&(anIntervalBlock->BBsInInterval),newBBnode2(bbinProcess));  //printf("added in interval ->%d\n",aBB->bbptr->pos);

       } else {

           IntervalBlock *curInterval = (IntervalBlock *)calloc(1, sizeof(IntervalBlock));
           List_new(&(curInterval->BBsInInterval));
           curInterval->ihead = bbinProcess;
           bbinProcess->head = curInterval->ihead;
           
           List_pushElement_back(&(curInterval->BBsInInterval),newBBnode2(bbinProcess));  //printf("added in new interval ->%d\n",aBB->bbptr->pos);
           List_pushElement_back(curAllfuncIntervals, curInterval);
       }
      

    }
   

}


void performT2() {

  

  printf("\n\nperfroming T2 transformation\n");

   for (int i=curBBlist->numItems; i>0; i--) {

       BasicBlock *bb = (BasicBlock *)get_bb_in_cfg(i, curBBlist);
       if (bb->merged) continue;

       printf("T2 processing [%d %s]\n",bb->pos, bb->label);
 //   printf("       next->%d else->%d\n",(bb->thenBB ? bb->thenBB->pos : 0),(bb->elseBB ? bb->elseBB->pos : 0));   
       
       if (canApplyT2(bb)) {

         BasicBlock *thenBB = bb->thenBB;
         IntervalBlock *thenBBintervalb = get_IntervalBlock(thenBB);
         //add thenBB to the interval's latchNodes if it does not exist

         //delete thenBB from its interval
         delete_from_Interval(&(thenBBintervalb->BBsInInterval), thenBB);
         //merge bb with thenBB
         mergeT2(bb,bb->thenBB);
         //delete thenBBinterval if no more bbs in it
         if (List_is_empty(&(thenBBintervalb->BBsInInterval)))
           List_remove(curAllfuncIntervals,thenBBintervalb );
       }


    }

}


void control_flow(List *funcBlocks) {

 
   
   List_reset(funcBlocks);     //point to the first funcblock
   stack = List_new(NULL);  //create the list for stack emulation
   curAllfuncIntervals = List_new(NULL);

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
       List_destroy(curAllfuncIntervals); //empty the list of all elements

       findIntervals();
  
  
       display_allfuncIntervals();

       //for each interval ??? really?
       performT2();
       
        printf("\n processing intervals\n");
        for (int i = 0; i<curAllfuncIntervals->numItems; i++) {
            IntervalBlock *intervalBlock = (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
            List *intervalnodes = &(intervalBlock->BBsInInterval);
            bool loopfound = check_for_loops(intervalBlock);

            check_for_conditionals(intervalnodes);

            if (loopfound) merge_loop(intervalBlock);
        }
       
      performT2();
      List_destroy(curAllfuncIntervals); //empty the list of all elements

       findIntervals();

        display_allfuncIntervals();
//add here
      printf("\n processing intervals\n");

         for (int i = 0; i<curAllfuncIntervals->numItems; i++) {
            IntervalBlock *intervalBlock = (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
            List *intervalnodes = &(intervalBlock->BBsInInterval);
            bool loopfound = check_for_loops(intervalBlock);
 
            check_for_conditionals(intervalnodes);

            if (loopfound) merge_loop(intervalBlock);
        }
           performT2();
           
       List_destroy(curAllfuncIntervals); //empty the list of all elements
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
          printf("[%d %s] ",bb->bbptr->pos, bb->bbptr->label);

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
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utils/ll.h"
#include "models/assembly.h"
#include "models/cflow.h"
#include "models/dflow.h"



// UNIT_TEST is set to true for testing intervals, loops and conditionals
// compile with testcf.c instead of main.c
// no instructions are merged
extern bool UNIT_TEST;
extern bool VERBOSE_CF;

extern List *stack;
#define push(listElement) _List_pushElement(stack, (ListElement *)(listElement))
#define pop() (BBnode *)List_popElement(stack)
#define top() (BBnode *)List_getHead(stack)

extern char *get_Coper(char *insmnem);     // get it from the data flow module
extern char *getFuncDefinition(char *fun); // get it from the data flow module
extern char *getFuncReturn(char *fun);     // get it from the data flow module

void display_BBs_seq();
void print_interval(IntervalBlock *interval);
void display_allfuncIntervals();
void print_func_ClikeIns();
void save_func_ClikeIns();
void print_loop_nodes(List *loop_nodes);

List *curBBlist;
List *curAllfuncIntervals;

char *reverseOp(char *op) {
  if (!strcmp(op, ">"))
    return "<=";
  if (!strcmp(op, "<="))
    return ">";

  if (!strcmp(op, "=="))
    return "!=";
  if (!strcmp(op, "!="))
    return "==";

  if (!strcmp(op, ">="))
    return "<";
  if (!strcmp(op, "<"))
    return ">=";

  vbprintf(VERBOSE_CF, "could not reverse op ->%s", op);
  return ("??");
}

void number_bbs(BasicBlock *bb, int *n) {

  if ((bb) && (!bb->visited)) {
    bb->visited = true;
    number_bbs(bb->elseBB, n);
    number_bbs(bb->thenBB, n);
    // number_bbs(bb->elseBB, n);
    bb->pos = *n;
    *n = *n - 1;
  }
}

// create a new BBnode List element which has a pointer to a BB of an existing
// BBnode
BBnode *newBBnode(BBnode *bbn) {

  BBnode *newBB = (BBnode *)calloc(1, sizeof(BBnode));
  newBB->bbptr = bbn->bbptr;
  return newBB;
}

// create a new BBnode List element which has a pointer to an existing BB
BBnode *newBBnode2(BasicBlock *bb) {

  BBnode *newBB = (BBnode *)calloc(1, sizeof(BBnode));
  newBB->bbptr = bb;
  return newBB;
}

// create a new ClikeIns List element which has the string of an existing
// ClikeIns
ClikeIns *newClikeIns(ClikeIns *ins) {

  ClikeIns *newClikeIns = (ClikeIns *)calloc(1, sizeof(ClikeIns));
  newClikeIns->cins = strdup(ins->cins);
  return newClikeIns;
}

// create a new ClikeIns List element which has a specified string
ClikeIns *newClikeIns2(char *ins) {

  ClikeIns *newClikeIns = (ClikeIns *)calloc(1, sizeof(ClikeIns));
  newClikeIns->cins = strdup(ins);
  return newClikeIns;
}

void set_all_not_visited() {

  List_reset(curBBlist);

  for (int i = 0; i < curBBlist->numItems; i++) {

    BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);

    bb->visited = false;
  }
}

// get number of nodes in first interval
int num_of_nodes_in_interval() {

  IntervalBlock *intervalBlock =
      (IntervalBlock *)List_getHead(curAllfuncIntervals);
  List *intervalnodes = &(intervalBlock->BBsInInterval);

  return intervalnodes->numItems;
}

// get bb in limit interval. this should be the first and only interval in the
// intervals list
BasicBlock *get_bb_in_limit_interval() {

  IntervalBlock *intervalBlock =
      (IntervalBlock *)List_getHead(curAllfuncIntervals);
  List *intervalnodes = &(intervalBlock->BBsInInterval);

  BBnode *bbnode = (BBnode *)List_getHead(intervalnodes);
  return (bbnode->bbptr);
}

// remove a BB from a the predecessors list of another BB
void remove_predecessor(BasicBlock *bb, BasicBlock *fromBB) {

  List *predecessors = &(fromBB->Predecessors);
  ListElement *current = predecessors->current;

  List_reset(predecessors);
  for (int i = 0; i < predecessors->numItems; i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(predecessors);
    if (pred->bbptr->pos == bb->pos) {
      List_remove(predecessors, pred);
      return;
    }
  }
  vbprintf(VERBOSE_CF, "%d not found in predecessors\n", bb->pos);
}

// add a BB to the predecessors list of another BB
void add_predecessor(BasicBlock *bb, BasicBlock *toBB) {

  List *predecessors = &(toBB->Predecessors);

  Predecessor *newPred = (Predecessor *)calloc(1, sizeof(Predecessor));
  newPred->bbptr = bb;
  List_pushElement_back(predecessors, newPred);
}

BasicBlock *get_bb_in_cfg(int pos, List *BBlist) {

  ListElement *current = BBlist->current;
  List_reset(BBlist); // point to the first block

  for (int i = 0; i < BBlist->numItems; i++) {

    BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);

    if (bb->pos == pos) {
      BBlist->current = current;
      return bb;
    }
  }
  BBlist->current = current;
  vbprintf(VERBOSE_CF, "  bb with pos %d not found in bblist\n", pos);
  return NULL;
}

// return true if a return statement has already been added in C like
// instructions
bool returnAdded(List *clikeInsList) {

  ClikeIns *LastIns = (ClikeIns *)List_getTail(clikeInsList);
  if (!strncmp(LastIns->cins, "return", 6))
    return true;
  return false;
}

// apend the C like instructions in the thenChild bb to the
// C like instructions of its parent bb

void mergeT2instructions(BasicBlock *bb, BasicBlock *child) {

  List *parentInstructions = &(bb->ClikeInsL);
  List *childInstructions = &(child->ClikeInsL);

  List_reset(childInstructions);

  for (int i = 0; i < childInstructions->numItems; i++) {

    ClikeIns *childIns = (ClikeIns *)List_getNextElement(childInstructions);
    List_pushElement_back(parentInstructions, newClikeIns(childIns));
  }
}

void mergeCondInstructions(BasicBlock *bb, int condType) {

  char tempIns[100];
  memset(tempIns, 0, 100);

  if (UNIT_TEST)
    return;

  List *parentInstructions = &(bb->ClikeInsL);

  if (condType == IF_1) {
    // merge if (leftop op rightop) with {then-instructions}
    // merge with else-instructions
    strcpy(tempIns, "\n\nif (");
    strcat(tempIns, bb->leftOp);
    strcat(tempIns, get_Coper(bb->compOperator));
    strcat(tempIns, bb->rightOp);
    strcat(tempIns, ") {");
    List_pushElement_back(parentInstructions, newClikeIns2(tempIns));

    mergeT2instructions(bb, bb->thenBB);
    List_pushElement_back(parentInstructions, newClikeIns2("}\n\n"));
    mergeT2instructions(bb, bb->elseBB);
  } else if (condType == IF_2) {
    // reverse operator and merge with else instructions -> if (leftop !op
    // rightop) {else-instructions} then, merge with then-instructions
    strcpy(tempIns, "\n\nif (");
    strcat(tempIns, bb->leftOp);
    strcat(tempIns, reverseOp(get_Coper(bb->compOperator)));
    strcat(tempIns, bb->rightOp);
    strcat(tempIns, ") {");
    List_pushElement_back(parentInstructions, newClikeIns2(tempIns));

    mergeT2instructions(bb, bb->elseBB);
    List_pushElement_back(parentInstructions, newClikeIns2("}\n\n"));
    mergeT2instructions(bb, bb->thenBB);
  } else if (condType == IF_3) {
    // bb's then and else point to same child
    // this can occur when we have nested ifs and after the inner if is merged
    // merge if (leftop op rightop) with {then-instructions}
    strcpy(tempIns, "\n\nif (");
    strcat(tempIns, bb->leftOp);
    strcat(tempIns, reverseOp(get_Coper(bb->compOperator)));
    strcat(tempIns, bb->rightOp);
    strcat(tempIns, ") {");
    List_pushElement_back(parentInstructions, newClikeIns2(tempIns));

    mergeT2instructions(bb, bb->thenBB);
    List_pushElement_back(parentInstructions, newClikeIns2("}\n\n"));
  } else if (condType == IFELSE) {
    // merge if (leftop !op rightop)  {else-instructions}
    // merge else {then-instructions}
    strcpy(tempIns, "\n\nif (");
    strcat(tempIns, bb->leftOp);
    strcat(tempIns, reverseOp(get_Coper(bb->compOperator)));
    strcat(tempIns, bb->rightOp);
    strcat(tempIns, ") {");
    List_pushElement_back(parentInstructions, newClikeIns2(tempIns));

    mergeT2instructions(bb, bb->elseBB);
    List_pushElement_back(parentInstructions, newClikeIns2("}\nelse {"));
    mergeT2instructions(bb, bb->thenBB);
    List_pushElement_back(parentInstructions, newClikeIns2("}\n\n"));
  }
}

void mergeLoopInstructions(IntervalBlock *intervalb) {

  BasicBlock *loophead = intervalb->loophead;
  List *loopheadIns = &(loophead->ClikeInsL);
  char tempIns[100];
  memset(tempIns, 0, 100);

  if (UNIT_TEST)
    return;

  if (intervalb->looptype == PRE) {

    if (!List_is_empty(loopheadIns)) {
  
    }

    List_pushElement_back(loopheadIns, newClikeIns2("\n\nwhile("));
    strcpy(tempIns, loophead->leftOp);
    strcat(tempIns, get_Coper(loophead->compOperator));
    strcat(tempIns, loophead->rightOp);
    strcat(tempIns, ") {");
    List_pushElement_back(loopheadIns, newClikeIns2(tempIns));

    mergeT2instructions(loophead, loophead->thenBB);
    List_pushElement_back(loopheadIns, newClikeIns2("}\n\n"));
  } else if (intervalb->looptype == POST) {

    List_pushElement(loopheadIns, newClikeIns2("\n\ndo {"));
    if (loophead != loophead->thenBB)
      mergeT2instructions(loophead, loophead->thenBB);
    else {
      remove_predecessor(
          loophead->thenBB,
          loophead); 
      loophead->thenBB = 0;
    }

    List_pushElement_back(loopheadIns, newClikeIns2("}while("));
    strcpy(tempIns, loophead->leftOp);
    strcat(tempIns, get_Coper(loophead->compOperator));
    strcat(tempIns, loophead->rightOp);
    strcat(tempIns, ");\n\n");
    List_pushElement_back(loopheadIns, newClikeIns2(tempIns));
  } else if (intervalb->looptype == ENDLESS) {

    if (loophead != loophead->thenBB) {
      List_pushElement_back(loopheadIns, newClikeIns2("\n\nwhile(1){"));
      mergeT2instructions(loophead, loophead->thenBB);
    } else {
      List_pushElement(loopheadIns, newClikeIns2("while(1){"));
      remove_predecessor(
          loophead->thenBB,
          loophead); 
      loophead->thenBB = 0;
    }

    List_pushElement_back(loopheadIns, newClikeIns2("}\n\n"));
  }
}

// true if a BB with a specified pos belongs to a list of BBnode elements

bool belongs(int pos, List *aList) {

  ListElement *current = aList->current;

  List_reset(aList);
  for (int i = 0; i < aList->numItems; i++) {
    BBnode *bbNode = (BBnode *)List_getNextElement(aList);
    if (bbNode->bbptr->pos == pos) {
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
  for (int i = 0; i < predecesorsList->numItems; i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(predecesorsList);
    if (!belongs(pred->bbptr->pos, IntervalList)) {
      predecesorsList->current = current;
      return false;
    }
  }

  predecesorsList->current = current;
  return true;
}

List *get_intersection(IntervalBlock *intervalblock) {

  // get the intersection set between interval nodes and predecessors of
  // interval's head
  List *ihead_predecessors = &(intervalblock->ihead->Predecessors);
  List *intervalnodes = &(intervalblock->BBsInInterval);

  ListElement *current = ihead_predecessors->current;

  List_reset(ihead_predecessors);
  List_new(&(intervalblock->latchNodes));

  for (int i = 0; i < ihead_predecessors->numItems; i++) {
    Predecessor *pred = (Predecessor *)List_getNextElement(ihead_predecessors);
    if (belongs(pred->bbptr->pos, intervalnodes)) {

      List_pushElement_back(&(intervalblock->latchNodes),
                            newBBnode((BBnode *)pred));
    }
  }

  ihead_predecessors->current = current;
  return &(intervalblock->latchNodes);
}

int noOfBBchildren(BasicBlock *bb) {

  return (bb->thenBB ? 1 : 0) + (bb->elseBB ? 1 : 0);
}

bool canApplyT2(BasicBlock *bb) {

  if ((noOfBBchildren(bb) == 1)) {
    if (!(bb->thenBB->merged) && (bb != bb->thenBB) &&
        (bb->thenBB->Predecessors.numItems == 1) &&
        (noOfBBchildren(bb->thenBB) <= 1) && (bb != bb->thenBB->thenBB))
      return true;
  }
  return false;
}

// return intervalBlock that bb belongs to
IntervalBlock *get_IntervalBlock(BasicBlock *bb) {

  ListElement *current = curAllfuncIntervals->current;
  List_reset(curAllfuncIntervals);
  for (int i = 0; i < curAllfuncIntervals->numItems; i++) {

    IntervalBlock *interval =
        (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
    if (interval->ihead->pos == bb->head->pos) {
      curAllfuncIntervals->current = current;
      return interval;
    }
  }
  vbprintf(VERBOSE_CF, "interval for bb %d not found\n", bb->pos);
  curAllfuncIntervals->current = current;
  return NULL;
}

void delete_from_Interval(List *interval, BasicBlock *bb) {

  ListElement *current = interval->current;
  List_reset(interval);

  for (int i = 0; i < interval->numItems; i++) {

    BBnode *aBB = (BBnode *)List_getNextElement(interval);
    if (aBB->bbptr->pos == bb->pos) {

      // if we are about to delete the current element then adjust the current
      // pointer so that it points to the previous element
      if (current == (ListElement *)aBB) {
        if (current->prev == NULL)
          current = NULL;
        else
          current = current->prev;
      }
      List_remove(interval, aBB);
      interval->current = current;
      return;
    }
  }

  vbprintf(VERBOSE_CF, "bb %d could not be removed from interval\n", bb->pos);
}

void adjust_pred(BasicBlock *bb, BasicBlock *achild, BasicBlock *otherChild,
                 int cond) {

  List *predecesorsOfchild = &(achild->Predecessors);

  List *predecessorsParent = &(bb->Predecessors);

  ListElement *current = predecesorsOfchild->current;

  List_reset(predecesorsOfchild);

  for (int i = 0; i < predecesorsOfchild->numItems; i++) {

    Predecessor *pred = (Predecessor *)List_getNextElement(predecesorsOfchild);

    if ((pred->bbptr->pos != bb->pos) &&
        (((cond != IF_3) && (pred->bbptr->pos != otherChild->pos)) ||
         (cond == IF_3))) {
      vbprintf(VERBOSE_CF, "adjusting predecessor ->%d of child ->%d\n",
               pred->bbptr->pos, achild->pos);
      if (pred->bbptr->thenBB == achild)
        pred->bbptr->thenBB = bb;
      else if (pred->bbptr->elseBB == achild)
        pred->bbptr->elseBB = bb;
      else
        vbprintf(VERBOSE_CF, "could not adjust pred\n");
      if (!belongs(pred->bbptr->pos, predecessorsParent))
        add_predecessor(pred->bbptr, bb);
    }
  }

  predecesorsOfchild->current = current;
}

void mergeT2(BasicBlock *bb, BasicBlock *thenchild) {

  vbprintf(VERBOSE_CF, "bb [%s %d] will be merged with [%s %d]\n", bb->label,
           bb->pos, thenchild->label, thenchild->pos);

  thenchild->merged = true;

  if (thenchild->thenBB) {
    vbprintf(VERBOSE_CF, "merge case 4\n");
    bb->thenBB = thenchild->thenBB;
    remove_predecessor(thenchild, bb->thenBB);
    add_predecessor(bb, thenchild->thenBB);
  } else {
    vbprintf(VERBOSE_CF, "merge case 5\n");
    bb->thenBB = 0;
  }

  if ((bb->elseBB) && (!bb->thenBB)) {
    bb->thenBB = bb->elseBB;
    bb->elseBB = 0;
  }

  if (bb->thenBB == bb) {
    vbprintf(VERBOSE_CF, "pointing to itself\n");
    bb->thenBB = bb->elseBB;
    // erase bb from predecsessors of bb
    remove_predecessor(bb, bb);
  }

  mergeT2instructions(bb, thenchild);
}

int ifcond(BasicBlock *bb, List *interval) {

  // TODO first check if interval also has a loop
  if (!belongs(bb->thenBB->pos, interval) &&
      !belongs(bb->elseBB->pos, interval))
    return 0;

  if (bb->elseBB == bb->thenBB)
    return IF_3;
  else if (bb->thenBB->thenBB == bb->elseBB)
    return IF_1;
  else if (bb->elseBB->thenBB == bb->thenBB)
    return IF_2;

  return 0;
}

bool ifElsecond(BasicBlock *bb, List *interval) {

  // TODO first check if interval also has a loop
  if (!belongs(bb->thenBB->pos, interval) &&
      !belongs(bb->elseBB->pos, interval))
    return false;

  if (bb->thenBB->thenBB == bb->elseBB->thenBB) {
    return true;
  }

  return false;
}

void mergeCond(BasicBlock *bb, int condType) {

  vbprintf(VERBOSE_CF, "will merge conditional at bb -> [%s %d]\n", bb->label,
           bb->pos);

  BasicBlock *thenchild = bb->thenBB;
  BasicBlock *elsechild = bb->elseBB;

  if (condType == IF_1) {

    //  merge if (leftop op right op) with {then-instructions}
    // merge with else-instructions
    vbprintf(VERBOSE_CF, "merge case 1 ifcond\n");
    mergeCondInstructions(bb, condType);

    // merge links
    thenchild->merged = true;
    elsechild->merged = true;

    bb->thenBB = bb->elseBB;
    bb->elseBB = 0;
    if (elsechild->thenBB) {
      bb->thenBB = thenchild->thenBB;
      remove_predecessor(elsechild, elsechild->thenBB);
      add_predecessor(bb, elsechild->thenBB);
    } else {

      bb->thenBB = 0;
    }
  } else if (condType == IF_2) {

    // reverse operator and merge with else instructions -> if (leftop !op
    // rightop) {else intructions} merge with then instructions
    vbprintf(VERBOSE_CF, "merge case 2 ifcond\n");
    mergeCondInstructions(bb, condType);

    // merge links
    thenchild->merged = true;
    elsechild->merged = true;

    bb->elseBB = 0;
    if (thenchild->thenBB) {
      bb->thenBB = thenchild->thenBB;
      remove_predecessor(thenchild, thenchild->thenBB);
      add_predecessor(bb, thenchild->thenBB);
    } else {

      bb->thenBB = 0;
    }
  } else if (condType == IF_3) {
    // bb's then and else point to same child
    // this can occur when we have nested ifs and after the inner if is merged
    // merge if (leftop op rightop) with {then-instructions}
    vbprintf(VERBOSE_CF, "merge case 3 ifcond\n");
    mergeCondInstructions(bb, condType);

    // merge links
    thenchild->merged = true;
    elsechild->merged = true;

    bb->elseBB = 0;
    if (thenchild->thenBB) {
      bb->thenBB = thenchild->thenBB;
      remove_predecessor(thenchild, thenchild->thenBB);
      add_predecessor(bb, thenchild->thenBB);
    } else {

      bb->thenBB = 0;
    }
  } else {

    vbprintf(VERBOSE_CF, "merge case 4 ifelse\n");
    // merge if (leftop !op rightop)  {else instructions}
    // merge else {then intructions}
    mergeCondInstructions(bb, condType);

    thenchild->merged = true;
    elsechild->merged = true;

    bb->thenBB = thenchild->thenBB;
    bb->elseBB = 0;

    remove_predecessor(thenchild, thenchild->thenBB);
    remove_predecessor(elsechild, thenchild->thenBB);
    add_predecessor(bb, thenchild->thenBB);
  }

  // if a predx of a child is not bb or the other child make predx point to bb
  // and add predx to bb's preds if not exists
  if (condType != 3) {
    adjust_pred(bb, thenchild, elsechild, condType);
    adjust_pred(bb, elsechild, thenchild, condType);
  } else
    adjust_pred(bb, thenchild, 0, condType);

  if ((bb->elseBB) && (!bb->thenBB)) {
    bb->thenBB = bb->elseBB;
    bb->elseBB = 0;
  }
}

void check_for_conditionals(List *interval) {

  bool found = true;

  ListElement *current = interval->current;

  while (found) {

    found = false;
    List_reset(interval);
    for (int i = 0; i < interval->numItems; i++) {

      // start from bottom of the interval
      BasicBlock *bb = ((BBnode *)List_getPrevElement(interval))->bbptr;

      if ((noOfBBchildren(bb) == 2) && !(bb->thenBB->merged)) {
        int type;
        if ((type = ifcond(bb, interval))) {
          mergeCond(bb, type);
          //  found = true;
        } else if (ifElsecond(bb, interval)) {
          mergeCond(bb, IFELSE);
          //   found = true;
        }
      }
    }
  }
  interval->current = current;
}

void merge_loop(IntervalBlock *intervalblock) {

  BasicBlock *bb = intervalblock->loophead;
  BasicBlock *thenchild = intervalblock->loophead->thenBB;

  vbprintf(VERBOSE_CF, "will merge loop at bb [%s %d]\n", bb->label, bb->pos);

  // erase loophead's NEXT from the interval
  delete_from_Interval(&(intervalblock->BBsInInterval), thenchild);

  // merge the loop instructions before merging the loop nodes links
  mergeLoopInstructions(intervalblock);

  // merge loop nodes links

  if (thenchild->thenBB == bb) {
    thenchild->merged = true;
    bb->thenBB = 0;

    remove_predecessor(thenchild, bb);
    if (thenchild->elseBB) {
      bb->thenBB = thenchild->elseBB;
      remove_predecessor(thenchild, thenchild->elseBB);
      add_predecessor(bb, thenchild->elseBB);
    }
  } else
    vbprintf(VERBOSE_CF, "this is not a loop !!!\n");

  adjust_pred(bb, thenchild, 0, 3);

  if ((bb->elseBB) && (!bb->thenBB)) {
    bb->thenBB = bb->elseBB;
    bb->elseBB = 0;
  }
}

bool check_for_loops(IntervalBlock *intervalb) {

  // a loop exists in the interval if the intersection set between the interval
  // nodes and the predecessors of the interval's head is not empty this
  // intersection set are the latchnodes of the loop

  List *latchnodes = get_intersection(intervalb);

  if (List_is_empty(latchnodes))
    return false;

  // a loop exists. find its nodes
  intervalb->loophead = intervalb->ihead;
  List *loopnodes = List_new(&(intervalb->loopNodes));

  List_pushElement_back(loopnodes, newBBnode2(intervalb->loophead));

  for (int i = 0; i < latchnodes->numItems; i++) {

    List_destroy(stack);
    BBnode *latchnode = (BBnode *)List_getNextElement(latchnodes);
    if (!belongs(latchnode->bbptr->pos, loopnodes)) {
      List_pushElement_back(loopnodes, newBBnode(latchnode));
      push(newBBnode(latchnode));
    }

    while (!List_is_empty(stack)) {

      BasicBlock *bb = ((BBnode *)top())->bbptr;
      pop();

      List *predecessors = &(bb->Predecessors);
      List_reset(predecessors);

      for (int j = 0; j < predecessors->numItems; j++) {

        BBnode *pred = (BBnode *)List_getNextElement(
            predecessors); // BBnode and Predecessor are of the same struct type

        if (!belongs(pred->bbptr->pos, loopnodes)) {
          List_pushElement_back(loopnodes, newBBnode(pred));
          push(newBBnode(pred));
        }
      }
    }
  }

  vbprintf(VERBOSE_CF, "loop found in interval with loop head ->[%s %d]\n",
           intervalb->ihead->label, intervalb->loophead->pos);
  vbprintf(VERBOSE_CF, "loop nodes found ->%d\n", loopnodes->numItems);
  vbcall(VERBOSE_CF, print_loop_nodes, loopnodes);

  return true;
}

// evaluate the loop type found in an interval (pre-tested, post-tested,
// endless)
void eval_loop_type(IntervalBlock *intervalb) {

  List *latchnodes = &(intervalb->latchNodes);
  List *loopnodes = &(intervalb->loopNodes);

  BasicBlock *latchingnode =
      (BasicBlock *)((BBnode *)List_getHead(latchnodes))->bbptr;
  BasicBlock *loophead = intervalb->loophead;

  if ((noOfBBchildren(latchingnode)) == 2) {

    if (noOfBBchildren(loophead) == 2) {

      if (loophead == latchingnode)
        intervalb->looptype = POST;
      else if (belongs(loophead->thenBB->pos, loopnodes) &&
               belongs(loophead->elseBB->pos, loopnodes))
        intervalb->looptype = POST;
      else
        intervalb->looptype = PRE;
    } else
      intervalb->looptype = POST;
  } else if (noOfBBchildren(loophead) == 2)
    intervalb->looptype = PRE;
  else
    intervalb->looptype = ENDLESS;

  vbprintf(VERBOSE_CF, "loop type is ->%d\n", intervalb->looptype);
}

IntervalBlock *can_be_added_to_interval(List *curAllfuncIntervals,
                                        BasicBlock *bb) {

  ListElement *current = curAllfuncIntervals->current;
  List_reset(curAllfuncIntervals);

  for (int i = 0; i < curAllfuncIntervals->numItems; i++) {

    IntervalBlock *iblock =
        (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
    if (includesAll(&(iblock->BBsInInterval), &(bb->Predecessors))) {
      curAllfuncIntervals->current = current;
      return iblock;
    }
  }

  curAllfuncIntervals->current = current;
  return NULL;
}

void createIntervals() {

  IntervalBlock *anIntervalBlock;

  vbprintf(VERBOSE_CF, "\n\ncreating intervals\n");

  for (int i = 1; i <= curBBlist->numItems; i++) {

    BasicBlock *bbinProcess = (BasicBlock *)get_bb_in_cfg(i, curBBlist);
    if (bbinProcess->merged)
      continue; // ignore any bb that has  been merged with its parent

    if ((anIntervalBlock =
             can_be_added_to_interval(curAllfuncIntervals, bbinProcess))) {

      bbinProcess->head = anIntervalBlock->ihead;
      List_pushElement_back(&(anIntervalBlock->BBsInInterval),
                            newBBnode2(bbinProcess));
    } else {

      IntervalBlock *curInterval =
          (IntervalBlock *)calloc(1, sizeof(IntervalBlock));
      List_new(&(curInterval->BBsInInterval));
      curInterval->ihead = bbinProcess;
      bbinProcess->head = curInterval->ihead;

      List_pushElement_back(&(curInterval->BBsInInterval),
                            newBBnode2(bbinProcess));
      List_pushElement_back(curAllfuncIntervals, curInterval);
    }
  }
}

void performT2() {

  vbprintf(VERBOSE_CF, "\n\nperfroming T2 transformation\n");

  // start from bottom of the control flow tree
  for (int i = curBBlist->numItems; i > 0; i--) {

    BasicBlock *bb = (BasicBlock *)get_bb_in_cfg(i, curBBlist);
    if (bb->merged)
      continue;

    vbprintf(VERBOSE_CF, "T2 processing [%d %s]\n", bb->pos, bb->label);

    if (canApplyT2(bb)) {

      BasicBlock *thenBB = bb->thenBB;
      // find the interval the child of the bb belongs to
      IntervalBlock *thenBBintervalb = get_IntervalBlock(thenBB);

      // delete thenBB from its interval
      delete_from_Interval(&(thenBBintervalb->BBsInInterval), thenBB);
      // merge bb with thenBB
      mergeT2(bb, bb->thenBB);
      // delete thenBB's interval if no more bbs in it
      if (List_is_empty(&(thenBBintervalb->BBsInInterval)))
        List_remove(curAllfuncIntervals, thenBBintervalb);
    }
  }
}

void control_flow(List *funcBlocks) {

  List_reset(funcBlocks); // point to the first funcblock
  stack = List_new(NULL); // create the list for stack emulation
  curAllfuncIntervals = List_new(NULL);

  for (int i = 0; i < funcBlocks->numItems; i++) {

    FuncBlock *funcblock = (FuncBlock *)List_getNextElement(funcBlocks);
    vbprintf(VERBOSE_CF, "flow control analysis for function ->%s\n",
             funcblock->funcName);

    curBBlist = &(funcblock->funcBBlist);
    List_reset(curBBlist);

    BasicBlock *firstbb = (BasicBlock *)List_getHead(curBBlist);
    int n = curBBlist->numItems;
    // assign a number to each bb in accordance with its position in CFG
    number_bbs(firstbb, &n);
    vbcall(VERBOSE_CF, display_BBs_seq);

    List_destroy(curAllfuncIntervals);
    createIntervals();
    vbcall(VERBOSE_CF, display_allfuncIntervals);
    int nodesInInterval = 0;

    vbprintf(VERBOSE_CF, "\nstarting intervals reduction\n");

    // loop until just one interval remains (the limit interval) with just one
    // node in it

    do {

      performT2();

      // for each interval
      // check for loop and find loop nodes
      // evaluate the type of each loop
      // check for conditionals and merge
      // merge loop if one was found above

      vbprintf(VERBOSE_CF, "\nprocessing intervals\n");

      for (int i = 0; i < curAllfuncIntervals->numItems; i++) {

        IntervalBlock *intervalBlock =
            (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
        List *intervalnodes = &(intervalBlock->BBsInInterval);

        bool loopfound = check_for_loops(intervalBlock);
        if (loopfound)
          eval_loop_type(intervalBlock);

        check_for_conditionals(intervalnodes);

        if (loopfound)
          merge_loop(intervalBlock);
      }

      performT2();

      List_destroy(curAllfuncIntervals); // empty the list of all elements
      createIntervals();
      vbcall(VERBOSE_CF, display_allfuncIntervals);

      // get number of nodes in first interval.
      // it will be the only one when we reach the limit interval
      nodesInInterval = num_of_nodes_in_interval();
      
      vbprintf(
          VERBOSE_CF,
          "number of intervals ->%d, number of nodes in first interval ->%d\n",
          curAllfuncIntervals->numItems, nodesInInterval);

    } while ((curAllfuncIntervals->numItems > 1) || (nodesInInterval > 1));

    vbprintf(VERBOSE_CF, "\ncontrol flow analysis for function %s completed\n",
             funcblock->funcName);

    if (UNIT_TEST)
      return;

    // add on front of instructions in limit node the function definition -
    // func(parm1, parm2, ..) {
    char *funcdef = getFuncDefinition(funcblock->funcName);
    BasicBlock *limitnode = get_bb_in_limit_interval();
    List_pushElement(&(limitnode->ClikeInsL), newClikeIns2(funcdef));

    // add at the end return value if not already exists
    bool returnExists = returnAdded(&(limitnode->ClikeInsL));
    if (!returnExists) {
      char *funcRet = getFuncReturn(funcblock->funcName);
      List_pushElement_back(&(limitnode->ClikeInsL), newClikeIns2(funcRet));
    } else
      List_pushElement_back(&(limitnode->ClikeInsL), newClikeIns2("\n}"));

    print_func_ClikeIns();
    save_func_ClikeIns();

  } // end of control flow analysis of current function. get next one
}

void display_BBs_seq() {

  List_reset(curBBlist);

  printf("\n\nsequence of BBs in Link List\n");

  for (int i = 0; i < curBBlist->numItems; i++) {

    BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);
    printf("[%d %s], ", bb->pos, bb->label);
  }

  printf("\n\nsequence of BBs in CFG\n");

  for (int i = 0; i < curBBlist->numItems; i++) {

    BasicBlock *bb = (BasicBlock *)get_bb_in_cfg(i + 1, curBBlist);
    printf("[%d %s], ", bb->pos, bb->label);
  }
}

void print_interval(IntervalBlock *interval) {

  printf("Interval head ->%d\n", interval->ihead->pos);
  List *bbsIninterval = &(interval->BBsInInterval);
  ListElement *current = bbsIninterval->current;
  List_reset(bbsIninterval); // point to the first block
  printf("BBs in interval -> ");
  for (int i = 0; i < bbsIninterval->numItems; i++) {

    BBnode *bb = (BBnode *)List_getNextElement(bbsIninterval);
    printf("[%d %s] ", bb->bbptr->pos, bb->bbptr->label);
  }
  bbsIninterval->current = current;

  printf("\n");
}

void display_allfuncIntervals() {

  ListElement *current = curAllfuncIntervals->current;
  List_reset(curAllfuncIntervals);

  printf("\n\nAll intervals found\n");
  for (int i = 0; i < curAllfuncIntervals->numItems; i++) {

    IntervalBlock *interval =
        (IntervalBlock *)List_getNextElement(curAllfuncIntervals);
    print_interval(interval);
  }
  curAllfuncIntervals->current = current;
}

void print_func_ClikeIns() {

  List_reset(curBBlist);

  printf("\n\nC like instructions in function\n");

  for (int i = 0; i < curBBlist->numItems; i++) {

    BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);
    if (bb->merged)
      continue;
    printf("Instructions in ->[%d %s]\n", bb->pos, bb->label);
    List *clikeIns = &(bb->ClikeInsL);
    List_reset(clikeIns);

    for (int j = 0; j < clikeIns->numItems; j++) {
      ClikeIns *ins = (ClikeIns *)List_getNextElement(clikeIns);
      printf("%s", ins->cins);
    }

    printf("\n");
  }
}

void save_func_ClikeIns() {

  List_reset(curBBlist);

  printf("\n\nSaving C like instructions in recover.c\n");
  FILE *fPointer;
  fPointer = fopen("recover.c", "a+");
  if (!fPointer) {
    printf("could not open recover.c file\n");
    exit(1);
  }

  for (int i = 0; i < curBBlist->numItems; i++) {

    BasicBlock *bb = (BasicBlock *)List_getNextElement(curBBlist);
    if (bb->merged)
      continue;
    List *clikeIns = &(bb->ClikeInsL);
    List_reset(clikeIns);

    for (int j = 0; j < clikeIns->numItems; j++) {
      ClikeIns *ins = (ClikeIns *)List_getNextElement(clikeIns);
      fprintf(fPointer, "%s", ins->cins);
    }

    fprintf(fPointer, "\n\n");
    fclose(fPointer);
  }
}

void print_loop_nodes(List *loop_nodes) {

  ListElement *current = loop_nodes->current;
  List_reset(loop_nodes); // point to the first block
  printf("Loop Nodes -> ");
  for (int i = 0; i < loop_nodes->numItems; i++) {

    BBnode *bb = (BBnode *)List_getNextElement(loop_nodes);
    printf("[%d %s] ", bb->bbptr->pos, bb->bbptr->label);
  }
  loop_nodes->current = current;

  printf("\n");
}
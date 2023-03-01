#ifndef DECOMPILER_CFLOW_H
#define DECOMPILER_CFLOW_H

#include "../utils/ll.h"
#include "../models/assembly.h"



typedef enum  { 
       NONE, PRE, POST, ENDLESS 
	} LoopType;


typedef struct   {
  ListElement      listElement;          
  List             BBsInInterval;
  BasicBlock       *ihead;
  BasicBlock       *loophead;
  List             loopNodes;
  List             latchNodes;
  LoopType         looptype;

} IntervalBlock;


typedef struct {
  ListElement      listElement;
  BasicBlock       *bbptr;

} BBnode;


#endif
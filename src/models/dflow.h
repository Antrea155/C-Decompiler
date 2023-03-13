#ifndef DECOMPILER_DFLOW_H
#define DECOMPILER_DFLOW_H

#include "../utils/ll.h"
#define MAX_PARMS 5

typedef struct  _symbol {

  ListElement      listElement; 
  char            *index1;
  int              index2;      
  int              size;
  char             *name;
  char             *value;
  bool             nameSet;
  struct _symbol  *reference;

} Symbol;



typedef struct   {
  ListElement      listElement;          
  char             *fname;   //func name
  char             *parmtypes[MAX_PARMS]; //the parameter types of the function in the assembly listing
  //SYMTYPE          retType;
  char             *retValue;
  List             funcsymbols;

} FuncSymBlock;


typedef struct {
  ListElement      listElement;
  char             *cins;

} ClikeIns;

#endif
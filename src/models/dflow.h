#ifndef DECOMPILER_DFLOW_H
#define DECOMPILER_DFLOW_H

#include "../utils/ll.h"

typedef enum  { 
        INV=0,	
        REG,
		UNSIGNED, SIGNED,
		NOTYPE, 
		CHAR, BYTE,
		SINT,
		INT, LONG, FLOAT,
		DOUBLE,
		LDOUBLE,
		STRING,
        FUNC
	} SYMTYPE;

typedef struct  _symbol {

  ListElement      listElement; 
  char            *index1;
  int              index2;      
  SYMTYPE          type;
  int              size;
  char             *name;
  char             *value;
  char             *initvalue;
  bool             seen;
  bool             _unsigned;
  bool             nameSet;
  bool             isReg;
  struct _symbol  *reference;

} Symbol;



typedef struct   {
  ListElement      listElement;          
  char             *fname;   //func name
  SYMTYPE          retType;
  char             *retValue;
  int              maxLocalOffset;
  int              maxLocals;
  int              maxParOffset;
  int              maxPars;
  List             funcsymbols;

} FuncSymBlock;
/*
typedef struct   {
  ListElement      listElement;         
  Symbol           *symbol;

} stackEntry; 
*/

#endif
#ifndef DECOMPILER_DFLOW_H
#define DECOMPILER_DFLOW_H

#include "../utils/ll.h"
#define MAX_PARMS 5

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

/*
typedef struct   {
  ListElement      listElement;
  char             *index;           
  Symbol           *symbol;

} GlobalSymbols;  //SymTab

typedef struct   {
  ListElement      listElement;
  int              *index;           
  Symbol           *symbol;

} FuncSymbols;
*/
/*
typedef struct   {
  ListElement      listElement;          
  SYMTYPE          retType;
  char             *retValue;
  List             funcsymbols;

} FuncSymbols; //actRecord
*/

typedef struct   {
  ListElement      listElement;          
  char             *fname;   //func name
  char             *parmtypes[MAX_PARMS]; //the parameter types of the function in the assembly listing
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
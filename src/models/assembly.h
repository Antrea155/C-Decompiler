

typedef struct {
  ListElement      listElement;
  char             label[20];
  List             Instructions;        //List of Instructions
  
} Block;



typedef struct {
    
 ListElement      listElement;
 char mnemonic[20];
 char operant1[20];
 char operant2[20];
  
} Instruction;
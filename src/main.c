#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"


#define FOREVER for(;;)
#define MAX_CHARS_IN_LINE 200

void trim(char * s) {       
    char * p = s;       //input string pointer
    int l = strlen(p);

    while(isspace(p[l - 1])) //replacing the spaces at the end with zero
            p[--l] = 0;
            
    while(* p && isspace(* p)) //moving pointer until no space is found
            ++p, --l;

    memmove(s, p, l + 1); //overrite input string 

    
}   

void print_instructions(List *Instructions) {

    List_reset(Instructions);
    while   (!List_is_empty(Instructions)) {
      Instruction *ins = (Instruction *)List_popElement(Instructions);
      printf("   Instruction\n");
      printf("       mnemonic-> %s\n",ins->mnemonic);
      printf("       operant1-> %s\n",ins->operant1);
      printf("       operant2-> %s\n",ins->operant2);
      
    }
}


void print_blocks(List *Blocks) {

    List_reset(Blocks);     //point to the first block
    while   (!List_is_empty(Blocks)) {

      Block *block = (Block *)List_popElement(Blocks);

      printf("block->%s\n",block->label);
      print_instructions(&(block->Instructions));

    }
}

void process_instruction(char *inst, List *Instructions) {

    char *currentWord;

    printf("  processing inst-> %s\n",inst);
  
    Instruction *instructionP = malloc(sizeof(Instruction));
    instructionP->mnemonic[0] = 0;
    instructionP->operant1[0] = 0;
    instructionP->operant2[0] = 0;

    currentWord = strtok(inst, " ");        //tokenise instruction
    int index = 0;
    /*Run over current line (word-by-word)*/
    while (currentWord != NULL)
    {
            //printf("      word-> %s <-\n",currentWord);
                 
           
            if (index == 0) 
                strcpy((char *)&instructionP->mnemonic,currentWord);
            if (index == 1){             
                if(currentWord[strlen(currentWord)-1] ==',' )  
                    currentWord[strlen(currentWord)-1] =0           
                  strcpy((char *)&instructionP->operant1,currentWord); 
            }
            if (index == 2) 
                strcpy((char *)&instructionP->operant2,currentWord);
            
            index++;
           
            currentWord = strtok(NULL, " ");
            //continue;
    }
      
    List_pushElement_back( Instructions, instructionP); 
    
}

void parse_assembly(FILE* fpointer) {

    List *Blocks;         //list of pointers to Blocks
    Blocks = List_new(NULL);  //create the list of blocks
    Block *blockP;

    char currentLine[MAX_CHARS_IN_LINE];
    char testline[MAX_CHARS_IN_LINE];

    FOREVER     //endles loop
    {
        /*Hold the current line in file*/
        fgets(currentLine, MAX_CHARS_IN_LINE, fpointer); 
        currentLine[strlen(currentLine)-1]=0; 

                //fgets(testline, MAX_CHARS_IN_LINE, fpointer);
                // testline[strlen(testline)-1]=0; 
                //trim(testline);
                // char ch = ',';
                // int i,j;
                // int len = strlen(currentLine);
                // for(int i=0; i<len; i++){
                //     if(currentLine[i] == ch){
                //         for(j=i; j<len; j++){
                //             currentLine[j] = currentLine[j+1];
                //         }
                //         len--;
                //         i--;
                //     }
                // }
                //printf("String after removing '%c': %s", ch, testline);
        //remove leading spaces
        trim(currentLine);

        /*Check if we get to the end of the file*/
        if(feof(fpointer))
            break;

        // check if current line is the beginning of a block
        if (strchr(currentLine, ':')) { //search for ':' char

            
            printf("new block found-> %s\n",currentLine);
            
            blockP = malloc(sizeof(Block)); //creating memory space 
            strcpy((char *)&blockP->label,currentLine);     //copies the string in currentline to the address where blockP->label points to
            //create a list to hold the instructions of the block
            List_new(&(blockP->Instructions));
            //add current block to the blocks list
            List_pushElement_back( Blocks, blockP); 

            continue;
        }
        /*the line is an instruction. process it and add it to the current block
         list of instructions*/
        
        process_instruction(currentLine, &(blockP->Instructions));
    
    }

   print_blocks(Blocks);
}

int main(int argc, char* argv[]) {  // argc is the number of inputs thats entered in the commandline
                                    // argv is an array that holds those values

    FILE *fPointer;     //A file pointer stores the current position of a read or write within a file.
                        // to let the compiler perform input and output functions on the file.
    if(argc == 1)
    {
        printf("an assembly file listing from gcc -S is needed\n");
        exit(1);
    }

    fPointer = fopen(argv[1], "r+"); //opens the assembly file which is stored in argv[1]
	

    rewind(fPointer); //sets the file position to the beginning of the file of the given stream
    parse_assembly(fPointer);   
    fclose(fPointer);

}
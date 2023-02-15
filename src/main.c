#include <stdio.h>
#include "utils/ll.h"

#define VERBOSE_PARSE false
#define VERBOSE_CFG true
#define VERBOSE_DF true

void parse_assembly(FILE* fpointer, List *funBlocks, List *stringBlocks);
void print_blocks(List *Blocks);
void print_string_blocks(List *Blocks);
void create_CFG(List *funcblocks);
void display_successors(List *funcblocks);
void display_predecessors(List *funcBlocksP);
void generate_cfg_dot_images(List *funcblocks);
void data_flow(List *funcBlocks, List *stringBlocks);
void display_dfins(List *funcBlocks);
void control_flow(List *funcBlocks);
//void display_BBs_seq(List *funcBlocks);
//void display_intervals(List *funcBlocks);
bool is64bits = false;

int main(int argc, char* argv[]) {  // argc is the number of inputs thats entered in the commandline
                                    // argv is an array that holds those values

    FILE *fPointer;     
    
    List *funcBlocksP;         //list of pointers to Blocks
    funcBlocksP = List_new(NULL);  //create the list of blocks

    List *stringBlocksP;         //list of pointers to string Blocks
    stringBlocksP = List_new(NULL);  //create the list of blocks
    


    if(argc == 1)
    {
        printf("an assembly file listing from https://godbolt.org is needed\n");
        exit(1);
    }


    fPointer = fopen(argv[1], "r+"); //opens the assembly file which is stored in argv[1]
	

    rewind(fPointer); 
    parse_assembly(fPointer, funcBlocksP, stringBlocksP);   
    fclose(fPointer);

   if (VERBOSE_PARSE) {
    printf("\n\nBlocks in memory\n\n");
    printf("\nFUNCTION blocks\n");
    print_blocks(funcBlocksP);
    printf("\nSTRING blocks\n");
    print_string_blocks(stringBlocksP);
    printf("\n\nEND of Blocks in memory\n\n");
   }
    
    create_CFG(funcBlocksP);
    if (VERBOSE_CFG) {
      display_successors(funcBlocksP);
      display_predecessors(funcBlocksP);
      generate_cfg_dot_images(funcBlocksP);
    }

    data_flow(funcBlocksP, stringBlocksP);
    if (VERBOSE_DF) {
      display_dfins(funcBlocksP);
    }

    control_flow(funcBlocksP);
   // display_BBs_seq(funcBlocksP);
    //display_intervals(funcBlocksP);
}
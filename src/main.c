#include "utils/ll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool VERBOSE_PARSE = false; // get information from parse c file
bool VERBOSE_CFG = false;   // get information from cfg c file
bool VERBOSE_DF = false;    // get information from data flow c file
bool VERBOSE_CF = false;    // get information from control flow c file
bool GEN_GRAPH = false;     // enable graph generation

void parse_assembly(FILE *fpointer, List *funBlocks, List *stringBlocks);
void print_blocks(List *Blocks);
void print_string_blocks(List *Blocks);
void create_CFG(List *funcblocks);
void display_successors(List *funcblocks);
void display_predecessors(List *funcBlocksP);
void generate_cfg_dot_images(List *funcblocks);
void data_flow(List *funcBlocks, List *stringBlocks);
void display_dfins(List *funcBlocks);
void control_flow(List *funcBlocks);
void print_help(void);

bool is64bits = false;
bool UNIT_TEST = false;
int glob_variable = 0; // choose debug option to enable debuging

int main(int argc, char *argv[]) {
  // argc is the number of inputs thats entered in the commandline
  // argv is an array that holds those values

  FILE *fPointer;
  char cmdOption[20], assFile[25];

  List *funcBlocksP;            // list of pointers to Blocks
  funcBlocksP = List_new(NULL); // create the list of blocks

  List *stringBlocksP;            // list of pointers to string Blocks
  stringBlocksP = List_new(NULL); // create the list of blocks

  if (argc == 1) {
    printf("an assembly file listing from https://godbolt.org is needed\n");
    printf("Usage: dec [OPTIONS] assembly_file\n");
    printf("Try dec --help for more information\n");
    exit(1);
  }

  // parse command line argumnets
  for (int i = 1; i < argc; i++) {

    if (*argv[i] == '-') {
      ++argv[i];                  // next char same arg
      if (*argv[i] == '\0')       // end of arg
        ++i;                      // go to next arg
      else if (*argv[i] == '-') { // second -
        ++argv[i];
        if (*argv[i] == '\0')
          ++i;
        strcpy(cmdOption, argv[i]); // cmdOption = mode,graph,verbose,help

        if (!strcmp(cmdOption, "mode")) {
          ++i;
          if (!strcmp("32", argv[i])) {
            is64bits = false;
            printf("32bits assembly set\n");
          } else if (!strcmp("64", argv[i])) {
            is64bits = true;
            printf("64bits assembly set\n");
          } else {
            printf("invalid mode option - should be 32 or 64\n");
            exit(1);
          }
        } else if (!strcmp(cmdOption, "graph")) {
          GEN_GRAPH = true;
          printf("graph image genaration set\n");
        } else if (!strcmp(cmdOption, "verbose")) {
          ++i;
          if (!strcmp("parser", argv[i])) {
            VERBOSE_PARSE = true;
            printf("parser verbose set\n");
          } else if (!strcmp("cfg", argv[i])) {
            VERBOSE_CFG = true;
            printf("cfg verbose set\n");
          } else if (!strcmp("data", argv[i])) {
            VERBOSE_DF = true;
            printf("data verbose set\n");
          } else if (!strcmp("cf", argv[i])) {
            VERBOSE_CF = true;
            printf("Control Flow verbose set\n");
          } else {
            printf(
                "invalid verbose option - should be parser, cfg, data or cf\n");
            exit(1);
          }
        } else if (!strcmp(cmdOption, "help")) {

          print_help();
          exit(1);
        } else if (!strcmp(cmdOption, "debug")) {
          glob_variable = 2;
          printf("debugging enabled\n");
        } else {
          printf("invalid option\n");
          exit(1);
        }
      }
    } else
      strcpy(assFile, argv[i]);
  }

  printf("\n");
  // reset output file
  fPointer = fopen("recover.c", "w");
  fclose(fPointer);

  fPointer = fopen(assFile, "r+");
  if (!fPointer) {
    printf("could not open assembly file\n");
    exit(1);
  }

  rewind(fPointer);
  parse_assembly(fPointer, funcBlocksP, stringBlocksP);
  fclose(fPointer);

  if (VERBOSE_PARSE) {
    printf("\n\n--- Parser verbose ---\n\n");
    printf("\n\nBlocks in memory\n\n");
    printf("\nFUNCTION blocks\n");
    print_blocks(funcBlocksP);
    printf("\nSTRING blocks\n");
    print_string_blocks(stringBlocksP);
    printf("\n\nEND of Blocks in memory\n\n");
    printf("\n\n--- Parser verbose Completed ---\n\n");
  }

  create_CFG(funcBlocksP);
  if (GEN_GRAPH) {
    generate_cfg_dot_images(funcBlocksP);
  }
  if (VERBOSE_CFG) {
    printf("\n\n--- Control Flow Graph verbose ---\n\n");
    display_successors(funcBlocksP);
    display_predecessors(funcBlocksP);
    printf("\n\n--- Control Flow Graph verbose Completed ---\n\n");
  }

  data_flow(funcBlocksP, stringBlocksP);
  if (VERBOSE_DF) {
      printf("\n\n--- Data Flow verbose ---\n\n");
      display_dfins(funcBlocksP);
      printf("\n\n--- Data Flow verbose Completed ---\n\n");
    }

  
 if (VERBOSE_CF) {
    printf("\n\n--- Control Flow verbose for each function ---\n\n");
  }

  control_flow(funcBlocksP);
}

void print_help() {
  printf("\n");
  printf("Usage: dec [ OPTIONS ] assembly_file\n");
  printf("e.g. dec --mode 64 --verbose cf ../test_assemblies/assembly_file.s\n");
  printf("\n");
  printf("recover C code from an assembly listing in AT&T syntax\n");
  printf("\n");
  printf("Options:\n");
  printf("--mode 32/64                   the assembly is in 32 or 64 bits "
         "instructions\n");
  printf("--graph                        the CFG module will generate an image "
         "of the control flow\n");
  printf("--verbose parser/cfg/data/cf   enable verbose mode for the specified "
         "module\n");
  printf("--debug                        to enable debugging info\n");
  printf("--help                         print options help\n");
}
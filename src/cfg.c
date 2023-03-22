#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "utils/ll.h"
#include "models/assembly.h"

// find block by label
BasicBlock *find_block(char *label, List *blocks)
{

    List_reset(blocks); // point to the first block

    for (int i = 0; i < blocks->numItems; i++)
    {

        BasicBlock *block = (BasicBlock *)List_getNextElement(blocks);
        if (!strcmp(label, block->label))
            return (block);
    }
    dlprintf(1, "target block %s not found\n", label);
    return 0;
}

// find block by pointer
Predecessor *find_bbptr(BasicBlock *ptr, List *blocks)
{

    List_reset(blocks); // point to the first block
    Predecessor *block;

    for (int i = 0; i < blocks->numItems; i++)
    {

        block = (Predecessor *)List_getNextElement(blocks);
        if (ptr == block->bbptr)
            return (block);
    }

    dlprintf(1, "target block ptr not found\n");
    return 0;
}

void create_CFG(List *funcblocks)
{

    // link thenBB and elseBB pointers to target basic blocks
    // add to predecessors list of each bb the bb pointers of the bbs that connect to it

    BasicBlock *targetBlock = 0;

    List_reset(funcblocks); // point to the first funcblock

    for (int i = 0; i < funcblocks->numItems; i++)
    {

        FuncBlock *block = (FuncBlock *)List_getNextElement(funcblocks);
        printf("linking basic blocks for function -> %s\n", block->funcName);

        List *BBlist = &(block->funcBBlist);
        List_reset(BBlist);

        for (int i = 0; i < BBlist->numItems; i++)
        {

            targetBlock = 0;
            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);

            if (bb->thenLabel)
            {

                ListElement *current = BBlist->current; // save current pointer
                targetBlock = find_block(bb->thenLabel, BBlist);
                BBlist->current = current; // restore current pointer
                bb->thenBB = targetBlock;
                if (targetBlock)
                {
                    dlprintf(1, "found target block from label ->%s to label ->%s\n", bb->label, targetBlock->label);
                }

                // add this bb as a predecessor for the target bb
                List *predecessors = &(targetBlock->Predecessors);
                Predecessor *bbptr = (Predecessor *)malloc(sizeof(Predecessor));
                bbptr->bbptr = bb;
                List_pushElement_back(predecessors, bbptr);
            }
            targetBlock = 0;
            if (bb->elseLabel)
            {

                ListElement *current = BBlist->current; // save current pointer
                targetBlock = find_block(bb->elseLabel, BBlist);
                BBlist->current = current; // restore current pointer
                bb->elseBB = targetBlock;
                if (targetBlock)
                {
                    dlprintf(1, "found target block from label ->%s to label ->%s\n", bb->label, targetBlock->label);
                }

                // add this bb as a predecessor for the target bb
                List *predecessors = &(targetBlock->Predecessors);
                Predecessor *bbptr = (Predecessor *)malloc(sizeof(Predecessor));
                bbptr->bbptr = bb;
                List_pushElement_back(predecessors, bbptr);
            }
        }
        // delete empty blocks
        // if a bb is pointing to an empty bb then delete it and make the bb point to the successor
        // of the deleted bb

        List_reset(BBlist);
        for (int i = 0; i < BBlist->numItems; i++)
        {

            targetBlock = 0;
            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);

            if (bb->thenBB)
            {

                targetBlock = bb->thenBB;

                // if target BB has no instructions and no cmpOperator then delete it
                if ((!targetBlock->compOperator) && (List_is_empty(&(targetBlock->Instructions))))
                {
                    BasicBlock *newTargetBlock;
                    // the new thenbb is the successor of the deleted bb
                    bb->thenBB = targetBlock->thenBB;
                    List_remove(BBlist, targetBlock);
                    newTargetBlock = bb->thenBB;
                    dlprintf(1, "deleted empty then BB successor of %s\n", bb->label);

                    // add this bb as a predecessor for the new target bb
                    List *predecessors = &(newTargetBlock->Predecessors);
                    Predecessor *bbptr = (Predecessor *)malloc(sizeof(Predecessor));
                    bbptr->bbptr = bb;
                    List_pushElement_back(predecessors, bbptr);
                    Predecessor *bbptr2 = find_bbptr(targetBlock, predecessors);
                    List_remove(predecessors, bbptr2);
                }
            }

            if (bb->elseBB)
            {

                targetBlock = bb->elseBB;

                // if target BB has no instructions and no cmpOperator then delete it
                if ((!targetBlock->compOperator) && (List_is_empty(&(targetBlock->Instructions))))
                {
                    BasicBlock *newTargetBlock;
                    bb->elseBB = targetBlock->thenBB;
                    List_remove(BBlist, targetBlock);
                    newTargetBlock = bb->elseBB;
                    dlprintf(1, "deleted empty else BB successor of %s\n", bb->label);

                    // add this bb as a predecessor for the new target bb
                    List *predecessors = &(newTargetBlock->Predecessors);
                    Predecessor *bbptr = (Predecessor *)malloc(sizeof(Predecessor));
                    bbptr->bbptr = bb;
                    List_pushElement_back(predecessors, bbptr);
                    Predecessor *bbptr2 = find_bbptr(targetBlock, predecessors);
                    List_remove(predecessors, bbptr2);
                    // List_popElement(predecessors);
                }
            }
        }
    }
}

void display_successors(List *funcblocks)
{

    List_reset(funcblocks); // point to the first block

    for (int i = 0; i < funcblocks->numItems; i++)
    {

        FuncBlock *block = (FuncBlock *)List_getNextElement(funcblocks);
        printf("\n\nsuccessors of BBs in function -> %s\n", block->funcName);

        List *BBlist = &(block->funcBBlist);
        List_reset(BBlist);

        for (int i = 0; i < BBlist->numItems; i++)
        {

            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);

            if (bb->merged)
                continue;
            printf(" %s ->{", bb->label);

            if (bb->thenBB)
                printf("%s", bb->thenBB->label);
            else
                printf("E");

            if (bb->elseBB)
            {

                printf(", %s}\n", bb->elseBB->label);
            }
            else
                printf(", E}\n");
        }
    }
}

void display_predecessors(List *funcblocks)
{

    List_reset(funcblocks); // point to the first block

    for (int i = 0; i < funcblocks->numItems; i++)
    {

        FuncBlock *block = (FuncBlock *)List_getNextElement(funcblocks);
        printf("\n\npredecessors of BBs in function -> %s\n", block->funcName);

        List *BBlist = &(block->funcBBlist);
        List_reset(BBlist);

        for (int i = 0; i < BBlist->numItems; i++)
        {

            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);
            if (bb->merged)
                continue;
            printf(" %s ->{ ", bb->label);

            List *predecessors = &(bb->Predecessors);
            List_reset(predecessors);
            for (int i = 0; i < predecessors->numItems; i++)
            {
                Predecessor *bbptr = (Predecessor *)List_getNextElement(predecessors);
                printf("%s ", bbptr->bbptr->label);
            }

            printf("}\n");
        }
    }
}

// genearte image graphs
//  see https://graphviz.org/Gallery/directed/datastruct.html
//  decompiler generates the cfgimage.dot file
//  need to run below command to create the cfgimages.svg file from the .dot file. open it in a browser
//  C:\decompiler\dot>dot -Tsvg cfgimages.dot -o cfgimages.svg

FILE *init_image(char *filename)
{

    FILE *fpointer;
    char filen[40];

    strcpy(filen, "C:\\decompiler\\dot\\");
    strcat(filen, filename);
    strcat(filen, ".dot");
    fpointer = fopen(filen, "w+");

    fprintf(fpointer, "%s\n", "digraph g {");
    fprintf(fpointer, "%s\n", "fontname=\"Helvetica,Arial,sans-serif\"");
    fprintf(fpointer, "%s\n", "node [fontname=\"Helvetica,Arial,sans-serif\"]");
    fprintf(fpointer, "%s\n", "edge [fontname=\"Helvetica,Arial,sans-serif\"]");
    fprintf(fpointer, "%s\n", "graph [");
    fprintf(fpointer, "%s\n", "rankdir = \"LR\"");
    fprintf(fpointer, "%s\n", "];");
    fprintf(fpointer, "%s\n", "node [");
    fprintf(fpointer, "%s\n", "fontsize = \"14\"");
    fprintf(fpointer, "%s\n", "shape = \"ellipse\"");
    fprintf(fpointer, "%s\n", "];");
    fprintf(fpointer, "%s\n", "edge [");
    fprintf(fpointer, "%s\n", "];");

    return fpointer;
}

void init_cluster(char *cluster, FILE *fp)
{

    fprintf(fp, "%s%s%s\n", "subgraph \"", cluster, "\" {");
    fprintf(fp, "%s%s%s\n", "label = \"", cluster, "\";");
    fprintf(fp, "%s\n", "cluster=true;");
}

void add_dot_node(BasicBlock *bb, FILE *fp)
{

    List *instrList = &(bb->Instructions);
    char firstInstr[20];

    List_reset(instrList);
    Instruction *ins = (Instruction *)List_getHead(instrList);

    if (ins == NULL)
    {
        if (bb->compOperator)
            strcpy(firstInstr, bb->compOperator);
        else
            strcpy(firstInstr, "empty");
    }
    else
        strcpy(firstInstr, ins->mnemonic);
    fprintf(fp, "%s%s%s\n", "\"", bb->label, "\"[");
    fprintf(fp, "%s%s%s%s%s\n", "label = \"<f0>", bb->label, "| <f1> ", firstInstr, " |<f2>\"");
    fprintf(fp, "%s\n", "shape = \"record\"");
    fprintf(fp, "%s\n", "];");
}

void add_dot_link(BasicBlock *bb, BasicBlock *target, int pos, FILE *fp)
{
    if (pos == 0)
        fprintf(fp, "%s%s%s%s%s\n", "\"", bb->label, "\":f1 -> \"", target->label, "\":f0 [");
    else
        fprintf(fp, "%s%s%s%s%s\n", "\"", bb->label, "\":f2 -> \"", target->label, "\":f0 [");
    fprintf(fp, "%s\n", "id=1");
    fprintf(fp, "%s\n", "];");
}

////////////

void generate_cfg_dot_images(List *funcblocks)
{

    FILE *fp;

    BasicBlock *targetBlock = 0;

    fp = init_image("cfgimages");

    List_reset(funcblocks); // point to the first block

    for (int i = 0; i < funcblocks->numItems; i++)
    {

        FuncBlock *block = (FuncBlock *)List_getNextElement(funcblocks);
        printf("\n\ngenerating graph image for function -> %s\n", block->funcName);
        init_cluster(block->funcName, fp);

        List *BBlist = &(block->funcBBlist);
        List_reset(BBlist);

        for (int i = 0; i < BBlist->numItems; i++)
        {

            targetBlock = 0;
            BasicBlock *bb = (BasicBlock *)List_getNextElement(BBlist);

            add_dot_node(bb, fp);

            if (bb->thenBB)
            {

                add_dot_link(bb, bb->thenBB, 0, fp);
            }

            if (bb->elseBB)
            {

                add_dot_link(bb, bb->elseBB, 1, fp);
            }
        }
        fprintf(fp, "%s\n", "}");
        // fclose(fp);
    }
    fprintf(fp, "%s\n", "}");
    fclose(fp);
}
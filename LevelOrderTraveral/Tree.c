#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"
#include "Queue.h"

Tree InsertTree(int value, Tree tree){
    Tree T = tree;
	if(!T){
		T = malloc(sizeof(struct TreeNode));
		if(!T){
			exit(-1);
		}
		T->Element = value;
		T->Left = T->Right = NULL;
		return T;
	}
	if(value < T->Element){
		T->Left = InsertTree(value, T->Left);		
        return T;
	}
	if(value > T->Element){
		T->Right = InsertTree(value, T->Right);		
        return T;
	}
    return T;
}

int TestData[10];

Tree BuildTree(){
	int value;
	int num =10;
	Tree tree = NULL;
	while(num --) {

#ifndef __UT__
		scanf("%d", &value);
#else
        value = TestData[10-(num+1)];
#endif
		tree = InsertTree(value, tree);
	}

	return tree;
}

void FreeTree(Tree tree)
{
    if(tree) {
        FreeTree(tree->Left);
        FreeTree(tree->Right);
        free(tree);
    }
    return;
}

void PrintTreeInfixOrder(Tree tree)
{
    if(tree) {
        PrintTreeInfixOrder(tree->Left);
        printf("%d ", tree->Element);
        PrintTreeInfixOrder(tree->Right);
    }
    return;
}



#ifdef __UT__
typedef void (*test)(void);

void PrintNode( Tree NodePtr )
{
   printf(" %d", NodePtr->Element);
}

void Level_order ( Tree T, void (*visit)(Tree ThisNode) ){
    Queue Q = CreateQueue();
    Tree tree;

    Enqueue(T, Q);
    while(!IsEmpty(Q)) { 
        tree = FrontAndDequeue(Q);
        if(tree) {
            Enqueue(tree->Left, Q);
            Enqueue(tree->Right, Q);
            visit(tree);
        }
    }
    printf("\n");
    DisposeQueue(Q);
    return;
}

void TreeInsertCorrectWithRightBorder(void)
{
       int i;
       Tree tree;
       for(i=0; i<10; i++) {
            TestData[i] = i+1;
       }
        tree = BuildTree();
        PrintTreeInfixOrder(tree);
        printf("\n");
        FreeTree(tree);
}

void TreeInsertCorrectWithLeftBorder(void)
{
       int i;
       Tree tree;
       for(i=0; i<10; i++) {
            TestData[i] = 10 - i;
       }
        tree = BuildTree();
        PrintTreeInfixOrder(tree);
        printf("\n");
        FreeTree(tree);
}

void TreeInsertCorrectWithRandomData(void)
{
       int i;
       Tree tree;
       int data[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
       for(i=0; i< 10; i++) TestData[i] = data[i];
        tree = BuildTree();
        PrintTreeInfixOrder(tree);
        printf("\n");
        FreeTree(tree);
}

void TreeInsertCorrectWithRandomDataAgain(void)
{
       int i;
       Tree tree;
       int data[] = {7, 15, 16, 14, 13, 4, 6, 2, 1, 5};
       for(i=0; i< 10; i++) TestData[i] = data[i];
        tree = BuildTree();
        PrintTreeInfixOrder(tree);
        printf("\n");
        FreeTree(tree);
}

void TreeLevelOrderPrintCorrect(void)
{
       int i;
       Tree tree;

       //printf("TreeLevelOrderPrintCorrect\n");
       int data[] = {7, 15, 16, 14, 13, 4, 6, 2, 1, 5};
       for(i=0; i< 10; i++) TestData[i] = data[i];
        tree = BuildTree();
        //PrintTreeInfixOrder(tree);
        //printf("\n");
        Level_order(tree, PrintNode);
        FreeTree(tree);
}

void TreeLevelOrderPrintWithOneItemTree(void)
{
       int i;
       Tree tree;
       int data[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
       for(i=0; i< 10; i++) TestData[i] = data[i];
        tree = BuildTree();
        Level_order(tree, PrintNode);
        FreeTree(tree);
}

void TreeLevelOrderPrintWithNullTree(void)
{
        Level_order(NULL, PrintNode);
}
static test tests[] = {
    TreeInsertCorrectWithRightBorder,
    TreeInsertCorrectWithLeftBorder,
    TreeInsertCorrectWithRandomData,
    TreeInsertCorrectWithRandomDataAgain,
    TreeLevelOrderPrintCorrect,
    TreeLevelOrderPrintWithOneItemTree,
    TreeLevelOrderPrintWithNullTree,
	(test ) 0,
};

int main(void){
	int at= 0;
	while(tests[at]){
		tests[at++]();
	}
	printf("%d tests passed\n", at);
	return 0;
}

#endif

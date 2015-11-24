#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Tree.h"
#include "Queue.h"

Tree CreateTreeNode(double value)
{
    Tree tree;
    tree = malloc(sizeof(struct TreeNode));
    if(!tree){
        printf("malloc failed\n");
        return NULL;
    }
    tree->Element = value;
    tree->Left = tree->Right = NULL;
    return tree;
}

Tree InsertTreeInOrder(int value, Tree tree){
    Tree T = tree;
	if(!T){
		T = malloc(sizeof(struct TreeNode));
		if(!T){
            printf("malloc failed\n");
            return NULL;
		}
		T->Element = value;
		T->Left = T->Right = NULL;
		return T;
	}
	if(value < T->Element){
		T->Left = InsertTreeInOrder(value, T->Left);		
        return T;
	}
	if(value > T->Element){
		T->Right = InsertTreeInOrder(value, T->Right);		
        return T;
	}
    return T;
}

Tree InsertTreeInReverseOrder(int value, Tree tree){
    Tree T = tree;
	if(!T){
		T = malloc(sizeof(struct TreeNode));
		if(!T){
            printf("malloc failed\n");
            return NULL;
		}
		T->Element = value;
		T->Left = T->Right = NULL;
		return T;
	}
	if(value > T->Element){
		T->Left = InsertTreeInReverseOrder(value, T->Left);		
        return T;
	}
	if(value < T->Element){
		T->Right = InsertTreeInReverseOrder(value, T->Right);		
        return T;
	}
    return T;
}

int TestData[100];

Tree BuildTreeInOrder(){
	int value;
	int num =10;
	Tree tree = NULL;
	while(num --) {

#ifndef __UT__
		scanf("%d", &value);
#else
        value = TestData[10-(num+1)];
#endif
		tree = InsertTreeInOrder(value, tree);
	}

	return tree;
}

Tree BuildTreeInReverseOrder(){
	int value;
	int num =10;
	Tree tree = NULL;
	while(num --) {

#ifndef __UT__
		scanf("%d", &value);
#else
        value = TestData[10-(num+1)];
#endif
		tree = InsertTreeInReverseOrder(value, tree);
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
        if(tree->Left || tree->Right) {
            printf("%c ", (char)(tree->Element));
        }else {
            printf("%f ", tree->Element);
        }
        PrintTreeInfixOrder(tree->Right);
    }
    return;
}

void PrintTreePostfixOrder(Tree tree)
{
    if(tree) {
        PrintTreePostfixOrder(tree->Left);
        PrintTreePostfixOrder(tree->Right);
        if(tree->Left || tree->Right) {
            printf("%c ", (char)(tree->Element));
        }else {
            printf("%f ", tree->Element);
        }
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
    while(!IsEmptyQueue(Q)) { 
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

#endif

void RebuildTreeInOrder(Tree T);

/*int Isomorphic( Tree T1, Tree T2 )
{
    Queue Q1 = CreateQueue();
    Queue Q2 = CreateQueue();

    Tree tree1 = T1;
    Tree tree2 = T2;

    bool doExit = false;
    bool result = false;

    RebuildTreeInOrder(tree1);
    RebuildTreeInOrder(tree2);
    printf("Print: \n");
    PrintTreeInfixOrder(tree1);
    printf("\n");
    PrintTreeInfixOrder(tree2);
    printf("\n");

    Enqueue(tree1, Q1);
    Enqueue(tree2, Q2);
    while((!IsEmptyQueue(Q1)) && (!IsEmptyQueue(Q2))) { 
        tree2 = FrontAndDequeue(Q2);
        tree1 = FrontAndDequeue(Q1);

        if(tree1 && (!tree2)) {
            result = false; 
            doExit = true;
            break;
        }
        if((!tree1) && tree2) {
            result = false; 
            doExit = true;
            break;
        }

        if(tree1 && tree2) {
            if(tree1->Element != tree2->Element) {
                result = false;
                doExit = true;
                break;
            }

            Enqueue(tree1->Left, Q1);
            Enqueue(tree1->Right, Q1);

            Enqueue(tree2->Left, Q2);
            Enqueue(tree2->Right, Q2);
        }
    }

    if(doExit) {
        DisposeQueue(Q1);
        DisposeQueue(Q2);
        return false;
    }

    if(IsEmptyQueue(Q1) && IsEmptyQueue(Q2)) {
        result = true;
    }

    DisposeQueue(Q1);
    DisposeQueue(Q2);
    return result;
}
*/

int Isomorphic( Tree T1, Tree T2 )
{
    if((T1 == NULL) && (T2 == NULL)) return true;
    if((T1 == NULL) && (T2 != NULL)) return false;
    if((T2 == NULL) && (T1 != NULL)) return false;
    if((T1->Element != T2->Element)) return false;
    if((T1->Left == NULL) && (T2->Left == NULL)) return Isomorphic(T1->Right, T2->Right);
    if((T1->Left == NULL) && (T2->Right == NULL)) return Isomorphic(T1->Right, T2->Left);
    if((T1->Left && T2->Left) && (T1->Left->Element == T2->Left->Element)) return Isomorphic(T1->Left, T2->Left) && Isomorphic(T1->Right, T2->Right);
    if((T1->Left && T2->Right) && (T1->Left->Element == T2->Right->Element)) return Isomorphic(T1->Left, T2->Right) && Isomorphic(T1->Right, T2->Left);
    return false;
}

void swap(Tree T)
{
    Tree tmp;

    tmp = T->Left;
    T->Left = T->Right;
    T->Right = tmp;
    return;
}

void RebuildTreeInOrder(Tree T)
{
    if(!T) return;
    if((!T->Left) && (!T->Right)) return;
    if((T->Left && T->Right) && (T->Left->Element > T->Right->Element)) {
        swap(T);
    }

    if((T->Left == NULL)&&(T->Right)) {
        swap(T);
    }
    RebuildTreeInOrder(T->Left);
    RebuildTreeInOrder(T->Right);
}

#ifdef __UT__
void TreeInsertCorrectWithRightBorder(void)
{
    int i;
    Tree tree;
    for(i=0; i<10; i++) {
        TestData[i] = i+1;
    }
    printf("TreeInsertCorrectWithRightBorder:\n");
    tree = BuildTreeInOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    FreeTree(tree);

    tree = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
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
    printf("TreeInsertCorrectWithLeftBorder:\n");
    tree = BuildTreeInOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    FreeTree(tree);

    tree = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    printf("\n");
    FreeTree(tree);
}

void TreeInsertCorrectWithRandomData(void)
{
    int i;
    Tree tree;
    int data[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
    for(i=0; i< 10; i++) TestData[i] = data[i];

    printf("TreeInsertCorrectWithRandomData:\n");

    tree = BuildTreeInOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    FreeTree(tree);

    tree = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    printf("\n");
    FreeTree(tree);
}

void TreeInsertCorrectWithRandomDataAgain(void)
{
    int i;
    Tree tree;
    int data[] = {7, 15, 16, 14, 13, 4, 6, 2, 1, 5};
    for(i=0; i< 10; i++) TestData[i] = data[i];
    tree = BuildTreeInOrder();
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
    tree = BuildTreeInOrder();
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
    tree = BuildTreeInOrder();
    Level_order(tree, PrintNode);
    FreeTree(tree);
}

void RebuildTreeCorrectWithOneItemTree(void)
{
    int i;
    Tree tree;
    int data[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    for(i=0; i< 10; i++) TestData[i] = data[i];
    tree = BuildTreeInOrder();
    Level_order(tree, PrintNode);
    FreeTree(tree);
}

void RebuidTreeCorrectWithTreeOnlyHaveRightChild(void)
{
    int i;
    Tree tree;
    for(i=0; i<10; i++) {
        TestData[i] = i+1;
    }
    printf("RebuidTreeCorrectWithTreeOnlyHaveRightChild: \n");
    tree = BuildTreeInOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    FreeTree(tree);

    tree = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    printf("\n");
    RebuildTreeInOrder(tree);
    printf("After Rebuild\n");
    PrintTreeInfixOrder(tree);
    printf("\n");
    printf("\n");

    FreeTree(tree);
}

void RebuidTreeCorrectWithRandomData(void)
{
    int i;
    Tree tree;
    int data[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
    for(i=0; i< 10; i++) TestData[i] = data[i];

    printf("RebuidTreeCorrectWithRandomData:\n");

    tree = BuildTreeInOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    FreeTree(tree);

    tree = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree);
    printf("\n");
    printf("\n");
    RebuildTreeInOrder(tree);
    printf("After Rebuild\n");
    PrintTreeInfixOrder(tree);
    printf("\n");
    printf("\n");
    FreeTree(tree);
}

void TreeIsomorphicWithTreeOnlyHaveRightChild(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    for(i=0; i<10; i++) {
        TestData[i] = i+1;
    }
    printf("TreeIsomorphicWithTreeOnlyHaveRightChild: \n");
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == true);

    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeIsomorphicWithTreeHaveRandomData(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("RebuidTreeCorrectWithRandomData:\n");

    for(i=0; i< 10; i++) TestData[i] = data[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == true);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeIsomorphicWithTreeHaveDifferentData(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
    int data2[] = {7, 80, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("TreeIsomorphicWithTreeHaveDifferentData:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeIsomorphicWithTreeHaveOneDifferentElement(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    int data2[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

    printf("TreeIsomorphicWithTreeHaveOneDifferentElement:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeIsomorphicWithTreeHaveOneSameElement(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    int data2[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

    printf("TreeIsomorphicWithTreeHaveOneDifferentElement:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == true);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeIsomorphicWithTreeOnlyHaveLeftChild(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    for(i=0; i<10; i++) {
        TestData[i] = 10-i;
    }
    printf("TreeIsomorphicWithTreeOnlyHaveLeftChild: \n");
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == true);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbers(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
    int data2[] = {7, 8, 9, 7, 7, 7, 7, 7, 7, 7};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbers:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbersSwitchOver(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 7, 7, 7, 7, 7, 7, 7};
    int data2[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbers:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbers1(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
    int data2[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbers1:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbers1SwitchOver(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    int data2[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbers1SwitchOver:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}
void TreeNotIsomorphicWithTreeHaveDifferentDataNumbers2(void)
{
    int i;
    Tree tree1;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbers2:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    assert(Isomorphic(tree1, NULL) == false);
    FreeTree(tree1);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbers2SwitchOver(void)
{
    int i;
    Tree tree1;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbers2:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    assert(Isomorphic(NULL, tree1) == false);
    FreeTree(tree1);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbersOneTreeHaveMoreData(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 11};
    int data2[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 2};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbersOneTreeHaveMoreData:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeNotIsomorphicWithTreeHaveDifferentDataNumbersOneTreeHaveMoreDataSwitchOver(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 2};
    int data2[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 11};

    printf("TreeNotIsomorphicWithTreeHaveDifferentDataNumbersOneTreeHaveMoreData:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == false);
    FreeTree(tree1);
    FreeTree(tree2);
}
void TreeIsomorphicWithTreeBecauseTreeAreTheSame(void)
{
    int i;
    Tree tree1;
    Tree tree2;
    int data1[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};
    int data2[] = {7, 8, 9, 12, 5, 3, 4, 13, 11, 1};

    printf("TreeIsomorphicWithTreeBecauseTreeAreTheSame:\n");

    for(i=0; i< 10; i++) TestData[i] = data1[i];
    tree1 = BuildTreeInOrder();
    PrintTreeInfixOrder(tree1);
    printf("\n");

    for(i=0; i< 10; i++) TestData[i] = data2[i];
    tree2 = BuildTreeInReverseOrder();
    PrintTreeInfixOrder(tree2);
    printf("\n");
    printf("\n");

    assert(Isomorphic(tree1, tree2) == true);
    FreeTree(tree1);
    FreeTree(tree2);
}

void TreeIsomorphicWithTreeBothNull(void)
{
    assert(Isomorphic(NULL, NULL) == true);
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

    RebuidTreeCorrectWithTreeOnlyHaveRightChild,
    RebuidTreeCorrectWithRandomData,

    TreeIsomorphicWithTreeOnlyHaveRightChild,
    TreeIsomorphicWithTreeHaveRandomData,
    TreeIsomorphicWithTreeHaveDifferentData,
    TreeIsomorphicWithTreeHaveOneDifferentElement,
    TreeIsomorphicWithTreeHaveOneSameElement,
    TreeIsomorphicWithTreeOnlyHaveLeftChild,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbers,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbersSwitchOver,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbers1,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbers1SwitchOver,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbers2,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbers2SwitchOver,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbersOneTreeHaveMoreData,
    TreeNotIsomorphicWithTreeHaveDifferentDataNumbersOneTreeHaveMoreDataSwitchOver,
    TreeIsomorphicWithTreeBecauseTreeAreTheSame,
    TreeIsomorphicWithTreeBothNull,
//    RebuildTreeCorrect,
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Stack.h"
#include "Tree.h"

#define Max_Expr 30    //max size of expression 

Tree EvalPostfix( char *expr );

Tree CreateExpressionTree(void)
{
    Tree tree = NULL;
    char expr[Max_Expr];
    gets(expr);
    printf("%s\n", expr);
    tree = EvalPostfix( expr );
    if (tree == NULL) {
        printf("Expression Tree is empty\n");
        return NULL;
    }
    PrintTreeInfixOrder(tree);
    printf("\n");
    PrintTreePostfixOrder(tree);
    printf("\n");
    return tree;
}

int main()
{
    Tree tree1, tree2;
    int result = false;
    printf("Use to test if 2 trees are Isomorphic\n");

    tree1 = CreateExpressionTree();
    tree2 = CreateExpressionTree();

    result = Isomorphic(tree1, tree2);
    printf("result: %d\n", result);

    FreeTree(tree1);
    FreeTree(tree2);
    return 0;
}


Stack S = NULL;

enum {
    NORMAL,
    ERROR,
    NONE
};

int status = NORMAL;

bool IsOperator(char c)
{
    if( (c == '+') ||(c == '-')|| (c == '*') || (c == '/'))
        return true;
    return false;
}

Tree DoOperation(char c)
{
   Tree tree1, tree2;
   Tree tree = NULL;
   if(IsEmpty(S) == true) {
       status = ERROR;
       return NULL;
   }
   tree1 = TopAndPop(S);
   if(IsEmpty(S) == true) {
       status = ERROR;
       return NULL;
   }

   tree2 = TopAndPop(S);
   if((c == '+')||(c=='-')||(c=='*')||(c=='/')) {
       tree = CreateTreeNode(c);
       tree->Left = tree2;
       tree->Right = tree1;
       Push(tree, S);
       return tree;
   }
   return NULL;
}

void OutputResult(S)
{
    return;
}

Tree EvalPostfix( char *expr )
{
    double v;
    Tree tree;
    int len = strlen(expr);
    int i=0;
    S = CreateStack();

    sscanf(expr, "%lf", &v);
    Push(CreateTreeNode(v), S);
    while((expr[i] != ' ')  && (expr[i] != '\n')) i++;
    while((i < len)&&(status == NORMAL)) {
        while(expr[i] != ' ') i++;
        i++;
        if((IsOperator((char)(expr[i]))==true) && ((expr[i+1] == ' ')||(i+1 == len))) {
            tree = DoOperation(expr[i]);
            i++;
            continue;
        }

        sscanf(expr+i, "%lf", &v);
        Push(CreateTreeNode(v), S);
        i++;
    }
    if(status == NORMAL) {
        tree = TopAndPop(S);
        if(IsEmpty(S) == false) {
            printf("Stack is not empty!\n");
            //value = Infinity;
        }
    }
    DisposeStack(S);
    return tree;
}

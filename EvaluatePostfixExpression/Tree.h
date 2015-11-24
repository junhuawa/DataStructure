#ifndef __TREE_H__
#define __TREE_H__

#include "common.h"

typedef double TreeElemType;

struct TreeNode {
    TreeElemType Element;
    Tree  Left;
    Tree  Right;
};

Tree CreateTreeNode(double v);
Tree BuildTree(); /* details omitted */
void FreeTree(Tree tree);
void PrintTreeInfixOrder(Tree tree);
void PrintTreePostfixOrder(Tree tree);
int Isomorphic( Tree T1, Tree T2 );


#endif

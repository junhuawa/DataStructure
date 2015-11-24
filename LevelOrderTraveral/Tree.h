#ifndef __TREE_H__
#define __TREE_H__

#include "common.h"

typedef int ElementType;

struct TreeNode {
    ElementType Element;
    Tree  Left;
    Tree  Right;
};

Tree BuildTree(); /* details omitted */
void FreeTree(Tree tree);


#endif

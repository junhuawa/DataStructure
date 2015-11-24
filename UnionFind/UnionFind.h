#ifndef __UNION_FIND_H__
#define __UNION_FIND_H__
#include <stdbool.h>

bool Combine(int a, int b);
int Find(int a);
void Union(int root1, int root2);
void Init(int num);
void Free(void);
int GetComponents(void);
#endif

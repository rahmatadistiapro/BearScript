#ifndef __GC_H
#define __GC_H

#include "AST.h"

typedef struct {
    ASTNode* nodes;
    int size;
    int capacity;
} GC;

void gc_free_ast(ASTNode* node);

#endif // __GC_H
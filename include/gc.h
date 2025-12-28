#ifndef GC_H
#define GC_H

#include "AST.h"

typedef struct {
    ASTNode* nodes;
    int size;
    int capacity;
} GC;

void gc_free_ast(ASTNode* node);

#endif // GC_H
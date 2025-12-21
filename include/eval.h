#ifndef EVAL_H
#define EVAL_H

#include "AST.h"
#include "symbol_table.h"

double eval(ASTNode* node, SymbolTable* table);

#endif
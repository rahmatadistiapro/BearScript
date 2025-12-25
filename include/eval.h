#ifndef EVAL_H
#define EVAL_H

#include "AST.h"
#include "symbol_table.h"
#include "Value.h"

Value eval(ASTNode* node, SymbolTable* table);

#endif
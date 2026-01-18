#ifndef __EVAL_H
#define __EVAL_H

#include "AST.h"
#include "symbol_table.h"
#include "Value.h"

static bool is_truthy(Value value);
Value eval(ASTNode* node, SymbolTable* table);

#endif // __EVAL_H
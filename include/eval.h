#ifndef EVAL_H
#define EVAL_H

#include "AST.h"
#include "symbol_table.h"
#include "Value.h"

static bool is_truthy(Value value);
Value eval(ASTNode* node, SymbolTable* table);

#endif // EVAL_H
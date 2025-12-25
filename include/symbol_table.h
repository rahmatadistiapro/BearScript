#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "D:/BearScript/include/Value.h"

typedef struct {
    char* name;
    Value value;  // Changed from double to Value
} Symbol;

typedef struct {
    Symbol* symbols;
    int size;
    int capacity;
} SymbolTable;

void init_table(SymbolTable* table);
void free_table(SymbolTable* table);

// UPDATED SIGNATURES:
void set_variable(SymbolTable* table, const char* name, Value value);  // Value, not double
bool get_variable(SymbolTable* table, const char* name, Value* out_value);  // Value*, not double*

#endif // SYMBOL_TABLE_H
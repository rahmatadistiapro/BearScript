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
    bool is_immutable;
} Symbol;

typedef struct {
    Symbol* symbols;
    int size;
    int capacity;
} SymbolTable;

void init_table(SymbolTable* table);
void free_table(SymbolTable* table);

// copy value:
Value copy_value(Value original);

// Lookup:
Symbol* symbol_lookup(SymbolTable* table, const char* name);

// UPDATED SIGNATURES:
bool define_variable(SymbolTable* table,
                     const char* name,
                     Value value,
                     bool is_immutable
);  // Value, not double
bool assign_variable(SymbolTable* table,
                     const char* name,
                     Value value
);  // Value, not double
bool get_variable(SymbolTable* table, const char* name, Value* out_value);  // Value*, not double*

#endif // SYMBOL_TABLE_H
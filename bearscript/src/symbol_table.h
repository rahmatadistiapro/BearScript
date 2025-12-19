#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

typedef struct {
    char* name;
    double value;
} Symbol;

typedef struct {
    Symbol* symbols;
    int size;
    int capacity;
} SymbolTable;

void init_table(SymbolTable* table);
void free_table(SymbolTable* table);
void set_variable(SymbolTable* table, const char* name, double value);
bool get_variable(SymbolTable* table, const char* name, double* out_value);

#endif // SYMBOL_TABLE_H
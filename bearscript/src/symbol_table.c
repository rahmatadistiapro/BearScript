#include "symbol_table.h"
#include <string.h>
#include <stdbool.h>

void init_table(SymbolTable* table) {
    table->size = 0;
    table->capacity = 10;
    table->symbols = malloc(sizeof(Symbol) * table->capacity);
}

void free_table(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        free(table->symbols[i].name);
    }
    free(table->symbols);
}

void set_variable(SymbolTable *table, const char *name, double value) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            table->symbols[i].value = value;
            return;
        }
    }

    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }

    table->symbols[table->size].name = _strdup(name);
    table->symbols[table->size].value = value;
    table->size++;
}

bool get_variable(SymbolTable *table, const char *name, double *out_value) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            *out_value = table->symbols[i].value;
            return true;
        }
    }
    return false;
}
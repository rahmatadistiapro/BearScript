#include "D:/BearScript/include/symbol_table.h"
#include "D:/BearScript/include/Value.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void init_table(SymbolTable* table) {
    table->size = 0;
    table->capacity = 10;
    table->symbols = malloc(sizeof(Symbol) * table->capacity);
}

void free_table(SymbolTable* table) {
    for (int i = 0; i < table->size; i++) {
        free(table->symbols[i].name);
        free_value(table->symbols[i].value);  // Used existing function!
    }
    free(table->symbols);
    // Optional: reset table state
    table->size = 0;
    table->capacity = 0;
    table->symbols = NULL;
}

Symbol* symbol_lookup(SymbolTable* table, const char* name) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

bool define_variable(SymbolTable *table, const char *name, Value value, bool is_immutable) {
    if (symbol_lookup(table, name)) {
        return false;
    }
    
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }

    table->symbols[table->size].name = _strdup(name);
    table->symbols[table->size].value = copy_value(value);  // DEEP COPY
    table->symbols[table->size].is_immutable = is_immutable;
    table->size++;
    return true;
}

bool assign_variable(SymbolTable* table, const char* name, Value value) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            if (table->symbols[i].is_immutable) {
                return false;
            }

            // Free old value's string if needed
            if (table->symbols[i].value.type == VALUE_STRING ||
                table->symbols[i].value.type == VALUE_ERROR) {
                free(table->symbols[i].value.as.string);
            }

            // DEEP COPY the new value
            table->symbols[i].value = copy_value(value);
            return true;
        }
    }

    // Variable not found → define as mutable
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }

    table->symbols[table->size].name = _strdup(name);
    table->symbols[table->size].value = copy_value(value);  // DEEP COPY
    table->symbols[table->size].is_immutable = false;
    table->size++;
    return true;
}

// UPDATED THIS: Change signature to match header
bool get_variable(SymbolTable* table, const char* name, Value* out_value) {  // Value*, not double*!
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            *out_value = table->symbols[i].value;
            return true;
        }
    }
    return false;
}
#include "D:/BearScript/include/symbol_table.h"
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
        // Free the Value if it contains a string
        if (table->symbols[i].value.type == VALUE_STRING || 
            table->symbols[i].value.type == VALUE_ERROR) {
            free(table->symbols[i].value.as.string);
        }
    }
    free(table->symbols);
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
        // Variable already exists
        return false;
    }
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(
            table->symbols,
            sizeof(Symbol) * table->capacity
        );
    }

    table->symbols[table->size].name = _strdup(name);
    table->symbols[table->size].value = value;
    table->symbols[table->size].is_immutable = is_immutable;
    table->size++;
    return true;
}

bool assign_variable(SymbolTable* table, const char* name, Value value) {
    // Check if variable already exists
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {

            // ❌ IMMUTABLE CHECK (main point of this function)
            if (table->symbols[i].is_immutable) {
                return false; // cannot reassign immutable variable
            }

            // Free old string if needed
            if (table->symbols[i].value.type == VALUE_STRING ||
                table->symbols[i].value.type == VALUE_ERROR) {
                free(table->symbols[i].value.as.string);
            }

            // Assign new value
            table->symbols[i].value = value;
            return true;
        }
    }

    // Variable not found → define as mutable
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(
            table->symbols,
            sizeof(Symbol) * table->capacity
        );
    }

    table->symbols[table->size].name = _strdup(name);
    table->symbols[table->size].value = value;
    table->symbols[table->size].is_immutable = false; // default mutable
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
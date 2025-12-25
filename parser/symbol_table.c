#include "D:/BearScript/include/symbol_table.h"
#include <string.h>
#include <stdlib.h>

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

// UPDATE THIS: Change signature to match header
void set_variable(SymbolTable* table, const char* name, Value value) {  // Value, not double!
    // Check if variable already exists
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            // Free old string if it was a string
            if (table->symbols[i].value.type == VALUE_STRING || 
                table->symbols[i].value.type == VALUE_ERROR) {
                free(table->symbols[i].value.as.string);
            }
            // Assign new value
            table->symbols[i].value = value;
            return;
        }
    }

    // Add new variable
    if (table->size >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }

    table->symbols[table->size].name = _strdup(name);
    table->symbols[table->size].value = value;
    table->size++;
}

// UPDATE THIS: Change signature to match header
bool get_variable(SymbolTable* table, const char* name, Value* out_value) {  // Value*, not double*!
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            *out_value = table->symbols[i].value;
            return true;
        }
    }
    return false;
}
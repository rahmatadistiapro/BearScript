#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"
#include "Token.h"
#include "symbol_table.h"

SymbolTable table;

int main(int argc, char* argv[]) {
    init_table(&table);
    const char* source = "x = 9";

    Lexer lexer;
    lexer_init(&lexer, source);

    Token token;
    while (1) {
        make_token(&lexer, &token);
        if (token.type == T_EOF) {break;}

        // Check for assignment pattern: IDENTIFIER = expression
        if (token.type == T_IDENTIFIER) {
            char* var_name = _strdup(token.value);
            free(token.value);

            make_token(&lexer, &token);
            if (token.type != T_ASSIGN) {
                fprintf(stderr, "Error: Expected '=' after variable %s\n", var_name);
                free(var_name);
                break;
            }
            free(token.value);

            make_token(&lexer, &token);
            if (token.type != T_INTEGER && token.type != T_FLOAT) {
                fprintf(stderr, "Error: Expected number after '='\n");
                free(var_name);
                free(token.value);
                break;
            }
            double value = atof(token.value);  // convert string to double
            free(token.value);

            // Store in symbol table
            set_variable(&table, var_name, value);
            free(var_name);
            continue;
        }
    }
    double result;
    if (get_variable(&table, "x", &result)) {
        printf("Value of x: %f\n", result);
    } else {
        printf("Variable x not found.\n");
    }
    
    // Other tokens (operators, etc.) can be handled here
    free_table(&table);
    return 0;
}
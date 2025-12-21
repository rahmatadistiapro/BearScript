#include "Lexer.h"
#include "parser.h"
#include "eval.h"
#include "symbol_table.h"
#include <stdio.h>

int main() {
    SymbolTable table;
    init_table(&table);

    while (1) {
        char input[1024];

        printf(">>> ");
        if (!fgets(input, sizeof(input), stdin))
            break;

        Lexer lexer;
        lexer_init(&lexer, input);

        Parser parser;
        parser_init(&parser, &lexer, &table);

        ASTNode* tree = parse_line(&parser);

        double result = eval(tree, &table);
        printf("AST Enum values:\n");
        printf("  AST_INTEGER = %d\n", AST_INTEGER);
        printf("  AST_FLOAT = %d\n", AST_FLOAT);
        printf("  AST_VARIABLE = %d\n", AST_VARIABLE);
        printf("  AST_ASSIGN = %d\n", AST_ASSIGN);
        printf("  AST_BINARY_OP = %d\n", AST_BINARY_OP);
        printf("%g\n", result);
    }
    free_table(&table);
    return 0;
}
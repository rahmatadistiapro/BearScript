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
        printf("%g\n", result);
    }
    free_table(&table);
    return 0;
}
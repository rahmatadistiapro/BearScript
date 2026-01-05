#include "D:/BearScript/include/Lexer.h"
#include "D:/BearScript/include/parser.h"
#include "D:/BearScript/include/eval.h"
#include "D:/BearScript/include/symbol_table.h"
#include "D:/BearScript/include/gc.h"
#include <stdio.h>
#include <string.h>

/*
 * Interpret a single line of BearScript
 * - No printing to stdout
 */
Value interpret(const char* line, SymbolTable* table) {
    printf( "[DEBUG] Input: %s\n", line);

    Lexer lexer;
    lexer_init(&lexer, line);
    printf("[DEBUG] Lexer initialized\n");

    Parser parser;
    parser_init(&parser, &lexer, table);
    printf("[DEBUG] Parser initialized\n");
    ASTNode* tree = parse_line(&parser);
    printf("[DEBUG] AST parsed\n");

    Value result = eval(tree, table);
    printf("[DEBUG] Evaluation complete\n");

    // Copy result because AST will be freed
    Value result_copy = copy_value(result);

    gc_free_ast(tree);
    free_value(result);

    return result_copy;
}

int main(int argc, char *argv[]) {
    SymbolTable table;
    init_table(&table);

    printf( "[DEBUG] BearScript started\n");
    printf( "[DEBUG] argc = %d\n", argc);

    if (argc == 1) {
        // =========================
        // REPL MODE
        // =========================
        printf("[DEBUG] Entering REPL mode\n");

        while (1) {
            char input[1024];
            printf(">>> ");

            if (!fgets(input, sizeof(input), stdin)) {
                printf("[DEBUG] EOF received, exiting REPL\n");
                printf("\n");
                break;
            }

            Value result = interpret(input, &table);

            // REPL prints result immediately (intended behavior)
            print_value(result);
            printf("\n");

            free_value(result);
        }
    }
    else if (argc == 2) {
        // =========================
        // FILE MODE (SINGLE OUTPUT)
        // =========================
        printf("[DEBUG] File mode\n");
        printf("[DEBUG] File argument: %s\n", argv[1]);

        char* extension = strrchr(argv[1], '.');
        if (!extension || strcmp(extension, ".bearscript") != 0) {
            printf("[ERROR] Invalid file extension\n");
            printf("[ERROR] Expected .bearscript\n");
            free_table(&table);
            return 1;
        }

        FILE* file = fopen(argv[1], "r");
        if (!file) {
            printf("[ERROR] Cannot open file: %s\n", argv[1]);
            free_table(&table);
            return 1;
        }

        char line[1024];
        int line_number = 0;

        Value last_result = nil_value();

        while (fgets(line, sizeof(line), file)) {
            line_number++;

            // Remove newline
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }

            // Skip empty lines
            int empty = 1;
            for (int i = 0; line[i]; i++) {
                if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r') {
                    empty = 0;
                    break;
                }
            }
            if (empty) {
                printf("[DEBUG] Skipping empty line %d\n", line_number);
                continue;
            }

            printf("[DEBUG] Executing line %d\n", line_number);
            free_value(last_result);
            last_result = interpret(line, &table);
        }

        fclose(file);

        printf("[DEBUG] File execution complete\n");

        // ========= SINGLE PROGRAM OUTPUT =========
        print_value(last_result);
        printf("\n");

        free_value(last_result);
    }
    else {
        printf("[ERROR] Invalid arguments\n");
        printf("Usage:\n");
        printf("  bearscript\n");
        printf("  bearscript program.bearscript\n");
    }

    free_table(&table);
    printf("[DEBUG] BearScript shutdown\n");
    return 0;
}

#include "D:/BearScript/include/Lexer.h"
#include "D:/BearScript/include/parser.h"
#include "D:/BearScript/include/eval.h"
#include "D:/BearScript/include/symbol_table.h"
#include <stdio.h>
#include <string.h>

// Helper function to process one line of BearScript code
Value process_line(const char* line, SymbolTable* table) {
    Lexer lexer;
    lexer_init(&lexer, line);
    
    Parser parser;
    parser_init(&parser, &lexer, table);
    
    ASTNode* tree = parse_line(&parser);
    Value result = eval(tree, table);

    return result;
}

int main(int argc, char *argv[]) {
    SymbolTable table;
    init_table(&table);
    
    // TEST: Print arguments
    printf("Number of arguments: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }
    
    if (argc == 1) {
        // REPL MODE
        while (1) {
            char input[1024];
            printf(">>> ");
            if (!fgets(input, sizeof(input), stdin))
            {
                printf("\n");
                break;
            }
            
            Value result = process_line(input, &table);
            print_value(result);
            printf("\n");
        }
    } 
    else if (argc == 2) {
        printf("DEBUG: Processing file: %s\n", argv[1]);
    
        // Check for .bearscript extension
        char* extension = strrchr(argv[1], '.');
        if (extension == NULL || strcmp(extension, ".bearscript") != 0) {
            printf("Error: BearScript files must have .bearscript extension\n");
            printf("Example: program.bearscript\n");
            free_table(&table);
            return 1;
        }

        FILE* file = fopen(argv[1], "r");
        printf("DEBUG: file: %p\n", file);
        if (file == NULL) {
            printf("Error: Cannot open file '%s'\n", argv[1]);
            free_table(&table);
            return 1;
        }
    
        printf("=== Running BearScript file ===\n");
    
        char line[1024];
        int line_number = 0;
        
        while (fgets(line, sizeof(line), file)) {
            line_number++;
            
            // Remove newline character
            size_t len = strlen(line);
            if (len > 0 && line[len-1] == '\n') {
                line[len-1] = '\0';
            }
            
            // Skip empty lines
            int is_empty = 1;
            for (int i = 0; line[i]; i++) {
                if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r') {
                    is_empty = 0;
                    break;
                }
            }
            
            if (is_empty) {
                continue;  // Skip empty lines
            }
            
            // Process the line
            printf("[Line %d] ", line_number);
            Value result = process_line(line, &table);
            print_value(result);
            printf("\n");
        }
        
        fclose(file);
        printf("=== File execution complete ===\n");
        return 0;
    }
    else {
        printf("Usage: bearscript [%s.bearscript]\n", argv[1]);
        printf("       bearscript                   (starts REPL)\n");
        printf("       bearscript program.bearscript (runs file)\n");
    }
    
    free_table(&table);
    return 0;
}
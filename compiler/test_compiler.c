// test_compiler.c
#include "D:\BearScript\include\Parser.h"
#include "D:\BearScript\include\Compiler.h"
#include "D:\BearScript\include\gc.h"
#include "D:\BearScript\include\VM.h"
#include <stdio.h>

int main() {
    // Use your parser to create AST
    const char* source = "x = 7";
    
    Lexer lexer;
    lexer_init(&lexer, source);
    
    SymbolTable table;
    init_table(&table);
    
    Parser parser;
    parser_init(&parser, &lexer, &table);
    
    ASTNode* ast = parse_line(&parser);
    
    if (ast) {
        // Compile it
        BearCodeChunk* chunk = compile_ast_to_bearcode(ast);
        printf("chunk: %p\n", chunk);

        if (chunk) {
            printf("Compilation successful!\n");
            printf("Generated %zu bytecode instructions\n", chunk->count);
            
            // Print the bytecode
            debug_disassemble_chunk(chunk);
        } else {
            printf("Compilation failed!\n");
        }
        int result = BVM(chunk, &table);
        printf("VM Result: %d\n", result);
        free_bearcode_chunk(chunk);
        gc_free_ast(ast);
        return result;
    }
    else {
        printf("Parsing failed!\n");
    }
}
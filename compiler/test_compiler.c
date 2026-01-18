// test_compiler.c
#include "D:\BearScript\include\Parser.h"
#include "D:\BearScript\include\Compiler.h"
#include "D:\BearScript\include\gc.h"
#include <stdio.h>

// Add to compiler.c
void debug_disassemble_chunk(BearCodeChunk* chunk) {
    printf("=== DISASSEMBLY ===\n");
    printf("Instructions: %zu\n", chunk->count);
    printf("Constants: %zu\n", chunk->const_count);
    
    for (size_t i = 0; i < chunk->const_count; i++) {
        printf("  Constant #%zu = %f\n", i, chunk->constants[i]);
    }
    
    for (size_t i = 0; i < chunk->count; i++) {
        Instruction instr = chunk->code[i];
        const char* opname = "UNKNOWN";
        
        switch (instr.bearcode) {
            case BC_LOADC: opname = "LOADC"; break;
            case BC_ADD: opname = "ADD"; break;
            case BC_SUB: opname = "SUB"; break;
            case BC_MAUL: opname = "MAUL"; break;
            case BC_DIV: opname = "DIV"; break;
            case BC_PRINT: opname = "PRINT"; break;
            case BC_MOV: opname = "MOV"; break;
            case BC_HALT: opname = "HALT"; break;
        }
        
        printf("  [%zu] %s r%d, ", i, opname, instr.reg1);
        
        if (instr.bearcode == BC_LOADC) {
            printf("#%d (value=%f)", instr.reg2, 
                   instr.reg2 < chunk->const_count ? chunk->constants[instr.reg2] : 0.0);
        } else if (instr.bearcode == BC_ADD || instr.bearcode == BC_SUB || 
                   instr.bearcode == BC_MAUL || instr.bearcode == BC_DIV) {
            printf("r%d, r%d", instr.reg2, instr.reg3);
        }
        
        printf("\n");
    }
}

int main() {
    // Use your parser to create AST
    const char* source = "x = 5 + 3\ngrowl(x)\n";
    
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
        
        if (chunk) {
            printf("Compilation successful!\n");
            printf("Generated %zu bytecode instructions\n", chunk->count);
            
            // Print the bytecode
            debug_disassemble_chunk(chunk);
            
            free_bearcode_chunk(chunk);
        } else {
            printf("Compilation failed!\n");
        }
        
        gc_free_ast(ast);
    }
    return 0;
}
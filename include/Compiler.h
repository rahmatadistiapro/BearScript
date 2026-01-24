// compiler.h
#ifndef __BEARCOMPILER_H
#define __BEARCOMPILER_H

#include <stdint.h>
#include <stdbool.h>
#include "bearcode.h"
#include "AST.h"
#include "Token.h"

#define __MAX_REGISTERS 254

typedef struct {
    BearCodeChunk* chunk;
    uint8_t next_reg;      // Next available register
    
    // Symbol table (simple for now)
    struct {
        char** names;
        uint8_t* regs;     // Which register holds the value
        size_t count;
        size_t capacity;
    } symbols;
    
    // Control flow (for loops, etc.)
    struct {
        size_t start;      // Loop start
        size_t end;        // Loop end (for break)
        size_t* breaks;    // Break addresses to patch
        size_t break_count;
    } loop;
    
    bool had_error;
    bool regs_in_use[256]; // Track which registers are used
} BearCompiler;

// Function prototypes
void init_compiler(BearCompiler* compiler);
void free_compiler(BearCompiler* compiler);
BearCodeChunk* compile_ast_to_bearcode(ASTNode* ast);

// Compilation functions
void compile_node(BearCompiler* compiler, ASTNode* node);
uint8_t compile_expression(BearCompiler* compiler, ASTNode* expr);
void compile_statements(BearCompiler* compiler, ASTNode** statements, int count);

// Specific AST node compilers
void compile_growl(BearCompiler* compiler, ASTNode* node);
void compile_if(BearCompiler* compiler, ASTNode* node);
void compile_elif(BearCompiler* compiler, ASTNode* node);
void compile_else(BearCompiler* compiler, ASTNode* node);
void compile_assignment(BearCompiler* compiler, ASTNode* node);
void debug_disassemble_chunk(BearCodeChunk* chunk);

// Helper functions
uint8_t allocate_reg(BearCompiler* compiler);
void free_reg(BearCompiler* compiler, uint8_t reg);
int add_symbol(BearCompiler* compiler, const char* name, uint8_t reg);
int find_symbol(BearCompiler* compiler, const char* name);

#endif // __BEARCOMPILER_H
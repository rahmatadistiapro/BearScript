#ifndef BEARCOMPILER_H
#define BEARCOMPILER_H
#define MAX_REGISTERS 254

#include <stdint.h>
#include "bearcode.h"
#include "ast.h"
#include "Token.h"

typedef struct {
    uint8_t registers[MAX_REGISTERS];
    uint8_t next_free;
    uint8_t counts;
} RegisterPool;
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
    
    // Control flow
    struct {
        size_t start;      // Loop start
        size_t end;        // Loop end (for break)
        size_t* breaks;    // Break addresses to patch
        size_t break_count;
    } loop;
    
    bool had_error;
} BearCompiler;

// Main API
BearCodeChunk* compile_ast_to_bearcode(ASTNode* ast);
void compile_node(BearCompiler* compiler, ASTNode* node);
uint8_t compile_expression(BearCompiler* compiler, ASTNode* expr);
void compile_growl(BearCompiler* compiler, ASTNode* node);
void compile_if(BearCompiler* compiler, ASTNode* node);
void compile_elif(BearCompiler* compiler, ASTNode* node);
void compile_else(BearCompiler* compiler, ASTNode* node);

#endif
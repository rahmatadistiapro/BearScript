#include "D:/BearScript/include/Compiler.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Simple register allocator
static uint8_t allocate_reg(BearCompiler* compiler) {
    if (compiler->next_reg >= 254) { // Leave 255 for special uses
        fprintf(stderr, "Error: Register overflow!\n");
        compiler->had_error = true;
        return 255;
    }
    return compiler->next_reg++;
}

static void free_reg(BearCompiler* compiler, uint8_t reg) {
    if (reg != 255 && reg + 1 == compiler->next_reg) {
        compiler->next_reg--;
    }
}

// Symbol table helpers
static int find_symbol(BearCompiler* compiler, const char* name) {
    for (size_t i = 0; i < compiler->symbols.count; i++) {
        if (strcmp(compiler->symbols.names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static int add_symbol(BearCompiler* compiler, const char* name, uint8_t reg) {
    int idx = find_symbol(compiler, name);
    if (idx >= 0) {
        // Update existing symbol
        compiler->symbols.regs[idx] = reg;
        return idx;
    }
    
    // Add new symbol
    if (compiler->symbols.count >= compiler->symbols.capacity) {
        compiler->symbols.capacity *= 2;
        compiler->symbols.names = realloc(compiler->symbols.names, 
                                         sizeof(char*) * compiler->symbols.capacity);
        compiler->symbols.regs = realloc(compiler->symbols.regs,
                                        sizeof(uint8_t) * compiler->symbols.capacity);
    }
    
    compiler->symbols.names[compiler->symbols.count] = strdup(name);
    compiler->symbols.regs[compiler->symbols.count] = reg;
    return compiler->symbols.count++;
}

// Expression compilation
uint8_t compile_expression(BearCompiler* compiler, ASTNode* expr) {
    if (!expr || compiler->had_error) return 255;
    
    switch (expr->type) {
        case AST_INTEGER: {
            uint8_t reg = allocate_reg(compiler);
            // Convert long to double for constant pool
            double value = (double)expr->data.value.int_val;
            int const_idx = add_constant(compiler->chunk, value);
            write_instruction(compiler->chunk, BC_LOADC, reg, const_idx, 0, 0);
            return reg;
        }
        
        case AST_FLOAT: {
            uint8_t reg = allocate_reg(compiler);
            int const_idx = add_constant(compiler->chunk, expr->data.value.float_val);
            write_instruction(compiler->chunk, BC_LOADC, reg, const_idx, 0, 0);
            return reg;
        }
        
        case AST_STRING: {
            uint8_t reg = allocate_reg(compiler);
            // For strings, we need string constants - for now use globals
            int global_idx = add_global(compiler->chunk, expr->data.string.str_val);
            write_instruction(compiler->chunk, BC_LOAD_GLOBAL, reg, global_idx, 0, 0);
            return reg;
        }
        
        case AST_BOOLEAN: {
            uint8_t reg = allocate_reg(compiler);
            double value = expr->data.value.int_val ? 1.0 : 0.0;
            int const_idx = add_constant(compiler->chunk, value);
            write_instruction(compiler->chunk, BC_LOADC, reg, const_idx, 0, 0);
            return reg;
        }

        case AST_GROWL_STATEMENT: {
            uint8_t arg_reg = compile_expression(compiler, expr->data.growl_stmt.expression);
            if (compiler->had_error) return 255;

            uint8_t reg = allocate_reg(compiler);
            write_instruction(compiler->chunk, BC_PRINT, reg, arg_reg, 0, 0);
            
            free_reg(compiler, arg_reg);
            return reg;
        }
        
        case AST_VARIABLE: {
            // Look up variable in symbol table
            int sym_idx = find_symbol(compiler, expr->data.variable.var_name);
            if (sym_idx >= 0) {
                // Variable exists - load from its register
                uint8_t target_reg = allocate_reg(compiler);
                uint8_t source_reg = compiler->symbols.regs[sym_idx];
                write_instruction(compiler->chunk, BC_MOV, target_reg, source_reg, 0, 0);
                return target_reg;
            } else {
                // Undefined variable - treat as global
                uint8_t reg = allocate_reg(compiler);
                int global_idx = add_global(compiler->chunk, expr->data.variable.var_name);
                write_instruction(compiler->chunk, BC_LOAD_GLOBAL, reg, global_idx, 0, 0);
                return reg;
            }
        }

        case AST_ASSIGN: {
            // Evaluate the value on the right side of '='
            uint8_t val_reg = compile_expression(compiler, expr->data.assign.value);
            if (compiler->had_error) return 255;

            // Map the variable name to this register in your symbol table
            add_symbol(compiler, expr->data.assign.var_name, val_reg);

            return val_reg;
        }

        case AST_IMMUTABLE_ASSIGN: {
            // Evaluate the value on the right side of 'let'
            uint8_t val_reg = compile_expression(compiler, expr->data.immutable_assign.value);
            if (compiler->had_error) return 255;

            // Map the variable name to this register in your symbol table
            add_symbol(compiler, expr->data.immutable_assign.var_name, val_reg);

            return val_reg;
        }
        
        case AST_BINARY_OP: {
            uint8_t left_reg = compile_expression(compiler, expr->data.binary_op.left);
            if (compiler->had_error) return 255;
            
            uint8_t right_reg = compile_expression(compiler, expr->data.binary_op.right);
            if (compiler->had_error) {
                free_reg(compiler, left_reg);
                return 255;
            }
            
            uint8_t result_reg = allocate_reg(compiler);
            
            // Map TokenType to BearCode
            switch (expr->data.binary_op.op) {
                case T_ADD:
                    write_instruction(compiler->chunk, BC_ADD, result_reg, left_reg, right_reg, 0);
                    break;
                case T_SUBTRACT:
                    write_instruction(compiler->chunk, BC_SUB, result_reg, left_reg, right_reg, 0);
                    break;
                case T_MAUL:
                    write_instruction(compiler->chunk, BC_MAUL, result_reg, left_reg, right_reg, 0);
                    break;
                case T_DIVIDE:
                    write_instruction(compiler->chunk, BC_DIV, result_reg, left_reg, right_reg, 0);
                    break;
                case T_MODULO:
                    write_instruction(compiler->chunk, BC_MOD, result_reg, left_reg, right_reg, 0);
                    break;
                default:
                    fprintf(stderr, "Unknown binary operator: %d\n", expr->data.binary_op.op);
                    compiler->had_error = true;
                    return 255;
            }
            
            free_reg(compiler, left_reg);
            free_reg(compiler, right_reg);
            return result_reg;
        }
        
        case AST_COMPARE_OP: {
            uint8_t left_reg = compile_expression(compiler, expr->data.compare_op.left);
            if (compiler->had_error) return 255;
            
            uint8_t right_reg = compile_expression(compiler, expr->data.compare_op.right);
            if (compiler->had_error) {
                free_reg(compiler, left_reg);
                return 255;
            }
            
            uint8_t result_reg = allocate_reg(compiler);
            
            // Map TokenType to comparison BearCode
            switch (expr->data.compare_op.op) {
                case T_EQUAL:
                    write_instruction(compiler->chunk, BC_EQ, result_reg, left_reg, right_reg, 0);
                    break;
                case T_NOTEQ:
                    write_instruction(compiler->chunk, BC_NE, result_reg, left_reg, right_reg, 0);
                    break;
                case T_LT:
                    write_instruction(compiler->chunk, BC_LT, result_reg, left_reg, right_reg, 0);
                    break;
                case T_LTOREQ:
                    write_instruction(compiler->chunk, BC_LE, result_reg, left_reg, right_reg, 0);
                    break;
                case T_GT:
                    write_instruction(compiler->chunk, BC_GT, result_reg, left_reg, right_reg, 0);
                    break;
                case T_GTOREQ:
                    write_instruction(compiler->chunk, BC_GE, result_reg, left_reg, right_reg, 0);
                    break;
                default:
                    fprintf(stderr, "Unknown comparison operator: %d\n", expr->data.compare_op.op);
                    compiler->had_error = true;
                    return 255;
            }
            
            free_reg(compiler, left_reg);
            free_reg(compiler, right_reg);
            return result_reg;
        }
        
        default:
            fprintf(stderr, "Unsupported expression type: %d\n", expr->type);
            compiler->had_error = true;
            return 255;
    }
}

// Statement compilation
void compile_growl(BearCompiler* compiler, ASTNode* node) {
    if (!node || node->type != AST_GROWL_STATEMENT) return;
    
    uint8_t expr_reg = compile_expression(compiler, node->data.growl_stmt.expression);
    if (compiler->had_error) return;

    write_instruction(compiler->chunk, BC_PRINT, expr_reg, 0, 0, 0);
}

void compile_assign(BearCompiler* compiler, ASTNode* node) {
    if (!node || (node->type != AST_ASSIGN && 
                  node->type != AST_TYPED_ASSIGN && 
                  node->type != AST_IMMUTABLE_ASSIGN)) {
        return;
    }
    
    uint8_t value_reg;
    char* var_name;
    
    switch (node->type) {
        case AST_ASSIGN:
            value_reg = compile_expression(compiler, node->data.assign.value);
            var_name = node->data.assign.var_name;
            break;
        case AST_TYPED_ASSIGN:
            value_reg = compile_expression(compiler, node->data.typed_assign.value);
            var_name = node->data.typed_assign.var_name;
            // Note: type_name is ignored for now
            break;
        case AST_IMMUTABLE_ASSIGN:
            value_reg = compile_expression(compiler, node->data.immutable_assign.value);
            var_name = node->data.immutable_assign.var_name;
            // Note: immutable flag ignored for now
            break;
        default:
            return;
    }
    
    if (compiler->had_error) return;
    
    // Store in symbol table (local)
    add_symbol(compiler, var_name, value_reg);
    
    // Also store as global for persistence
    int global_idx = add_global(compiler->chunk, var_name);
    write_instruction(compiler->chunk, BC_STORE_CONST, value_reg, global_idx, 0, 0);
    
    free_reg(compiler, value_reg);
}

void compile_if(BearCompiler* compiler, ASTNode* node) {
    if (!node || node->type != AST_IF_STATEMENT) return;
    
    // Compile condition
    uint8_t cond_reg = compile_expression(compiler, node->data.if_stmt.condition);
    if (compiler->had_error) return;
    
    // Jump if false (offset to be patched)
    size_t jump_if_false_addr = compiler->chunk->count;
    write_instruction(compiler->chunk, BC_JMP_IFNOT, cond_reg, 0, 0, 0);
    free_reg(compiler, cond_reg);
    
    // Compile then statements
    for (int i = 0; i < node->data.if_stmt.then_count; i++) {
        compile_node(compiler, node->data.if_stmt.then_statement[i]);
        if (compiler->had_error) return;
    }
    
    // Jump over else/elif (if exists)
    size_t jump_over_addr = compiler->chunk->count;
    write_instruction(compiler->chunk, BC_JMP, 0, 0, 0, 0);
    
    // Patch first jump
    int16_t else_offset = compiler->chunk->count - jump_if_false_addr;
    compiler->chunk->code[jump_if_false_addr].immediate = else_offset;
    
    // Compile elif chain
    if (node->data.if_stmt.elif_branch) {
        compile_elif(compiler, node->data.if_stmt.elif_branch);
        if (compiler->had_error) return;
    }
    
    // Compile else
    if (node->data.if_stmt.else_branch) {
        compile_else(compiler, node->data.if_stmt.else_branch);
        if (compiler->had_error) return;
    }
    
    // Patch jump over else/elif
    int16_t end_offset = compiler->chunk->count - jump_over_addr;
    compiler->chunk->code[jump_over_addr].immediate = end_offset;
}

void compile_elif(BearCompiler* compiler, ASTNode* node) {
    if (!node || node->type != AST_ELIF_STATEMENT) return;
    
    // Compile condition
    uint8_t cond_reg = compile_expression(compiler, node->data.elif_stmt.condition);
    if (compiler->had_error) return;
    
    // Jump if false
    size_t jump_if_false_addr = compiler->chunk->count;
    write_instruction(compiler->chunk, BC_JMP_IFNOT, cond_reg, 0, 0, 0);
    free_reg(compiler, cond_reg);
    
    // Compile then statements
    for (int i = 0; i < node->data.elif_stmt.count; i++) {
        compile_node(compiler, node->data.elif_stmt.then_statements[i]);
        if (compiler->had_error) return;
    }
    
    // Jump over else/next elif
    size_t jump_over_addr = compiler->chunk->count;
    write_instruction(compiler->chunk, BC_JMP, 0, 0, 0, 0);
    
    // Patch first jump
    int16_t next_offset = compiler->chunk->count - jump_if_false_addr;
    compiler->chunk->code[jump_if_false_addr].immediate = next_offset;
    
    // Compile next elif or else
    if (node->data.elif_stmt.next_elif) {
        compile_elif(compiler, node->data.elif_stmt.next_elif);
    } else if (node->data.elif_stmt.else_branch) {
        compile_else(compiler, node->data.elif_stmt.else_branch);
    }
    
    // Patch jump over
    int16_t end_offset = compiler->chunk->count - jump_over_addr;
    compiler->chunk->code[jump_over_addr].immediate = end_offset;
}

void compile_else(BearCompiler* compiler, ASTNode* node) {
    if (!node || node->type != AST_ELSE_STATEMENT) return;
    
    for (int i = 0; i < node->data.else_stmt.count; i++) {
        compile_node(compiler, node->data.else_stmt.then_statements[i]);
        if (compiler->had_error) return;
    }
}

void compile_node(BearCompiler* compiler, ASTNode* node) {
    if (!node || compiler->had_error) return;
    
    switch (node->type) {
        case AST_GROWL_STATEMENT:
            compile_growl(compiler, node);
            break;
        case AST_ASSIGN:
        case AST_TYPED_ASSIGN:
        case AST_IMMUTABLE_ASSIGN:
            compile_assign(compiler, node);
            break;
        case AST_IF_STATEMENT:
            compile_if(compiler, node);
            break;
        case AST_ELIF_STATEMENT:
            compile_elif(compiler, node);
            break;
        case AST_ELSE_STATEMENT:
            compile_else(compiler, node);
            break;
        case AST_BINARY_OP:
        case AST_COMPARE_OP:
        case AST_INTEGER:
        case AST_FLOAT:
        case AST_STRING:
        case AST_BOOLEAN:
        case AST_VARIABLE: {
            // Expression as statement - compile and discard result
            uint8_t reg = compile_expression(compiler, node);
            if (!compiler->had_error && reg != 255) {
                write_instruction(compiler->chunk, BC_POP, reg, 0, 0, 0);
                free_reg(compiler, reg);
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown AST node type: %d\n", node->type);
            compiler->had_error = true;
            break;
    }
}

BearCodeChunk* compile_ast_to_bearcode(ASTNode* ast) {
    BearCompiler compiler = {
        .chunk = create_bearcode_chunk(),
        .next_reg = 0,
        .had_error = false
    };
    
    // Initialize symbol table
    compiler.symbols.capacity = 16;
    compiler.symbols.count = 0;
    compiler.symbols.names = malloc(sizeof(char*) * compiler.symbols.capacity);
    compiler.symbols.regs = malloc(sizeof(uint8_t) * compiler.symbols.capacity);
    
    // Initialize loop state
    compiler.loop.start = 0;
    compiler.loop.end = 0;
    compiler.loop.breaks = NULL;
    compiler.loop.break_count = 0;
    
    // Compile the AST
    compile_node(&compiler, ast);
    
    if (!compiler.had_error) {
        write_instruction(compiler.chunk, BC_HALT, 0, 0, 0, 0);
    }
    
    // Cleanup
    for (size_t i = 0; i < compiler.symbols.count; i++) {
        free(compiler.symbols.names[i]);
    }
    free(compiler.symbols.names);
    free(compiler.symbols.regs);
    free(compiler.loop.breaks);
    
    if (compiler.had_error) {
        free_bearcode_chunk(compiler.chunk);
        return NULL;
    }
    
    return compiler.chunk;
}
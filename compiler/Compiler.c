// Compiler.c
#include "D:\BearScript\include\Compiler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_compiler(BearCompiler* compiler) {
    compiler->chunk = create_bearcode_chunk();
    compiler->next_reg = 0;
    compiler->had_error = false;
    
    // Initialize symbol table
    compiler->symbols.capacity = 16;
    compiler->symbols.count = 0;
    compiler->symbols.names = malloc(compiler->symbols.capacity * sizeof(char*));
    compiler->symbols.regs = malloc(compiler->symbols.capacity * sizeof(uint8_t));
    
    // Initialize loop info
    compiler->loop.start = 0;
    compiler->loop.end = 0;
    compiler->loop.breaks = NULL;
    compiler->loop.break_count = 0;
    
    // Initialize register tracking
    memset(compiler->regs_in_use, 0, sizeof(compiler->regs_in_use));
}

void free_compiler(BearCompiler* compiler) {
    if (compiler->chunk) {
        free_bearcode_chunk(compiler->chunk);
        compiler->chunk = NULL;
    }
    
    // Free symbol table
    for (size_t i = 0; i < compiler->symbols.count; i++) {
        free(compiler->symbols.names[i]);
    }
    free(compiler->symbols.names);
    free(compiler->symbols.regs);
    
    // Free loop break array
    if (compiler->loop.breaks) {
        free(compiler->loop.breaks);
    }
}

uint8_t allocate_reg(BearCompiler* compiler) {
    // Simple linear allocation
    for (uint8_t i = 0; i < __MAX_REGISTERS; i++) {
        if (!compiler->regs_in_use[i]) {
            compiler->regs_in_use[i] = true;
            if (i >= compiler->next_reg) {
                compiler->next_reg = i + 1;
            }
            return i;
        }
    }
    
    fprintf(stderr, "Compiler error: Out of registers\n");
    compiler->had_error = true;
    return 255;
}

void free_reg(BearCompiler* compiler, uint8_t reg) {
    if (reg < __MAX_REGISTERS) {
        compiler->regs_in_use[reg] = false;
        if (reg < compiler->next_reg) {
            compiler->next_reg = reg;
        }
    }
}

int add_symbol(BearCompiler* compiler, const char* name, uint8_t reg) {
    // Check if symbol already exists
    for (size_t i = 0; i < compiler->symbols.count; i++) {
        if (strcmp(compiler->symbols.names[i], name) == 0) {
            // Update existing symbol
            compiler->symbols.regs[i] = reg;
            return i;
        }
    }
    
    // Add new symbol
    if (compiler->symbols.count >= compiler->symbols.capacity) {
        size_t new_capacity = compiler->symbols.capacity * 2;
        char** new_names = realloc(compiler->symbols.names, new_capacity * sizeof(char*));
        uint8_t* new_regs = realloc(compiler->symbols.regs, new_capacity * sizeof(uint8_t));
        
        if (!new_names || !new_regs) {
            fprintf(stderr, "Compiler error: Out of memory for symbol table\n");
            compiler->had_error = true;
            return -1;
        }
        
        compiler->symbols.names = new_names;
        compiler->symbols.regs = new_regs;
        compiler->symbols.capacity = new_capacity;
    }
    
    compiler->symbols.names[compiler->symbols.count] = strdup(name);
    compiler->symbols.regs[compiler->symbols.count] = reg;
    return compiler->symbols.count++;
}

int find_symbol(BearCompiler* compiler, const char* name) {
    for (size_t i = 0; i < compiler->symbols.count; i++) {
        if (strcmp(compiler->symbols.names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

uint8_t compile_expression(BearCompiler* compiler, ASTNode* expr) {
    if (!expr || compiler->had_error) return 255;
    
    switch (expr->type) {
        case AST_INTEGER: {
            uint8_t reg = allocate_reg(compiler);
            double value = (double)expr->data.value.int_val;
            int const_idx = add_constant(compiler->chunk, value);
            write_instruction(compiler->chunk, BC_LOADC, reg, 0, 0, const_idx);
            return reg;
        }
        
        case AST_FLOAT: {
            uint8_t reg = allocate_reg(compiler);
            int const_idx = add_constant(compiler->chunk, expr->data.value.float_val);
            write_instruction(compiler->chunk, BC_LOADC, reg, 0, 0, const_idx);
            return reg;
        }
        
        case AST_STRING: {
            uint8_t reg = allocate_reg(compiler);
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

        case AST_VARIABLE: {
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
        
        case AST_BINARY_OP: {
            uint8_t left_reg = compile_expression(compiler, expr->data.binary_op.left);
            if (compiler->had_error) return 255;
            
            uint8_t right_reg = compile_expression(compiler, expr->data.binary_op.right);
            if (compiler->had_error) {
                free_reg(compiler, left_reg);
                return 255;
            }
            
            uint8_t result_reg = allocate_reg(compiler);
            
            BearCode opcode;
            switch (expr->data.binary_op.op) {
                case T_ADD: opcode = BC_ADD; break;
                case T_SUBTRACT: opcode = BC_SUB; break;
                case T_MAUL: opcode = BC_MAUL; break;
                case T_DIVIDE: opcode = BC_DIV; break;
                case T_MODULO: opcode = BC_MOD; break;
                default:
                    fprintf(stderr, "Unknown binary operator: %d\n", expr->data.binary_op.op);
                    compiler->had_error = true;
                    free_reg(compiler, left_reg);
                    free_reg(compiler, right_reg);
                    return 255;
            }
            
            write_instruction(compiler->chunk, opcode, result_reg, left_reg, right_reg, 0);
            
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
            
            BearCode opcode;
            switch (expr->data.compare_op.op) {
                case T_EQUAL: opcode = BC_EQ; break;
                case T_NOTEQ: opcode = BC_NE; break;
                case T_LT: opcode = BC_LT; break;
                case T_LTOREQ: opcode = BC_LE; break;
                case T_GT: opcode = BC_GT; break;
                case T_GTOREQ: opcode = BC_GE; break;
                default:
                    fprintf(stderr, "Unknown comparison operator: %d\n", expr->data.compare_op.op);
                    compiler->had_error = true;
                    free_reg(compiler, left_reg);
                    free_reg(compiler, right_reg);
                    return 255;
            }
            
            write_instruction(compiler->chunk, opcode, result_reg, left_reg, right_reg, 0);
            
            free_reg(compiler, left_reg);
            free_reg(compiler, right_reg);
            return result_reg;
        }
        
        default:
            fprintf(stderr, "Unsupported expression type in compile_expression: %d\n", expr->type);
            compiler->had_error = true;
            return 255;
    }
}

void compile_growl(BearCompiler* compiler, ASTNode* node) {
    uint8_t expr_reg = compile_expression(compiler, node->data.growl_stmt.expression);
    if (compiler->had_error) return;
    
    write_instruction(compiler->chunk, BC_PRINT, expr_reg, 0, 0, 0);
    free_reg(compiler, expr_reg);
}

void compile_assignment(BearCompiler* compiler, ASTNode* node) {
    uint8_t value_reg = compile_expression(compiler, node->data.assign.value);
    if (compiler->had_error) return;
    
    add_symbol(compiler, node->data.assign.var_name, value_reg);
    // The register stays allocated - owned by the variable
}

void compile_node(BearCompiler* compiler, ASTNode* node) {
    if (!node || compiler->had_error) return;
    
    switch (node->type) {
        case AST_GROWL_STATEMENT:
            compile_growl(compiler, node);
            break;
            
        case AST_ASSIGN:
        case AST_IMMUTABLE_ASSIGN:
            compile_assignment(compiler, node);
            break;
            
        default: {
            // Try to compile as expression
            uint8_t reg = compile_expression(compiler, node);
            if (reg != 255 && !compiler->had_error) {
                // Expression was compiled successfully
                // For standalone expressions, we might want to discard the result
                free_reg(compiler, reg);
            }
        }
            break;
    }
}

void compile_statements(BearCompiler* compiler, ASTNode** statements, int count) {
    for (int i = 0; i < count; i++) {
        compile_node(compiler, statements[i]);
        if (compiler->had_error) break;
    }
}

BearCodeChunk* compile_ast_to_bearcode(ASTNode* ast) {
    BearCompiler compiler;
    init_compiler(&compiler);
    
    compile_node(&compiler, ast);
    
    if (compiler.had_error) {
        free_compiler(&compiler);
        return NULL;
    }
    
    // Add HALT instruction at the end
    write_instruction(compiler.chunk, BC_HALT, 2, 0, 0, 0);
    
    BearCodeChunk* chunk = compiler.chunk;
    // Don't free the chunk - it's being returned
    compiler.chunk = NULL;
    free_compiler(&compiler);
    
    return chunk;
}

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
            printf("#%d (value=%f)", instr.immediate,
                   instr.immediate < chunk->const_count ? chunk->constants[instr.immediate] : 0.0);
        } else if (instr.bearcode == BC_ADD || instr.bearcode == BC_SUB || 
                   instr.bearcode == BC_MAUL || instr.bearcode == BC_DIV) {
            printf("r%d, r%d", instr.reg2, instr.reg3);
        }
        
        printf("\n");
    }
}
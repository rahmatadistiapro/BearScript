// #include "D:\BearScript\include\eval.h"
// #include "D:\BearScript\include\Value.h"
// #include "D:\BearScript\include\Func.h"
#include "D:\BearScript\include\BearCode.h"
#include "D:\BearScript\include\symbol_table.h"
#include "D:\BearScript\include\VM.h"
#include <stdio.h>
// #include <string.h>

/*void growl(const char *message) {
    printf("%s\n", message);
}

static bool is_truthy(Value value) {
    if (is_nil(value)) {
        return false;
    }
    if (is_integer(value)) {
        return as_integer(value) != 0;
    }
    if (is_float(value)) {
        return as_float(value) != 0.0;
    }
    if (is_string(value)) {
        return strlen(as_string(value)) > 0;
    }
    return true; // Default to true for other types
}*/

double BVM(BearCodeChunk* chunk, SymbolTable* table) {
    if (!chunk || !chunk->code || chunk->count == 0) {
        printf("Error: Invalid Chunk.\n");
        return 0;
    }

    // Initialize registers
    double registers[__MAX_REGISTERS] = {0};
    int ip = 0;
    double last_result = 0;
    
    while (ip < chunk->count) {
        Instruction* current = &chunk->code[ip];
        
        switch (current->bearcode) {
            case BC_LOADC: {
                int const_index = (int)current->immediate;
                if (const_index < 0 || const_index >= chunk->const_count) {
                    printf("Error: Invalid constant index %d\n", const_index);
                    return 0;
                }
                __CHECK_REG(current->reg1);
                registers[current->reg1] = chunk->constants[const_index];
                last_result = registers[current->reg1];
                printf("LOADC r%d = %f\n", current->reg1, last_result);
                break;
            }
            case BC_LOAD_GLOBAL: {
                int global_index = (int)current->reg2; // Using reg2 for global index
                if (global_index < 0 || global_index >= chunk->global_count) {
                    printf("Error: Invalid global index %d\n", global_index);
                    return 0;
                }
                __CHECK_REG(current->reg1);
                registers[current->reg1] = 0; // Placeholder for global value
                last_result = registers[current->reg1];
                printf("LOAD_GLOBAL r%d = '%s' (placeholder value)\n", current->reg1, chunk->globals[global_index]);
                break;
            }
            case BC_ADD: {
                __CHECK_REG(current->reg1);
                __CHECK_REG(current->reg2);
                __CHECK_REG(current->reg3);
                double operand1 = registers[current->reg2];
                double operand2 = registers[current->reg3];
                double result = operand1 + operand2;
                registers[current->reg1] = result;
                last_result = result;
                printf("ADD r%d = r%d(%f) + r%d(%f) = %f\n", 
                       current->reg1, current->reg2, operand1, 
                       current->reg3, operand2, result);
                break;
            }
            case BC_SUB: {
                __CHECK_REG(current->reg1);
                __CHECK_REG(current->reg2);
                __CHECK_REG(current->reg3);
                double operand1 = registers[current->reg2];
                double operand2 = registers[current->reg3];
                double result = operand1 - operand2;
                registers[current->reg1] = result;
                last_result = result;
                break;
            }
            case BC_MAUL: {
                __CHECK_REG(current->reg1);
                __CHECK_REG(current->reg2);
                __CHECK_REG(current->reg3);
                double operand1 = registers[current->reg2];
                double operand2 = registers[current->reg3];
                double result = operand1 * operand2;
                registers[current->reg1] = result;
                last_result = result;
                break;
            }
            case BC_DIV: {
                __CHECK_REG(current->reg1);
                __CHECK_REG(current->reg2);
                __CHECK_REG(current->reg3);
                double operand1 = registers[current->reg2];
                double operand2 = registers[current->reg3];
                if (operand2 == 0) {
                    printf("Error: Division by zero\n");
                    return 0;
                }
                double result = operand1 / operand2;
                registers[current->reg1] = result;
                last_result = result;
                break;
            }
            case BC_MOD: {
                __CHECK_REG(current->reg1);
                __CHECK_REG(current->reg2);
                __CHECK_REG(current->reg3);
                double operand1 = registers[current->reg2];
                double operand2 = registers[current->reg3];
                if (operand2 == 0) {
                    printf("Error: Modulo by zero\n");
                    return 0;
                }
                int op1 = (int)operand1;
                int op2 = (int)operand2;
                double result = op1 % op2;
                registers[current->reg1] = result;
                last_result = result;
                break;
            }
            case BC_PRINT: {
                __CHECK_REG(current->reg1);
                printf("PRINT r%d = %f\n", current->reg1, registers[current->reg1]);
                double value = registers[current->reg1];
                printf("PRINT: %f\n", value);
                last_result = value;
                break;
            }
            case BC_MOV: {
                __CHECK_REG(current->reg1);
                __CHECK_REG(current->reg2);
                double value = registers[current->reg1];
                registers[current->reg2] = value;
                last_result = value;
                break;
            }
            case BC_POP: {
                __CHECK_REG(current->reg1);
                double value = registers[current->reg1];
                registers[current->reg1] = 0;
                last_result = value;
                break;
            }
            case BC_HALT: {
                printf("Program halted. returning register=[r%d=%f]\n", current->reg1, registers[current->reg1]);
                // Return value from specified register
                if (current->reg1 >= 0 && current->reg1 < __MAX_REGISTERS) {
                    return registers[current->reg1];
                }
                printf("Final Result: %f\n", last_result);
                return last_result;
            }
            default: {
                printf("Unknown BearCode instruction: 0x%X\n", current->bearcode);
                return 0;
            }
        }
        ip++;
    }
    return last_result;
}
#include "D:\BearScript\include\eval.h"
#include "D:\BearScript\include\Value.h"
#include "D:\BearScript\include\Func.h"
#include "D:\BearScript\include\BearCode.h"
#include "D:\BearScript\include\symbol_table.h"
#include "D:\BearScript\include\VM.h"
#include <complex.h>
#include <string.h>

void growl(const char *message) {
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
}

double BVM(BearCodeChunk* chunk, SymbolTable* table) {
    switch (chunk->code->bearcode) {
        case BC_LOADC: {
            int const_index = (int)chunk->code->immediate;
            __CHECK_REG(const_index);
            __CHECK_REG(chunk->code->reg1);
            double constant = chunk->constants[const_index];
            chunk->constants[chunk->code->reg1] = constant;
            return constant;
        }
        case BC_ADD: {
            __CHECK_REG(chunk->code->reg1);
            __CHECK_REG(chunk->code->reg2);
            __CHECK_REG(chunk->code->reg3);
            double operand1 = chunk->constants[chunk->code->reg2];
            double operand2 = chunk->constants[chunk->code->reg3];
            double result = operand1 + operand2;
            chunk->constants[chunk->code->reg1] = result;
            return result;
        }
        case BC_SUB: {
            __CHECK_REG(chunk->code->reg1);
            __CHECK_REG(chunk->code->reg2);
            __CHECK_REG(chunk->code->reg3);
            double operand1 = chunk->constants[chunk->code->reg2];
            double operand2 = chunk->constants[chunk->code->reg3];
            double result = operand1 - operand2;
            chunk->constants[chunk->code->reg1] = result;
            return result;
        }
        case BC_MAUL: {
            __CHECK_REG(chunk->code->reg1);
            __CHECK_REG(chunk->code->reg2);
            __CHECK_REG(chunk->code->reg3);
            double operand1 = chunk->constants[chunk->code->reg2];
            double operand2 = chunk->constants[chunk->code->reg3];
            double result = operand1 * operand2;
            chunk->constants[chunk->code->reg1] = result;
            return result;
        }
        case BC_DIV: {
            __CHECK_REG(chunk->code->reg1);
            __CHECK_REG(chunk->code->reg2);
            __CHECK_REG(chunk->code->reg3);
            double operand1 = chunk->constants[chunk->code->reg2];
            double operand2 = chunk->constants[chunk->code->reg3];
            if (operand2 == 0) {
                printf("Error: Division by zero\n");
                return 0;
            }
            double result = operand1 / operand2;
            chunk->constants[chunk->code->reg1] = result;
            return result;
        }
        case BC_MOD: {
            __CHECK_REG(chunk->code->reg1);
            __CHECK_REG(chunk->code->reg2);
            __CHECK_REG(chunk->code->reg3);
            double operand1 = chunk->constants[chunk->code->reg2];
            double operand2 = chunk->constants[chunk->code->reg3];
            if (operand2 == 0) {
                printf("Error: Modulo by zero\n");
                return 0;
            }
            double result = (int)operand1 % (int)operand2;
            chunk->constants[chunk->code->reg1] = result;
            return result;
        }
        case BC_PRINT: {
            __CHECK_REG(chunk->code->reg1);
            double value = chunk->constants[chunk->code->reg1];
            printf("%f\n", value);
            return value;
        }
        case BC_MOV: {
            double value = chunk->constants[chunk->code->reg1];
            chunk->constants[chunk->code->reg2] = value;
            return value;
        }
        case BC_POP: {
            double value = chunk->constants[chunk->code->reg1];
            chunk->constants[chunk->code->reg1] = 0; // Clear the value
            return value;
        }
        case BC_HALT: {
            printf("Program halted.\n");
            return 0;
            break;
        }
        default: {
            printf("Unknown BearCode instruction: 0x%X\n", chunk->code->bearcode);
            return 0;
        }
    }
}
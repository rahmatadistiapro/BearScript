#ifndef __VM_H
#define __VM_H

#include "Value.h"
#include "BearCode.h"
#include "Compiler.h"
#include "symbol_table.h"

#define __CHECK_REG(reg) \
            if ((reg) < 0 || (reg) >= __MAX_REGISTERS) { \
                printf("Error: Invalid register %d\n", reg); \
                return 0; \
            }

typedef struct {
    double registers[__MAX_REGISTERS];
    BearCodeChunk* chunk;
    SymbolTable* table;
    int ip;
} VM;
static bool is_truthy(Value value);
double BVM(BearCodeChunk* chunk, SymbolTable* table);

#endif
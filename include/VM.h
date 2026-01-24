#ifndef __VM_H
#define __VM_H
#define __CHECK_REG(reg) \
            if ((reg) < 0 || (reg) >= chunk->const_count) { \
                printf("Error: Invalid register %d\n", reg); \
                return 0; \
            }

#include "Value.h"
#include "BearCode.h"
#include "symbol_table.h"

static bool is_truthy(Value value);
double BVM(BearCodeChunk* chunk, SymbolTable* table);

#endif
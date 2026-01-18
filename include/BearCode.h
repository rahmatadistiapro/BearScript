#ifndef __BEARCODE_H
#define __BEARCODE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TYPE_STRING = 0X01,
    TYPE_INTEGER = 0X02,
    TYPE_FLOAT = 0X03,
    TYPE_BOOLEAN = 0X04,
    TYPE_MIXED = 0X05,
    TYPE_ANY = 0x06,
    TYPE_NOTHING = 0X07,
    TYPE_UNKNOWN = 0XFF
} TypeCode;

typedef enum {
    // load/store operations
    BC_LOADC = 0X01, // rX, const_index
    BC_LOAD_GLOBAL = 0X02, // rX, global_index
    BC_STORE_CONST = 0X03, // rX, const_index

    // built-in functions
    BC_PRINT = 0x04, // rX

    // arithmetic operations
    BC_ADD = 0x10, // rX, rY, rZ (general add)
    BC_ADD_INT = 0X11, // rX, rY, rZ (INTEGER)
    BC_ADD_FLOAT = 0X12, // rX, rY, rZ (FLOAT)
    BC_SUB = 0X13, // rX, rY, rZ (general subtract)
    BC_SUB_INT = 0X14, // rX, rY, rZ (INTEGER)
    BC_SUB_FLOAT = 0X15, // rX, rY, rZ (FLOAT)
    BC_MAUL = 0X16, // rX, rY, rZ (general multiply)
    BC_MAUL_INT = 0X17, // rX, rY, rZ (INTEGER)
    BC_MAUL_FLOAT = 0X18, // rX, rY, rZ (FLOAT)
    BC_DIV = 0X19, // rX, rY, rZ (general divide)
    BC_DIV_INT = 0X1A, // rX, rY, rZ (INTEGER)
    BC_DIV_FLOAT = 0X1B, // rX, rY, rZ (FLOAT)
    BC_MOD = 0X1C, // rX, rY, rZ (general modulo)
    BC_MOD_INT = 0X1D, // rX, rY, rZ (INTEGER)
    BC_MOD_FLOAT = 0X1E, // rX, rY, rZ (FLOAT)
    BC_NEG = 0X1F, // rX, rY

    // Type conversion
    BC_INT_TO_FLOAT = 0X20, // rX, rY (convert int to float)
    BC_FLOAT_TO_INT = 0X21, // rX, rY (convert float to int)

    // comparison operations
    BC_EQ = 0X22, // rX, rY, rZ (general equal)
    BC_EQ_INT = 0X23, // rX, rY, rZ (INTEGER)
    BC_EQ_FLOAT = 0X24, // rX, rY, rZ (FLOAT)
    BC_NE = 0X25, // rX, rY, rZ (general not equal)
    BC_NE_INT = 0X26, // rX, rY, rZ (INTEGER)
    BC_NE_FLOAT = 0X27, // rX, rY, rZ (FLOAT)
    BC_LT = 0X28, // rX, rY, rZ (general less than)
    BC_LT_INT = 0X29, // rX, rY, rZ (INTEGER)
    BC_LT_FLOAT = 0X2A, // rX, rY, rZ (FLOAT)
    BC_LE = 0X2B, // rX, rY, rZ (general less than or equal)
    BC_LE_INT = 0X2C, // rX, rY, rZ (INTEGER)
    BC_LE_FLOAT = 0X2D, // rX, rY, rZ (FLOAT)
    BC_GT = 0X2E, // rX, rY, rZ (general greater than)
    BC_GT_INT = 0X2F, // rX, rY, rZ (INTEGER)
    BC_GT_FLOAT = 0X30, // rX, rY, rZ (FLOAT)
    BC_GE = 0X31, // rX, rY, rZ (general greater than or equal)
    BC_GE_INT = 0X32, // rX, rY, rZ (INTEGER)
    BC_GE_FLOAT = 0X33, // rX, rY, rZ (FLOAT)

    // Type checking
    BC_CHECK_TYPE = 0X34, // rX, type_code (checks if value in rX matches type_code)
    BC_ASSERT_TYPE = 0X35, // rX, type_code (assert type, error if wrong)

    // control flow operations
    BC_JMP = 0X36, // jump_index
    BC_JMP_IF = 0X37, // rX, jump_index
    BC_JMP_IFNOT = 0X38, // rX, jump_index

    // function operations
    BC_CALL = 0X40, // function_index, arg_count
    BC_RET = 0X41, // rX

    // stack operations
    BC_MOV = 0X50, // rX, rY
    BC_POP = 0X51, // rX

    // other operations
    BC_HALT = 0XFF, // halt the program (stops execution)
} BearCode;

typedef struct {
    uint8_t bearcode;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;
    int16_t immediate;
} Instruction;

typedef struct {
    Instruction* code;
    size_t capacity;
    size_t count;

    // constant pool
    double* constants;
    size_t const_capacity;
    size_t const_count;

    // immutable constant pool
    struct {
        double value;
        TypeCode type;
    }* typed_constants;
    size_t typed_const_capacity;
    size_t typed_const_count;

    // global pool
    char** globals;
    size_t global_capacity;
    size_t global_count;

    // typed global pool
    struct {
        char* name;
        TypeCode type;
        bool immutable;
    }* typed_globals;
    size_t typed_global_capacity;
    size_t typed_global_count;
} BearCodeChunk;

BearCodeChunk* create_bearcode_chunk();
void free_bearcode_chunk(BearCodeChunk* chunk);
int add_constant(BearCodeChunk* chunk, double value);
int add_global(BearCodeChunk* chunk, const char* name);
void emit_instruction(BearCodeChunk* chunk, Instruction instr);
void write_instruction(BearCodeChunk* chunk, uint8_t bearcode, uint8_t reg1,
                        uint8_t reg2, uint8_t reg3, int16_t imm);

#endif  // __BEARCODE_H
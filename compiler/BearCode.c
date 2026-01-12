#include "D:/BearScript/include/BearCode.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

BearCodeChunk* create_bearcode_chunk() {
    BearCodeChunk* chunk = malloc(sizeof(BearCodeChunk));
    chunk->capacity = 256;
    chunk->count = 0;
    chunk->code = malloc(sizeof(Instruction) * chunk->capacity);

    chunk->const_capacity = 256;
    chunk->const_count = 0;
    chunk->constants = malloc(sizeof(double) * chunk->const_capacity);

    chunk->global_capacity = 256;
    chunk->global_count = 0;
    chunk->globals = malloc(sizeof(char*) * chunk->global_capacity);

    return chunk;
}

void free_bearcode_chunk(BearCodeChunk* chunk) {
    if (!chunk) return;
    free(chunk->code);
    free(chunk->constants);

    for (size_t i = 0; i < chunk->global_count; i++) {
        free(chunk->globals[i]);
    }
    free(chunk->globals);
    free(chunk);
}

int add_constant(BearCodeChunk *chunk, double value) {
    for ( size_t i = 0; i < chunk->const_count; i++) {
        if (chunk->constants[i] == value) {
            return i;
        }
    }

    if (chunk->const_count >= chunk->const_capacity) {
        chunk->const_capacity *= 2;
        chunk->constants = realloc(chunk->constants,
             sizeof(double) * chunk->const_capacity);
    }

    chunk->constants[chunk->const_count] = value;
    return chunk->const_count++;
}

int add_global(BearCodeChunk *chunk, const char* name) {
    for ( size_t i = 0; i < chunk->global_count; i++) {
        if (strcmp(chunk->globals[i], name) == 0) {
            return i;
        }
    }

    if (chunk->global_count >= chunk->global_capacity) {
        chunk->global_capacity *= 2;
        chunk->globals = realloc(chunk->globals,
                            sizeof(char*) * chunk->global_capacity);
    }

    chunk->globals[chunk->global_count] = strdup(name);
    return chunk->global_count++;
}

void emit_instruction(BearCodeChunk *chunk, Instruction instr) {
    if (chunk->count >= chunk->capacity) {
        chunk->capacity *= 2;
        chunk->code = realloc(chunk->code, sizeof(Instruction) * chunk->capacity);
    }
    chunk->code[chunk->count++] = instr;
}

void write_instruction(BearCodeChunk *chunk, uint8_t bearcode, uint8_t reg1,
                        uint8_t reg2, uint8_t reg3, int16_t imm) {
    Instruction instr = {bearcode, reg1, reg2, reg3, imm};
    emit_instruction(chunk, instr);
}
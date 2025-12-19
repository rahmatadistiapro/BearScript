#ifndef TOKEN_H
#define TOKEN_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef enum {
    T_GROWL,
    T_INTEGER,
    T_FLOAT,
    T_STRING,
    T_ADD,
    T_SUBTRACT,
    T_MUL,
    T_DIVIDE,
    T_MODULO,
    T_LPAREN,
    T_RPAREN,
    T_EQUAL,
    T_ASSIGN,
    T_UNKNOWN,
    T_EOF
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

#endif
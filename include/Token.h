#ifndef TOKEN_H
#define TOKEN_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef enum {
    T_GROWL, // growl value
    T_INTEGER, // 1, 2, 3, 4, 5, 6, 7, 8, 9, 0
    T_FLOAT, // 1.0, 2.5, 3.14
    T_STRING, // "hello", 'world'
    T_ADD, // +
    T_SUBTRACT, // -
    T_MAUL, // *
    T_DIVIDE, // /
    T_MODULO, // %
    T_LPAREN, // (
    T_RPAREN, // )
    T_ASSIGN, // =
    T_IF, // if
    T_ELIF, // elif
    T_ELSE, // else
    T_FOR, // for
    T_WHILE, // while
    T_TRUE, // TRUE
    T_FALSE, // FALSE
    T_EQUAL, // ==
    T_NOTEQ, // !=
    T_LT, // <
    T_LTOREQ, // <=
    T_GT, // >
    T_GTOREQ, // >=
    T_HONEY, // honey (to end control flow, loops, etc.)
    T_IDENTIFIER, // variable names
    T_LET, // let keyword for static variable declaration
    T_COLON, // :
    T_UNKNOWN, // unknown token
    T_EOF // end of file
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

#endif
#ifndef LEXER_H
#define LEXER_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Token.h"

typedef struct Lexer {
    const char* source;
    int position;
    char current_char;
    int line;
    int column;
} Lexer;

// Function to initialize a Lexer
void lexer_init(Lexer* lexer, const char* source);
void make_token(Lexer* lexer, Token* token);
void advance(Lexer* lexer);
void skip_whitespace(Lexer* lexer);

#endif // LEXER_H
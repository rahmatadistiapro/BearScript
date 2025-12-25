#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include "D:/BearScript/include/Token.h"
#include "D:/BearScript/include/Lexer.h"

static void lex_number(Lexer* lexer, Token* token);
static void lex_identifier(Lexer* lexer, Token* token);

static void lex_number(Lexer* lexer, Token* token) {
    int start_pos = lexer->position;
    int has_dot = 0;
    while (isdigit(lexer->current_char) || lexer->current_char == '.') {
        if (lexer->current_char == '.') {
            if (has_dot) {
                break;
            }
            has_dot = 1;
        }
        advance(lexer);
    }

    int length = lexer->position - start_pos;
    token->value = (char*)malloc(length + 1);
    memcpy(token->value, lexer->source + start_pos, length);
    token->value[length] = '\0';
    token->type = has_dot ? T_FLOAT : T_INTEGER;
}

static void lex_identifier(Lexer* lexer, Token* token) {
    int start_pos = lexer->position;
    while (isalnum(lexer->current_char) || lexer->current_char == '_') {
        advance(lexer);
    }
    int length = lexer->position - start_pos;
    token->value = (char*)malloc(length + 1);
    memcpy(token->value, lexer->source + start_pos, length);
    token->value[length] = '\0';

    // --- KEY CHANGE HERE ---
    if (strcmp(token->value, "growl") == 0) {
        token->type = T_GROWL;
    }
    else if (strcmp(token->value, "let") == 0) {
        token->type = T_LET;
    }
    else if (strcmp(token->value, "str") == 0) {
        token->type = T_STRING;
    }
    else if (strcmp(token->value, "int") == 0) {
        token->type = T_INTEGER;
    }
    else if (strcmp(token->value, "float") == 0) {
        token->type = T_FLOAT;
    }
    else {
        token->type = T_IDENTIFIER;
    }
}

static void lex_string(Lexer* lexer, Token* token) {
    char quote_char = lexer->current_char; // saves the quote type like is it " or '
    advance(lexer);

    int start_pos = lexer->position;
    while (lexer->current_char != quote_char && lexer->current_char != '\0') {
        advance(lexer);
    }

    if (lexer->current_char != quote_char) {
        printf("Error: Unterminated string literal\n");
        exit(EXIT_FAILURE);
    }

    int length = lexer->position - start_pos;
    token->value = (char*)malloc(length + 1);
    if (!token->value) {
        printf("Error: Memory allocation failed for string token\n");
        exit(EXIT_FAILURE);
    }
    memcpy(token->value, lexer->source + start_pos, length);
    token->value[length] = '\0';

    token->type = T_STRING;
    printf("DEBUG lex_string: String token value=%s\n", token->value);
    advance(lexer); // Skip closing quote
}

void lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->position = 0;
    lexer->current_char = source[0];
    lexer->line = 1;
    lexer->column = 1;
};

void advance(Lexer* lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 0;
    } else{
        lexer->column++;
    }
    lexer->position++;
    lexer->current_char = lexer->source[lexer->position];
};

void skip_whitespace(Lexer* lexer) {
    while (isspace(lexer->current_char)) {
        advance(lexer);
    }
};

void make_token(Lexer* lexer, Token* token) {
    printf("DEBUG make_token: Starting at char '%c' (ASCII %d)\n", 
           lexer->current_char, lexer->current_char);
    skip_whitespace(lexer);

    if (lexer->current_char == '\0') {
        token->type = T_EOF;
        token->value = _strdup("EOF");
        return;
    }

    if (isalpha(lexer->current_char) || lexer->current_char == '_') {
        skip_whitespace(lexer);
        lex_identifier(lexer, token);
        return;
    }

    if (lexer->current_char == '"' || lexer->current_char == '\'') {
        lex_string(lexer, token);
        return;
    }

    if (isdigit(lexer->current_char)) {
        lex_number(lexer, token);
        return;
    }

    char c = lexer->current_char;
    advance(lexer);

    token->value = (char*)malloc(2);
    token->value[0] = c;
    token->value[1] = '\0';

    switch (c) {
        case '+': token->type = T_ADD; break;
        case '-': token->type = T_SUBTRACT; break;
        case '*': token->type = T_MAUL; break;
        case '/': token->type = T_DIVIDE; break;
        case '%': token->type = T_MODULO; break;
        case '(': token->type = T_LPAREN; break;
        case ')': token->type = T_RPAREN; break;
        case '=': token->type = T_ASSIGN; break;
        case ':': token->type = T_COLON; break;
        default: token->type = T_UNKNOWN; break;
    }
};
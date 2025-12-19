#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"
#include "Token.h"

int main() {
    const char* source = "12 + 34 * (56 - 7) / 8";

    Lexer lexer;
    lexer_init(&lexer, source);

    Token token;
    while (1) {
    make_token(&lexer, &token);
    if (token.type == T_EOF) break;

    printf("Token: type=%d, value=%s\n", token.type, token.value);
    free(token.value);  // ONLY here
}
    return 0;
}
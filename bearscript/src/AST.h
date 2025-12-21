#ifndef AST_H
#define AST_H

#include "Token.h"

typedef enum {
    AST_ASSIGN,
    AST_INTEGER,
    AST_FLOAT,
    AST_VARIABLE,
    AST_BINARY_OP
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {           // for assignment
            char* var_name;
            struct ASTNode* value;
        } assign;
        double number;     // for integer or float
        char* string;      // for string literals
        char* var_name;    // for variable references
        struct {           // for binary operations
            struct ASTNode* left;
            TokenType op;
            struct ASTNode* right;
        } binary_op;
    };
} ASTNode;

#endif // AST_H

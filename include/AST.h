#ifndef AST_H
#define AST_H

#include "Token.h"

typedef enum {
    AST_ASSIGN,
    AST_INTEGER,
    AST_FLOAT,
    AST_GROWL_STATEMENT,
    AST_VARIABLE,
    AST_BINARY_OP
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {           // for growl command
            struct ASTNode* expression;
        } growl_stmt;
        struct {           // for assignment
            char* var_name;
            struct ASTNode* value;
        } assign;
        struct {           // for values
            union {
                long int_val;    // Separate int and float!
                double float_val;
                char* str_val;
            };
        } value;
        struct {           // for variable references
            char* var_name;
        } variable;
        struct {           // for binary operations
            struct ASTNode* left;
            TokenType op;
            struct ASTNode* right;
        } binary_op;
    } data;  // Give the union a name!
} ASTNode;

#endif // AST_H
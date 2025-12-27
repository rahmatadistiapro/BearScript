#ifndef AST_H
#define AST_H

#include "Token.h"
#include <stdbool.h>
typedef enum {
    AST_ASSIGN, // 0
    AST_TYPED_ASSIGN, // 1
    AST_IMMUTABLE_ASSIGN, // 2
    AST_INTEGER, // 3
    AST_FLOAT, // 4
    AST_STRING, // 5
    AST_GROWL_STATEMENT, // 6
    AST_VARIABLE, // 7
    AST_BINARY_OP // 8
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {           // for growl command
            struct ASTNode* expression;
        } growl_stmt;
        struct {
            char* str_val;
        } string;
        struct {           // for assignment
            char* var_name;
            struct ASTNode* value;
        } assign;
        struct {
            char* var_name;
            char* type_name;
            struct ASTNode* value;
        } typed_assign;

        struct {
            char* var_name;
            char* type_name;
            struct ASTNode* value;
        } immutable_assign;
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
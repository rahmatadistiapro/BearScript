#include "eval.h"
#include "AST.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

double eval(ASTNode* node, SymbolTable* table) {
    switch (node->type) {

        case AST_INTEGER:
        case AST_FLOAT:
            return node->number;
        case AST_VARIABLE: {
            double value;
            if (!get_variable(table, node->var_name, &value)) {
                printf("Error: Undefined variable '%s'\n", node->var_name);
                exit(1);
            }
            return value;
        }
        case AST_ASSIGN: {
                    double value = eval(node->binary_op.right, table);
                    set_variable(table, node->binary_op.right->var_name, value);
                    return value;
        }
        case AST_BINARY_OP: {
            double left = eval(node->binary_op.left, table);
            double right = eval(node->binary_op.right, table);
            switch (node->binary_op.op) {
                case T_ADD:
                    return left + right;
                case T_SUBTRACT:
                    return left - right;
                case T_MUL:
                    return left * right;
                case T_DIVIDE:
                    if (right == 0) {
                        printf("Error: Division by zero\n");
                        exit(1);
                    }
                    return left / right;
                case T_MODULO:
                    if ((int)right == 0) {
                        printf("Error: Modulo by zero\n");
                        exit(1);
                    }
                    return (int)left % (int)right;
                default: {
                    printf("Error: Unknown operator\n");
                    exit(1);
                }
            }
        }
    }
    return 0;
}
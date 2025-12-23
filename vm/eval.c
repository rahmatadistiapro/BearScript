#include "D:/BearScript/include/eval.h"
#include "D:/BearScript/include/AST.h"
#include "D:/BearScript/include/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

double eval(ASTNode* node, SymbolTable* table) {
    switch (node->type) {

        case AST_STRING: {
            printf("Debug: Found string literal: %s\n", node->data.string.str_val);
            return 0.0;
        }
        case AST_INTEGER: {
            return (double)node->data.value.int_val;
        }
        case AST_FLOAT: {
            return node->data.value.float_val;
        }
        case AST_VARIABLE: {
            double value;
            if (!get_variable(table, node->data.variable.var_name, &value)) {
                printf("Error: Undefined variable '%s'\n", node->data.variable.var_name);
                exit(1);
            }
            return value;
        }
        case AST_ASSIGN: {
            char* var_name = node->data.assign.var_name;
            double value = eval(node->data.assign.value, table);
            set_variable(table, var_name, value);
            return value;
        }
        case AST_GROWL_STATEMENT: {
            printf("Debug eval: Executing GROWL statement\n"); 
            ASTNode* expr = node->data.growl_stmt.expression;
            if (expr->type == AST_STRING) {
                printf("%s\n", expr->data.string.str_val);
                return 0.0;
            }
            else if (expr->type == AST_INTEGER || expr->type == AST_FLOAT) {
                double value = eval(expr, table);
                printf("GROWL: %f\n", value);
                return value;
            }
            else {
                double value = eval(expr, table);
                printf("GROWL: %f\n", value);
                return value;
            }
        }
        case AST_BINARY_OP: {
            printf("DEBUG eval: Evaluating binary op %d\n", node->data.binary_op.op);
            double left = eval(node->data.binary_op.left, table);
            double right = eval(node->data.binary_op.right, table);
            switch (node->data.binary_op.op) {
                case T_ADD: {
                    double result = left + right;
                    printf("DEBUG eval: %f + %f = %f\n", left, right, result);
                    return result;
                }
                case T_SUBTRACT: {
                    double result = left - right;
                    printf("DEBUG eval: %f - %f = %f\n", left, right, result);
                    return result;
                }
                case T_MAUL: {
                    double result = left * right;
                    printf("DEBUG eval: %f * %f = %f\n", left, right, result);
                    return result;
                }
                case T_DIVIDE: {
                    if (right == 0) {
                        printf("Error: Division by zero\n");
                        exit(1);
                    }
                    double result = left / right;
                    printf("DEBUG eval: %f / %f = %f\n", left, right, result);
                    return result;
                }
                case T_MODULO: {
                    if ((int)right == 0) {
                        printf("Error: Modulo by zero\n");
                        exit(1);
                    }
                    double result = (int)left % (int)right;
                    printf("DEBUG eval: %d %% %d = %d\n", (int)left, (int)right, (int)result);
                    return result;
                }
                default: {
                    printf("Error: Unknown operator %d\n", node->data.binary_op.op);
                    exit(1);
                }
            }
        }
        default: {
            printf("Error: Unknown AST node type %d\n", node->type);
            exit(1);
        }
    }
    return 0;
}
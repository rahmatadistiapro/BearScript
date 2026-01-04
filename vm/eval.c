#include "D:/BearScript/include/eval.h"
#include "D:/BearScript/include/Value.h"
#include "D:/BearScript/include/symbol_table.h"
#include <complex.h>
#include <stdbool.h>
#include <string.h>

static bool is_truthy(Value value) {
    if (is_boolean(value)) {
        return as_boolean(value);
    }
    if (is_nil(value)) {
        return false;
    }
    if (is_number(value)) {
        if (is_integer(value)) {
            return as_integer(value) != 0;
        } else if (is_float(value)) {
            return as_float(value) != 0.0;
        }
    }
    if (is_string(value)) {
        return strlen(as_string(value)) > 0;
    }
    return false;
}

Value eval(ASTNode* node, SymbolTable* table) {
    switch (node->type) {
        case AST_INTEGER: {
            return integer_value(node->data.value.int_val);
        }
        case AST_FLOAT: {
            return float_value(node->data.value.float_val);
        }
        case AST_STRING: {
            return string_value(_strdup(node->data.string.str_val));
        }
        case AST_BOOLEAN: {
            return boolean_value(node->data.boolean.value);
        }
        case AST_VARIABLE: {
            Value value;
            if (!get_variable(table, node->data.variable.var_name, &value)) {
                return error_value(_strdup("Undefined variable, cannot find variable"));
            }
            return value;
        }
        case AST_ASSIGN: {
            char* var_name = node->data.assign.var_name;
            Value value = eval(node->data.assign.value, table);
            if (!assign_variable(table, var_name, value)) {
                return error_value(_strdup("Cannot reassign to immutable variable"));
            }
            return value;
        }
        case AST_TYPED_ASSIGN: {
            char* var_name = node->data.typed_assign.var_name;
            char* type_name = node->data.typed_assign.type_name;
            Value value = eval(node->data.typed_assign.value, table);
            
            if (strcmp(type_name, "str") == 0) {
                if (!is_string(value)) {
                    return error_value(_strdup("String variable requires to have a string value"));
                }
                assign_variable(table, var_name, value);
                return value;
            } 
            else if (strcmp(type_name, "int") == 0 || strcmp(type_name, "float") == 0) {
                if (!is_integer(value)) {
                    return error_value(_strdup("Number variable requires to have a number value"));
                }
                assign_variable(table, var_name, value);
                return value;
            }
            else if (strcmp(type_name, "float") == 0) {
                if (!is_float(value)) {
                    return error_value(_strdup("Float variable requires to have a float value"));
                }
                assign_variable(table, var_name, value);
                return value;
            }
            else {
                return error_value(_strdup("Unknown type, did you mean: 'str', 'int', or 'float'?"));
            }
        }
        case AST_IMMUTABLE_ASSIGN: {
            char* var_name = node->data.immutable_assign.var_name;
            char* type_name = node->data.immutable_assign.type_name;
            Value value = eval(node->data.immutable_assign.value, table);
            if (!define_variable(table, var_name, value, true)) {
                return error_value(_strdup("cannot defined a variable that is already defined variable"));
            }
            if (type_name == NULL) {
                return value;
            }
            if (strcmp(type_name, "str") == 0) {
                if (!is_string(value)) {
                    return error_value(_strdup("String variable requires to have a string value"));
                }
                return value;
            } 
            else if (strcmp(type_name, "int") == 0 || strcmp(type_name, "float") == 0) {
                if (!is_integer(value)) {
                    return error_value(_strdup("Number variable requires to have a number value"));
                }
                return value;
            }
            else if (strcmp(type_name, "float") == 0) {
                if (!is_float(value)) {
                    return error_value(_strdup("Float variable requires to have a float value"));
                }
                return value;
            }
            else {
                return error_value(_strdup("Unknown type, did you mean: 'str', 'int', or 'float'?"));
            }
        }
        case AST_GROWL_STATEMENT: {
            Value value = eval(node->data.growl_stmt.expression, table);
            return value;
        }
        case AST_IF_STATEMENT: {
            printf("=== Evaluating If Statement ===\n");
            printf("Evaluating condition...\n");
            printf("Condition AST Type: %d\n", node->data.if_stmt.condition->type);
            Value condition = eval(node->data.if_stmt.condition, table);
            printf("Debug: Condition evaluated to type %d\n", condition.type);

            if (is_error(condition)) {
                printf("Error evaluating condition\n");
                return condition;
            }

            if (is_truthy(condition)) {
                print_value(condition);
                printf("Condition is truthy, executing then branch\n");
                printf("Then branch has %d statements\n", node->data.if_stmt.then_count);
                Value last_result = nil_value();
                for (int i = 0; i < node->data.if_stmt.then_count; i++) {
                    printf("Executing then statement %d\n", i);
                    Value result = eval(node->data.if_stmt.then_statements[i], table);
                    printf("Then statement %d executed\n", i);
                    if (is_error(result)) {
                        free_value(condition);
                        free_value(last_result);
                        return result;
                    }
                    printf("Result of then statement %d: ", i);
                    free_value(last_result);
                    last_result = result;
                }
                free_value(condition);
                return last_result;
            } else if (!is_truthy(condition)) {
                printf("Condition is falsy, checking elif branches\n");
                // Check elif branches
                ASTNode* elif_branch = node->data.if_stmt.elif_branch;
                while (elif_branch != NULL) {
                    Value elif_condition = eval(elif_branch->data.elif_stmt.condition, table);
                    if (is_error(elif_condition)) {
                        return elif_condition;
                    }

                    if (is_truthy(elif_condition)) {
                        printf("Elif condition is truthy, executing then branch\n");
                        Value last_result = nil_value();
                        for (int i = 0; i < elif_branch->data.elif_stmt.count; i++) {
                            Value result = eval(elif_branch->data.elif_stmt.then_statements[i], table);
                            if (is_error(result)) {
                                free_value(condition);
                                free_value(last_result);
                                return result;
                            }
                            free_value(last_result);
                            last_result = result;
                        }
                        free_value(condition);
                        return last_result;
                    }
                    elif_branch = elif_branch->data.elif_stmt.next_elif;
                }
                return nil_value();
            }

                // Else branch
                if (node->data.if_stmt.else_branch != NULL) {
                    Value last_result = nil_value();
                    for (int i = 0; i < node->data.if_stmt.else_branch->data.else_stmt.count; i++) {
                        Value result = eval(&node->data.if_stmt.else_branch->data.else_stmt.then_statements[i], table);
                        if (is_error(result)) {
                            free_value(condition);
                            free_value(last_result);
                            return result;
                        }
                        free_value(last_result);
                        last_result = result;
                    }
                    free_value(condition);
                    return last_result;
            }
        }
        case AST_ELIF_STATEMENT: {
            return error_value(_strdup("Unexpected elif statement, elif without if"));
        }
        case AST_ELSE_STATEMENT: {
            return error_value(_strdup("Unexpected else statement, else without if or elif"));
        }
        case AST_BINARY_OP: {
            Value left = eval(node->data.binary_op.left, table);
            Value right = eval(node->data.binary_op.right, table);
            
            // Handle number operations
            if (is_integer(left) && is_integer(right)) {
                long l = as_integer(left);
                long r = as_integer(right);
                
                switch (node->data.binary_op.op) {
                    case T_ADD:   { return integer_value(l + r); }
                    case T_SUBTRACT: { return integer_value(l - r); }
                    case T_MAUL:     { return integer_value(l * r); }
                    case T_DIVIDE: {
                        if (r == 0) { return error_value(_strdup("cannot do Division by zero")); }
                        return integer_value(l / r);
                    }
                    case T_MODULO: {
                        if (r == 0) { return error_value(_strdup("cannot do Modulo by zero")); }
                        return integer_value(l % r);
                    }
                    default:         return error_value(_strdup("Unknown operator, did you mean: '+', '-', '*', '/', '%'?"));
                }
            }
            if (is_number(left) && is_number(right)) {
                double l = as_float(left);
                double r = as_float(right);
                
                switch (node->data.binary_op.op) {
                    case T_ADD:   { return float_value(l + r); }
                    case T_SUBTRACT: { return float_value(l - r); }
                    case T_MAUL:     { return float_value(l * r); }
                    case T_DIVIDE: {
                        if (r == 0) { return error_value(_strdup("cannot do Division by zero")); }
                        return float_value(l / r);
                    }
                    case T_MODULO: {
                        if ((int)r == 0) { return error_value(_strdup("cannot do Modulo by zero")); }
                        return integer_value((int)l % (int)r);
                    }
                    default:         return error_value(_strdup("Unknown operator, did you mean: '+', '-', '*', '/', '%'?"));
                }
            }
            // Handle string concatenation (future)
            else if (is_string(left) && is_string(right) && node->data.binary_op.op == T_ADD) {
                // TODO: Implement string concatenation
                return error_value(_strdup("String concatenation not implemented yet"));
            }
            else {
                return error_value(_strdup("Type mismatch in operation"));
            }
        }
        case AST_COMPARE_OP: {
            printf("=== Evaluating Comparison Operation ===\n");
            printf("Operator Type: %d\n", node->data.compare_op.op);
            Value left = eval(node->data.compare_op.left, table);
            printf("Left Value Type: %d\n", left.type);
            Value right = eval(node->data.compare_op.right, table);
            printf("Right Value Type: %d\n", right.type);

            if (is_error(left)) {
                free_value(right);
                return left;
            }
            if (is_error(right)) {
                free_value(left);
                return right;
            }

            if (is_number(left) && is_number(right)) {
                double l = as_float(left);
                double r = as_float(right);

                bool result;
                switch (node->data.compare_op.op) {
                    case T_EQUAL:      { result = (l == r); break; }
                    case T_NOTEQ:  { result = (l != r); break; }
                    case T_LT:         { result = (l < r); break; }
                    case T_LTOREQ:  { result = (l <= r); break; }
                    case T_GT:         { result = (l > r); break; }
                    case T_GTOREQ:  { result = (l >= r); break; }
                    default: {
                        free_value(left);
                        free_value(right);
                        return error_value(_strdup("Unknown comparison operator")); 
                    }
                }
                free_value(left);
                free_value(right);
                return boolean_value(result);
            }
            free_value(left);
            free_value(right);
            return error_value(_strdup("Comparison requires numbers"));
        }
    }
}
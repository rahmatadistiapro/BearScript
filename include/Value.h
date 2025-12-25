#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum {
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_BOOLEAN,
    VALUE_NIL,
    VALUE_ERROR
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        char* string;
        bool boolean;
        char* error_message;
    } as;
} Value;

Value number_value(double value);
Value string_value(const char* value);
Value boolean_value(bool value);
Value nil_value();
Value error_value(const char* message);

bool is_number(Value value);
bool is_string(Value value);
bool is_boolean(Value value);
bool is_nil(Value value);
bool is_error(Value value);

double as_number(Value value);
char* as_string(Value value);
bool as_boolean(Value value);

void print_value(Value value);
bool values_equal(Value a, Value b);
void free_value(Value value);

#endif // VALUE_H
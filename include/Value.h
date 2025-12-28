#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum {
    VALUE_INTEGER,
    VALUE_FLOAT,
    VALUE_STRING,
    VALUE_BOOLEAN,
    VALUE_NIL,
    VALUE_ERROR
} ValueType;

typedef struct {
    ValueType type;
    union {
        long integer;
        double float_val;
        char* string;
        bool boolean;
        char* error_message;
    } as;
} Value;

Value number_value(double value);
Value integer_value(long value);
Value float_value(double value);
Value string_value(const char* value);
Value boolean_value(bool value);
Value nil_value();
Value error_value(const char* message);

bool is_number(Value value);
bool is_integer(Value value);
bool is_float(Value value);
bool is_string(Value value);
bool is_boolean(Value value);
bool is_nil(Value value);
bool is_immutable(Value value);
bool is_error(Value value);

double as_number(Value value);
long as_integer(Value value);
double as_float(Value value);
char* as_string(Value value);
bool as_boolean(Value value);

void print_value(Value value);
bool values_equal(Value a, Value b);
void free_value(Value value);
Value copy_value(Value original);

#endif // VALUE_H
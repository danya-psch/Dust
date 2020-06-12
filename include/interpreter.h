#pragma once

#include<tree.h>

int Interpreter_execute(Tree * astTree, FILE * file) ;

//

typedef enum {
   ValueType_NUMBER,
   ValueType_BOOL,
   ValueType_STRING,
   ValueType_ARRAY,
   ValueType_UNDEFIND
} ValueType;

typedef struct __Array Array;
struct __Array {
    ValueType type;
    List * list;
};

typedef struct __Value Value;
struct __Value {
    ValueType type;
    void * value;
};

typedef struct __Program Program;

void Value_free(Value * self);
void Value_print(Value * self, FILE * file);
FILE * Program_getFile(Program * self);
Value * Value_newNumber(double number);
Value * Value_newString(const char * str);
Value * Value_newBool(bool boolean);
Value * Value_newArray(List * list);
Value * Value_newUndefined(void);
char * Value_string(Value * self);
double Value_number(Value * self);
Value * Value_new(ValueType type, void * value);
Value * Value_newCopy(Value * origin);


void Program_setError(Program * self, char * error);

typedef Value * (*Function)(Program * program, List * values);
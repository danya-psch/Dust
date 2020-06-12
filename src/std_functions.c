#include <std_functions.h>
#include <string_buffer.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <string_dup.h>
#include <math.h>
#include <list.h>

Value * std_print(Program * program, List * values) {
    for (int i = 0; i < List_count(values); i++) {
        Value * value = List_at(values, i);
        // fprintf( Program_getFile(program), ">> ");
        Value_print(value, Program_getFile(program));
        fprintf( Program_getFile(program), " ");
    }
    fprintf( Program_getFile(program), "\n");
    // printf("# %s()", __func__);
    return Value_newUndefined();
}

Value * std_scan(Program * program, List * values) {
    char strVal[1000];
    for (int i = 0; i < 1000; i++) {
        strVal[i] = '\0';
    }
    fgets(strVal,1000,stdin);
    int i = 0;
    while (strVal[i] != '\0') {
        i++;
    }
    strVal[i - 1] = '\0';
    return Value_newString(strVal);
}

Value * std_strlen(Program * program, List * values) {
    if (List_count(values) != 1) {
        Program_setError(program, strdup("Invalid number of strlen arguments"));
        return NULL;
    }
    Value * strVal = List_at(values, 0);
    if (strVal->type != ValueType_STRING) {
        Program_setError(program, strdup("Invalid strlen argument type"));
        return NULL;
    }
    char * str = Value_string(strVal);
    int len = strlen(str);
    // printf("# %s()", __func__);
    return Value_newNumber(len);
}

Value * std_sqrt(Program * program, List * values) {
    if (List_count(values) != 1) {
        Program_setError(program, strdup("Invalid number of sqrt arguments"));
        return NULL;
    }   
    Value * Val = List_at(values, 0);
    if (Val->type != ValueType_NUMBER) {
        Program_setError(program, strdup("Invalid sqrt argument type"));
        return NULL;
    }
    double x = sqrt(Value_number(Val));
    return Value_newNumber(x);
}

Value * std_pow(Program * program, List * values) {
    if (List_count(values) != 2) {
        Program_setError(program, strdup("Invalid number of pow arguments"));
        return NULL;
    }
    Value * Val1 = List_at(values, 0);
    Value * Val2 = List_at(values, 1);
    if (Val1->type != ValueType_NUMBER || Val2->type != ValueType_NUMBER) {
        Program_setError(program, strdup("Invalid pow argument type"));
        return NULL;
    }
    double x = pow(Value_number(Val1), Value_number(Val2));
    return Value_newNumber(x);
}

Value * std_strtonum(Program * program, List * values) {
    if (List_count(values) != 1) {
        Program_setError(program, strdup("Invalid number of strtonum arguments"));
        return NULL;
    }   
    Value * strVal = List_at(values, 0);
    if (strVal->type != ValueType_STRING) {
        Program_setError(program, strdup("Invalid strtonum argument type"));
        return NULL;
    }
    double x = atof(Value_string(strVal));
    // Value_number(atof(Value_string(strVal))); 
    return Value_newNumber(x);
}

Value * std_numtostr(Program * program, List * values) {
    if (List_count(values) != 1) {
        Program_setError(program, strdup("Invalid number of numtostr arguments"));
        return NULL;
    }   
    Value * val = List_at(values, 0);
    if (val->type != ValueType_NUMBER) {
        Program_setError(program, strdup("Invalid numtostr argument type"));
        return NULL;
    }
    double x = Value_number(val);
    return Value_newString("Гы"); 
}


Value * std_push(Program * program, List * values) { 
    if (List_count(values) != 2) {
        Program_setError(program, strdup("Invalid number of push arguments"));
        return NULL;
    } 
    Value * val1 = List_at(values, 0);
    if (val1->type != ValueType_ARRAY) {
        Program_setError(program, strdup("Invalid push argument type"));
        return NULL;
    }
    Array * arr = val1->value;
    Value * val2 = List_at(values, 1);
    List * list = arr->list;
    if (arr->type == ValueType_UNDEFIND) { //((Value*)List_at(list, 0))
        arr->type = val2->type;
        void * v = List_removeAt(list, 0);
        free(v);
    } else if (arr->type != val2->type) {
        Program_setError(program, strdup("Invalid _push argument type"));
        return NULL;
    }
    
    List_add(arr->list, Value_newCopy(val2));
    return Value_newUndefined();
}

Value * std_count(Program * program, List * values) { 
    if (List_count(values) != 1) {
        Program_setError(program, strdup("Invalid number of count arguments"));
        return NULL;
    } 
    Value * val1 = List_at(values, 0);
    if (val1->type != ValueType_ARRAY) {
        Program_setError(program, strdup("Invalid count argument type"));
        return NULL;
    }
    Array * arr = val1->value;
    return Value_newNumber(List_count(arr->list));
}


Value * std_at(Program * program, List * values) {
    if (List_count(values) != 2) {
        Program_setError(program, strdup("Invalid number of at arguments"));
        return NULL;
    } 
    Value * val1 = List_at(values, 0);
    if (val1->type != ValueType_ARRAY) {
        Program_setError(program, strdup("Invalid at argument type"));
        return NULL;
    }
    Array * arr = val1->value;
    Value * val2 = List_at(values, 1);
    if (ValueType_NUMBER != val2->type) {
        Program_setError(program, strdup("Invalid _at argument type"));
        return NULL;
    }
    int num = *(double*)val2->value;
    if (List_count(arr->list) < num) {
        Program_setError(program, strdup("Invalid __at argument type"));
        return NULL;
    }
    Value * valForRet = (Value*)List_at(arr->list, num);
    switch(arr->type) {
        case ValueType_NUMBER: return Value_newNumber(*(double*)valForRet->value);
        case ValueType_STRING: return Value_newString((char *)valForRet->value);
        case ValueType_BOOL: return Value_newBool(*(bool*)valForRet->value);
        default : return Value_newUndefined();
    }
    return Value_newUndefined();
}

Value * std_slice(Program * program, List * values) {
    if (List_count(values) != 1) {
        Program_setError(program, strdup("Invalid number of slice arguments"));
        return NULL;
    } 
    Value * val1 = List_at(values, 0);
    if (val1->type != ValueType_STRING) {
        Program_setError(program, strdup("Invalid slice argument type"));
        return NULL;
    }

    List * list = List_new();
    const char * str = (char *)val1->value;
    if (str != NULL || strlen(str) != 0) {
        enum {BUF_LEN = 100};
        char buf[BUF_LEN] = "";
        int n = 0;
        while (*str != '\0') {
            buf[n] = *str;
            n++;
            str++;
            if (*str == ' ' || (*str == '\0' && n != 0)) {
                buf[n] = '\0';
                List_add(list, Value_newString(buf));
                // if(!*str) break;
                n = 0; 
                str++;
                for (int i = 0;i < BUF_LEN;i++) {
                    buf[i] = '\0';
                }
            }
        }
    }
    return Value_newArray(list);
}

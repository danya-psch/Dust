#include<interpreter.h> 
#include<stdlib.h> 
#include<stdio.h>
#include<string.h>
#include<assert.h>  
#include<ast.h>    
#include<math.h>     
#include<dict.h>  
#include<std_functions.h> 
#include <string_dup.h>

void Interpreter_releaseTheTree(Tree * root);

typedef struct {
    Function ptr;
} StdFunction;

StdFunction * StdFunction_new(Function ptr) {
    StdFunction * self = malloc(sizeof(StdFunction));
    self->ptr = ptr;
    return self;
}

void StdFunction_free(StdFunction * self) {
    free(self);
}


Value * Value_new(ValueType type, void * value) {
    Value * self = malloc(sizeof(Value));
    self->type = type;
    self->value = value;
    return self;
}

void Value_free(Value * self) {
    free(self->value); 
    free(self);
}

void ValueType_print(ValueType self, FILE * file) {
    switch (self) {
        // case ValueType_NUMBER : fprintf(file,"number"); break;
        // case ValueType_STRING : fprintf(file,"string"); break;
        // case ValueType_BOOL : fprintf(file,"bool"); break;
        // case ValueType_ARRAY : fprintf(file," : array"); break;
        // case ValueType_UNDEFIND : fprintf(file,"undefind"); break;
        default: /*fprintf(file,"<?>");*/ break; 
    }
}

void Value_print(Value * self, FILE * file) {
    switch (self->type) {
        case ValueType_NUMBER : {
            double val = *(double*)self->value;
            fprintf(file, "%lf", val);
            break;
        }
        case ValueType_STRING : {
            char * val = (char*)self->value;
            fprintf(file, "\"%s\"", val);
            break;
        }
        case ValueType_BOOL : {
            bool val = *(bool*)self->value;
            fprintf(file, "%s", val ? "TRUE" : "FALSE");
            break;
        }
        case ValueType_ARRAY : {
            Array * arr = self->value;
            for (int i = 0 ; i < List_count(arr->list) - 1; i ++ ) {
                Value * value = List_at(arr->list, i);
                Value_print(value, file);
                fprintf(file, ", ");
            }
            Value * value = List_at(arr->list, List_count(arr->list) - 1);
            Value_print(value, file);
            break;
        }
        case ValueType_UNDEFIND : {
            // fprintf(file ,"undefind | ");
            break;
        }
        
        default: fprintf(file, "<?>"); break; 
    }
    ValueType_print(self->type, file);
}

struct __Program {
    Dict * variables;
    Dict * functions;
    FILE * file;
    char * error; 
};

FILE * Program_getFile(Program * self) {
    return self->file;
}

void Program_setError(Program * self, char * error) {
    self->error = error;
}

// Value * Value_copy(Value * self) {
//     List * vals = self;
//     Value * val = 
//     for(int i = 0; i < List_count(vals); i++) {
//         void * value = List_at(vals, i);
//         List_add(dest, value);
//     }
//     return *((bool*)self->value);
// }

Value * Value_newNumber(double number) {
    double * numberMem = malloc(sizeof(double));
    *numberMem = number;
    return Value_new(ValueType_NUMBER, numberMem);
}

Value * Value_newString(const char * str) {
    return Value_new(ValueType_STRING, strdup((char *)str));
}

Value * Value_newArray(List * list) {
    Array * arr = malloc(sizeof(Array));
    List * arrList = List_new();
    List_copyTo(list, arrList);
    List_free(list);
    arr->list = arrList;
    ValueType type = ((Value*)List_at(arrList, 0))->type;
    arr->type = type;
    return Value_new(ValueType_ARRAY, arr);
}

Value * Value_newBool(bool boolean) {
    bool * mem = malloc(sizeof(bool));
    *mem = boolean;
    return Value_new(ValueType_BOOL, mem);
}

Value * Value_newUndefined(void) {
    return Value_new(ValueType_UNDEFIND, NULL);
}

double Value_number(Value * self) {
    assert(self->type == ValueType_NUMBER);
    return *((double*)self->value);
}

List * Value_array(Value * self) {
    assert(self->type == ValueType_ARRAY);
    return ((Array*)self->value)->list;
}

bool Value_bool(Value * self) {
    assert(self->type == ValueType_BOOL);
    return *((bool*)self->value);
}

char * Value_string(Value * self) {
    assert(self->type == ValueType_STRING);
    return ((char*)self->value);
}

typedef bool (*a)(Value * a, Value * b);

bool Value_compare(Value * a, Value * b) {
    if (a->type != b->type) assert(0 && "Not supposed");;
    switch(a->type) {
        case ValueType_NUMBER : return Value_number(a) < Value_number(b);
        default : assert(0 && "Not supposed");
    }
}

bool Value_compare_ap(Value * a, Value * b) {
    if (a->type != b->type) assert(0 && "Not supposed");;
    switch(a->type) {
        case ValueType_NUMBER : return Value_number(a) <= Value_number(b);
        case ValueType_BOOL : return Value_bool(a) == Value_bool(b);

        default : assert(0 && "Not supposed");
    }
}

bool Value_equals(Value * a, Value * b) {
    if (a->type != b->type) return false;
    switch(a->type) {
        case ValueType_BOOL : return Value_bool(a) == Value_bool(a);
        case ValueType_NUMBER : return fabs(Value_number(a) - Value_number(b)) < 1e-6;
        case ValueType_STRING : return 0 == strcmp(Value_string(a), Value_string(b));
        case ValueType_ARRAY : false;
        case ValueType_UNDEFIND : return true;
        default : assert(0 && "Not supposed");
    }
}

bool Value_asBool(Value * self) {
    switch(self->type) {
        case ValueType_BOOL : return Value_bool(self);
        case ValueType_NUMBER : return fabs(Value_number(self)) > 1e-6;
        case ValueType_STRING : return Value_string(self) != NULL; //@todo empty string are true or false?
        case ValueType_ARRAY : false; //@todo empty string are true or false?
        case ValueType_UNDEFIND : return false;
        default : assert(0 && "Not supposed");
    }
}

Value * Value_newCopy(Value * origin) {
    switch(origin->type) {
        case ValueType_BOOL : return Value_newBool(Value_bool(origin));
        case ValueType_NUMBER : return Value_newNumber(Value_number(origin));
        case ValueType_STRING : return Value_newString(Value_string(origin));
        case ValueType_ARRAY : return origin;// Value_newArray(Value_array(origin));
        case ValueType_UNDEFIND : assert(0 && "Implament undefind"); return NULL;
        default : assert(0 && "Not supposed"); return NULL;
    }
}

Value * Program_getVariableValue(Program * self,const char * varId) {
    if (!Dict_contains(self->variables, (char *)varId)) {
        self->error = strdup("var id not found");
        return NULL;
    }
    return Dict_get(self->variables, (char *)varId);
} 

Value * eval(Program * program, Tree * node);

bool Ast_eq(Program * program, Tree * node, a funk) {
    Tree * firstChild = List_at(node->children, 0);
    Value * firstValue = eval(program, firstChild);
    if (program->error) return NULL;
    Tree * secondChild = List_at(node->children, 1);
    Value * secondValue = eval(program, secondChild);
    if (program->error) return NULL;
    bool res = funk(firstValue, secondValue);
    Value_free(firstValue);
    Value_free(secondValue);
    return res;
}

Value * eval(Program * program, Tree * node) {
    AstNode * astNode = node->value;
    // printf("name = %s\n", astNode->name);
    switch(astNode->type) {
        case AstNodeType_NUMBER: {
            double number = atof(astNode->name);
            return Value_newNumber(number);
        }
        case AstNodeType_STRING: {
            return Value_newString(astNode->name);
        }
        case AstNodeType_BOOL: {
            return Value_newBool(0 == strcmp(astNode->name, "true") ? true : false);
        }
        case AstNodeType_ID: {
            
            const char * varId = astNode->name;
            if (List_count(node->children) != 0) {
                Tree * argListNode = List_at(node->children, 0);
                AstNode * argList = argListNode->value;
                assert(argList->type == AstNodeType_ARGLIST);
                //
                if (!Dict_contains(program->functions, (char *)varId)) {
                    program->error = strdup("Call unknown function");
                    return NULL;
                }
                //
                List * arguments = List_new();
                for (int i = 0; i < List_count(argListNode->children); i++) {
                    Tree * argListChildNode = List_at(argListNode->children, i);
                    Value * argumentValue = eval(program, argListChildNode);

                    if (program->error) {
                        // @todo free all arguments values
                        List_free(arguments);
                        return NULL;
                    }
                    List_add(arguments, argumentValue);
                }
                // call function 

                StdFunction * func = Dict_get(program->functions, (char *)varId);
                Value * retValue = func->ptr(program, arguments);
                
                for (int i = 0; i < List_count(arguments); i++) {
                    Value * argListChildNode = List_at(arguments, i);
                    if (argListChildNode->type != ValueType_ARRAY) {
                        Value_free(argListChildNode);
                    }
                }

                List_free(arguments);
                return retValue;
            }
            Value * varValue = Program_getVariableValue(program, varId);
            if (program->error) return NULL;
            return Value_newCopy(varValue);
        }
        case AstNodeType_ARGLIST: {
            if (List_count(node->children) != 0) {
                assert(astNode->type == AstNodeType_ARGLIST);
                List * arguments = List_new();
                Value * firstValue = eval(program, List_at(node->children, 0));
                List_add(arguments, firstValue);
                for (int i = 1; i < List_count(node->children); i++) {
                    Value * argumentValue = eval(program, List_at(node->children, i));
                    // bool val = *(bool*)argumentValue->value;
                    // printf("!!%s\n", val ? "TRUE" : "FALSE");
                    if (firstValue->type != argumentValue->type) {
                        program->error = strdup("Array can't take values with different types");
                        List_free(arguments);
                        return NULL;
                    }
                    if (program->error) {
                        List_free(arguments);
                        return NULL;
                    }
                    List_add(arguments, argumentValue);
                }
                return Value_newArray(arguments);
            }
            List * arguments = List_new();
            Value * firstValue = Value_new(ValueType_UNDEFIND, NULL);
            List_add(arguments, firstValue);
            return Value_newArray(arguments);
        }
        case AstNodeType_ADD: {
            if (List_count(node->children) != 0) {
                Tree * firstChild = List_at(node->children, 0);
                Value * firstValue = eval(program, firstChild);
                if (program->error) return NULL;
                if (firstValue->type != ValueType_NUMBER) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    return NULL;
                }
                int nchild = List_count(node->children);
                if (nchild == 1) {
                    return firstValue;
                } else {
                    Tree * secondChild = List_at(node->children, 1);
                    Value * secondValue = eval(program, secondChild);
                    if (program->error) return NULL;
                    if (secondValue->type != ValueType_NUMBER) {
                        program->error = strdup("Invalid operation");
                        Value_free(firstValue);
                        Value_free(secondValue);
                        return NULL;
                    }
                    double res = Value_number(firstValue) + Value_number(secondValue);
                    Value_free(firstValue);
                    Value_free(secondValue);
                    return Value_newNumber(res);
                }
            }
            return Value_newUndefined();
        }
        case AstNodeType_SUB: {
            if (List_count(node->children) != 0) {
                Tree * firstChild = List_at(node->children, 0);
                Value * firstValue = eval(program, firstChild);
                if (program->error) return NULL;
                if (firstValue->type != ValueType_NUMBER) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    return NULL;
                }
                int nchild = List_count(node->children);
                if (nchild == 1) {
                    double res = -Value_number(firstValue);
                    Value_free(firstValue); 
                    return Value_newNumber(res);;
                } else {
                    Tree * secondChild = List_at(node->children, 1);
                    Value * secondValue = eval(program, secondChild);
                    if (program->error) return NULL;
                    if (secondValue->type != ValueType_NUMBER) {
                        program->error = strdup("Invalid operation");
                        Value_free(firstValue);
                        Value_free(secondValue);
                        return NULL;
                    }
                    double res = Value_number(firstValue) - Value_number(secondValue);
                    Value_free(firstValue);
                    Value_free(secondValue);
                    return Value_newNumber(res);
                }
            }
            return Value_newUndefined();
        }
        case AstNodeType_MUL: {
            if (List_count(node->children) != 0) {
                Tree * firstChild = List_at(node->children, 0);
                Value * firstValue = eval(program, firstChild);
                if (program->error) return NULL;
                if (firstValue->type != ValueType_NUMBER) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    return NULL;
                }
                Tree * secondChild = List_at(node->children, 1);
                Value * secondValue = eval(program, secondChild);
                if (program->error) return NULL;
                if (secondValue->type != ValueType_NUMBER) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    Value_free(secondValue);
                    return NULL;
                }
                double res = Value_number(firstValue) * Value_number(secondValue);
                Value_free(firstValue);
                Value_free(secondValue);
                return Value_newNumber(res);
            }
            return Value_newUndefined();
        }
        case AstNodeType_DIV: {
            if (List_count(node->children) != 0) {
                Tree * firstChild = List_at(node->children, 0);
                Value * firstValue = eval(program, firstChild);
                if (program->error) return NULL;
                if (firstValue->type != ValueType_NUMBER) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    return NULL;
                }
                Tree * secondChild = List_at(node->children, 1);
                Value * secondValue = eval(program, secondChild);
                if (program->error) return NULL;
                if (secondValue->type != ValueType_NUMBER) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    Value_free(secondValue);
                    return NULL;
                }
                if (fabs(Value_number(secondValue)) < 1e-6) {
                    program->error = strdup("Invalid operation");
                    Value_free(firstValue);
                    Value_free(secondValue);
                    return NULL;
                }
                double res = Value_number(firstValue) / Value_number(secondValue);
                Value_free(firstValue);
                Value_free(secondValue);
                return Value_newNumber(res);
            }
            return Value_newUndefined();
        }
        case AstNodeType_AND: {
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if (program->error) return NULL;
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if (program->error) return NULL;
            bool res = Value_asBool(firstValue) && Value_asBool(secondValue);
            Value_free(firstValue);
            Value_free(secondValue);
            return Value_newBool(res);
        }
        case AstNodeType_OR: {
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if (program->error) return NULL;
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if (program->error) return NULL;
            bool res = Value_asBool(firstValue) || Value_asBool(secondValue);
            Value_free(firstValue);
            Value_free(secondValue);
            return Value_newBool(res);
        }
        case AstNodeType_EQUAL: {
            bool eq = Ast_eq(program, node, Value_equals);
            return Value_newBool(eq);
        }
        case AstNodeType_NOTEQUAL: {
            bool eq = Ast_eq(program, node, Value_equals);
            return Value_newBool(!eq);
        }
        case AstNodeType_LESS: {
            bool eq = Ast_eq(program, node, Value_compare);
            return Value_newBool(eq);
        }
        case AstNodeType_MORE: {
            bool eq = Ast_eq(program, node, Value_compare);
            return Value_newBool(!eq);
        }
        case AstNodeType_LESS_OR_EQUAL: {
            bool eq = Ast_eq(program, node, Value_compare_ap);
            return Value_newBool(eq);
        }
        case AstNodeType_MORE_OR_EQUAL: {
            bool eq = Ast_eq(program, node, Value_compare_ap);
            return Value_newBool(!eq);
        }
        case AstNodeType_ASSIGN: {
            Tree * firstChildNode = List_at(node->children, 0);
            AstNode * firstChild = firstChildNode->value;
            if (firstChild->type != AstNodeType_ID) {
                program->error = strdup("Can't assign to rvalue");
                return NULL;
            }
            char * varId = (char *)firstChild->name;
            //
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if (program->error) return NULL;
            //
            if (!Dict_contains(program->variables, varId)) {
                program->error = strdup("Var for assign not found");
                Value_free(secondValue);
                return NULL;
            }
            if (secondValue->type == ValueType_ARRAY) {
                program->error = strdup("Assign array");
                Value_free(secondValue);
                return NULL;
            }
            Value * oldValue = Dict_set(program->variables, varId, Value_newCopy(secondValue));
            Value_free(oldValue);

            return secondValue;
            break;
        }
        default : {
            assert(0 && "Not implemented");
            break;
        }
    }
    return NULL;
}

void executeStatement(Program * program, Tree * childNode);


void executeIf(Program * program, Tree * node) {
    Tree * exprNode = List_at(node->children, 0);
    Value * testValue = eval(program, exprNode);
    if (program->error) return;
    bool testBool = Value_asBool(testValue);
    Value_free(testValue);
    if (testBool) {
        executeStatement(program, List_at(node->children, 1));
    } else if (List_count(node->children) > 2) {
        executeStatement(program, List_at(node->children, 2));
    } 
}

void executeWhile(Program * program, Tree * node) {
    Tree * exprNode = List_at(node->children, 0);
    while (true) {
        Value * testValue = eval(program, exprNode);
        bool testBool = Value_asBool(testValue);
        Value_free(testValue);
        if (!testBool) break;
        //
        executeStatement(program, List_at(node->children, 1));
        if (program->error) return;
    }
}

void executeBlock(Program * program, Tree * blockTreeNode) {
    for (int i = 0; i < List_count(blockTreeNode->children); i++) {
        Tree * childNode = List_at(blockTreeNode->children, i);
        AstNode * child = childNode->value;
        //
        executeStatement(program, childNode);
        if (program->error) break;
    }
}

void executeStatement(Program * program, Tree * childNode) {
    AstNode * child = childNode->value;
    switch(child->type) {
        case AstNodeType_IF: {
            executeIf(program, childNode);
            if (program->error) return;
            break;
        }
        case AstNodeType_WHILE: {
            executeWhile(program, childNode);
            if (program->error) return;
            break;
        }
        case AstNodeType_BLOCK: {
            executeBlock(program, childNode);
            if (program->error) return;
            break;
        }
        default: {
            Value * val = eval(program, childNode); 
            if (program->error) return;
            // Value_print(val, program->file);
            // puts("");
            Value_free(val);
        }
    }
}

int Interpreter_execute(Tree * astTree, FILE * file) {
    AstNode * astNode = astTree->value;
    assert(astNode->type == AstNodeType_PROGRAM);
    Program program = {
        .variables = Dict_new(CleaningType_VARIABLES),
        .functions = Dict_new(CleaningType_FUNCTION),
        .file = file,
        .error = NULL
    };
    Dict_add(program.functions, "print", StdFunction_new(std_print));
    Dict_add(program.functions, "strlen", StdFunction_new(std_strlen));
    Dict_add(program.functions, "sqrt", StdFunction_new(std_sqrt));
    Dict_add(program.functions, "pow", StdFunction_new(std_pow));
    Dict_add(program.functions, "scan", StdFunction_new(std_scan));
    Dict_add(program.functions, "strtonum", StdFunction_new(std_strtonum));
    Dict_add(program.functions, "numtostr", StdFunction_new(std_numtostr));
    Dict_add(program.functions, "push", StdFunction_new(std_push));
    Dict_add(program.functions, "count", StdFunction_new(std_count));
    Dict_add(program.functions, "at", StdFunction_new(std_at));
    Dict_add(program.functions, "slice", StdFunction_new(std_slice));
    for (int i = 0; i < List_count(astTree->children); i++) {
        Tree * childNode = List_at(astTree->children, i);
        AstNode * child = childNode->value;
        //
        switch(child->type) {
            case AstNodeType_DECLAREVAR: {
                if (program.error) break;
                Tree * fChildNode = List_at(childNode->children, 0);
                AstNode * fChild = fChildNode->value;
                Tree * sChildNode = List_at(childNode->children, 1);
                AstNode * sChild = sChildNode->value;
                //
                char * varName = strdup((char *)fChild->name);
                
                Value * varValue = eval(&program, sChildNode);
                if (program.error) break;
                //
                if (Dict_contains(program.variables, varName)) {
                    program.error = strdup("Duplicate variable id");
                    Value_free(varValue);
                    free(varName);
                    break;
                }
                Dict_add(program.variables, varName, varValue);
                break;
            }
            case AstNodeType_IF: {
                executeIf(&program, childNode);
                if (program.error) break;
                break;
            }
            case AstNodeType_WHILE: {
                executeWhile(&program, childNode);
                if (program.error) break;
                break;
            }
            case AstNodeType_BLOCK: {
                executeBlock(&program, childNode);
                if (program.error) break;
                break;
            }
            default: {
                Value * val = eval(&program, childNode); 
                if (program.error) break;
                // Value_print(val, program.file);
                // puts("");
                Value_free(val);
            }
        }
    }
    //
    List * keys = List_new();
    Dict_keys(program.variables, keys);
    for (int i = 0; i < List_count(keys); i++) {
        char * key = List_at(keys, i);
        Value * val = Dict_get(program.variables, key);
        // fprintf(program.file, "{'%s': ", key);
        // Value_print(val, program.file);
        Value_free(val);
        // fprintf(program.file, "}\n");
    }
    List_free(keys);
    // @todo free all dict values 
    //
    List * list = List_new();
    Dict_values(program.functions, list);
    List_free(list);
    
    Dict_free(program.variables);
    Dict_free(program.functions);
    if (program.error) {
        fprintf(stderr, "Run-time error: %s\n", program.error);
        free(program.error);
        return 1;
    }
    return 0;
}

void Interpreter_releaseTheTree(Tree * root) {
    if (!root) return;

    AstNode_free(root->value);

    size_t count = List_count(root->children);
    for (int i = 0; i < count; i++) {
        Interpreter_releaseTheTree(List_at(root->children, i));
    }

    List_free(root->children);
    free(root);
}
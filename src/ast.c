#include<ast.h>
#include <stdlib.h>
#include <stdio.h>

AstNode * AstNode_new(AstNodeType type, char * name) {
    AstNode * self = malloc(sizeof(AstNode));
    self->type = type;
    self->name = name;
    return self; 
}

void AstNode_free(AstNode * self) {
    if (self == NULL) return;
    if (self->type != AstNodeType_UNKNOWN
        && self->type != AstNodeType_ARGLIST
        && self->type != AstNodeType_BLOCK
        && self->type != AstNodeType_IF
        && self->type != AstNodeType_WHILE
        && self->type != AstNodeType_DECLAREVAR
        && self->type != AstNodeType_PROGRAM) {
        free(self->name);
    }
    free(self);
}
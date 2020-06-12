#pragma once

typedef enum {
   AstNodeType_UNKNOWN,
   //
   AstNodeType_ASSIGN,
   AstNodeType_ADD,
   AstNodeType_SUB,
   AstNodeType_MUL,
   AstNodeType_DIV,
   AstNodeType_MOD,
   AstNodeType_EQUAL,
   AstNodeType_NOTEQUAL,
   AstNodeType_NOT,
   AstNodeType_MORE,
   AstNodeType_LESS,
   AstNodeType_MORE_OR_EQUAL,
   AstNodeType_LESS_OR_EQUAL,
   AstNodeType_AND,
   AstNodeType_OR,
   //...
   AstNodeType_NUMBER,
   AstNodeType_STRING,
   AstNodeType_ID,
   AstNodeType_BOOL,
   //
   AstNodeType_ARGLIST,
   //
   AstNodeType_BLOCK,
   AstNodeType_IF,
   AstNodeType_WHILE,
   AstNodeType_DECLAREVAR,
   //
   AstNodeType_PROGRAM,
} AstNodeType;

typedef struct __AstNode AstNode;
struct __AstNode {
   AstNodeType type;
   char * name;
   // @todo extra data
};

AstNode * AstNode_new(AstNodeType type, char * name);
void AstNode_free(AstNode * self);

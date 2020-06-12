#pragma once

#include<list.h>

typedef enum {
    TokenType_PLUS,
    TokenType_MINUS,
    TokenType_MULT,
    TokenType_DIV,
    TokenType_MOD,
    TokenType_AND,
    TokenType_OR,
    TokenType_NOT,
    TokenType_EQUAL,
    TokenType_ASSIGN,
    TokenType_NOTEQUAL,
    TokenType_MORE,
    TokenType_LESS,
    TokenType_LESS_OR_EQUAL,
    TokenType_MORE_OR_EQUAL,
    TokenType_LEFT_BRACKET,
    TokenType_RIGHT_BRACKET,
    TokenType_SEMICOLON,
    TokenType_LEFT_BRACE,
    TokenType_RIGHT_BRACE,
    TokenType_LEFT_SQUARE_BRACKET,
    TokenType_RIGHT_SQUARE_BRACKET,
    //////////////////
    TokenType_NUMBER,
    TokenType_STRING,
    TokenType_BOOL,
    TokenType_NULL,
    //////////////////
    TokenType_LET,
    TokenType_WHILE,
    TokenType_IF,
    TokenType_ELSE,
    TokenType_ID,
    //////////////////
    TokenType_SLASHN,
    TokenType_COMMA,
    TokenType_COMMENT,
    TokenType_ABSENT,
} TokenType;



typedef struct __Token Token;
struct __Token {
   TokenType type;
   char * name;
};

Token * Token_new(TokenType type, char * name);
void Token_free(Token * self);
int Token_equals(Token * token_1,Token * token_2);

char * TokenType_toString(TokenType type);


int Lexer_splitTokens(const char * input, List * tokens);
void Lexer_clearTokens(List * tokens);

void Lexer_printTokens(List * tokens,FILE * filestream);
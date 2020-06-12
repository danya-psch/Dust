#include<lexer.h>
#include<string.h>
#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef enum {
    TokenGroup_NAME,
    TokenGroup_STRING,
    TokenGroup_NUMERAL,
    TokenGroup_ABSENT
} TokenGroup;

static int skipSpaces(const char * input) {
    int n = 0;
    while(isspace(*input) || *input == '\n') {
        input++;
        n++;
    }
    return n;
}

Token * Token_new(TokenType type, char * name) {
    // Token * temp = NULL;
    Token * temp = (Token *)malloc(sizeof(Token));
    temp->name = (char *)malloc(sizeof(char) * strlen(name) + 1);
    strcpy(temp->name, name);
    temp->type = type;
    return temp;
}

void Token_free(Token * self) {
    if (self->name) {
        free(self->name);
    }
    free(self);
}

int Lexer_splitTokens(const char * input, List * tokens) {

    if (input == NULL) return EXIT_FAILURE;
    int n = 0;
    TokenType type = TokenType_ABSENT;
    enum {BUF_LEN = 100};
    char buf[BUF_LEN] = "";
    bool isName = false; 
    TokenGroup group = TokenGroup_ABSENT;

    while (*input != '\0') {
        if (group != TokenGroup_STRING) {
            int quantityOfSpaces = skipSpaces(input);
            if (n != 0 && quantityOfSpaces != 0 && type == TokenType_ABSENT) return EXIT_FAILURE;
            input += quantityOfSpaces;
        }
        if (*input == '\0') break;

        buf[n] = *input;
        if (n == 0 && (isupper(buf[n]) || islower(buf[n]) || buf[n] == '_') && group == TokenGroup_ABSENT && buf[n] != '\n') {
            group = TokenGroup_NAME;
        } else if (n == 0 && isdigit(buf[n]) && group == TokenGroup_ABSENT) { 
            group = TokenGroup_NUMERAL;
        } else if (n == 0 && buf[n] == '\"' && group == TokenGroup_ABSENT) { 
            group = TokenGroup_STRING;
            n--;
        }

        input++;
        n++;

        if (n == 1 && buf[0] == '#'){
            while (*input != '\n') {
                input++;
            }
            n = 0;
            buf[0] = '\0';
        }

        if (group == TokenGroup_NAME) {
            if (0 == strcmp(buf, "let")) {
                type = TokenType_LET;
            } else if (0 == strcmp(buf, "while")) {
                type = TokenType_WHILE;
            } else if (0 == strcmp(buf, "if")) {
                type = TokenType_IF;
            } else if (0 == strcmp(buf, "else")) {
                type = TokenType_ELSE;
            } else if (0 == strcmp(buf, "false") || 0 == strcmp(buf, "true")) {
                type = TokenType_BOOL;
            } else if (!isalnum(*input) && *input != '_') {
                type = TokenType_ID;
            }
        } else if (group == TokenGroup_NUMERAL){
            if (!isdigit(*input) && *input != '.') {
                type = TokenType_NUMBER;
            }
        } else if (group == TokenGroup_STRING) {
            if (n != 1 && buf[n - 1] == '\"' && buf[n - 2] != (char)92) {
                type = TokenType_STRING;
            }
            if (n == 1 && buf[n - 1] == '\"') {
                type = TokenType_STRING;
            }
        } else {
            if (0 == strcmp(buf, "+")) {
                type = TokenType_PLUS;
            } else if (0 == strcmp(buf, "-")) {
                type = TokenType_MINUS;
            } else if (0 == strcmp(buf, "*")) {
                type = TokenType_MULT;
            } else if (0 == strcmp(buf, "/")) {
                type = TokenType_DIV;
            } else if (0 == strcmp(buf, "%")) {
                type = TokenType_MOD;
            } else if (0 == strcmp(buf, "&&")) {
                type = TokenType_AND;
            } else if (0 == strcmp(buf, "||")) {
                type = TokenType_OR;
            } else if (0 == strcmp(buf, "!") && !ispunct(*input)) {
                type = TokenType_NOT;
            } else if (0 == strcmp(buf, "!=")) {
                type = TokenType_NOTEQUAL;
            } else if (0 == strcmp(buf, "=") && !ispunct(*input)) {
                type = TokenType_ASSIGN;
            } else if (0 == strcmp(buf, "==")) {
                type = TokenType_EQUAL;
            } else if (0 == strcmp(buf, ">") && !ispunct(*input)) {
                type = TokenType_MORE;
            } else if (0 == strcmp(buf, "<") && !ispunct(*input)) {
                type = TokenType_LESS;
            } else if (0 == strcmp(buf, ">=")) {
                type = TokenType_MORE_OR_EQUAL;
            } else if (0 == strcmp(buf, "<=")) {
                type = TokenType_LESS_OR_EQUAL;
            } else if (0 == strcmp(buf, "(")) {
                type = TokenType_LEFT_BRACKET;
            } else if (0 == strcmp(buf, ")")) {
                type = TokenType_RIGHT_BRACKET;
            } else if (0 == strcmp(buf, ";")) {
                type = TokenType_SEMICOLON;
            } else if (0 == strcmp(buf, "{")) {
                type = TokenType_LEFT_BRACE;
            } else if (0 == strcmp(buf, "}")) {
                type = TokenType_RIGHT_BRACE;
            } else if (0 == strcmp(buf, "[")) {
                type = TokenType_LEFT_SQUARE_BRACKET;
            } else if (0 == strcmp(buf, "]")) {
                type = TokenType_RIGHT_SQUARE_BRACKET;
            } else if (0 == strcmp(buf, ",")) {
                type = TokenType_COMMA;
            } else if (buf[0] == '#' && *input == '\n') {
                type = TokenType_COMMENT;
            } 
            // else if (buf[0] == '\'' && buf[2] == '\'') {
            //     type = TokenType_COMMENT;
            // }
        }

        if (type != TokenType_ABSENT) {
            if (type == TokenType_STRING) buf[n-1] = '\0';
            Token *temp = Token_new(type, buf);
            List_add(tokens,temp);
            n = 0;
            type = TokenType_ABSENT;
            group = TokenGroup_ABSENT;
            for (int i = 0;i < BUF_LEN;i++) {
                buf[i] = '\0';
            }
        }
    }

    if (n != 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void Lexer_clearTokens(List *tokens) {
    int length = List_count(tokens);
    while (length > 0) {
        Token * cur = (Token*)List_removeAt(tokens, 0);
        Token_free(cur);
        length--;
    }
}


void Lexer_printTokens(List * tokens,FILE * filestream) {

    if (tokens == NULL || List_count(tokens) == 0) return;
    Iterator * begin = List_getNewBeginIterator(tokens);
    Iterator * end = List_getNewEndIterator(tokens);
    TokenType type;
    while (!Iterator_equals(begin,end)) {
        Token * token = Iterator_value(begin);
        if (token->type == TokenType_ID) fprintf(filestream, "<ID,%s>",token->name);
        else if (token->type == TokenType_STRING) fprintf(filestream, "<STR,\"%s\">",token->name);
        else if (token->type == TokenType_COMMENT) fprintf(filestream, "<COMM,%s>",token->name);
        else if (token->type == TokenType_NUMBER) fprintf(filestream, "<NUM,%s>",token->name);
        else if (token->type == TokenType_BOOL) fprintf(filestream, "<BOOL,%s>",token->name);
        else if (token->type == TokenType_LEFT_BRACE) {
            Iterator_next(begin);
            type = ((Token*)Iterator_value(begin))->type;
            Iterator_prev(begin);
            if (type == TokenType_RIGHT_BRACE) fprintf(filestream, "<%s>",token->name);
            else fprintf(filestream, "<%s>\n",token->name);
        } else if (token->type == TokenType_LEFT_BRACE) {
            Iterator_prev(begin);
            type = ((Token*)Iterator_value(begin))->type;
            Iterator_next(begin);
            if (type == TokenType_LEFT_BRACE) fprintf(filestream, "<%s>",token->name);
            else fprintf(filestream, "<%s>\n",token->name);
        } else if (token->type == TokenType_SEMICOLON) fprintf(filestream, "<%s>\n",token->name);
        else fprintf(filestream, "<%s>",token->name);
        Iterator_next(begin);
    }

    Iterator_free(begin);
    Iterator_free(end);
}

int Token_equals(Token * token_1,Token * token_2) {
    if (token_1->type == token_2->type && strcmp(token_1->name,token_2->name)) return 0;
    return 1;
}

char * TokenType_toString(TokenType type) {
    switch (type) {
        case TokenType_PLUS :                 return "+";
        case TokenType_MINUS :                return "-";
        case TokenType_MULT :                 return "*";
        case TokenType_DIV :                  return "/";
        case TokenType_MOD :                  return "%";
        case TokenType_AND :                  return "&&";
        case TokenType_OR :                   return "||";
        case TokenType_NOT :                  return "!";
        case TokenType_EQUAL :                return "==";
        case TokenType_ASSIGN :               return "=";
        case TokenType_NOTEQUAL :             return "!=";
        case TokenType_MORE :                 return ">";
        case TokenType_LESS :                 return "<";
        case TokenType_LESS_OR_EQUAL :        return ">=";
        case TokenType_MORE_OR_EQUAL :        return "<=";
        case TokenType_LEFT_BRACKET :         return "(";
        case TokenType_RIGHT_BRACKET :        return ")";
        case TokenType_SEMICOLON :            return ";";
        case TokenType_LEFT_BRACE :           return "{";
        case TokenType_RIGHT_BRACE :          return "}";
        case TokenType_LEFT_SQUARE_BRACKET :  return "[";
        case TokenType_RIGHT_SQUARE_BRACKET : return "]";
        case TokenType_NUMBER :               return "NUMBER";
        case TokenType_STRING :               return "STRING";
        case TokenType_BOOL :                 return "BOOL";
        case TokenType_NULL :                 return "NULL";
        case TokenType_LET :                  return "LET";
        case TokenType_WHILE :                return "WHILE";
        case TokenType_IF :                   return "IF";
        case TokenType_ELSE :                 return "ELSE";
        case TokenType_ID :                   return "ID";
        case TokenType_SLASHN :               return "SN";
        case TokenType_COMMA :                return ",";
        case TokenType_COMMENT :              return "#.....";
        case TokenType_ABSENT :               return " ";
        default :                             return "";
    }
}
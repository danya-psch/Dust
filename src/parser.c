#include<list.h>
#include<parser.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<stdbool.h>
#include<lexer.h>
#include<iterator.h>
#include<string_buffer.h>
#include<string.h>
#include<string_dup.h>
#include<ast.h>

typedef struct {
    Iterator * tokens;
    Iterator * tokensEnd;
    char * error;
    int level;
} Parser;

static Tree * accept(Parser * self, TokenType tokenType);
static Tree * expect(Parser * parser, TokenType tokenType);

static bool bool_accept(Parser * self, TokenType tokenType);    
static bool bool_expect(Parser * parser, TokenType tokenType);
static bool bool_reject(Parser * parser, TokenType tokenType);

static Tree * ID           (Parser * parser);
static Tree * NUMBER       (Parser * parser);
static Tree * STRING       (Parser * parser);
static Tree * BOOL         (Parser * parser);
static Tree * ID_expect    (Parser * parser);
static Tree * NUMBER_expect(Parser * parser);
static Tree * STRING_expect(Parser * parser);
static Tree * prog         (Parser * parser);
static Tree * var_decl     (Parser * parser);
static Tree * st           (Parser * parser);
static Tree * expr         (Parser * parser);
static Tree * expr_st      (Parser * parser);
static Tree * block_st     (Parser * parser);
static Tree * select_st    (Parser * parser);
static Tree * iter_st      (Parser * parser);
static Tree * assign       (Parser * parser);
static Tree * assign_ap    (Parser * parser);
static Tree * log_or       (Parser * parser);
static Tree * log_or_ap    (Parser * parser);
static Tree * log_and      (Parser * parser);
static Tree * log_and_ap   (Parser * parser);
static Tree * eq           (Parser * parser);
static Tree * eq_ap        (Parser * parser);
static Tree * rel          (Parser * parser);
static Tree * rel_ap       (Parser * parser);
static Tree * add          (Parser * parser);
static Tree * add_ap       (Parser * parser);
static Tree * mult         (Parser * parser);
static Tree * mult_ap      (Parser * parser);
static Tree * unary        (Parser * parser);
static Tree * primary      (Parser * parser);
static Tree * var_or_call  (Parser * parser);
static Tree * parentheses  (Parser * parser);
static Tree * fn_call      (Parser * parser);
static Tree * arg_list     (Parser * parser);
static Tree * data         (Parser * parser);
static Tree * arr_ini      (Parser * parser);

//
void Parser_releaseTheTree(Tree * root) {
    if (!root) return;

    AstNode_free(root->value);

    size_t count = List_count(root->children);
    for (int i = 0; i < count; i++) {
        Parser_releaseTheTree(List_at(root->children, i));
    }

    List_free(root->children);
    free(root);
}

Tree * Parser_buildNewAstTree(List * tokens) {
    if (tokens == NULL || List_count(tokens) == 0) return NULL;
    
    Parser parser = {
        .tokens = List_getNewBeginIterator(tokens),
        .tokensEnd = List_getNewEndIterator(tokens),
        .error = NULL,
        .level = -1
    };
    Tree * progNode = prog(&parser);
    if (parser.error) {
        fprintf(stderr, ">>> ERROR: %s\n", parser.error);
        Iterator_free(parser.tokens);
        Iterator_free(parser.tokensEnd);
        free((void *)parser.error);
        return NULL;
    } else {
        if (!Iterator_equals(parser.tokens, parser.tokensEnd)) {
            Token * token = Iterator_value(parser.tokens); 
            fprintf(stderr, ">>> ERROR: unexpected token '%s'.\n", TokenType_toString(token->type));
            Iterator_free(parser.tokens);
            Iterator_free(parser.tokensEnd);
            return NULL;
        }
    }
    Iterator_free(parser.tokens);
    Iterator_free(parser.tokensEnd);
    return progNode;
}

//

static bool eoi(Parser * self) {
    return Iterator_equals(self->tokens, self->tokensEnd);
}

#define TRACE_EXPECT(TYPE) \
    ;

// traceExpect(parser, TYPE);

#define TRACE_CALL() \
    parser->level++; \
    Parser * _parserPtr __attribute__((cleanup(Parcer_decLevel))) = parser;

// trace(parser, __func__); 

void Parcer_decLevel(Parser ** parserPtr) {
    (*parserPtr)->level--;
}

static void traceLevel(int level) {
    for (int i = 0; i < level; i++) {
        putchar('.');
        putchar('.');
    }
}

static void traceExpect(Parser * parser, TokenType expectedType) {
    traceLevel(parser->level);
    printf("<%s>\n", TokenType_toString(expectedType));
}

static void trace(Parser * parser, const char * fn) {
    traceLevel(parser->level);
    if (eoi(parser)) {
        printf("%s: <EOI>\n", fn);
        return;
    }
    Token * token = Iterator_value(parser->tokens);
    TokenType type = token->type;
    switch(type) {
        case TokenType_ID : case TokenType_NUMBER : case TokenType_STRING : {
            printf("%s: <%s,%s>\n", fn, TokenType_toString(type), token->name);
            break;
        }
        default : {
            printf("%s: <%s>\n", fn, TokenType_toString(type));
            break;
        }
    }
}

//

static AstNodeType TokenType_toAstNodeType(TokenType type) {
    switch(type) {
        case TokenType_ASSIGN : return AstNodeType_ASSIGN;
        case TokenType_PLUS : return AstNodeType_ADD;
        case TokenType_MINUS : return AstNodeType_SUB;
        case TokenType_MULT : return AstNodeType_MUL;
        case TokenType_DIV : return AstNodeType_DIV;
        case TokenType_MOD : return AstNodeType_MOD;
        case TokenType_EQUAL : return AstNodeType_EQUAL;
        case TokenType_NOTEQUAL : return AstNodeType_NOTEQUAL;
        case TokenType_NOT : return AstNodeType_NOT;
        case TokenType_MORE : return AstNodeType_MORE;
        case TokenType_LESS : return AstNodeType_LESS;
        case TokenType_MORE_OR_EQUAL : return AstNodeType_MORE_OR_EQUAL;
        case TokenType_LESS_OR_EQUAL : return AstNodeType_LESS_OR_EQUAL;
        case TokenType_AND : return AstNodeType_AND;
        case TokenType_OR : return AstNodeType_OR;
        //
        case TokenType_ID : return AstNodeType_ID;
        case TokenType_NUMBER : return AstNodeType_NUMBER;
        case TokenType_STRING : return AstNodeType_STRING;
        case TokenType_BOOL : return AstNodeType_BOOL;
        //
        default : return AstNodeType_UNKNOWN;
    }
}

static Tree * accept(Parser * self, TokenType tokenType) {
    if (eoi(self)) return NULL;
    Token * token = Iterator_value(self->tokens);
    if (token->type == tokenType) {
        AstNodeType astType = TokenType_toAstNodeType(tokenType);
        char * astName = strdup((char *)token->name);
        AstNode * node = AstNode_new(astType, astName);
        Tree * tree = Tree_new(node);
        Iterator_next(self->tokens);
        return tree;
    }
    return NULL;
}

static bool bool_reject(Parser * parser, TokenType tokenType) {
    if (eoi(parser)) return false;
    Token * token = Iterator_value(parser->tokens);
    if (token->type != tokenType) {
        return false;
    }
    parser->error = strdup("troubles with the array");
    return true;
}

static Tree * expect(Parser * parser, TokenType tokenType) {
    Tree * tree = accept(parser, tokenType);
    if (tree != NULL) {
        TRACE_EXPECT(tokenType);
        return tree;
    }
    char * currentTokenType = eoi(parser) 
        ? "end-of-input"
        : TokenType_toString(((Token *)Iterator_value(parser->tokens))->type);

    StringBuffer * sb = StringBuffer_new();
    StringBuffer_appendFormat(sb, "expected '%s' got '%s'.\n", TokenType_toString(tokenType),  currentTokenType);
    parser->error = StringBuffer_toNewString(sb);
    StringBuffer_free(sb);
    return NULL;
}

static bool bool_accept(Parser * self, TokenType tokenType) {
    if (eoi(self)) return false;
    Token * token = Iterator_value(self->tokens);
    
    if (token->type == tokenType) {
        Iterator_next(self->tokens);
        return true;
    }
    return false;
}

static bool bool_expect(Parser * parser, TokenType tokenType) {
    if (bool_accept(parser, tokenType)) {
        TRACE_EXPECT(tokenType);
        return true;
    }
    char * currentTokenType = eoi(parser) 
        ? "end-of-input"
        : TokenType_toString(((Token *)Iterator_value(parser->tokens))->type);

    StringBuffer * sb = StringBuffer_new();
    StringBuffer_appendFormat(sb, "expected '%s' got '%s'.\n", TokenType_toString(tokenType),  currentTokenType);
    parser->error = StringBuffer_toNewString(sb);
    StringBuffer_free(sb);
    return false;
}

typedef Tree * (*GrammarRule)(Parser * parser);

static bool ebnf_multiple(Parser * parser, List * nodes, GrammarRule rule) {
    Tree * node = NULL;
    while((node = rule(parser)) && !parser->error) {
        if (node == NULL) return false;
        List_add(nodes, node);
    }
    return parser->error == NULL ? true : false;
}

static Tree * ebnf_select(Parser * parser, GrammarRule rules[], size_t rulesLen) {
    Tree * node = NULL;
    for (int i = 0; i < rulesLen && !node; i++) {
        GrammarRule rule = rules[i];
        node = rule(parser);
        if (parser->error) {
            Parser_releaseTheTree(node);
            return NULL;
        }
    }
    return node;
}

static Tree * ebnf_selectLexems(Parser * parser, TokenType types[], size_t typesLen) {
    Tree * node = NULL;
    for (int i = 0; i < typesLen && !node; i++) {
        node = accept(parser, types[i]);
    }
    return node;
}

static Tree * ebnf_ap_main_rule(Parser * parser, GrammarRule next, GrammarRule ap) {
    Tree * nextNode = next(parser);
    if (nextNode) {
        Tree * apNode = ap(parser);
        if (apNode) {
            List_insert(apNode->children, nextNode, 0);
            return apNode;
        }
        return nextNode;
    }
    return NULL;
}

static Tree * ebnf_ap_recursive_rule(Parser * parser, TokenType types[], size_t typesLen, GrammarRule next, GrammarRule at) {
    
    Tree * opNode = ebnf_selectLexems(parser, types, typesLen);
    if (opNode == NULL) return NULL;
    Tree * node = NULL;
    Tree * nextNode = next(parser);
    Tree * apNode = at(parser);
    if (apNode) {
        List_insert(apNode->children, nextNode, 0);
        node = apNode;
    } else {
        node = nextNode;
    }   
    List_add(opNode->children, node);
    return opNode;
}

//

static Tree * ID         (Parser * parser) {
    TRACE_CALL();
    return accept(parser, TokenType_ID);
}
static Tree * NUMBER     (Parser * parser) {
    TRACE_CALL();
    return accept(parser, TokenType_NUMBER);
}
static Tree * STRING     (Parser * parser) {
    TRACE_CALL();
    return accept(parser, TokenType_STRING);
}
static Tree * BOOL     (Parser * parser) {
    TRACE_CALL();
    return accept(parser, TokenType_BOOL);
}
static Tree * ID_expect         (Parser * parser) {
    Tree * idNode = expect(parser, TokenType_ID);
    if (bool_accept(parser, TokenType_LEFT_SQUARE_BRACKET)) {
        if (!bool_expect(parser, TokenType_RIGHT_SQUARE_BRACKET)) {
            Parser_releaseTheTree(idNode);
            return NULL;
        }
    }
    return idNode;
}

static Tree * prog       (Parser * parser) {
    TRACE_CALL();
    Tree * progNode = Tree_new(AstNode_new(AstNodeType_PROGRAM, "program"));
    if (!ebnf_multiple(parser, progNode->children, var_decl)) {
        Parser_releaseTheTree(progNode);
        return NULL;
    }
    if (!ebnf_multiple(parser, progNode->children, st))  {
        Parser_releaseTheTree(progNode);
        return NULL;
    }
    return progNode;
}
static Tree * data       (Parser * parser) {
    TRACE_CALL();
    return ebnf_select(parser,
    (GrammarRule[]) {
        arr_ini,
        expr
    }, 2);
}
static Tree * arr_ini    (Parser * parser) {
    TRACE_CALL();
    if (!bool_accept(parser, TokenType_LEFT_BRACE)) return NULL;
    Tree * argListNode = arg_list(parser);
    bool_expect(parser, TokenType_RIGHT_BRACE); // @todo mb 
    return argListNode;
}
static Tree * var_decl   (Parser * parser) {
    TRACE_CALL();
    if (!bool_accept(parser, TokenType_LET)) return NULL;
    Tree * idNode = ID_expect(parser);
    if (!idNode) return NULL;
    if (!bool_expect(parser, TokenType_ASSIGN)) {
        Parser_releaseTheTree(idNode);
        // parser->error = strdup("ERROR");
        // @todo error
        return NULL;
    }
    // Tree * exprNode = expr(parser);
    Tree * dataNode = data(parser);
    if (dataNode == NULL) {
        Parser_releaseTheTree(idNode);
        Parser_releaseTheTree(dataNode);
        
        // @todo error
        return NULL;
    } 
    if (!bool_expect(parser, TokenType_SEMICOLON)) {
        Parser_releaseTheTree(idNode);
        Parser_releaseTheTree(dataNode);
        // @todo error
        return NULL;
    }

    Tree * varDecl = Tree_new(AstNode_new(AstNodeType_DECLAREVAR, "declareVar"));

    List_add(varDecl->children, idNode);
    List_add(varDecl->children, dataNode);
    return varDecl;
}
static Tree * st         (Parser * parser) {
    TRACE_CALL();
    return ebnf_select(parser,
    (GrammarRule[]) {
        block_st,
        select_st,
        iter_st,
        expr_st 
    }, 4);
}
static Tree * expr       (Parser * parser) {
    TRACE_CALL();
    return assign(parser);
}
static Tree * expr_st    (Parser * parser) {
    TRACE_CALL();
    Tree * exprNode = expr(parser);
    if (exprNode) {
        if (!bool_expect(parser, TokenType_SEMICOLON)) {
            Parser_releaseTheTree(exprNode);
        }
    } else {
        bool_accept(parser, TokenType_SEMICOLON);
    }
    return exprNode;
}
static Tree * block_st   (Parser * parser){
    TRACE_CALL();
    if (!bool_accept(parser, TokenType_LEFT_BRACE)) return NULL;
    Tree * blockNode = Tree_new(AstNode_new(AstNodeType_BLOCK, "block"));
    if (ebnf_multiple(parser, blockNode->children, st)) {
        if (!bool_expect(parser, TokenType_RIGHT_BRACE)) {
            Parser_releaseTheTree(blockNode);
            return NULL;
        }
    }
    return blockNode;
}
static Tree * select_st  (Parser * parser) {
    TRACE_CALL();

    if (!bool_accept(parser, TokenType_IF)
        || !bool_expect(parser, TokenType_LEFT_BRACKET)) return NULL;
    Tree * testExprNode = expr(parser);
    if (!testExprNode) {
        Parser_releaseTheTree(testExprNode);
        // @todo error   
        return NULL;
    }
    if (!bool_expect(parser, TokenType_RIGHT_BRACKET)) {
        Parser_releaseTheTree(testExprNode);
        // @todo clear testExpr
        // @todo error   
        return NULL;
    }
    Tree * stNode = st(parser);
    if (stNode == NULL) {
        Parser_releaseTheTree(testExprNode);
        Parser_releaseTheTree(stNode);
        // @todo clear testExpr
        // @todo error 
        return NULL;
    }
    Tree * ifNode = Tree_new(AstNode_new(AstNodeType_IF, "if"));
    List_add(ifNode->children, testExprNode);
    List_add(ifNode->children, stNode);    

    if (bool_accept(parser, TokenType_ELSE)) {
        Tree * elseNode = st(parser);
        if (elseNode == NULL || parser->error) {
            Parser_releaseTheTree(elseNode);
            // @todo error, clear
            return NULL;
        }
        List_add(ifNode->children, elseNode); 
    }
    return ifNode;
}
static Tree * iter_st    (Parser * parser) {
    TRACE_CALL();
    if (!bool_accept(parser, TokenType_WHILE)
        || !bool_expect(parser, TokenType_LEFT_BRACKET)) return NULL;
    Tree * testExprNode = expr(parser);
    if (!testExprNode) {
        Parser_releaseTheTree(testExprNode);
        // @todo error   
        return NULL;
    }
    if (!bool_expect(parser, TokenType_RIGHT_BRACKET)) {
        Parser_releaseTheTree(testExprNode);
        // @todo clear testExpr
        // @todo error   
        return NULL;
    }
    Tree * stNode = st(parser);
    if (stNode == NULL) {
        Parser_releaseTheTree(testExprNode);
        Parser_releaseTheTree(stNode);
        // @todo clear testExpr
        // @todo error 
        return NULL;
    }
    Tree * whileNode = Tree_new(AstNode_new(AstNodeType_WHILE, "while"));
    List_add(whileNode->children, testExprNode);
    List_add(whileNode->children, stNode);    
    return whileNode;
}
static Tree * assign     (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, log_or, assign_ap);
}
static Tree * assign_ap  (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_ASSIGN
    }, 1,
    log_or, assign_ap);
}
static Tree * log_or     (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, log_and, log_or_ap);
}
static Tree * log_or_ap  (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_OR
    }, 1,
    log_and, log_or_ap);
}
static Tree * log_and    (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, eq, log_and_ap);
}
static Tree * log_and_ap (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_AND
    }, 1,
    eq, log_and_ap);
}
static Tree * eq         (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, rel, eq_ap);
}
static Tree * eq_ap      (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_EQUAL,
        TokenType_NOTEQUAL
    }, 2,
    rel, eq_ap);
}
static Tree * rel        (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, add, rel_ap);
}
static Tree * rel_ap     (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_MORE,
        TokenType_LESS,
        TokenType_MORE_OR_EQUAL,
        TokenType_LESS_OR_EQUAL
    }, 4,
    add, rel_ap);
}
static Tree * add        (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, mult, add_ap);
}
static Tree * add_ap     (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_PLUS,
        TokenType_MINUS
    }, 2,
    mult, add_ap);
}
static Tree * mult       (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_main_rule(parser, unary, mult_ap);
}
static Tree * mult_ap    (Parser * parser) {
    TRACE_CALL();
    return ebnf_ap_recursive_rule(parser,
    (TokenType[]) {
        TokenType_MULT,
        TokenType_DIV,
        TokenType_MOD
    }, 3,
    unary, mult_ap);
}
static Tree * unary      (Parser * parser) {
    TRACE_CALL();
    Tree * opNode = ebnf_selectLexems(parser, 
    (TokenType[]) {
        TokenType_PLUS,
        TokenType_MINUS,
        TokenType_NOT,
    }, 3);
    Tree * primNode = primary(parser);
    // @todo check NULL prim
    if (opNode) {
        List_add(opNode->children, primNode);
        return opNode;
    }
    return primNode;
}
static Tree * primary    (Parser * parser) {
    TRACE_CALL();
    return ebnf_select(parser,
    (GrammarRule[]) {
        NUMBER,
        STRING,
        BOOL,
        parentheses,
        var_or_call
    }, 5);
}

static Tree * var_or_call(Parser * parser) {
    TRACE_CALL();
    Tree * varNode = ID(parser);
    Tree * argListNode = fn_call(parser);
    if (argListNode) {
        List_add(varNode->children, argListNode);
    }
    return varNode;
}
static Tree * parentheses(Parser * parser) {
    TRACE_CALL();
    if (!bool_accept(parser, TokenType_LEFT_BRACKET)) return NULL;
    Tree * exprNode = expr(parser);
    bool_expect(parser, TokenType_RIGHT_BRACKET); // @todo mb 
    return exprNode;
}
static Tree * fn_call    (Parser * parser) {
    TRACE_CALL();
    if (bool_reject(parser, TokenType_LEFT_SQUARE_BRACKET)) return NULL;
    if (bool_reject(parser, TokenType_RIGHT_SQUARE_BRACKET)) return NULL;
    if (!bool_accept(parser, TokenType_LEFT_BRACKET)) return NULL;
    Tree * argListNode = arg_list(parser);
    bool_expect(parser, TokenType_RIGHT_BRACKET); // @todo mb 
    return argListNode;
}
static Tree * arg_list   (Parser * parser) {
    TRACE_CALL();
    Tree * exprNode = expr(parser);
    Tree * argListNode = Tree_new(AstNode_new(AstNodeType_ARGLIST, "arglist"));
    if (exprNode != NULL) {
        List_add(argListNode->children, exprNode);
        while(true) {
            if (!bool_accept(parser, TokenType_COMMA)) break;
            exprNode = expr(parser);
            if (exprNode) {
                List_add(argListNode->children, exprNode);
            } else {
                break;
            }
        }
    }
    return argListNode;
}

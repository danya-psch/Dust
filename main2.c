#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fs.h>
#include <list.h>
#include <lexer.h>
#include <iterator.h>
#include <parser.h>
#include <ast.h>
#include <print_pretty.h>
#include <interpreter.h>

void Tree_releaseTheTree(Tree *root);

int main(void) {
    const char *fileName = "../main.dpp"; 
    int len = getFileSize(fileName);
    char input[len + 1];
    readFileToBuffer(fileName, input, len + 1);
    input[len] = '\0';
    // puts(input);

    List *list = List_new();
    if (0 != Lexer_splitTokens(input, list)) {
        printf("!\n");
        Lexer_clearTokens(list);
        List_free(list);
        return EXIT_FAILURE;
    }
    
    // Lexer_printTokens(list, stdout);
    Tree *root = Parser_buildNewAstTree(list);
    if (!root) {
        Lexer_clearTokens(list);
        List_free(list);
        return EXIT_FAILURE;
    }
    
    FILE * filestreamForWrite = fopen("../main_ast.txt", "w");
    AstTree_prettyPrint(root, filestreamForWrite);

    Tree_releaseTheTree(root);
    fclose(filestreamForWrite);
    Lexer_clearTokens(list);
    List_free(list);

    return 0;
}

void Tree_releaseTheTree(Tree *root)
{
    if (!root)
        return;

    AstNode_free(root->value);

    size_t count = List_count(root->children);
    for (int i = 0; i < count; i++)
    {
        Tree_releaseTheTree(List_at(root->children, i));
    }

    List_free(root->children);
    free(root);
}

//

#include<print_pretty.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<string_dup.h>

static char * str_append(const char * str, const char * append);
static void PrintPretty(Tree * node, const char * indent, int root, int last, FILE * filestream);

void AstTree_prettyPrint(Tree * astTree,FILE * filestream) {
    char * indent = strdup("");
    PrintPretty(astTree, indent, 1, 1, filestream);
    free((void*)indent);
}

static char * str_append(const char * str, const char * append) {
    size_t newLen = strlen(str) + strlen(append) + 1;
    char * buf = malloc(sizeof(char) * newLen);
    buf[0] = '\0';
    sprintf(buf, "%s%s", str, append);
    return buf; 
}

static void PrintPretty(Tree * node, const char * indent, int root, int last, FILE * filestream) {
    fprintf(filestream, "%s", indent);
    char * newIndent = NULL;
    if (last) {
        if (!root) {
            fprintf(filestream, "└─");
            newIndent = str_append(indent, "••");
        } else {
            newIndent = str_append(indent, "");
        }
    } else {
        fprintf(filestream, "├─");
        newIndent = str_append(indent, "|•");
    }
    AstNode * astNode = (node->value);
    fprintf(filestream, "%s\n", astNode->name);
    List * children = (node->children);
    size_t count = List_count(children);
    for (int i = 0; i < count; i++) {
        void * child = List_at(children, i);
        PrintPretty(child, newIndent, 0, i == count - 1,  filestream);
    }
    free((void*)newIndent);
}
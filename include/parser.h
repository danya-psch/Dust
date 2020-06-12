#pragma once 

#include<list.h>
#include<tree.h>

bool Parser_match(List * tokens);

Tree * Parser_buildNewAstTree(List * tokens);
void Parser_releaseTheTree(Tree * root);
    

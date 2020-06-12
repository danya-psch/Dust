#include <check.h>
#include <parser.h>
#include <ast.h>
#include <lexer.h>
#include <string.h>
#include <string_dup.h>

START_TEST(buildNewAstTree_empty_satisfaction)
{
    List *tokens = List_new();
    Tree *program = Parser_buildNewAstTree(tokens);
    ck_assert(program == NULL);
    Parser_releaseTheTree(program);
    List_free(tokens);
}
END_TEST

START_TEST(buildNewAstTree_fourNumberToken_TreeAndSatisfaction)
{
    List *tokens = List_new();
    List_add(tokens, Token_new(TokenType_NUMBER, "56"));
    List_add(tokens, Token_new(TokenType_SEMICOLON, ";"));
    List_add(tokens, Token_new(TokenType_NUMBER, "a"));
    List_add(tokens, Token_new(TokenType_SEMICOLON, ";"));

    Tree *program = Parser_buildNewAstTree(tokens);

    ck_assert(List_count(tokens) == 4);

    ck_assert(List_count(program->children) == 2);
    AstNode *node = (AstNode *)((Tree *)List_at(program->children, 0))->value;
    ck_assert(node->type == AstNodeType_NUMBER && !strcmp(node->name, "56"));
    ck_assert(List_count(((Tree *)List_at(program->children, 0))->children) == 0);

    Parser_releaseTheTree(program);
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST(buildNewAstTree_oneMinusTokenAndOneNumberTokenAndOneSemicolon_oneNumberSubTree)
{
    List *tokens = List_new();
    List_add(tokens, Token_new(TokenType_MINUS, "-"));
    List_add(tokens, Token_new(TokenType_NUMBER, "4.13"));
    List_add(tokens, Token_new(TokenType_SEMICOLON, ";"));

    Tree *program = Parser_buildNewAstTree(tokens);

    AstNode *node = (AstNode *)((Tree *)List_at(program->children, 0))->value;
    ck_assert(node->type == AstNodeType_SUB);

    AstNode *node2 = (AstNode *)((Tree *)List_at(((Tree *)List_at(program->children, 0))->children, 0))->value;

    ck_assert(node2->type == AstNodeType_NUMBER && !strcmp(node2->name, "4.13"));

    Parser_releaseTheTree(program);
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST(buildNewAstTree_buildNewAstTreewithoutLet_NULL)
{
    List *tokens = List_new();

    int a = Lexer_splitTokens(" = 0.12 - 0.11;", tokens) ;
       

    // Tree *program = Parser_buildNewAstTree(tokens);
    // ck_assert(List_count(program->children) != 1);

    // Parser_releaseTheTree(program);

    List_insert(tokens, Token_new(TokenType_LET, "let"), 0);
    List_insert(tokens, Token_new(TokenType_ID, "a"), 1);
    // exer_printTokens(tokens , stdout);

    Tree * program = Parser_buildNewAstTree(tokens);

    ck_assert(List_count(program->children) == 1);

    Parser_releaseTheTree(program);
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST(buildNewAstTree_TwoLineToken_NULL)
{
    List *tokens = List_new();

    int a = Lexer_splitTokens("let a = 2;let b = \"123.566\";", tokens) ;

    Tree *program = Parser_buildNewAstTree(tokens);

    ck_assert(List_count(program->children) == 2);

    ck_assert(program);

    Parser_releaseTheTree(program);
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST(buildNewAstTree_JustBigTest_SatisfactionAndTree)
{
    List *tokens = List_new();
    int a = Lexer_splitTokens("a = 2*(1 + 56);", tokens) ;

    Tree *program = Parser_buildNewAstTree(tokens);

    Tree *assSubTree = List_at(program->children, 0);
    Tree *mulSubTree = List_at(assSubTree->children, 1);
    Tree *numSubTree = List_at(mulSubTree->children, 1);
    

    AstNode *firstNumberNode = ((Tree *)List_at(assSubTree->children, 0))->value;
    ck_assert_str_eq(firstNumberNode->name, "a");
    AstNode *secondNumberNode = ((Tree *)List_at(mulSubTree->children, 1))->value;
    ck_assert_str_eq(secondNumberNode->name, "+");
    AstNode *thirdNumberNode = ((Tree *)List_at(numSubTree->children, 1))->value;
    ck_assert_str_eq(thirdNumberNode->name, "56");


    ck_assert(firstNumberNode->type == AstNodeType_ID && secondNumberNode->type == AstNodeType_ADD && thirdNumberNode->type == AstNodeType_NUMBER);
    ck_assert(((AstNode *)assSubTree->value)->type == AstNodeType_ASSIGN && ((AstNode *)mulSubTree->value)->type == AstNodeType_MUL);

    Parser_releaseTheTree(program);
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

Suite *test_suite()
{
    Suite *s = suite_create("Parser");
    TCase *tc_sample = tcase_create("AstTreeBuildTest");
    //
    tcase_add_test(tc_sample, buildNewAstTree_empty_satisfaction);
    tcase_add_test(tc_sample, buildNewAstTree_fourNumberToken_TreeAndSatisfaction);
    tcase_add_test(tc_sample, buildNewAstTree_oneMinusTokenAndOneNumberTokenAndOneSemicolon_oneNumberSubTree);
    tcase_add_test(tc_sample, buildNewAstTree_buildNewAstTreewithoutLet_NULL);
    tcase_add_test(tc_sample, buildNewAstTree_TwoLineToken_NULL);
    tcase_add_test(tc_sample, buildNewAstTree_JustBigTest_SatisfactionAndTree);
    //
    suite_add_tcase(s, tc_sample);
    return s;
}

int main(void)
{
    Suite *s = test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK); // important for debugging!
    srunner_run_all(sr, CK_VERBOSE);

    int num_tests_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return num_tests_failed;
}
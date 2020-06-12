#include <check.h>
#include <stdio.h>
#include <lexer.h>
#include<fs.h>
#include <parser.h>
#include <tree.h>
#include <ast.h>
#include <math.h>
#include <unistd.h>
#include<interpreter.h> 

const char * file = "../main.prog";

START_TEST(programRun_printEmptyString_nothingToOutput)
{
    FILE *outStream = fopen(file, "w");
    List *tokens = List_new();
    Lexer_splitTokens("print(\"\");", tokens);
    Tree *tree = Parser_buildNewAstTree(tokens);
    int ranStatus = Interpreter_execute(tree, outStream);
    fclose(outStream);
    Parser_releaseTheTree(tree);
    Lexer_clearTokens(tokens);
    List_free(tokens);

    int len = getFileSize(file);
    char input[len + 1];
    readFileToBuffer(file, input, len + 1);
    input[len] = '\0';
    // printf("int = %i\n", (int)strlen(input));
    ck_assert(4 == (int)strlen(input));
}
END_TEST

START_TEST(programRun_empty_nothingToOutput)
{
    FILE *outStream = fopen(file, "w");
    List *tokens = List_new();
    Lexer_splitTokens("let a = 2;print(a);a = 4;print(a);", tokens);
    Tree *tree = Parser_buildNewAstTree(tokens);
    int ranStatus = Interpreter_execute(tree, outStream);
    fclose(outStream);
    Parser_releaseTheTree(tree);
    Lexer_clearTokens(tokens);
    List_free(tokens);


    int len = getFileSize(file);
    char input[len + 1];
    readFileToBuffer(file, input, len + 1);
    // printf("int = %i\n", (int)strlen(input));
    ck_assert(22 == (int)strlen(input));
}
END_TEST

START_TEST(programRun_returnMathExpressionResult_appropriateNumber)
{

    //
    FILE *outStream = fopen(file, "w");
    List *tokens = List_new();
    Lexer_splitTokens("let i = -4/0.5 + 2*1.5;print(i);", tokens);
    Tree *tree = Parser_buildNewAstTree(tokens);
    int ranStatus = Interpreter_execute(tree, outStream);
    fclose(outStream);
    Parser_releaseTheTree(tree);
    Lexer_clearTokens(tokens);
    List_free(tokens);

    int len = getFileSize(file);
    char input[len + 1];
    readFileToBuffer(file, input, len + 1);
    input[len] = '\0';
    char * ch = strstr(input, "-5.000000");

    ck_assert(ch != NULL);
}
END_TEST

START_TEST(programRun_printBoolArray_appropriateBoolArray)
{
    //

    //
    FILE *outStream = fopen(file, "w");
    List *tokens = List_new();
    Lexer_splitTokens("let a[] = {true, false};\nprint(a);\npush(a,false);\nprint(a);", tokens);
    Tree *tree = Parser_buildNewAstTree(tokens);
    int ranStatus = Interpreter_execute(tree, outStream);
    fclose(outStream);
    Parser_releaseTheTree(tree);
    Lexer_clearTokens(tokens);
    List_free(tokens);

    int len = getFileSize(file);
    char input[len + 1];
    readFileToBuffer(file, input, len + 1);
    input[len] = '\0';
    // puts(input);
    char * ch1 = strstr(input, "TRUE, FALSE ");
    char * ch2 = strstr(input, "TRUE, FALSE, FALSE ");

    ck_assert(ch1 != NULL);
    ck_assert(ch2 != NULL);
}
END_TEST



START_TEST(programRun_while_satisfaction)
{   
    FILE *outStream = fopen(file, "w");
    List *tokens = List_new();
    Lexer_splitTokens("let a = 1;while (a < 4) {print(a);a = a + 1;}", tokens);
    Tree *tree = Parser_buildNewAstTree(tokens);
    int ranStatus = Interpreter_execute(tree, outStream);
    fclose(outStream);
    Parser_releaseTheTree(tree);
    Lexer_clearTokens(tokens);
    List_free(tokens);

    int len = getFileSize(file);
    char input[len + 1];
    readFileToBuffer(file, input, len + 1);
    input[len] = '\0';
    //
    char * ch1 = strstr(input, "1.000000");
    ck_assert(ch1 != NULL);
    char * ch2 = strstr(input, "2.000000");
    ck_assert(ch2 != NULL);
    char * ch3 = strstr(input, "3.000000");
    ck_assert(ch3 != NULL);

}
END_TEST

START_TEST(programRun_ifElse_satisfaction)
{
    //
    FILE *outStream = fopen(file, "w");
    List *tokens = List_new();
    Lexer_splitTokens("let str = \"13\";if (1 + 2 < 2) {print(strlen(str));} else {print(strlen(str) + 10);}", tokens);
    Tree *tree = Parser_buildNewAstTree(tokens);
    int ranStatus = Interpreter_execute(tree, outStream);
    fclose(outStream);
    Parser_releaseTheTree(tree);
    Lexer_clearTokens(tokens);
    List_free(tokens);

    int len = getFileSize(file);
    char input[len + 1];
    readFileToBuffer(file, input, len + 1);
    input[len] = '\0';

    char * ch1 = strstr(input, "12.000000");
    ck_assert(ch1 != NULL);
    //

}
END_TEST

Suite *test_suite()
{
    Suite *s = suite_create("Program");
    TCase *tc_sample = tcase_create("RunTest");
    //
    tcase_add_test(tc_sample, programRun_printEmptyString_nothingToOutput);
    tcase_add_test(tc_sample, programRun_empty_nothingToOutput);
    tcase_add_test(tc_sample, programRun_returnMathExpressionResult_appropriateNumber);
    tcase_add_test(tc_sample, programRun_printBoolArray_appropriateBoolArray);
    tcase_add_test(tc_sample, programRun_while_satisfaction);
    tcase_add_test(tc_sample, programRun_ifElse_satisfaction);
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
    return 0;
}
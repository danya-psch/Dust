#include <stdio.h>
#include <stdlib.h>

#include <check.h>
#include <lexer.h>

START_TEST (split_empty_empty)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 0);
    List_free(tokens);
}
END_TEST

START_TEST (split_oneNumber_oneNumberToken)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("13", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 1);
    Token * firstToken = (Token *)List_at(tokens, 0);
    Token testToken = { TokenType_NUMBER, "13" };
    ck_assert(Token_equals(firstToken, &testToken));
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST (split_onePlus_onePlusToken)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("+", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 1);
    Token * firstToken = (Token *)List_at(tokens, 0);
    Token testToken = { TokenType_PLUS, "+" };
    ck_assert(Token_equals(firstToken, &testToken));
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST (split_oneMinus_oneMinusToken)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("-", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 1);
    Token * firstToken = (Token *)List_at(tokens, 0);
    Token testToken = { TokenType_MINUS, "-" };
    ck_assert(Token_equals(firstToken, &testToken));
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST (split_oneMult_oneMultToken)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("*", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 1);
    Token * firstToken = (Token *)List_at(tokens, 0);
    Token testToken = { TokenType_MULT, "*" };
    ck_assert(Token_equals(firstToken, &testToken));
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST (split_oneDiv_oneDivToken)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("/", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 1);
    Token * firstToken = (Token *)List_at(tokens, 0);
    Token testToken = { TokenType_DIV, "/" };
    ck_assert(Token_equals(firstToken, &testToken));
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST

START_TEST (split_oneMod_oneModToken)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("%", tokens);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(List_count(tokens), 1);
    Token * firstToken = (Token *)List_at(tokens, 0);
    Token testToken = { TokenType_MOD, "%" };
    ck_assert(Token_equals(firstToken, &testToken));
    Lexer_clearTokens(tokens);
    List_free(tokens);
}
END_TEST


START_TEST (split_invalidChar_errorCode)
{
    List * tokens = List_new();
    int status = Lexer_splitTokens("&", tokens);
    ck_assert_int_eq(status, 1);
    ck_assert_int_eq(List_count(tokens), 0);
    List_free(tokens);
}
END_TEST

Suite *test_suite() {
    Suite *s = suite_create("Lexer");
    TCase *tc_sample = tcase_create("SplitTest");
 //
    tcase_add_test(tc_sample, split_empty_empty);
    tcase_add_test(tc_sample, split_oneNumber_oneNumberToken);
    tcase_add_test(tc_sample, split_onePlus_onePlusToken);
    tcase_add_test(tc_sample, split_oneMinus_oneMinusToken);
    tcase_add_test(tc_sample, split_oneMult_oneMultToken);
    tcase_add_test(tc_sample, split_oneDiv_oneDivToken);
    tcase_add_test(tc_sample, split_oneMod_oneModToken);
    tcase_add_test(tc_sample, split_invalidChar_errorCode);
 //
    suite_add_tcase(s, tc_sample);
    return s;
}

int main(void) {
    Suite *s = test_suite();
    SRunner *sr = srunner_create(s);
    srunner_set_fork_status(sr, CK_NOFORK);  // important for debugging!
    srunner_run_all(sr, CK_VERBOSE);

    int num_tests_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return num_tests_failed;
}

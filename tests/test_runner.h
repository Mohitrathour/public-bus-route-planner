/**
 * @file test_runner.h
 * @brief Simple assertion-style test framework without external libraries.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "../include/common.h"

#define MAX_TESTS 150

typedef struct {
    char category[32];
    char name[64];
    char expected[48];
    char actual[48];
    int passed;
} TestRecord;

extern TestRecord g_test_results[MAX_TESTS];
extern int g_total_tests;
extern int g_passed_tests;

void test_init(void);
void test_record(const char *category, const char *name, const char *expected, const char *actual, int passed);
void test_print_summary(void);

#define ASSERT_INT_EQ(cat, name, actual, expected) do { \
    char exp_str[48], act_str[48]; \
    sprintf(exp_str, "%d", (int)(expected)); \
    sprintf(act_str, "%d", (int)(actual)); \
    int pass = ((int)(actual) == (int)(expected)); \
    test_record(cat, name, exp_str, act_str, pass); \
} while(0)

#define ASSERT_STR_EQ(cat, name, actual, expected) do { \
    int pass = (strcmp((actual), (expected)) == 0); \
    test_record(cat, name, expected, actual, pass); \
} while(0)

#define ASSERT_TRUE(cat, name, condition) do { \
    test_record(cat, name, "TRUE", (condition) ? "TRUE" : "FALSE", (condition) ? 1 : 0); \
} while(0)

int run_automated_tests(void);

#endif /* TEST_RUNNER_H */

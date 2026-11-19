/**
 * @file string_matching.h
 * @brief String matching algorithms for transit incident logs and stop names:
 *        1. Naive String Matching
 *        2. Rabin-Karp Algorithm (Rolling Hash)
 *        3. Finite Automaton Matcher
 *        4. Knuth-Morris-Pratt (KMP) Algorithm
 */

#ifndef STRING_MATCHING_H
#define STRING_MATCHING_H

#include "models.h"

#define MAX_MATCH_POS 100

typedef struct {
    int matches[MAX_MATCH_POS];
    int match_count;
    unsigned long long char_comparisons;
    double preprocess_time_ms;
} StringMatchResult;

/* Algorithms */
StringMatchResult naive_string_search(const char *text, const char *pattern, Metrics *m, int verbose);
StringMatchResult rabin_karp_search(const char *text, const char *pattern, Metrics *m, int verbose);
StringMatchResult finite_automaton_search(const char *text, const char *pattern, Metrics *m, int verbose);
StringMatchResult kmp_search(const char *text, const char *pattern, Metrics *m, int verbose);

/* Submenu and Comparison */
void run_string_matching_menu(TransitSystem *sys);
void compare_string_matching_all(const char *text, const char *pattern);

#endif /* STRING_MATCHING_H */

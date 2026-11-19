/**
 * @file string_matching.c
 * @brief String Matching Algorithms: Naive, Rabin-Karp, Finite Automaton, KMP.
 *
 * Domain Use:
 *   Search passenger query keywords, stop names, and dispatch incident event logs.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/string_matching.h"

/* ========================================================================= */
/*                          1. NAIVE STRING MATCHING                         */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * NAIVE_STRING_MATCHER(T, P):
 *   n = length(T), m = length(P)
 *   for s = 0 to n - m:
 *     if P[0..m-1] == T[s..s+m-1]:
 *       print "Pattern occurs with shift" s
 *
 * COMPLEXITY:
 *   Time Complexity : O((n - m + 1) * m)
 *   Space Complexity: O(1)
 */
StringMatchResult naive_string_search(const char *text, const char *pattern, Metrics *m, int verbose) {
    StringMatchResult res;
    res.match_count = 0;
    res.char_comparisons = 0;
    res.preprocess_time_ms = 0.0;

    int n = (int)strlen(text);
    int p_len = (int)strlen(pattern);

    for (int s = 0; s <= n - p_len; s++) {
        int match = 1;
        for (int j = 0; j < p_len; j++) {
            res.char_comparisons++;
            if (m) m->comparisons++;

            if (text[s + j] != pattern[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            if (res.match_count < MAX_MATCH_POS) {
                res.matches[res.match_count++] = s;
            }
            if (verbose) {
                printf("    [Naive Match] Found pattern '%s' at index %d\n", pattern, s);
            }
        }
    }

    return res;
}

/* ========================================================================= */
/*                          2. RABIN-KARP ALGORITHM                          */
/* ========================================================================= */

/**
 * ROLLING HASH ALGORITHM:
 *   hash(s+1) = (d * (hash(s) - T[s]*h) + T[s+m]) mod q
 *
 * COMPLEXITY:
 *   Preprocessing: O(m)
 *   Matching     : O(n) average, O(n * m) worst (spurious hash collisions)
 *   Space        : O(1)
 */
#define RK_BASE 256
#define RK_PRIME 101

StringMatchResult rabin_karp_search(const char *text, const char *pattern, Metrics *m, int verbose) {
    StringMatchResult res;
    res.match_count = 0;
    res.char_comparisons = 0;

    clock_t t0 = timer_start();
    int n = (int)strlen(text);
    int p_len = (int)strlen(pattern);
    int p_hash = 0;
    int t_hash = 0;
    int h = 1;
    int spurious_hits = 0;

    /* h = (RK_BASE^(p_len - 1)) % RK_PRIME */
    for (int i = 0; i < p_len - 1; i++) {
        h = (h * RK_BASE) % RK_PRIME;
    }

    /* Compute initial hash values */
    for (int i = 0; i < p_len; i++) {
        p_hash = (RK_BASE * p_hash + pattern[i]) % RK_PRIME;
        t_hash = (RK_BASE * t_hash + text[i]) % RK_PRIME;
    }

    res.preprocess_time_ms = timer_stop_ms(t0);

    for (int s = 0; s <= n - p_len; s++) {
        if (p_hash == t_hash) {
            /* Full character check on hash match */
            int match = 1;
            for (int j = 0; j < p_len; j++) {
                res.char_comparisons++;
                if (m) m->comparisons++;
                if (text[s + j] != pattern[j]) {
                    match = 0;
                    spurious_hits++;
                    break;
                }
            }
            if (match) {
                if (res.match_count < MAX_MATCH_POS) {
                    res.matches[res.match_count++] = s;
                }
                if (verbose) {
                    printf("    [Rabin-Karp Match] Hash hit (%d) confirmed at index %d\n", p_hash, s);
                }
            }
        }

        /* Compute rolling hash for next window */
        if (s < n - p_len) {
            t_hash = (RK_BASE * (t_hash - text[s] * h) + text[s + p_len]) % RK_PRIME;
            if (t_hash < 0) t_hash += RK_PRIME;
        }
    }

    if (verbose && spurious_hits > 0) {
        printf("    [Rabin-Karp Notice] %d spurious hash collision(s) detected.\n", spurious_hits);
    }

    return res;
}

/* ========================================================================= */
/*                      3. FINITE AUTOMATON MATCHER                          */
/* ========================================================================= */

/**
 * DETERMINISTIC FINITE AUTOMATON (DFA):
 *   Transition function delta(q, a) computes next state after reading character 'a'.
 *   Once built, text scanning takes exactly 1 state transition per character (Theta(n)).
 *
 * COMPLEXITY:
 *   Preprocessing: O(m^3 * |Sigma|) (simplified) or O(m * |Sigma|)
 *   Matching     : Theta(n) strictly with NO character backtracking!
 */
static int get_next_state(const char *pattern, int m_len, int state, int x) {
    if (state < m_len && x == pattern[state]) return state + 1;

    for (int ns = state; ns > 0; ns--) {
        if (pattern[ns - 1] == x) {
            int match = 1;
            for (int i = 0; i < ns - 1; i++) {
                if (pattern[i] != pattern[state - ns + 1 + i]) {
                    match = 0;
                    break;
                }
            }
            if (match) return ns;
        }
    }
    return 0;
}

StringMatchResult finite_automaton_search(const char *text, const char *pattern, Metrics *m, int verbose) {
    StringMatchResult res;
    res.match_count = 0;
    res.char_comparisons = 0;

    clock_t t0 = timer_start();
    int n = (int)strlen(text);
    int p_len = (int)strlen(pattern);

    /* Allocate DFA table: states 0..p_len across ASCII characters */
    int **TF = (int **)malloc((p_len + 1) * sizeof(int *));
    for (int i = 0; i <= p_len; i++) {
        TF[i] = (int *)malloc(256 * sizeof(int));
        for (int c = 0; c < 256; c++) {
            TF[i][c] = get_next_state(pattern, p_len, i, c);
        }
    }

    res.preprocess_time_ms = timer_stop_ms(t0);

    int state = 0;
    for (int i = 0; i < n; i++) {
        res.char_comparisons++;
        if (m) {
            m->comparisons++;
            m->iterations++;
        }

        unsigned char c = (unsigned char)text[i];
        state = TF[state][c];

        if (state == p_len) {
            int pos = i - p_len + 1;
            if (res.match_count < MAX_MATCH_POS) {
                res.matches[res.match_count++] = pos;
            }
            if (verbose) {
                printf("    [DFA Match] Reached accepting state %d at text index %d\n", p_len, pos);
            }
        }
    }

    for (int i = 0; i <= p_len; i++) free(TF[i]);
    free(TF);

    return res;
}

/* ========================================================================= */
/*                          4. KNUTH-MORRIS-PRATT (KMP)                      */
/* ========================================================================= */

/**
 * PREFIX FUNCTION (LPS TABLE):
 *   pi[q] stores length of longest proper prefix of P[0..q] that is also a suffix.
 *
 * COMPLEXITY:
 *   Preprocessing: O(m)
 *   Matching     : O(n)
 *   Space        : O(m)
 *
 * DOMAIN USE:
 *   High-speed text search in real-time dispatch radio and incident logging.
 */
static void compute_lps_array(const char *pattern, int m_len, int lps[], Metrics *m) {
    int len = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m_len) {
        if (m) m->comparisons++;
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

StringMatchResult kmp_search(const char *text, const char *pattern, Metrics *m, int verbose) {
    StringMatchResult res;
    res.match_count = 0;
    res.char_comparisons = 0;

    clock_t t0 = timer_start();
    int n = (int)strlen(text);
    int p_len = (int)strlen(pattern);

    int *lps = (int *)malloc(p_len * sizeof(int));
    if (!lps) return res;

    compute_lps_array(pattern, p_len, lps, m);
    res.preprocess_time_ms = timer_stop_ms(t0);

    if (verbose) {
        printf("    [KMP LPS Table Computed]: ");
        for (int k = 0; k < p_len; k++) printf("%d ", lps[k]);
        printf("\n");
    }

    int i = 0; /* text pointer */
    int j = 0; /* pattern pointer */

    while (i < n) {
        res.char_comparisons++;
        if (m) {
            m->comparisons++;
            m->iterations++;
        }

        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == p_len) {
            int pos = i - j;
            if (res.match_count < MAX_MATCH_POS) {
                res.matches[res.match_count++] = pos;
            }
            if (verbose) {
                printf("    [KMP Match] Found pattern match at index %d\n", pos);
            }
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    return res;
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_string_matching_all(const char *text, const char *pattern) {
    Metrics m_naive, m_rk, m_fa, m_kmp;
    RESET_METRICS(m_naive);
    RESET_METRICS(m_rk);
    RESET_METRICS(m_fa);
    RESET_METRICS(m_kmp);

    clock_t t0 = timer_start();
    StringMatchResult res_naive = naive_string_search(text, pattern, &m_naive, 0);
    m_naive.time_ms = timer_stop_ms(t0);

    clock_t t1 = timer_start();
    StringMatchResult res_rk = rabin_karp_search(text, pattern, &m_rk, 0);
    m_rk.time_ms = timer_stop_ms(t1);

    clock_t t2 = timer_start();
    StringMatchResult res_fa = finite_automaton_search(text, pattern, &m_fa, 0);
    m_fa.time_ms = timer_stop_ms(t2);

    clock_t t3 = timer_start();
    StringMatchResult res_kmp = kmp_search(text, pattern, &m_kmp, 0);
    m_kmp.time_ms = timer_stop_ms(t3);

    print_banner("STRING MATCHING ALGORITHMS COMPARISON");
    printf("  Target Pattern : '%s' (Length: %zu)\n", pattern, strlen(pattern));
    printf("  Search Corpus  : %zu characters\n\n", strlen(text));

    printf("  +---------------------+-----------+---------------+--------------------+----------------+\n");
    printf("  | Matcher Algorithm   | Time (ms) | Preprocess ms | Char Comparisons   | Matches Found  |\n");
    printf("  +---------------------+-----------+---------------+--------------------+----------------+\n");
    printf("  | Naive Search        | %-9.4f | %-13.4f | %-18llu | %-14d |\n",
           m_naive.time_ms, res_naive.preprocess_time_ms, res_naive.char_comparisons, res_naive.match_count);
    printf("  | Rabin-Karp (Hash)   | %-9.4f | %-13.4f | %-18llu | %-14d |\n",
           m_rk.time_ms, res_rk.preprocess_time_ms, res_rk.char_comparisons, res_rk.match_count);
    printf("  | Finite Automaton    | %-9.4f | %-13.4f | %-18llu | %-14d |\n",
           m_fa.time_ms, res_fa.preprocess_time_ms, res_fa.char_comparisons, res_fa.match_count);
    printf("  | Knuth-Morris-Pratt  | %-9.4f | %-13.4f | %-18llu | %-14d |\n",
           m_kmp.time_ms, res_kmp.preprocess_time_ms, res_kmp.char_comparisons, res_kmp.match_count);
    printf("  +---------------------+-----------+---------------+--------------------+----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> KMP is the best choice: Preprocessing takes only O(m) time/space with linear O(n) streaming search and 0 text backtracking.\n");
}

void run_string_matching_menu(TransitSystem *sys) {
    (void)sys;
    int choice = -1;
    const char *sample_log =
        "TRANSIT INCIDENT LOG [2026-11-19]:\n"
        "08:15 ISBT Depot: Bus 101 dispatched on Route 101.\n"
        "08:45 Clock Tower: Traffic jam near Clock Tower crossing, minor delay.\n"
        "09:10 Ballupur: Bus 102 reported tire puncture near Ballupur Chowk.\n"
        "09:30 ISBT Depot: Backup bus 108 dispatched from ISBT Depot to assist Ballupur passengers.\n"
        "10:15 Rajpur Road: Heavy passenger demand at Rajpur Road terminal.\n"
        "11:00 Clock Tower: Route cleared at Clock Tower, regular schedule restored.\n";

    while (choice != 0) {
        print_banner("MENU 10: STRING MATCHING ALGORITHMS");
        printf("  1. Naive String Matching (Step-by-step trace)\n");
        printf("  2. Rabin-Karp Algorithm (Rolling Hash & Collision trace)\n");
        printf("  3. Finite Automaton Matcher (DFA state transition trace)\n");
        printf("  4. Knuth-Morris-Pratt (KMP - Prefix function LPS trace)\n");
        printf("  5. Compare all (Naive vs Rabin-Karp vs Finite Automaton vs KMP)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 5);
        if (choice == 0) break;

        char query[MAX_STRING];
        get_string_input("Enter search keyword (e.g. 'Clock Tower', 'ISBT', 'Ballupur')", query, MAX_STRING);

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. NAIVE STRING SEARCH TRACE");
                StringMatchResult res = naive_string_search(sample_log, query, &m, 1);
                printf("\n  >> Total Occurrences: %d | Comparisons: %llu\n", res.match_count, res.char_comparisons);
                break;
            }
            case 2: {
                print_banner("2. RABIN-KARP ROLLING HASH TRACE");
                StringMatchResult res = rabin_karp_search(sample_log, query, &m, 1);
                printf("\n  >> Total Occurrences: %d | Comparisons: %llu\n", res.match_count, res.char_comparisons);
                break;
            }
            case 3: {
                print_banner("3. FINITE AUTOMATON MATCHER TRACE");
                StringMatchResult res = finite_automaton_search(sample_log, query, &m, 1);
                printf("\n  >> Total Occurrences: %d | DFA Transitions: %llu\n", res.match_count, res.char_comparisons);
                break;
            }
            case 4: {
                print_banner("4. KNUTH-MORRIS-PRATT (KMP) TRACE");
                StringMatchResult res = kmp_search(sample_log, query, &m, 1);
                printf("\n  >> Total Occurrences: %d | Comparisons: %llu\n", res.match_count, res.char_comparisons);
                break;
            }
            case 5: {
                compare_string_matching_all(sample_log, query);
                break;
            }
        }

        press_enter_to_continue();
    }
}

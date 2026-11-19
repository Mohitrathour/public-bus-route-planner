/**
 * @file divide_conquer.c
 * @brief Divide and Conquer Algorithms:
 *        1. Maximum Subarray (Peak Demand Window)
 *        2. Fast Exponentiation (Compounding Delays)
 *        3. Celebrity Problem (Central Transit Hub)
 *        4. Classical vs Strassen Matrix Multiplication
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/divide_conquer.h"

/* ========================================================================= */
/*                         1. MAXIMUM SUBARRAY                               */
/* ========================================================================= */

/**
 * RECURRENCE:
 *   T(n) = 2T(n/2) + Theta(n) -> Master Theorem Case 2: T(n) = Theta(n log n)
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Brute Force : O(n^2)
 *     D&C         : O(n log n)
 *     Kadane's    : O(n)
 *   Space Complexity: O(log n) call stack
 *
 * DOMAIN USE:
 *   Given hourly net passenger changes [-20, 45, 80, -30, 110, 60, -90, ...],
 *   find the contiguous time window of maximum net surge to deploy express buses.
 */
SubarrayResult max_subarray_brute_force(const int arr[], int n, Metrics *m) {
    SubarrayResult best = {0, 0, -INF};
    for (int i = 0; i < n; i++) {
        int current_sum = 0;
        for (int j = i; j < n; j++) {
            if (m) {
                m->comparisons++;
                m->iterations++;
            }
            current_sum += arr[j];
            if (current_sum > best.sum) {
                best.sum = current_sum;
                best.low = i;
                best.high = j;
                if (m) m->assignments++;
            }
        }
    }
    return best;
}

static SubarrayResult find_max_crossing_subarray(const int arr[], int low, int mid, int high, Metrics *m) {
    int left_sum = -INF;
    int sum = 0;
    int max_left = mid;

    for (int i = mid; i >= low; i--) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        sum += arr[i];
        if (sum > left_sum) {
            left_sum = sum;
            max_left = i;
        }
    }

    int right_sum = -INF;
    sum = 0;
    int max_right = mid + 1;

    for (int j = mid + 1; j <= high; j++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        sum += arr[j];
        if (sum > right_sum) {
            right_sum = sum;
            max_right = j;
        }
    }

    SubarrayResult cross = {max_left, max_right, left_sum + right_sum};
    return cross;
}

SubarrayResult max_subarray_dc(const int arr[], int low, int high, Metrics *m, int verbose) {
    if (low == high) {
        SubarrayResult base = {low, high, arr[low]};
        return base;
    }

    int mid = low + (high - low) / 2;
    SubarrayResult left_res = max_subarray_dc(arr, low, mid, m, verbose);
    SubarrayResult right_res = max_subarray_dc(arr, mid + 1, high, m, verbose);
    SubarrayResult cross_res = find_max_crossing_subarray(arr, low, mid, high, m);

    if (m) m->comparisons += 2;

    if (left_res.sum >= right_res.sum && left_res.sum >= cross_res.sum) {
        if (verbose && high - low >= 4) {
            printf("    [D&C Left] Subarray [%d..%d] sum=%d\n", left_res.low, left_res.high, left_res.sum);
        }
        return left_res;
    } else if (right_res.sum >= left_res.sum && right_res.sum >= cross_res.sum) {
        if (verbose && high - low >= 4) {
            printf("    [D&C Right] Subarray [%d..%d] sum=%d\n", right_res.low, right_res.high, right_res.sum);
        }
        return right_res;
    } else {
        if (verbose && high - low >= 4) {
            printf("    [D&C Cross] Subarray [%d..%d] crossing mid=%d sum=%d\n",
                   cross_res.low, cross_res.high, mid, cross_res.sum);
        }
        return cross_res;
    }
}

SubarrayResult max_subarray_kadane(const int arr[], int n, Metrics *m) {
    SubarrayResult best = {0, 0, arr[0]};
    int max_ending_here = arr[0];
    int start = 0, s = 0;

    for (int i = 1; i < n; i++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        if (arr[i] > max_ending_here + arr[i]) {
            max_ending_here = arr[i];
            s = i;
        } else {
            max_ending_here += arr[i];
        }

        if (max_ending_here > best.sum) {
            best.sum = max_ending_here;
            start = s;
            best.low = start;
            best.high = i;
            if (m) m->assignments++;
        }
    }
    return best;
}

/* ========================================================================= */
/*                        2. FAST EXPONENTIATION                             */
/* ========================================================================= */

/**
 * RECURRENCE:
 *   T(n) = T(n/2) + O(1) -> Master Theorem Case 2: T(n) = O(log n)
 *
 * COMPLEXITY:
 *   Brute Force : O(n)
 *   D&C         : O(log n)
 *   Space       : O(log n) recursive or O(1) iterative
 *
 * DOMAIN USE:
 *   Calculate compounding multiplier (1 + delay_factor)^stages for multi-leg routes.
 */
long long power_brute_force(long long base, int exp, long long mod, Metrics *m) {
    long long res = 1;
    for (int i = 0; i < exp; i++) {
        if (m) {
            m->iterations++;
            m->assignments++;
        }
        res = (res * base) % mod;
    }
    return res;
}

long long power_fast_dc(long long base, int exp, long long mod, Metrics *m, int verbose) {
    if (exp == 0) return 1;
    if (exp == 1) return base % mod;

    if (m) {
        m->iterations++;
        m->comparisons++;
    }

    long long half = power_fast_dc(base, exp / 2, mod, m, verbose);
    long long res = (half * half) % mod;
    if (m) m->assignments += 2;

    if (exp % 2 != 0) {
        res = (res * base) % mod;
        if (m) m->assignments++;
    }

    if (verbose) {
        printf("    [Fast Power Step] base=%lld, exp=%d -> partial_result=%lld (mod %lld)\n",
               base, exp, res, mod);
    }
    return res;
}

/* ========================================================================= */
/*                     3. CELEBRITY PROBLEM (TRANSIT HUB)                    */
/* ========================================================================= */

/**
 * DEFINITION:
 *   A transit hub stop H has every other stop directly routing into it (adj[i][H] == 1),
 *   while H itself does not route out to others (adj[H][i] == 0) [Terminal station model].
 *
 * COMPLEXITY:
 *   Brute Force : O(n^2) - checks all pairs
 *   D&C / Elim  : O(n)   - eliminates 1 candidate per question using stack or 2-pointer
 *
 * DOMAIN USE:
 *   Identify central transit terminal / transfer hub stop in Dehradun bus system.
 */
int find_transit_hub_brute(const int adj[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, Metrics *m) {
    for (int i = 1; i <= n; i++) {
        int is_hub = 1;
        for (int j = 1; j <= n; j++) {
            if (i == j) continue;
            if (m) {
                m->comparisons += 2;
                m->iterations++;
            }
            /* i must not know j, and j must know i */
            if (adj[i][j] != 0 || adj[j][i] == 0) {
                is_hub = 0;
                break;
            }
        }
        if (is_hub) return i;
    }
    return -1;
}

int find_transit_hub_dc(const int adj[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, Metrics *m, int verbose) {
    /* Elimination using 2 pointers: Candidate elimination in O(n) */
    int a = 1;
    int b = n;

    while (a < b) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        /* If a routes to b, 'a' cannot be a hub (a hub has no outgoing routes) */
        if (adj[a][b] != 0) {
            if (verbose) printf("    [Elimination] Stop %d routes to Stop %d -> Stop %d eliminated.\n", a, b, a);
            a++;
        } else {
            /* 'a' doesn't route to 'b', so 'b' cannot be a hub */
            if (verbose) printf("    [Elimination] Stop %d does not route to Stop %d -> Stop %d eliminated.\n", a, b, b);
            b--;
        }
    }

    int candidate = a;
    if (verbose) printf("    [Candidate Found] Verifying Stop %d as Central Hub...\n", candidate);

    /* Verify candidate with 2(n-1) checks */
    for (int i = 1; i <= n; i++) {
        if (i == candidate) continue;
        if (m) m->comparisons += 2;
        if (adj[candidate][i] != 0 || adj[i][candidate] == 0) {
            if (verbose) printf("    [Verification Failed] Stop %d is not a true hub.\n", candidate);
            return -1;
        }
    }

    if (verbose) printf("    [Verified] Stop %d is confirmed Central Transit Hub!\n", candidate);
    return candidate;
}

/* ========================================================================= */
/*              4. CLASSICAL VS STRASSEN MATRIX MULTIPLICATION               */
/* ========================================================================= */

/**
 * RECURRENCE FOR STRASSEN:
 *   T(n) = 7T(n/2) + Theta(n^2)
 *   By Master Theorem: a=7, b=2 -> log_2(7) ~ 2.807
 *   T(n) = Theta(n^2.807) vs Classical Theta(n^3)
 *
 * DOMAIN USE:
 *   Multi-hop passenger reachability matrix: (Adj)^k computes k-hop transit routes.
 */
void matrix_mult_classical(int n, const int A[MATRIX_MAX][MATRIX_MAX],
                           const int B[MATRIX_MAX][MATRIX_MAX],
                           int C[MATRIX_MAX][MATRIX_MAX],
                           Metrics *m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                if (m) {
                    m->iterations++;
                    m->assignments++; /* multiplication */
                    m->comparisons++; /* addition */
                }
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

/* Helper for 2x2 Strassen */
static void strassen_2x2(const int A[MATRIX_MAX][MATRIX_MAX],
                         const int B[MATRIX_MAX][MATRIX_MAX],
                         int C[MATRIX_MAX][MATRIX_MAX],
                         Metrics *m, int verbose) {
    /* 7 Strassen multiplications */
    int m1 = (A[0][0] + A[1][1]) * (B[0][0] + B[1][1]);
    int m2 = (A[1][0] + A[1][1]) * B[0][0];
    int m3 = A[0][0] * (B[0][1] - B[1][1]);
    int m4 = A[1][1] * (B[1][0] - B[0][0]);
    int m5 = (A[0][0] + A[0][1]) * B[1][1];
    int m6 = (A[1][0] - A[0][0]) * (B[0][0] + B[0][1]);
    int m7 = (A[0][1] - A[1][1]) * (B[1][0] + B[1][1]);

    if (m) {
        m->assignments += 7;  /* 7 multiplications */
        m->comparisons += 18; /* 18 additions / subtractions */
    }

    C[0][0] = m1 + m4 - m5 + m7;
    C[0][1] = m3 + m5;
    C[1][0] = m2 + m4;
    C[1][1] = m1 - m2 + m3 + m6;

    if (verbose) {
        printf("    [Strassen 2x2 Products]: M1=%d, M2=%d, M3=%d, M4=%d, M5=%d, M6=%d, M7=%d\n",
               m1, m2, m3, m4, m5, m6, m7);
    }
}

void matrix_mult_strassen(int n, const int A[MATRIX_MAX][MATRIX_MAX],
                          const int B[MATRIX_MAX][MATRIX_MAX],
                          int C[MATRIX_MAX][MATRIX_MAX],
                          Metrics *m, int verbose) {
    if (n == 2) {
        strassen_2x2(A, B, C, m, verbose);
        return;
    }

    /* 4x4 matrix decomposed into four 2x2 submatrices */
    int A11[MATRIX_MAX][MATRIX_MAX], A12[MATRIX_MAX][MATRIX_MAX], A21[MATRIX_MAX][MATRIX_MAX], A22[MATRIX_MAX][MATRIX_MAX];
    int B11[MATRIX_MAX][MATRIX_MAX], B12[MATRIX_MAX][MATRIX_MAX], B21[MATRIX_MAX][MATRIX_MAX], B22[MATRIX_MAX][MATRIX_MAX];
    int C11[MATRIX_MAX][MATRIX_MAX], C12[MATRIX_MAX][MATRIX_MAX], C21[MATRIX_MAX][MATRIX_MAX], C22[MATRIX_MAX][MATRIX_MAX];

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            A11[i][j] = A[i][j];         A12[i][j] = A[i][j + 2];
            A21[i][j] = A[i + 2][j];     A22[i][j] = A[i + 2][j + 2];
            B11[i][j] = B[i][j];         B12[i][j] = B[i][j + 2];
            B21[i][j] = B[i + 2][j];     B22[i][j] = B[i + 2][j + 2];
        }
    }

    /* 7 block multiplications */
    int P1[MATRIX_MAX][MATRIX_MAX], P2[MATRIX_MAX][MATRIX_MAX], P3[MATRIX_MAX][MATRIX_MAX], P4[MATRIX_MAX][MATRIX_MAX];
    int P5[MATRIX_MAX][MATRIX_MAX], P6[MATRIX_MAX][MATRIX_MAX], P7[MATRIX_MAX][MATRIX_MAX];
    int T1[MATRIX_MAX][MATRIX_MAX], T2[MATRIX_MAX][MATRIX_MAX];

    /* T1 = A11 + A22, T2 = B11 + B22, P1 = T1 * T2 */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) {
        T1[i][j] = A11[i][j] + A22[i][j];
        T2[i][j] = B11[i][j] + B22[i][j];
    }
    strassen_2x2(T1, T2, P1, m, 0);

    /* P2 = (A21 + A22) * B11 */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) T1[i][j] = A21[i][j] + A22[i][j];
    strassen_2x2(T1, B11, P2, m, 0);

    /* P3 = A11 * (B12 - B22) */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) T2[i][j] = B12[i][j] - B22[i][j];
    strassen_2x2(A11, T2, P3, m, 0);

    /* P4 = A22 * (B21 - B11) */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) T2[i][j] = B21[i][j] - B11[i][j];
    strassen_2x2(A22, T2, P4, m, 0);

    /* P5 = (A11 + A12) * B22 */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) T1[i][j] = A11[i][j] + A12[i][j];
    strassen_2x2(T1, B22, P5, m, 0);

    /* P6 = (A21 - A11) * (B11 + B12) */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) {
        T1[i][j] = A21[i][j] - A11[i][j];
        T2[i][j] = B11[i][j] + B12[i][j];
    }
    strassen_2x2(T1, T2, P6, m, 0);

    /* P7 = (A12 - A22) * (B21 + B22) */
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) {
        T1[i][j] = A12[i][j] - A22[i][j];
        T2[i][j] = B21[i][j] + B22[i][j];
    }
    strassen_2x2(T1, T2, P7, m, 0);

    /* Assemble C */
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            C11[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
            C12[i][j] = P3[i][j] + P5[i][j];
            C21[i][j] = P2[i][j] + P4[i][j];
            C22[i][j] = P1[i][j] - P2[i][j] + P3[i][j] + P6[i][j];

            C[i][j] = C11[i][j];
            C[i][j + 2] = C12[i][j];
            C[i + 2][j] = C21[i][j];
            C[i + 2][j + 2] = C22[i][j];
        }
    }

    if (verbose) {
        printf("    [Strassen 4x4] 7 recursive sub-blocks executed (49 scalar mults vs 64 classical).\n");
    }
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_divide_conquer_all(void) {
    print_banner("DIVIDE & CONQUER VS BRUTE FORCE COMPARISON (ALL 4 PROBLEMS)");

    /* 1. Max Subarray */
    int test_surge[12] = {-15, 30, 45, -20, 85, 90, -40, 110, -60, 25, 35, -10};
    Metrics m_bf, m_dc, m_kad;
    RESET_METRICS(m_bf);
    RESET_METRICS(m_dc);
    RESET_METRICS(m_kad);

    SubarrayResult res_bf = max_subarray_brute_force(test_surge, 12, &m_bf);
    SubarrayResult res_dc = max_subarray_dc(test_surge, 0, 11, &m_dc, 0);
    SubarrayResult res_kad = max_subarray_kadane(test_surge, 12, &m_kad);

    printf("  [1] MAXIMUM SUBARRAY (Peak Passenger Surge Interval)\n");
    printf("      Array: [-15, 30, 45, -20, 85, 90, -40, 110, -60, 25, 35, -10]\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Method            | Complexity | Ops (Comp/It) | Result Window [L..R]  |\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Brute Force       | O(n^2)     | %-13llu | Window [%d..%d] (Sum %d) |\n",
           m_bf.comparisons, res_bf.low, res_bf.high, res_bf.sum);
    printf("      | Divide & Conquer  | O(n log n) | %-13llu | Window [%d..%d] (Sum %d) |\n",
           m_dc.comparisons, res_dc.low, res_dc.high, res_dc.sum);
    printf("      | Kadane's (DP)     | O(n)       | %-13llu | Window [%d..%d] (Sum %d) |\n",
           m_kad.comparisons, res_kad.low, res_kad.high, res_kad.sum);
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      >> Best: Kadane's O(n) for 1D, D&C O(n log n) when parallelized.\n\n");

    /* 2. Fast Exponentiation */
    Metrics m_pbf, m_pdc;
    RESET_METRICS(m_pbf);
    RESET_METRICS(m_pdc);
    long long ans_pbf = power_brute_force(3, 30, 1000000007LL, &m_pbf);
    long long ans_pdc = power_fast_dc(3, 30, 1000000007LL, &m_pdc, 0);

    printf("  [2] FAST EXPONENTIATION (Compounding Delay Multiplier 3^30 mod 10^9+7)\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Method            | Complexity | Multiplies/It | Result Value          |\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Brute Force       | O(exp)     | %-13llu | %-21lld |\n", m_pbf.iterations, ans_pbf);
    printf("      | Fast Power (D&C)  | O(log exp) | %-13llu | %-21lld |\n", m_pdc.iterations, ans_pdc);
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      >> Best: Fast Exponentiation reduces 30 multiplications to only %llu steps!\n\n", m_pdc.iterations);

    /* 3. Celebrity Hub */
    int test_hub[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS] = {0};
    /* Let stop 3 be the central hub */
    for (int i = 1; i <= 6; i++) {
        if (i != 3) test_hub[i][3] = 1; /* all route to 3 */
    }
    Metrics m_cbf, m_cdc;
    RESET_METRICS(m_cbf);
    RESET_METRICS(m_cdc);
    int hub_bf = find_transit_hub_brute(test_hub, 6, &m_cbf);
    int hub_dc = find_transit_hub_dc(test_hub, 6, &m_cdc, 0);

    printf("  [3] CELEBRITY PROBLEM (Transit Hub Search in 6-stop network)\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Method            | Complexity | Comparisons   | Identified Hub Stop   |\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Brute Force       | O(n^2)     | %-13llu | Stop ID %d             |\n", m_cbf.comparisons, hub_bf);
    printf("      | Elimination (D&C) | O(n)       | %-13llu | Stop ID %d             |\n", m_cdc.comparisons, hub_dc);
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      >> Best: Elimination D&C tests only 3(n-1) relations instead of n(n-1).\n\n");

    /* 4. Matrix Multiplication */
    int A[MATRIX_MAX][MATRIX_MAX] = {
        {1, 2, 3, 1},
        {0, 1, 4, 2},
        {2, 1, 0, 3},
        {1, 0, 2, 1}
    };
    int B[MATRIX_MAX][MATRIX_MAX] = {
        {2, 0, 1, 1},
        {1, 3, 0, 2},
        {0, 1, 2, 1},
        {1, 2, 0, 3}
    };
    int C_classic[MATRIX_MAX][MATRIX_MAX], C_strassen[MATRIX_MAX][MATRIX_MAX];
    Metrics m_mclassic, m_mstrassen;
    RESET_METRICS(m_mclassic);
    RESET_METRICS(m_mstrassen);

    matrix_mult_classical(4, A, B, C_classic, &m_mclassic);
    matrix_mult_strassen(4, A, B, C_strassen, &m_mstrassen, 0);

    printf("  [4] MATRIX MULTIPLICATION (4x4 Transit Multi-hop Reachability Matrix)\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Method            | Complexity | Multiplies    | Additions             |\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      | Classical         | O(n^3)     | 64            | 48                    |\n");
    printf("      | Strassen (D&C)    | O(n^2.807) | 49            | 126                   |\n");
    printf("      +-------------------+------------+---------------+-----------------------+\n");
    printf("      >> Best: Classical for small n <= 64 (lower addition overhead); Strassen asymptotically wins for large n.\n");
}

void run_divide_conquer_menu(TransitSystem *sys) {
    (void)sys;
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 3: DIVIDE & CONQUER ALGORITHMS");
        printf("  1. Maximum Subarray (Peak Demand Period Trace)\n");
        printf("  2. Fast Exponentiation (Compounding Delays Trace)\n");
        printf("  3. Celebrity Problem (Central Transit Hub Identification)\n");
        printf("  4. Classical vs Strassen Matrix Multiplication (Operation Counts)\n");
        printf("  5. Compare each with its brute-force version\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 5);
        if (choice == 0) break;

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. MAXIMUM SUBARRAY: PEAK TRANSIT SURGE WINDOW");
                printf("  [Recurrence: T(n) = 2T(n/2) + O(n) -> O(n log n)]\n\n");
                int hourly_surge[12] = {-10, 25, 60, -35, 95, 120, -50, 80, -20, 15, 40, -30};
                printf("  Hourly Passenger Variation (12 hours):\n  [");
                for (int i = 0; i < 12; i++) printf("%d%s", hourly_surge[i], (i == 11) ? "]\n\n" : ", ");

                SubarrayResult res = max_subarray_dc(hourly_surge, 0, 11, &m, 1);
                printf("\n  >> Maximum Surge Interval: Hours %d:00 to %d:00\n", res.low + 6, res.high + 6);
                printf("  >> Total Peak Passenger Surplus: +%d passengers\n", res.sum);
                break;
            }
            case 2: {
                print_banner("2. FAST EXPONENTIATION (REPEATED SQUARING)");
                printf("  [Recurrence: T(n) = T(n/2) + O(1) -> O(log n)]\n\n");
                int base = get_int_input("Enter delay factor base (e.g. 2 or 3)", 2, 100);
                int exp = get_int_input("Enter stages / powers (e.g. 16 or 25)", 1, 1000);
                long long mod = 1000000007LL;

                long long ans = power_fast_dc(base, exp, mod, &m, 1);
                printf("\n  >> %d^%d mod 10^9+7 = %lld\n", base, exp, ans);
                printf("  >> Steps: %llu (vs %d in brute force)\n", m.iterations, exp);
                break;
            }
            case 3: {
                print_banner("3. CELEBRITY PROBLEM: CENTRAL TRANSIT HUB");
                printf("  [Elimination D&C Time: O(n) with 3(n-1) checks]\n\n");
                int adj[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS] = {0};
                /* Create 5-stop graph where Stop 2 (Clock Tower) is the hub */
                int n_stops = 5;
                for (int i = 1; i <= n_stops; i++) {
                    if (i != 2) adj[i][2] = 1; /* All feed into Clock Tower */
                }
                printf("  5-Stop Network: Stop 1: Ballupur, Stop 2: Clock Tower, Stop 3: ISBT, Stop 4: Prem Nagar, Stop 5: Rajpur\n");
                int hub = find_transit_hub_dc(adj, n_stops, &m, 1);
                if (hub != -1) {
                    printf("\n  >> Identified Central Hub: Stop ID %d (Clock Tower)\n", hub);
                } else {
                    printf("\n  >> No universal transit hub found in network.\n");
                }
                break;
            }
            case 4: {
                print_banner("4. CLASSICAL VS STRASSEN MATRIX MULTIPLICATION (4x4)");
                printf("  [Classical: O(n^3)=64 mults | Strassen: O(n^2.807)=49 mults]\n\n");
                int A[MATRIX_MAX][MATRIX_MAX] = {
                    {1, 2, 0, 1},
                    {0, 1, 1, 0},
                    {1, 0, 2, 1},
                    {0, 1, 0, 1}
                };
                int B[MATRIX_MAX][MATRIX_MAX] = {
                    {1, 0, 1, 0},
                    {0, 2, 0, 1},
                    {1, 1, 0, 0},
                    {0, 0, 1, 2}
                };
                int C1[MATRIX_MAX][MATRIX_MAX], C2[MATRIX_MAX][MATRIX_MAX];
                Metrics m1, m2;
                RESET_METRICS(m1);
                RESET_METRICS(m2);

                matrix_mult_classical(4, A, B, C1, &m1);
                matrix_mult_strassen(4, A, B, C2, &m2, 1);

                printf("\n  Result Matrix C = A x B:\n");
                for (int i = 0; i < 4; i++) {
                    printf("    [ ");
                    for (int j = 0; j < 4; j++) printf("%-3d ", C2[i][j]);
                    printf("]\n");
                }
                printf("\n  Classical Operations : %llu mults, %llu adds\n", (unsigned long long)64, (unsigned long long)48);
                printf("  Strassen Operations  : %llu mults, %llu adds\n", (unsigned long long)49, (unsigned long long)126);
                break;
            }
            case 5: {
                compare_divide_conquer_all();
                break;
            }
        }

        press_enter_to_continue();
    }
}

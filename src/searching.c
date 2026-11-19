/**
 * @file searching.c
 * @brief Searching algorithms implementation: Linear, Binary Iterative, Binary Recursive.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 * Preserves student's original function signatures and naming conventions.
 */

#include "../include/searching.h"
#include "../include/sorting.h"

/* ========================================================================= */
/*                             1. LINEAR SEARCH                              */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * LINEAR_SEARCH(A, n, key):
 *   for i = 0 to n - 1:
 *     if A[i].id == key:
 *       return i
 *   return -1
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Best Case    : O(1)     (Element found at index 0)
 *     Average Case : O(n)
 *     Worst Case   : O(n)     (Element at end or absent)
 *   Space Complexity: O(1)
 *
 * DOMAIN USE:
 *   Search unsorted stop lists or search by alphanumeric stop name.
 */
int linear_search_stop(const Stop arr[], int n, int target_id, Metrics *m, int verbose) {
    for (int i = 0; i < n; i++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        if (verbose && (i < 5 || arr[i].id == target_id)) {
            printf("    [Step %d] Checking stop ID %d ('%s') against target %d...\n",
                   i + 1, arr[i].id, arr[i].name, target_id);
        }
        if (arr[i].id == target_id) {
            if (verbose) {
                printf("    [Success] Target stop ID %d found at index %d!\n", target_id, i);
            }
            return i;
        }
    }
    if (verbose) {
        printf("    [Not Found] Target stop ID %d not found after %d inspections.\n", target_id, n);
    }
    return -1;
}

int linear_search_stop_by_name(const Stop arr[], int n, const char *name, Metrics *m) {
    for (int i = 0; i < n; i++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        if (strstr(arr[i].name, name) != NULL) {
            return i;
        }
    }
    return -1;
}

/* ========================================================================= */
/*                        2. BINARY SEARCH ITERATIVE                         */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * BINARY_SEARCH_ITERATIVE(A, n, key):
 *   low = 0, high = n - 1
 *   while low <= high:
 *     mid = low + (high - low) / 2
 *     if A[mid].id == key:
 *       return mid
 *     else if A[mid].id < key:
 *       low = mid + 1
 *     else:
 *       high = mid - 1
 *   return -1
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Best Case    : O(1)     (Target is the middle element)
 *     Average Case : O(log n)
 *     Worst Case   : O(log n)
 *   Space Complexity: O(1)    (Iterative pointer tracking)
 *
 * DOMAIN USE:
 *   Instant sub-millisecond timetable/stop lookup in sorted indexes.
 */
int binary_search_stop_iterative(const Stop arr[], int n, int target_id, Metrics *m, int verbose) {
    int low = 0;
    int high = n - 1;
    int step = 1;

    while (low <= high) {
        if (m) m->iterations++;
        int mid = low + (high - low) / 2;

        if (m) m->comparisons++;
        if (verbose) {
            printf("    [Step %d] Search range [%d..%d], mid index %d (Stop ID %d)\n",
                   step++, low, high, mid, arr[mid].id);
        }

        if (arr[mid].id == target_id) {
            if (verbose) {
                printf("    [Success] Target ID %d located at mid index %d!\n", target_id, mid);
            }
            return mid;
        }

        if (arr[mid].id < target_id) {
            if (verbose) printf("    [Branch Right] Target %d > mid %d, moving low to %d\n", target_id, arr[mid].id, mid + 1);
            low = mid + 1;
        } else {
            if (verbose) printf("    [Branch Left] Target %d < mid %d, moving high to %d\n", target_id, arr[mid].id, mid - 1);
            high = mid - 1;
        }
    }

    if (verbose) printf("    [Not Found] Search space exhausted.\n");
    return -1;
}

/* ========================================================================= */
/*                        3. BINARY SEARCH RECURSIVE                         */
/* ========================================================================= */

/**
 * RECURRENCE:
 *   T(n) = T(n/2) + O(1) -> By Master Theorem (Case 2 with k=0): T(n) = O(log n)
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Best Case    : O(1)
 *     Average Case : O(log n)
 *     Worst Case   : O(log n)
 *   Space Complexity: O(log n) call stack frames
 */
int binary_search_stop_recursive(const Stop arr[], int low, int high, int target_id, Metrics *m, int verbose) {
    if (low > high) {
        if (verbose) printf("    [Base Case] low (%d) > high (%d). Target absent.\n", low, high);
        return -1;
    }

    if (m) {
        m->iterations++;
        m->comparisons++;
    }

    int mid = low + (high - low) / 2;
    if (verbose) {
        printf("    [Recurse Level] Range [%d..%d], mid %d (Stop ID %d)\n",
               low, high, mid, arr[mid].id);
    }

    if (arr[mid].id == target_id) {
        if (verbose) printf("    [Success] Target ID %d found at index %d!\n", target_id, mid);
        return mid;
    }

    if (arr[mid].id < target_id) {
        return binary_search_stop_recursive(arr, mid + 1, high, target_id, m, verbose);
    } else {
        return binary_search_stop_recursive(arr, low, mid - 1, target_id, m, verbose);
    }
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_searching_algorithms(const Stop arr[], int n, int target_id) {
    if (n <= 0) {
        printf("  [!] Error: No stop data available to search.\n");
        return;
    }

    /* Create sorted copy by ID for fair binary search */
    Stop *sorted_stops = (Stop *)malloc(n * sizeof(Stop));
    if (!sorted_stops) return;
    memcpy(sorted_stops, arr, n * sizeof(Stop));

    /* Simple insertion sort by ID to ensure array is sorted */
    for (int i = 1; i < n; i++) {
        Stop key = sorted_stops[i];
        int j = i - 1;
        while (j >= 0 && sorted_stops[j].id > key.id) {
            sorted_stops[j + 1] = sorted_stops[j];
            j--;
        }
        sorted_stops[j + 1] = key;
    }

    Metrics m_lin, m_binit, m_binrec;
    RESET_METRICS(m_lin);
    RESET_METRICS(m_binit);
    RESET_METRICS(m_binrec);

    /* 1. Linear Search */
    clock_t t0 = timer_start();
    int idx_lin = linear_search_stop(sorted_stops, n, target_id, &m_lin, 0);
    m_lin.time_ms = timer_stop_ms(t0);

    /* 2. Binary Search Iterative */
    clock_t t1 = timer_start();
    int idx_bit = binary_search_stop_iterative(sorted_stops, n, target_id, &m_binit, 0);
    m_binit.time_ms = timer_stop_ms(t1);

    /* 3. Binary Search Recursive */
    clock_t t2 = timer_start();
    int idx_brec = binary_search_stop_recursive(sorted_stops, 0, n - 1, target_id, &m_binrec, 0);
    m_binrec.time_ms = timer_stop_ms(t2);

    print_banner("SEARCHING ALGORITHM COMPARISON (SAME TARGET ID)");
    printf("  Target Stop ID : %d\n", target_id);
    printf("  Dataset Size   : %d stops (Sorted by ID)\n\n", n);
    printf("  +-------------------------+-----------+-------------+------------+----------------+\n");
    printf("  | Algorithm               | Time (ms) | Comparisons | Iterations | Index Found    |\n");
    printf("  +-------------------------+-----------+-------------+------------+----------------+\n");
    printf("  | Linear Search           | %-9.4f | %-11llu | %-10llu | %-14d |\n",
           m_lin.time_ms, m_lin.comparisons, m_lin.iterations, idx_lin);
    printf("  | Binary Search Iterative | %-9.4f | %-11llu | %-10llu | %-14d |\n",
           m_binit.time_ms, m_binit.comparisons, m_binit.iterations, idx_bit);
    printf("  | Binary Search Recursive | %-9.4f | %-11llu | %-10llu | %-14d |\n",
           m_binrec.time_ms, m_binrec.comparisons, m_binrec.iterations, idx_brec);
    printf("  +-------------------------+-----------+-------------+------------+----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> Binary Search Iterative is the best choice: O(log n) time, O(1) auxiliary space, no stack frame overhead.\n");

    free(sorted_stops);
}

void run_searching_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 2: SEARCHING ALGORITHMS");
        printf("  1. Linear Search (Sequential trace)\n");
        printf("  2. Binary Search Iterative (Interval halving trace)\n");
        printf("  3. Binary Search Recursive (Call stack trace)\n");
        printf("  4. Compare all (Linear vs Binary Iterative vs Recursive)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 4);
        if (choice == 0) break;

        if (sys->stop_count == 0) {
            printf("  [!] Please load or enter stop data first (Menu 12)!\n");
            press_enter_to_continue();
            continue;
        }

        int target_id = get_int_input("Enter Stop ID to search", 1, 9999);
        Metrics m;
        RESET_METRICS(m);

        /* Pre-sort stops by ID for binary searches */
        Stop *sorted = (Stop *)malloc(sys->stop_count * sizeof(Stop));
        if (!sorted) continue;
        memcpy(sorted, sys->stops, sys->stop_count * sizeof(Stop));
        for (int i = 1; i < sys->stop_count; i++) {
            Stop key = sorted[i];
            int j = i - 1;
            while (j >= 0 && sorted[j].id > key.id) {
                sorted[j + 1] = sorted[j];
                j--;
            }
            sorted[j + 1] = key;
        }

        switch (choice) {
            case 1: {
                print_banner("1. LINEAR SEARCH TRACE");
                printf("  [Time Complexity: O(n) | Space: O(1)]\n\n");
                clock_t t0 = timer_start();
                int idx = linear_search_stop(sys->stops, sys->stop_count, target_id, &m, 1);
                m.time_ms = timer_stop_ms(t0);
                if (idx != -1) {
                    printf("\n  Found Stop: ID=%d, Name='%s', Demand=%d pax/hr\n",
                           sys->stops[idx].id, sys->stops[idx].name, sys->stops[idx].passengers);
                }
                printf("  Execution Time: %.4f ms | Comparisons: %llu\n", m.time_ms, m.comparisons);
                break;
            }
            case 2: {
                print_banner("2. BINARY SEARCH ITERATIVE TRACE");
                printf("  [Time Complexity: O(log n) | Space: O(1)]\n\n");
                clock_t t0 = timer_start();
                int idx = binary_search_stop_iterative(sorted, sys->stop_count, target_id, &m, 1);
                m.time_ms = timer_stop_ms(t0);
                if (idx != -1) {
                    printf("\n  Found Stop: ID=%d, Name='%s', Demand=%d pax/hr\n",
                           sorted[idx].id, sorted[idx].name, sorted[idx].passengers);
                }
                printf("  Execution Time: %.4f ms | Comparisons: %llu\n", m.time_ms, m.comparisons);
                break;
            }
            case 3: {
                print_banner("3. BINARY SEARCH RECURSIVE TRACE");
                printf("  [Recurrence: T(n) = T(n/2) + O(1) -> O(log n) | Space: O(log n)]\n\n");
                clock_t t0 = timer_start();
                int idx = binary_search_stop_recursive(sorted, 0, sys->stop_count - 1, target_id, &m, 1);
                m.time_ms = timer_stop_ms(t0);
                if (idx != -1) {
                    printf("\n  Found Stop: ID=%d, Name='%s', Demand=%d pax/hr\n",
                           sorted[idx].id, sorted[idx].name, sorted[idx].passengers);
                }
                printf("  Execution Time: %.4f ms | Comparisons: %llu\n", m.time_ms, m.comparisons);
                break;
            }
            case 4: {
                compare_searching_algorithms(sys->stops, sys->stop_count, target_id);
                break;
            }
        }

        free(sorted);
        press_enter_to_continue();
    }
}

/* ========================================================================= */
/*                   ORIGINAL STUDENT CODE WRAPPERS                          */
/* ========================================================================= */

int linearSearch(struct node stops[], int count, int id) {
    return linear_search_stop(stops, count, id, NULL, 0);
}

int binarySearchIterative(struct node stops[], int count, int id) {
    return binary_search_stop_iterative(stops, count, id, NULL, 0);
}

int binarySearchRecursive(struct node stops[], int low, int high, int id) {
    return binary_search_stop_recursive(stops, low, high, id, NULL, 0);
}

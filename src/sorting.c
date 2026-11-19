/**
 * @file sorting.c
 * @brief Sorting Algorithms Implementation: Bubble Sort, Merge Sort, Quick Sort.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 * Heavy comments, step-by-step traces, and complexity analysis for viva voce.
 */

#include "../include/sorting.h"

/* ========================================================================= */
/*                              1. BUBBLE SORT                               */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * BUBBLE_SORT(A, n):
 *   for i = 0 to n - 2:
 *     swapped = false
 *     for j = 0 to n - i - 2:
 *       if A[j].passengers < A[j+1].passengers: // Descending order
 *         swap(A[j], A[j+1])
 *         swapped = true
 *     if not swapped:
 *       break
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Best Case    : O(n)     (Already sorted array with swapped flag)
 *     Average Case : O(n^2)
 *     Worst Case   : O(n^2)   (Reverse sorted array)
 *   Space Complexity: O(1)    (In-place sorting)
 *   Stability       : Stable
 *
 * DOMAIN USE:
 *   Baseline educational sort for sorting small lists of bus stops by demand.
 */
void bubble_sort_stops_by_demand(Stop arr[], int n, Metrics *m, int verbose) {
    if (m) m->memory_bytes = sizeof(Stop) * n;

    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        if (m) m->iterations++;

        for (int j = 0; j < n - i - 1; j++) {
            if (m) m->comparisons++;

            /* Sort descending: highest passenger demand first */
            if (arr[j].passengers < arr[j + 1].passengers) {
                Stop temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
                if (m) m->assignments += 3;
            }
        }

        if (verbose && (i < 3 || i == n - 2 || swapped == 0)) {
            printf("    [Pass %d] Stop '%s' (%d pax) moved toward front.\n",
                   i + 1, arr[0].name, arr[0].passengers);
        }

        if (!swapped) {
            if (verbose) printf("    [Early Exit] Array fully sorted at pass %d.\n", i + 1);
            break;
        }
    }
}

/* ========================================================================= */
/*                               2. MERGE SORT                               */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * MERGE(A, l, m, r):
 *   n1 = m - l + 1, n2 = r - m
 *   Create arrays L[n1] and R[n2]
 *   Copy A[l..m] to L, A[m+1..r] to R
 *   i = 0, j = 0, k = l
 *   while i < n1 and j < n2:
 *     if L[i].passengers >= R[j].passengers:
 *       A[k++] = L[i++]
 *     else:
 *       A[k++] = R[j++]
 *   Copy remaining elements of L and R
 *
 * MERGE_SORT(A, l, r):
 *   if l < r:
 *     m = l + (r - l) / 2
 *     MERGE_SORT(A, l, m)
 *     MERGE_SORT(A, m + 1, r)
 *     MERGE(A, l, m, r)
 *
 * RECURRENCE:
 *   T(n) = 2T(n/2) + O(n) -> By Master Theorem: Case 2: T(n) = O(n log n)
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Best Case    : O(n log n)
 *     Average Case : O(n log n)
 *     Worst Case   : O(n log n)
 *   Space Complexity: O(n) auxiliary memory for merge buffer
 *   Stability       : Stable
 *
 * DOMAIN USE:
 *   Guaranteed O(n log n) predictability for large bus stop networks and timetable sorting.
 */
static void merge_stops(Stop arr[], int l, int m_idx, int r, Metrics *m, int verbose) {
    int n1 = m_idx - l + 1;
    int n2 = r - m_idx;

    Stop *L = (Stop *)malloc(n1 * sizeof(Stop));
    Stop *R = (Stop *)malloc(n2 * sizeof(Stop));
    if (!L || !R) {
        if (L) free(L);
        if (R) free(R);
        return;
    }

    if (m) m->memory_bytes += (n1 + n2) * sizeof(Stop);

    for (int i = 0; i < n1; i++) {
        L[i] = arr[l + i];
        if (m) m->assignments++;
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[m_idx + 1 + j];
        if (m) m->assignments++;
    }

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        /* Descending demand */
        if (L[i].passengers >= R[j].passengers) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        if (m) m->assignments++;
        k++;
    }

    while (i < n1) {
        arr[k++] = L[i++];
        if (m) m->assignments++;
    }
    while (j < n2) {
        arr[k++] = R[j++];
        if (m) m->assignments++;
    }

    if (verbose && (r - l >= 2)) {
        printf("    [Merge] Subarrays [%d..%d] and [%d..%d] merged into [%d..%d]\n",
               l, m_idx, m_idx + 1, r, l, r);
    }

    free(L);
    free(R);
}

void merge_sort_stops_by_demand(Stop arr[], int l, int r, Metrics *m, int verbose) {
    if (l < r) {
        int mid = l + (r - l) / 2;
        merge_sort_stops_by_demand(arr, l, mid, m, verbose);
        merge_sort_stops_by_demand(arr, mid + 1, r, m, verbose);
        merge_stops(arr, l, mid, r, m, verbose);
    }
}

/* ========================================================================= */
/*                               3. QUICK SORT                               */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * PARTITION(A, low, high):
 *   pivot = A[high].passengers
 *   i = low - 1
 *   for j = low to high - 1:
 *     if A[j].passengers >= pivot: // Descending order
 *       i++
 *       swap(A[i], A[j])
 *   swap(A[i+1], A[high])
 *   return i + 1
 *
 * QUICK_SORT(A, low, high):
 *   if low < high:
 *     pi = PARTITION(A, low, high)
 *     QUICK_SORT(A, low, pi - 1)
 *     QUICK_SORT(A, pi + 1, high)
 *
 * RECURRENCE:
 *   Average Case: T(n) = 2T(n/2) + O(n) -> O(n log n)
 *   Worst Case  : T(n) = T(n-1) + O(n) -> O(n^2) (Already sorted with bad pivot)
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     Best Case    : O(n log n)
 *     Average Case : O(n log n)
 *     Worst Case   : O(n^2)
 *   Space Complexity: O(log n) call stack
 *   Stability       : Unstable
 *
 * DOMAIN USE:
 *   In-place, cache-friendly sorting of bus lines and stops by throughput.
 */
static int partition_stops(Stop arr[], int low, int high, Metrics *m, int verbose) {
    int pivot_val = arr[high].passengers;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        /* Descending order */
        if (arr[j].passengers >= pivot_val) {
            i++;
            Stop temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            if (m) m->assignments += 3;
        }
    }

    Stop temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    if (m) m->assignments += 3;

    if (verbose && high - low >= 2) {
        printf("    [Partition] Pivot '%s' (%d pax) placed at index %d\n",
               arr[i + 1].name, arr[i + 1].passengers, i + 1);
    }

    return i + 1;
}

void quick_sort_stops_by_demand(Stop arr[], int low, int high, Metrics *m, int verbose) {
    if (low < high) {
        int pi = partition_stops(arr, low, high, m, verbose);
        quick_sort_stops_by_demand(arr, low, pi - 1, m, verbose);
        quick_sort_stops_by_demand(arr, pi + 1, high, m, verbose);
    }
}

void quick_sort_routes_by_cost(Route arr[], int low, int high, Metrics *m, int verbose) {
    if (low < high) {
        double pivot = arr[high].operating_cost;
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (m) m->comparisons++;
            if (arr[j].operating_cost <= pivot) {
                i++;
                Route temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
                if (m) m->assignments += 3;
            }
        }
        Route temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;
        if (m) m->assignments += 3;

        int pi = i + 1;
        if (verbose) {
            printf("    [Route Partition] Pivot '%s' (Cost Rs %.1f) placed at index %d\n",
                   arr[pi].name, arr[pi].operating_cost, pi);
        }
        quick_sort_routes_by_cost(arr, low, pi - 1, m, verbose);
        quick_sort_routes_by_cost(arr, pi + 1, high, m, verbose);
    }
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_sorting_algorithms(Stop arr[], int n) {
    if (n <= 0) {
        printf("  [!] Error: No stop data available to sort.\n");
        return;
    }

    Stop *copy1 = (Stop *)malloc(n * sizeof(Stop));
    Stop *copy2 = (Stop *)malloc(n * sizeof(Stop));
    Stop *copy3 = (Stop *)malloc(n * sizeof(Stop));
    if (!copy1 || !copy2 || !copy3) {
        printf("  [!] Memory allocation failed for comparison.\n");
        if (copy1) free(copy1);
        if (copy2) free(copy2);
        if (copy3) free(copy3);
        return;
    }

    memcpy(copy1, arr, n * sizeof(Stop));
    memcpy(copy2, arr, n * sizeof(Stop));
    memcpy(copy3, arr, n * sizeof(Stop));

    Metrics m_bubble, m_merge, m_quick;
    RESET_METRICS(m_bubble);
    RESET_METRICS(m_merge);
    RESET_METRICS(m_quick);

    /* 1. Run Bubble Sort */
    clock_t t0 = timer_start();
    bubble_sort_stops_by_demand(copy1, n, &m_bubble, 0);
    m_bubble.time_ms = timer_stop_ms(t0);

    /* 2. Run Merge Sort */
    clock_t t1 = timer_start();
    merge_sort_stops_by_demand(copy2, 0, n - 1, &m_merge, 0);
    m_merge.time_ms = timer_stop_ms(t1);

    /* 3. Run Quick Sort */
    clock_t t2 = timer_start();
    quick_sort_stops_by_demand(copy3, 0, n - 1, &m_quick, 0);
    m_quick.time_ms = timer_stop_ms(t2);

    print_banner("SORTING ALGORITHM COMPARISON (SAME DATASET)");
    printf("  Dataset Size: %d bus stops\n\n", n);
    printf("  +---------------+-----------+-------------+-------------+-----------+----------------+\n");
    printf("  | Algorithm     | Time (ms) | Comparisons | Assignments | Memory KB | Solution Order |\n");
    printf("  +---------------+-----------+-------------+-------------+-----------+----------------+\n");
    printf("  | Bubble Sort   | %-9.4f | %-11llu | %-11llu | %-9.2f | %-14s |\n",
           m_bubble.time_ms, m_bubble.comparisons, m_bubble.assignments,
           (double)m_bubble.memory_bytes / 1024.0, "Descending Demand");
    printf("  | Merge Sort    | %-9.4f | %-11llu | %-11llu | %-9.2f | %-14s |\n",
           m_merge.time_ms, m_merge.comparisons, m_merge.assignments,
           (double)m_merge.memory_bytes / 1024.0, "Descending Demand");
    printf("  | Quick Sort    | %-9.4f | %-11llu | %-11llu | %-9.2f | %-14s |\n",
           m_quick.time_ms, m_quick.comparisons, m_quick.assignments,
           (double)sizeof(Stop) * n / 1024.0, "Descending Demand");
    printf("  +---------------+-----------+-------------+-------------+-----------+----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    if (m_quick.time_ms <= m_merge.time_ms) {
        printf("  >> Quick Sort is the best choice: In-place O(n log n) with lowest constant factor and cache locality.\n");
    } else {
        printf("  >> Merge Sort is the best choice: Guaranteed O(n log n) worst-case time with stable relative order.\n");
    }

    free(copy1);
    free(copy2);
    free(copy3);
}

void run_sorting_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 1: SORTING ALGORITHMS");
        printf("  1. Bubble Sort (Trace step-by-step)\n");
        printf("  2. Merge Sort (Divide & Conquer Trace)\n");
        printf("  3. Quick Sort (Partitioning Trace)\n");
        printf("  4. Compare all (Bubble vs Merge vs Quick on same input)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 4);
        if (choice == 0) break;

        if (sys->stop_count == 0) {
            printf("  [!] Please load or enter stop data first (Menu 12)!\n");
            press_enter_to_continue();
            continue;
        }

        Stop *working_arr = (Stop *)malloc(sys->stop_count * sizeof(Stop));
        if (!working_arr) continue;
        memcpy(working_arr, sys->stops, sys->stop_count * sizeof(Stop));

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. BUBBLE SORT (DESCENDING PASSENGER DEMAND)");
                printf("  [Time Complexity: O(n^2) worst/avg, O(n) best | Space: O(1)]\n\n");
                clock_t t0 = timer_start();
                bubble_sort_stops_by_demand(working_arr, sys->stop_count, &m, 1);
                m.time_ms = timer_stop_ms(t0);
                display_stops(working_arr, sys->stop_count);
                printf("\n  Execution Time : %.4f ms\n", m.time_ms);
                printf("  Comparisons    : %llu\n", m.comparisons);
                printf("  Assignments    : %llu\n", m.assignments);
                break;
            }
            case 2: {
                print_banner("2. MERGE SORT (DIVIDE AND CONQUER)");
                printf("  [Recurrence: T(n) = 2T(n/2) + O(n) -> O(n log n) | Space: O(n)]\n\n");
                clock_t t0 = timer_start();
                merge_sort_stops_by_demand(working_arr, 0, sys->stop_count - 1, &m, 1);
                m.time_ms = timer_stop_ms(t0);
                display_stops(working_arr, sys->stop_count);
                printf("\n  Execution Time : %.4f ms\n", m.time_ms);
                printf("  Comparisons    : %llu\n", m.comparisons);
                printf("  Assignments    : %llu\n", m.assignments);
                break;
            }
            case 3: {
                print_banner("3. QUICK SORT (PARTITIONING)");
                printf("  [Average: O(n log n), Worst: O(n^2) | Space: O(log n)]\n\n");
                clock_t t0 = timer_start();
                quick_sort_stops_by_demand(working_arr, 0, sys->stop_count - 1, &m, 1);
                m.time_ms = timer_stop_ms(t0);
                display_stops(working_arr, sys->stop_count);
                printf("\n  Execution Time : %.4f ms\n", m.time_ms);
                printf("  Comparisons    : %llu\n", m.comparisons);
                printf("  Assignments    : %llu\n", m.assignments);
                break;
            }
            case 4: {
                compare_sorting_algorithms(sys->stops, sys->stop_count);
                break;
            }
        }

        free(working_arr);
        press_enter_to_continue();
    }
}

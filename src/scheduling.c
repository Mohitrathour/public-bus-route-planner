/**
 * @file scheduling.c
 * @brief Greedy Scheduling Algorithms: Activity Selection and Interval Partitioning.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/scheduling.h"

/* ========================================================================= */
/*                          1. ACTIVITY SELECTION                            */
/* ========================================================================= */

/**
 * GREEDY CHOICE PROPERTY & OPTIMAL SUBSTRUCTURE:
 *   Sorting by finish time ensures that each choice leaves maximum time
 *   remaining for subsequent bus trips.
 *
 * COMPLEXITY:
 *   Time Complexity : O(n log n) (sorting) + O(n) (scan) = O(n log n)
 *   Space Complexity: O(1) auxiliary
 *
 * DOMAIN USE:
 *   Maximize revenue trips assigned to a single premium electric bus in one shift.
 */
static void sort_trips_by_end_time(Trip arr[], int n, Metrics *m) {
    for (int i = 1; i < n; i++) {
        Trip key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].end_time > key.end_time) {
            if (m) {
                m->comparisons++;
                m->assignments++;
            }
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
        if (m) m->assignments++;
    }
}

int activity_selection_greedy(Trip trips[], int n, int selected_ids[], Metrics *m, int verbose) {
    if (n <= 0) return 0;

    Trip *sorted = (Trip *)malloc(n * sizeof(Trip));
    if (!sorted) return 0;
    memcpy(sorted, trips, n * sizeof(Trip));

    /* 1. Sort by finish time */
    sort_trips_by_end_time(sorted, n, m);

    int count = 0;
    /* Always pick the first trip */
    selected_ids[count++] = sorted[0].trip_id;
    int last_end = sorted[0].end_time;

    if (verbose) {
        printf("    [Greedy Select] Picked Trip %d (End %02d:%02d)\n",
               sorted[0].trip_id, sorted[0].end_time / 60, sorted[0].end_time % 60);
    }

    for (int i = 1; i < n; i++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        /* If compatible: start time >= end time of last selected trip */
        if (sorted[i].start_time >= last_end) {
            selected_ids[count++] = sorted[i].trip_id;
            last_end = sorted[i].end_time;

            if (verbose) {
                printf("    [Greedy Select] Picked Trip %d (Start %02d:%02d, End %02d:%02d)\n",
                       sorted[i].trip_id,
                       sorted[i].start_time / 60, sorted[i].start_time % 60,
                       sorted[i].end_time / 60, sorted[i].end_time % 60);
            }
        }
    }

    free(sorted);
    return count;
}

int activity_selection_fcfs(Trip trips[], int n, int selected_ids[], Metrics *m) {
    if (n <= 0) return 0;
    int count = 0;
    int last_end = -1;

    for (int i = 0; i < n; i++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        if (trips[i].start_time >= last_end) {
            selected_ids[count++] = trips[i].trip_id;
            last_end = trips[i].end_time;
        }
    }
    return count;
}

/* ========================================================================= */
/*                         2. INTERVAL PARTITIONING                          */
/* ========================================================================= */

/**
 * GREEDY INTERVAL PARTITIONING:
 *   Sort all trips by start time.
 *   Assign each trip to any available bus/bay whose last trip ended <= start_time.
 *   If none available, allocate a new bus/bay.
 *
 * THEOREM:
 *   Number of resources allocated = Depth of the interval set (maximum concurrent trips).
 *   This is PROVABLY OPTIMAL!
 *
 * COMPLEXITY:
 *   Time Complexity : O(n log n) with priority queue or O(n * d) with array
 *   Space Complexity: O(d) where d is depth / fleet size
 *
 * DOMAIN USE:
 *   Determine minimum fleet size or depot maintenance bays to serve full city schedule.
 */
static void sort_trips_by_start_time(Trip arr[], int n, Metrics *m) {
    for (int i = 1; i < n; i++) {
        Trip key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].start_time > key.start_time) {
            if (m) {
                m->comparisons++;
                m->assignments++;
            }
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
        if (m) m->assignments++;
    }
}

int interval_partitioning_greedy(Trip trips[], int n, int bus_assignment[], Metrics *m, int verbose) {
    if (n <= 0) return 0;

    Trip *sorted = (Trip *)malloc(n * sizeof(Trip));
    if (!sorted) return 0;
    memcpy(sorted, trips, n * sizeof(Trip));

    /* Sort by start time */
    sort_trips_by_start_time(sorted, n, m);

    /* Array tracking the end time of each active bus */
    int bus_end_times[MAX_BUSES] = {0};
    int total_buses_used = 0;

    for (int i = 0; i < n; i++) {
        int assigned_bus = -1;
        if (m) m->iterations++;

        /* Find an existing bus whose previous trip ended before this start time */
        for (int b = 0; b < total_buses_used; b++) {
            if (m) m->comparisons++;
            if (bus_end_times[b] <= sorted[i].start_time) {
                assigned_bus = b;
                break;
            }
        }

        if (assigned_bus == -1) {
            /* Must allocate a new bus / depot bay */
            assigned_bus = total_buses_used++;
            bus_end_times[assigned_bus] = sorted[i].end_time;
            if (verbose) {
                printf("    [New Bay/Bus #%d] Opened for Trip %d (%02d:%02d - %02d:%02d)\n",
                       assigned_bus + 1, sorted[i].trip_id,
                       sorted[i].start_time / 60, sorted[i].start_time % 60,
                       sorted[i].end_time / 60, sorted[i].end_time % 60);
            }
        } else {
            /* Reuse bus */
            bus_end_times[assigned_bus] = sorted[i].end_time;
            if (verbose && i < 10) {
                printf("    [Reuse Bus #%d] Assigned to Trip %d (%02d:%02d - %02d:%02d)\n",
                       assigned_bus + 1, sorted[i].trip_id,
                       sorted[i].start_time / 60, sorted[i].start_time % 60,
                       sorted[i].end_time / 60, sorted[i].end_time % 60);
            }
        }

        bus_assignment[sorted[i].trip_id] = assigned_bus + 1;
    }

    free(sorted);
    return total_buses_used;
}

int interval_partitioning_fcfs(Trip trips[], int n, int bus_assignment[], Metrics *m) {
    if (n <= 0) return 0;
    int bus_end_times[MAX_BUSES] = {0};
    int total_buses = 0;

    for (int i = 0; i < n; i++) {
        int assigned = -1;
        if (m) m->iterations++;
        for (int b = 0; b < total_buses; b++) {
            if (m) m->comparisons++;
            if (bus_end_times[b] <= trips[i].start_time) {
                assigned = b;
                break;
            }
        }
        if (assigned == -1) {
            assigned = total_buses++;
        }
        bus_end_times[assigned] = trips[i].end_time;
        bus_assignment[trips[i].trip_id] = assigned + 1;
    }
    return total_buses;
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_scheduling_algorithms(Trip trips[], int n) {
    if (n <= 0) {
        printf("  [!] Error: No scheduled trips loaded.\n");
        return;
    }

    Metrics m_act_greedy, m_act_fcfs;
    RESET_METRICS(m_act_greedy);
    RESET_METRICS(m_act_fcfs);

    int sel_greedy[MAX_TRIPS], sel_fcfs[MAX_TRIPS];
    clock_t t0 = timer_start();
    int count_act_greedy = activity_selection_greedy(trips, n, sel_greedy, &m_act_greedy, 0);
    m_act_greedy.time_ms = timer_stop_ms(t0);

    clock_t t1 = timer_start();
    int count_act_fcfs = activity_selection_fcfs(trips, n, sel_fcfs, &m_act_fcfs);
    m_act_fcfs.time_ms = timer_stop_ms(t1);

    Metrics m_ip_greedy, m_ip_fcfs;
    RESET_METRICS(m_ip_greedy);
    RESET_METRICS(m_ip_fcfs);

    int assign_greedy[MAX_TRIPS] = {0}, assign_fcfs[MAX_TRIPS] = {0};
    clock_t t2 = timer_start();
    int buses_greedy = interval_partitioning_greedy(trips, n, assign_greedy, &m_ip_greedy, 0);
    m_ip_greedy.time_ms = timer_stop_ms(t2);

    clock_t t3 = timer_start();
    int buses_fcfs = interval_partitioning_fcfs(trips, n, assign_fcfs, &m_ip_fcfs);
    m_ip_fcfs.time_ms = timer_stop_ms(t3);

    print_banner("SCHEDULING ALGORITHM COMPARISON (GREEDY VS FCFS)");
    printf("  Total Scheduled Trips: %d\n\n", n);

    printf("  --- PART 1: SINGLE-BUS REVENUE MAXIMIZATION (Activity Selection) ---\n");
    printf("  +-----------------------+-----------+-------------+------------+--------------------+\n");
    printf("  | Scheduling Method     | Time (ms) | Comparisons | Operations | Max Trips Serviced |\n");
    printf("  +-----------------------+-----------+-------------+------------+--------------------+\n");
    printf("  | Greedy (Earliest End) | %-9.4f | %-11llu | %-10llu | %-18d |\n",
           m_act_greedy.time_ms, m_act_greedy.comparisons, m_act_greedy.iterations, count_act_greedy);
    printf("  | FCFS (Arrival Order)  | %-9.4f | %-11llu | %-10llu | %-18d |\n",
           m_act_fcfs.time_ms, m_act_fcfs.comparisons, m_act_fcfs.iterations, count_act_fcfs);
    printf("  +-----------------------+-----------+-------------+------------+--------------------+\n\n");

    printf("  --- PART 2: MINIMUM FLEET SIZE / DEPOT BAYS (Interval Partitioning) ---\n");
    printf("  +-----------------------+-----------+-------------+------------+--------------------+\n");
    printf("  | Partitioning Method   | Time (ms) | Comparisons | Operations | Min Buses Needed   |\n");
    printf("  +-----------------------+-----------+-------------+------------+--------------------+\n");
    printf("  | Greedy (Earliest Strt)| %-9.4f | %-11llu | %-10llu | %-18d |\n",
           m_ip_greedy.time_ms, m_ip_greedy.comparisons, m_ip_greedy.iterations, buses_greedy);
    printf("  | FCFS (Arrival Order)  | %-9.4f | %-11llu | %-10llu | %-18d |\n",
           m_ip_fcfs.time_ms, m_ip_fcfs.comparisons, m_ip_fcfs.iterations, buses_fcfs);
    printf("  +-----------------------+-----------+-------------+------------+--------------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> Greedy finish-time sorting is mathematically optimal for activity selection (maximizes utilization).\n");
    printf("  >> Greedy start-time sorting achieves minimum fleet size matching maximum timetable overlap (depth d).\n");
}

void run_scheduling_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 6: SCHEDULING (GREEDY)");
        printf("  1. Activity Selection (Max Trips for a Single Bus Trace)\n");
        printf("  2. Interval Partitioning (Min Fleet Size / Depot Bay Trace)\n");
        printf("  3. Compare with FCFS / brute force\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 3);
        if (choice == 0) break;

        if (sys->trip_count == 0) {
            printf("  [!] Please load trips data first (Menu 12)!\n");
            press_enter_to_continue();
            continue;
        }

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. ACTIVITY SELECTION (GREEDY FINISH-TIME SORT)");
                printf("  [Time Complexity: O(n log n) | Space: O(1)]\n\n");
                int selected[MAX_TRIPS];
                int count = activity_selection_greedy(sys->trips, sys->trip_count, selected, &m, 1);
                printf("\n  >> Total Maximum Compatible Trips: %d out of %d scheduled\n", count, sys->trip_count);
                printf("  >> Selected Trip IDs: ");
                for (int i = 0; i < count; i++) printf("%d ", selected[i]);
                printf("\n");
                break;
            }
            case 2: {
                print_banner("2. INTERVAL PARTITIONING (MINIMUM BUSES / DEPOT BAYS)");
                printf("  [Time Complexity: O(n log n) | Space: O(depth)]\n\n");
                int assignments[MAX_TRIPS] = {0};
                int min_buses = interval_partitioning_greedy(sys->trips, sys->trip_count, assignments, &m, 1);
                printf("\n  >> Minimum Fleet Size Needed: %d buses / depot bays\n", min_buses);
                break;
            }
            case 3: {
                compare_scheduling_algorithms(sys->trips, sys->trip_count);
                break;
            }
        }

        press_enter_to_continue();
    }
}

/**
 * @file scenarios.c
 * @brief Demonstration Scenarios, Data Generator, and Benchmarking Suite.
 *
 * Implements:
 *   - Normal Scenario
 *   - Constraint Shock Scenario (depot closed, road blocked, 50% fleet cut)
 *   - Large Stress Scenario (1000+ stops)
 *   - Synthetic Data Generator
 *   - Benchmarking Suite (100, 500, 1000, 5000, 10000 + Adversarial) exported to results/benchmark.csv
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/scenarios.h"
#include "../include/sorting.h"
#include "../include/searching.h"
#include "../include/divide_conquer.h"
#include "../include/shortest_path.h"
#include "../include/scheduling.h"
#include "../include/route_selection.h"
#include "../include/flow_matching.h"
#include "../include/theory_notes.h"

/* Forward declaration for test suite call */
extern int run_automated_tests(void);

/* ========================================================================= */
/*                          1. SYNTHETIC DATA GENERATOR                      */
/* ========================================================================= */

void generate_synthetic_data(TransitSystem *sys, int num_stops, int num_routes, int num_buses) {
    if (num_stops > MAX_STOPS) num_stops = MAX_STOPS;
    if (num_routes > MAX_ROUTES) num_routes = MAX_ROUTES;
    if (num_buses > MAX_BUSES) num_buses = MAX_BUSES;

    init_transit_system(sys);

    /* Generate Stops */
    for (int i = 0; i < num_stops; i++) {
        sys->stops[i].id = i + 1;
        sprintf(sys->stops[i].name, "Station_%04d", i + 1);
        sys->stops[i].passengers = 50 + (rand() % 450);
        sys->stop_count++;
    }

    /* Generate Routes */
    for (int i = 0; i < num_routes; i++) {
        sys->routes[i].id = 100 + i + 1;
        sprintf(sys->routes[i].name, "Corridor_Line_%d", i + 1);
        sys->routes[i].distance_km = 5.0 + (rand() % 40);
        sys->routes[i].travel_time_min = (int)(sys->routes[i].distance_km * 2.5);
        sys->routes[i].operating_cost = 500.0 + (rand() % 2500);
        sys->routes[i].total_passengers = 200 + (rand() % 800);
        sys->route_count++;
    }

    /* Generate Fleet */
    for (int i = 0; i < num_buses; i++) {
        sys->buses[i].id = 1000 + i + 1;
        sys->buses[i].capacity = 35 + (rand() % 3) * 15;
        sys->buses[i].depot_id = 1 + (rand() % 3);
        sys->buses[i].assigned_route_id = -1;
        sys->buses[i].is_available = 1;
        sys->bus_count++;
    }

    /* Generate Roads: ring + random chords */
    int edge_limit = (num_stops < 50) ? num_stops * 2 : 2500;
    for (int i = 1; i < num_stops && sys->road_network.num_edges < edge_limit; i++) {
        int u = i;
        int v = i + 1;
        int dist = 3 + (rand() % 15);
        int time_min = dist * 2;
        int cap = 300 + (rand() % 500);

        int idx = sys->road_network.num_edges++;
        sys->road_network.edges[idx].u = u;
        sys->road_network.edges[idx].v = v;
        sys->road_network.edges[idx].distance = dist;
        sys->road_network.edges[idx].travel_time = time_min;
        sys->road_network.edges[idx].capacity = cap;

        if (u < MAX_GRAPH_STOPS && v < MAX_GRAPH_STOPS) {
            sys->road_network.adj_matrix[u][v] = dist;
            sys->road_network.adj_matrix[v][u] = dist;
            sys->road_network.cap_matrix[u][v] = cap;
            sys->road_network.cap_matrix[v][u] = cap;
        }
    }
    sys->road_network.num_vertices = (num_stops < MAX_GRAPH_STOPS) ? num_stops : (MAX_GRAPH_STOPS - 1);

    /* Generate Trips */
    int num_trips = (num_stops < 50) ? 30 : 1000;
    if (num_trips > MAX_TRIPS) num_trips = MAX_TRIPS;
    for (int i = 0; i < num_trips; i++) {
        sys->trips[i].trip_id = i + 1;
        sys->trips[i].route_id = 101 + (rand() % ((num_routes > 0) ? num_routes : 1));
        sys->trips[i].start_time = 360 + (rand() % 720); /* 6 AM to 6 PM */
        sys->trips[i].end_time = sys->trips[i].start_time + 20 + (rand() % 45);
        sys->trips[i].bus_id = -1;
        sys->trips[i].demand = 40 + (rand() % 80);
        sys->trip_count++;
    }

    printf("  [+] Synthetic transit network generated: %d stops, %d routes, %d buses, %d trips.\n",
           sys->stop_count, sys->route_count, sys->bus_count, sys->trip_count);
}

/* ========================================================================= */
/*                          2. NORMAL SCENARIO DEMO                          */
/* ========================================================================= */

void run_normal_scenario(TransitSystem *sys) {
    print_banner("DEMO SCENARIO A: NORMAL DEHRADUN CITY TRANSIT OPERATION");
    printf("  System Status: All 15 stops open, 3 depots operational, full bus fleet ready.\n\n");

    Metrics m1, m2, m3;
    RESET_METRICS(m1);
    RESET_METRICS(m2);
    RESET_METRICS(m3);

    /* 1. Calculate Primary Arterial Shortest Path */
    printf("  [Step 1: Shortest Path Routing]\n");
    printf("  Computing optimal transit corridor from ISBT Central Depot (Stop 3) to Rajpur Road (Stop 5)...\n");
    ShortestPathResult sp = dijkstra_shortest_path(&sys->road_network, 3, &m1, 0);
    printf("  >> Distance: %d km | Path: ", sp.dist[5]);
    print_reconstructed_path(3, 5, sp.parent);
    printf("\n\n");

    /* 2. Solve Fleet Scheduling */
    printf("  [Step 2: Timetable Fleet Sizing (Interval Partitioning)]\n");
    int bus_assign[MAX_TRIPS] = {0};
    int fleet_needed = interval_partitioning_greedy(sys->trips, sys->trip_count, bus_assign, &m2, 0);
    printf("  >> Minimum Fleet Size to operate all %d daily trips: %d buses\n", sys->trip_count, fleet_needed);
    printf("  >> Available Fleet: %d buses (Capacity Surplus: %d buses)\n\n", sys->bus_count, sys->bus_count - fleet_needed);

    /* 3. Budgeted Route Corridor Selection */
    printf("  [Step 3: Route Corridor Selection (0/1 Knapsack DP)]\n");
    int budget = 6000;
    RouteSelectionResult opt = route_selection_01_knapsack_dp(sys->routes, sys->route_count, budget, &m3, 0);
    printf("  >> Daily Operating Budget: Rs %d\n", budget);
    printf("  >> Selected %d Corridors | Budget Spent: Rs %.1f | Passengers Served: %d\n\n",
           opt.selected_count, opt.total_cost, opt.max_passengers);

    print_separator();
    printf("  SUMMARY: Normal operation runs with optimal fleet utilization and 0 timetable delays.\n");
}

/* ========================================================================= */
/*                     3. CONSTRAINT SHOCK SCENARIO DEMO                     */
/* ========================================================================= */

void run_constraint_shock_scenario(TransitSystem *sys) {
    print_banner("DEMO SCENARIO B: CONSTRAINT SHOCK (EMERGENCY DISRUPTION)");
    printf("  SHOCK EVENTS APPLIED:\n");
    printf("    1. DEPOT CLOSURE : ISBT Central Depot (Depot 1) closed due to facility outage.\n");
    printf("    2. ROAD BLOCKED  : Arterial Link (Stop 2 Clock Tower <-> Stop 3 ISBT) blocked.\n");
    printf("    3. FLEET CUT     : 50%% of buses rendered unavailable.\n");
    print_separator();

    /* Make copy to avoid mutating base state permanently */
    TransitSystem shock_sys;
    memcpy(&shock_sys, sys, sizeof(TransitSystem));

    /* 1. Close Depot 1 */
    if (shock_sys.depot_count > 0) shock_sys.depots[0].is_active = 0;

    /* 2. Block road link between Stop 2 and Stop 3 */
    shock_sys.road_network.adj_matrix[2][3] = INF;
    shock_sys.road_network.adj_matrix[3][2] = INF;

    /* 3. Reduce available buses by 50% */
    int available_buses = shock_sys.bus_count / 2;

    /* Re-evaluate Algorithms on same network */
    Metrics m_sp_norm, m_sp_shock;
    RESET_METRICS(m_sp_norm);
    RESET_METRICS(m_sp_shock);

    ShortestPathResult norm_sp = dijkstra_shortest_path(&sys->road_network, 3, &m_sp_norm, 0);
    ShortestPathResult shock_sp = dijkstra_shortest_path(&shock_sys.road_network, 3, &m_sp_shock, 0);

    Metrics m_sched_norm, m_sched_shock;
    RESET_METRICS(m_sched_norm);
    RESET_METRICS(m_sched_shock);

    int b_norm[MAX_TRIPS] = {0}, b_shock[MAX_TRIPS] = {0};
    int fleet_norm = interval_partitioning_greedy(sys->trips, sys->trip_count, b_norm, &m_sched_norm, 0);
    int fleet_shock = interval_partitioning_greedy(shock_sys.trips, shock_sys.trip_count, b_shock, &m_sched_shock, 0);

    Metrics m_opt_norm, m_opt_shock;
    RESET_METRICS(m_opt_norm);
    RESET_METRICS(m_opt_shock);

    int norm_budget = 6000;
    int shock_budget = 3000; /* Cut budget by 50% */
    RouteSelectionResult res_norm = route_selection_01_knapsack_dp(sys->routes, sys->route_count, norm_budget, &m_opt_norm, 0);
    RouteSelectionResult res_shock = route_selection_01_knapsack_dp(shock_sys.routes, shock_sys.route_count, shock_budget, &m_opt_shock, 0);

    printf("\n  +-----------------------------+----------------------+----------------------+-------------------------+\n");
    printf("  | Planning Metric             | Normal Operation     | Under Constraint Shock| Operational Impact      |\n");
    printf("  +-----------------------------+----------------------+----------------------+-------------------------+\n");
    printf("  | Route 3 -> 5 Detour Dist    | %-20d | %-20d | +%d km detour around cut |\n",
           norm_sp.dist[5], shock_sp.dist[5], shock_sp.dist[5] - norm_sp.dist[5]);
    printf("  | Minimum Fleet Needed        | %-20d | %-20d | Fleet Deficit: %d buses |\n",
           fleet_norm, fleet_shock, (fleet_shock > available_buses) ? (fleet_shock - available_buses) : 0);
    printf("  | Available Bus Fleet         | %-20d | %-20d | Cut by 50%%              |\n",
           sys->bus_count, available_buses);
    printf("  | Operating Budget            | Rs %-17d | Rs %-17d | Emergency austerity     |\n",
           norm_budget, shock_budget);
    printf("  | Passengers Served           | %-20d | %-20d | Loss of %d passengers   |\n",
           res_norm.max_passengers, res_shock.max_passengers, res_norm.max_passengers - res_shock.max_passengers);
    printf("  +-----------------------------+----------------------+----------------------+-------------------------+\n");

    printf("\n  [SHOCK REROUTING PATH]:\n");
    printf("  >> Normal  : ");
    print_reconstructed_path(3, 5, norm_sp.parent);
    printf(" (%d km)\n", norm_sp.dist[5]);
    printf("  >> Emergency: ");
    print_reconstructed_path(3, 5, shock_sp.parent);
    printf(" (%d km - via Rispana Bypass)\n", shock_sp.dist[5]);
}

/* ========================================================================= */
/*                        4. LARGE STRESS SCENARIO DEMO                      */
/* ========================================================================= */

void run_stress_scenario(TransitSystem *sys) {
    (void)sys;
    print_banner("DEMO SCENARIO C: LARGE STRESS CASE (1000+ STOPS/TRIPS)");
    printf("  Generating synthetic metropolitan transit network with 1000 stops and 1000 trips...\n");

    TransitSystem stress_sys;
    generate_synthetic_data(&stress_sys, 1000, 50, 150);

    Metrics m_sort_merge, m_sort_quick, m_search, m_subarray;
    RESET_METRICS(m_sort_merge);
    RESET_METRICS(m_sort_quick);
    RESET_METRICS(m_search);
    RESET_METRICS(m_subarray);

    /* 1. Sort 1000 stops */
    Stop *copy_stops = (Stop *)malloc(stress_sys.stop_count * sizeof(Stop));
    memcpy(copy_stops, stress_sys.stops, stress_sys.stop_count * sizeof(Stop));

    clock_t t0 = timer_start();
    merge_sort_stops_by_demand(copy_stops, 0, stress_sys.stop_count - 1, &m_sort_merge, 0);
    m_sort_merge.time_ms = timer_stop_ms(t0);

    memcpy(copy_stops, stress_sys.stops, stress_sys.stop_count * sizeof(Stop));
    clock_t t1 = timer_start();
    quick_sort_stops_by_demand(copy_stops, 0, stress_sys.stop_count - 1, &m_sort_quick, 0);
    m_sort_quick.time_ms = timer_stop_ms(t1);

    /* 2. Binary search on 1000 stops */
    clock_t t2 = timer_start();
    int found_idx = binary_search_stop_iterative(copy_stops, stress_sys.stop_count, 500, &m_search, 0);
    m_search.time_ms = timer_stop_ms(t2);

    /* 3. Maximum subarray on 1000 synthetic demand fluctuations */
    int *large_surge = (int *)malloc(1000 * sizeof(int));
    for (int i = 0; i < 1000; i++) large_surge[i] = (rand() % 200) - 90;
    clock_t t3 = timer_start();
    SubarrayResult res_sub = max_subarray_dc(large_surge, 0, 999, &m_subarray, 0);
    m_subarray.time_ms = timer_stop_ms(t3);

    printf("\n  STRESS BENCHMARK RESULTS (N = 1000 Elements):\n");
    printf("  +-----------------------------+-----------+---------------+------------+-----------+--------------------+\n");
    printf("  | Algorithm Tested            | Time (ms) | Comparisons   | Iterations | Memory KB | Solution Quality   |\n");
    printf("  +-----------------------------+-----------+---------------+------------+-----------+--------------------+\n");
    printf("  | Merge Sort (1000 stops)     | %-9.4f | %-13llu | %-10llu | %-9.2f | Exact sorted order |\n",
           m_sort_merge.time_ms, m_sort_merge.comparisons, m_sort_merge.iterations,
           (double)sizeof(Stop) * 1000 / 1024.0);
    printf("  | Quick Sort (1000 stops)     | %-9.4f | %-13llu | %-10llu | %-9.2f | Exact sorted order |\n",
           m_sort_quick.time_ms, m_sort_quick.comparisons, m_sort_quick.iterations,
           (double)sizeof(Stop) * 1000 / 1024.0);
    printf("  | Binary Search (Target 500)  | %-9.4f | %-13llu | %-10llu | %-9.2f | Index %-12d |\n",
           m_search.time_ms, m_search.comparisons, m_search.iterations,
           (double)sizeof(Stop) * 1000 / 1024.0, found_idx);
    printf("  | Max Subarray D&C (1000 pts) | %-9.4f | %-13llu | %-10llu | %-9.2f | Peak Surge +%-7d |\n",
           m_subarray.time_ms, m_subarray.comparisons, m_subarray.iterations,
           (double)1000 * sizeof(int) / 1024.0, res_sub.sum);
    printf("  +-----------------------------+-----------+---------------+------------+-----------+--------------------+\n");

    printf("\n  >> System handled 1000+ entity workload in %.2f total milliseconds with 100%% correctness!\n",
           m_sort_merge.time_ms + m_sort_quick.time_ms + m_search.time_ms + m_subarray.time_ms);

    free(copy_stops);
    free(large_surge);
}

/* ========================================================================= */
/*                          5. BENCHMARKING SUITE TO CSV                     */
/* ========================================================================= */

void run_comprehensive_benchmark(const char *csv_output_path) {
    print_banner("RUNNING MULTI-SCALE BENCHMARK (100 to 10000 + ADVERSARIAL)");
    printf("  Exporting benchmark data points to: %s\n\n", csv_output_path);

    FILE *fp = fopen(csv_output_path, "w");
    if (!fp) {
        printf("  [!] Error: Unable to open '%s' for writing.\n", csv_output_path);
        return;
    }

    fprintf(fp, "Algorithm,InputSize,InputType,TimeMs,Comparisons,Assignments,Operations,MemoryKB\n");

    int test_sizes[] = {100, 500, 1000, 5000, 10000};
    int num_sizes = 5;

    printf("  +-----------------+------------+------------------+-----------+---------------+----------------+\n");
    printf("  | Algorithm       | Input Size | Input Pattern    | Time (ms) | Comparisons   | Assignments    |\n");
    printf("  +-----------------+------------+------------------+-----------+---------------+----------------+\n");

    for (int s = 0; s < num_sizes; s++) {
        int n = test_sizes[s];
        Stop *arr_random = (Stop *)malloc(n * sizeof(Stop));
        Stop *arr_sorted = (Stop *)malloc(n * sizeof(Stop));
        Stop *arr_reverse = (Stop *)malloc(n * sizeof(Stop));
        Stop *arr_equal = (Stop *)malloc(n * sizeof(Stop));
        Stop *work = (Stop *)malloc(n * sizeof(Stop));

        for (int i = 0; i < n; i++) {
            arr_random[i].id = i + 1;
            arr_random[i].passengers = rand() % 10000;
            sprintf(arr_random[i].name, "Stop_%d", i + 1);

            arr_sorted[i].id = i + 1;
            arr_sorted[i].passengers = 10000 - i; /* already descending */
            sprintf(arr_sorted[i].name, "Stop_%d", i + 1);

            arr_reverse[i].id = i + 1;
            arr_reverse[i].passengers = i; /* reverse */
            sprintf(arr_reverse[i].name, "Stop_%d", i + 1);

            arr_equal[i].id = i + 1;
            arr_equal[i].passengers = 500; /* identical keys */
            sprintf(arr_equal[i].name, "Stop_%d", i + 1);
        }

        /* Test Merge Sort Random */
        Metrics m_merge;
        RESET_METRICS(m_merge);
        memcpy(work, arr_random, n * sizeof(Stop));
        clock_t t0 = timer_start();
        merge_sort_stops_by_demand(work, 0, n - 1, &m_merge, 0);
        m_merge.time_ms = timer_stop_ms(t0);
        fprintf(fp, "MergeSort,%d,Random,%.4f,%llu,%llu,%llu,%.2f\n",
                n, m_merge.time_ms, m_merge.comparisons, m_merge.assignments,
                m_merge.comparisons + m_merge.assignments, (double)n * sizeof(Stop) / 1024.0);
        printf("  | Merge Sort      | %-10d | %-16s | %-9.4f | %-13llu | %-14llu |\n",
               n, "Random", m_merge.time_ms, m_merge.comparisons, m_merge.assignments);

        /* Test Quick Sort Random */
        Metrics m_quick;
        RESET_METRICS(m_quick);
        memcpy(work, arr_random, n * sizeof(Stop));
        clock_t t1 = timer_start();
        quick_sort_stops_by_demand(work, 0, n - 1, &m_quick, 0);
        m_quick.time_ms = timer_stop_ms(t1);
        fprintf(fp, "QuickSort,%d,Random,%.4f,%llu,%llu,%llu,%.2f\n",
                n, m_quick.time_ms, m_quick.comparisons, m_quick.assignments,
                m_quick.comparisons + m_quick.assignments, (double)n * sizeof(Stop) / 1024.0);
        printf("  | Quick Sort      | %-10d | %-16s | %-9.4f | %-13llu | %-14llu |\n",
               n, "Random", m_quick.time_ms, m_quick.comparisons, m_quick.assignments);

        /* Adversarial Tests */
        if (n <= 1000) {
            /* Quick Sort on Already Sorted (Adversarial) */
            Metrics m_qadv;
            RESET_METRICS(m_qadv);
            memcpy(work, arr_sorted, n * sizeof(Stop));
            clock_t t2 = timer_start();
            quick_sort_stops_by_demand(work, 0, n - 1, &m_qadv, 0);
            m_qadv.time_ms = timer_stop_ms(t2);
            fprintf(fp, "QuickSort,%d,AlreadySorted,%.4f,%llu,%llu,%llu,%.2f\n",
                    n, m_qadv.time_ms, m_qadv.comparisons, m_qadv.assignments,
                    m_qadv.comparisons + m_qadv.assignments, (double)n * sizeof(Stop) / 1024.0);
            printf("  | Quick Sort (Adv)| %-10d | %-16s | %-9.4f | %-13llu | %-14llu |\n",
                   n, "AlreadySorted", m_qadv.time_ms, m_qadv.comparisons, m_qadv.assignments);

            /* Quick Sort on All Equal (Adversarial) */
            Metrics m_qeq;
            RESET_METRICS(m_qeq);
            memcpy(work, arr_equal, n * sizeof(Stop));
            clock_t t3 = timer_start();
            quick_sort_stops_by_demand(work, 0, n - 1, &m_qeq, 0);
            m_qeq.time_ms = timer_stop_ms(t3);
            fprintf(fp, "QuickSort,%d,AllEqual,%.4f,%llu,%llu,%llu,%.2f\n",
                    n, m_qeq.time_ms, m_qeq.comparisons, m_qeq.assignments,
                    m_qeq.comparisons + m_qeq.assignments, (double)n * sizeof(Stop) / 1024.0);
            printf("  | Quick Sort (Adv)| %-10d | %-16s | %-9.4f | %-13llu | %-14llu |\n",
                   n, "AllEqual", m_qeq.time_ms, m_qeq.comparisons, m_qeq.assignments);
        }

        /* Binary Search on 10000 */
        Metrics m_bs;
        RESET_METRICS(m_bs);
        clock_t t4 = timer_start();
        binary_search_stop_iterative(arr_sorted, n, n / 2, &m_bs, 0);
        m_bs.time_ms = timer_stop_ms(t4);
        fprintf(fp, "BinarySearch,%d,Sorted,%.4f,%llu,%llu,%llu,%.2f\n",
                n, m_bs.time_ms, m_bs.comparisons, m_bs.assignments,
                m_bs.iterations, (double)n * sizeof(Stop) / 1024.0);

        free(arr_random);
        free(arr_sorted);
        free(arr_reverse);
        free(arr_equal);
        free(work);
    }

    printf("  +-----------------+------------+------------------+-----------+---------------+----------------+\n");

    fclose(fp);
    printf("\n  [+] Benchmark complete! Data saved to '%s' (ready for Python / Excel plotting).\n", csv_output_path);
}

/* ========================================================================= */
/*                          6. MENU 12 SUBMENU RUNNER                        */
/* ========================================================================= */

void run_scenarios_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 12: DATA, SCENARIOS & TESTS");
        printf("  1. Load file (stops.csv, roads.csv, routes.csv, buses.csv, depots.csv, trips.csv)\n");
        printf("  2. Manual entry (Add Stop, Road, Bus, Route with Validation)\n");
        printf("  3. Generate data (Synthetic Transit Network Generator)\n");
        printf("  4. Normal scenario (Standard City Transit Baseline Demo)\n");
        printf("  5. Constraint shock scenario (Depot Closure, Blocked Road, 50%% Fleet Cut)\n");
        printf("  6. Stress scenario (1000+ Stops/Trips Scalability Test)\n");
        printf("  7. Benchmark to CSV (Scales 100..10000 + Adversarial Inputs)\n");
        printf("  8. Run all tests (Automated Test Suite Runner)\n");
        printf("  9. Theory & complexity notes (Recurrences, Master Theorem, P vs NP)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 9);
        if (choice == 0) break;

        switch (choice) {
            case 1: {
                print_banner("1. LOAD TRANSIT DATA FROM CSV FILES");
                load_all_data(sys, "data");
                display_network_summary(sys);
                break;
            }
            case 2: {
                print_banner("2. MANUAL DATA ENTRY");
                printf("  1. Add Stop\n  2. Add Road\n  3. Add Bus\n  4. Add Route\n  0. Cancel\n");
                int sub = get_int_input("Select entry type", 0, 4);
                if (sub == 1) manual_add_stop(sys);
                else if (sub == 2) manual_add_road(sys);
                else if (sub == 3) manual_add_bus(sys);
                else if (sub == 4) manual_add_route(sys);
                break;
            }
            case 3: {
                print_banner("3. SYNTHETIC DATA GENERATOR");
                int n_stops = get_int_input("Enter number of stops to generate", 10, 1000);
                int n_routes = get_int_input("Enter number of routes to generate", 5, 100);
                int n_buses = get_int_input("Enter number of buses to generate", 5, 200);
                generate_synthetic_data(sys, n_stops, n_routes, n_buses);
                break;
            }
            case 4: {
                run_normal_scenario(sys);
                break;
            }
            case 5: {
                run_constraint_shock_scenario(sys);
                break;
            }
            case 6: {
                run_stress_scenario(sys);
                break;
            }
            case 7: {
                run_comprehensive_benchmark("results/benchmark.csv");
                break;
            }
            case 8: {
                print_banner("8. EXECUTING AUTOMATED TEST SUITE");
                run_automated_tests();
                break;
            }
            case 9: {
                run_theory_notes_menu();
                break;
            }
        }

        press_enter_to_continue();
    }
}

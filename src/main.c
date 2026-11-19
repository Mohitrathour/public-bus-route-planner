/**
 * @file main.c
 * @brief Public Bus Route Frequency and Depot Planner (Project 12).
 *        CSEG3053 - Design and Analysis of Algorithms (DAA).
 *
 * Senior C mentor implementation designed for B.Tech student examination.
 * Strict 12-Topic Menu Layout + 0 Exit.
 */

#include "../include/common.h"
#include "../include/models.h"
#include "../include/sorting.h"
#include "../include/searching.h"
#include "../include/divide_conquer.h"
#include "../include/shortest_path.h"
#include "../include/spanning_tree.h"
#include "../include/scheduling.h"
#include "../include/route_selection.h"
#include "../include/dp_tsp.h"
#include "../include/flow_matching.h"
#include "../include/string_matching.h"
#include "../include/compression_sync.h"
#include "../include/scenarios.h"

extern int run_automated_tests(void);

int main(int argc, char *argv[]) {
    /* Initialize random seed */
    srand((unsigned int)time(NULL));

    /* Initialize system state and attempt auto-loading default datasets */
    init_transit_system(&g_system);
    print_banner("PUBLIC BUS ROUTE FREQUENCY & DEPOT PLANNER");
    printf("  Project 12: Design and Analysis of Algorithms (CSEG3053)\n");
    printf("  B.Tech Computer Science & Engineering | Academic Year 2026\n");
    print_separator();

    /* Try loading default datasets */
    int loaded = load_all_data(&g_system, "data");
    if (loaded == 0) {
        /* Fallback attempt to original subfolder */
        load_all_data(&g_system, "bus planner/data");
    }

    /* CLI Flag handling for direct verification & benchmarking */
    if (argc > 1) {
        if (strcmp(argv[1], "--test") == 0) {
            return run_automated_tests();
        } else if (strcmp(argv[1], "--benchmark") == 0) {
            run_comprehensive_benchmark("results/benchmark.csv");
            return 0;
        } else if (strcmp(argv[1], "--normal") == 0) {
            run_normal_scenario(&g_system);
            return 0;
        } else if (strcmp(argv[1], "--shock") == 0) {
            run_constraint_shock_scenario(&g_system);
            return 0;
        } else if (strcmp(argv[1], "--stress") == 0) {
            run_stress_scenario(&g_system);
            return 0;
        }
    }

    int choice = -1;
    while (choice != 0) {
        printf("\n");
        printf("======================================================================\n");
        printf("                    PUBLIC BUS PLANNER - MAIN MENU                    \n");
        printf("======================================================================\n");
        printf("  1.  Sorting (Bubble, Merge, Quick, Compare all)\n");
        printf("  2.  Searching (Linear, Binary iterative, Binary recursive, Compare)\n");
        printf("  3.  Divide & Conquer (Max subarray, Fast power, Celebrity, Strassen)\n");
        printf("  4.  Shortest Path (Dijkstra, Bellman-Ford, Floyd-Warshall, Compare)\n");
        printf("  5.  Spanning Tree (Prim, Kruskal with Union-Find, Compare)\n");
        printf("  6.  Scheduling (Greedy Activity Selection, Interval Partitioning)\n");
        printf("  7.  Route Selection (Greedy, DP, Backtracking, B&B FIFO/LIFO/LC, N-Queens)\n");
        printf("  8.  DP & TSP (LCS, Matrix Chain, OBST, TSP Brute Force vs Bitmask DP)\n");
        printf("  9.  Flow & Matching (Ford-Fulkerson, Edmonds-Karp, Bipartite Matching)\n");
        printf("  10. String Matching (Naive, Rabin-Karp, Finite Automaton, KMP)\n");
        printf("  11. Compression & Sync (Huffman, Fixed-length, CRT Schedule Sync)\n");
        printf("  12. Data, Scenarios & Tests (Load, Generate, Normal/Shock/Stress, Benchmark)\n");
        printf("  0.  Exit Program\n");
        printf("======================================================================\n");

        choice = get_int_input("Enter your choice", 0, 12);

        switch (choice) {
            case 1:  run_sorting_menu(&g_system); break;
            case 2:  run_searching_menu(&g_system); break;
            case 3:  run_divide_conquer_menu(&g_system); break;
            case 4:  run_shortest_path_menu(&g_system); break;
            case 5:  run_spanning_tree_menu(&g_system); break;
            case 6:  run_scheduling_menu(&g_system); break;
            case 7:  run_route_selection_menu(&g_system); break;
            case 8:  run_dp_tsp_menu(&g_system); break;
            case 9:  run_flow_matching_menu(&g_system); break;
            case 10: run_string_matching_menu(&g_system); break;
            case 11: run_compression_sync_menu(&g_system); break;
            case 12: run_scenarios_menu(&g_system); break;
            case 0:
                print_banner("THANK YOU FOR USING PUBLIC BUS PLANNER!");
                printf("  Academic project by B.Tech student (Project 12, CSEG3053).\n");
                printf("  Exiting application...\n\n");
                break;
            default:
                printf("  [!] Invalid selection! Please enter an integer from 0 to 12.\n");
                break;
        }
    }

    return 0;
}

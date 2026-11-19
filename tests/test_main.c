/**
 * @file test_main.c
 * @brief Automated Test Suite for all DAA algorithms in Project 12.
 *
 * Verifies Normal, Boundary, Invalid, Stress, and Adversarial test cases
 * against known mathematical solutions or brute-force benchmarks.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "test_runner.h"
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

TestRecord g_test_results[MAX_TESTS];
int g_total_tests = 0;
int g_passed_tests = 0;

void test_init(void) {
    g_total_tests = 0;
    g_passed_tests = 0;
}

void test_record(const char *category, const char *name, const char *expected, const char *actual, int passed) {
    if (g_total_tests >= MAX_TESTS) return;
    strncpy(g_test_results[g_total_tests].category, category, 31);
    strncpy(g_test_results[g_total_tests].name, name, 63);
    strncpy(g_test_results[g_total_tests].expected, expected, 47);
    strncpy(g_test_results[g_total_tests].actual, actual, 47);
    g_test_results[g_total_tests].passed = passed;

    if (passed) g_passed_tests++;
    g_total_tests++;
}

void test_print_summary(void) {
    print_banner("AUTOMATED TEST EXECUTION SUMMARY TABLE");
    printf("  +-----------------+--------------------------------------+------------+------------+--------+\n");
    printf("  | Category        | Test Case Description                | Expected   | Actual     | Status |\n");
    printf("  +-----------------+--------------------------------------+------------+------------+--------+\n");

    for (int i = 0; i < g_total_tests; i++) {
        printf("  | %-15s | %-36s | %-10s | %-10s | %s |\n",
               g_test_results[i].category,
               g_test_results[i].name,
               g_test_results[i].expected,
               g_test_results[i].actual,
               g_test_results[i].passed ? "[PASS]" : "[FAIL]");
    }

    printf("  +-----------------+--------------------------------------+------------+------------+--------+\n");
    printf("  Test Verdict: %d / %d Tests Passed (%.1f%% Success Rate)\n",
           g_passed_tests, g_total_tests, (double)g_passed_tests * 100.0 / g_total_tests);
    print_separator();
}

int run_automated_tests(void) {
    test_init();

    /* ===================================================================== */
    /* 1. SORTING TESTS                                                      */
    /* ===================================================================== */
    {
        Stop stops[5] = {
            {1, "Ballupur", 120},
            {2, "Clock Tower", 150},
            {3, "ISBT", 200},
            {4, "Prem Nagar", 90},
            {5, "Rajpur Road", 180}
        };

        Stop test_bubble[5], test_merge[5], test_quick[5];
        memcpy(test_bubble, stops, 5 * sizeof(Stop));
        memcpy(test_merge, stops, 5 * sizeof(Stop));
        memcpy(test_quick, stops, 5 * sizeof(Stop));

        bubble_sort_stops_by_demand(test_bubble, 5, NULL, 0);
        merge_sort_stops_by_demand(test_merge, 0, 4, NULL, 0);
        quick_sort_stops_by_demand(test_quick, 0, 4, NULL, 0);

        ASSERT_INT_EQ("Sorting", "BubbleSort Max Demand Stop", test_bubble[0].id, 3);
        ASSERT_INT_EQ("Sorting", "BubbleSort Min Demand Stop", test_bubble[4].id, 4);
        ASSERT_INT_EQ("Sorting", "MergeSort Max Demand Stop", test_merge[0].id, 3);
        ASSERT_INT_EQ("Sorting", "QuickSort Max Demand Stop", test_quick[0].id, 3);

        /* Boundary Case: Single element */
        Stop single[1] = {{1, "Solo", 100}};
        merge_sort_stops_by_demand(single, 0, 0, NULL, 0);
        ASSERT_INT_EQ("Sorting", "Boundary: Single Element", single[0].passengers, 100);

        /* Adversarial Case: Identical elements */
        Stop ident[3] = {{1, "A", 50}, {2, "B", 50}, {3, "C", 50}};
        quick_sort_stops_by_demand(ident, 0, 2, NULL, 0);
        ASSERT_INT_EQ("Sorting", "Adversarial: All Equal Keys", ident[0].passengers, 50);
    }

    /* ===================================================================== */
    /* 2. SEARCHING TESTS                                                    */
    /* ===================================================================== */
    {
        Stop sorted_stops[5] = {
            {1, "Ballupur", 120},
            {2, "Clock Tower", 150},
            {3, "ISBT", 200},
            {4, "Prem Nagar", 90},
            {5, "Rajpur Road", 180}
        };

        int idx_lin = linear_search_stop(sorted_stops, 5, 3, NULL, 0);
        int idx_bit = binary_search_stop_iterative(sorted_stops, 5, 3, NULL, 0);
        int idx_brec = binary_search_stop_recursive(sorted_stops, 0, 4, 3, NULL, 0);
        int idx_missing = binary_search_stop_iterative(sorted_stops, 5, 999, NULL, 0);

        ASSERT_INT_EQ("Searching", "LinearSearch Target in Middle", idx_lin, 2);
        ASSERT_INT_EQ("Searching", "BinarySearch Iterative Middle", idx_bit, 2);
        ASSERT_INT_EQ("Searching", "BinarySearch Recursive Middle", idx_brec, 2);
        ASSERT_INT_EQ("Searching", "Invalid: Target Missing", idx_missing, -1);
    }

    /* ===================================================================== */
    /* 3. DIVIDE & CONQUER TESTS                                             */
    /* ===================================================================== */
    {
        /* Max Subarray */
        int surge[9] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
        SubarrayResult res_dc = max_subarray_dc(surge, 0, 8, NULL, 0);
        SubarrayResult res_kad = max_subarray_kadane(surge, 9, NULL);
        ASSERT_INT_EQ("Divide&Conquer", "MaxSubarray D&C Sum", res_dc.sum, 6);
        ASSERT_INT_EQ("Divide&Conquer", "MaxSubarray Kadane Sum", res_kad.sum, 6);

        /* Fast Exponentiation */
        long long p1 = power_fast_dc(2, 10, 1000000007LL, NULL, 0);
        long long p2 = power_fast_dc(3, 0, 1000000007LL, NULL, 0);
        ASSERT_INT_EQ("Divide&Conquer", "FastPower 2^10", (int)p1, 1024);
        ASSERT_INT_EQ("Divide&Conquer", "FastPower Base 0 Exponent", (int)p2, 1);

        /* Celebrity Hub */
        int adj[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS] = {0};
        for (int i = 1; i <= 4; i++) {
            if (i != 2) adj[i][2] = 1; /* Stop 2 is hub */
        }
        int hub = find_transit_hub_dc(adj, 4, NULL, 0);
        ASSERT_INT_EQ("Divide&Conquer", "Celebrity Transit Hub", hub, 2);

        /* Strassen Matrix Multiplication 2x2 */
        int A[MATRIX_MAX][MATRIX_MAX] = {{1, 2}, {3, 4}};
        int B[MATRIX_MAX][MATRIX_MAX] = {{2, 0}, {1, 2}};
        int C_classic[MATRIX_MAX][MATRIX_MAX];
        int C_strassen[MATRIX_MAX][MATRIX_MAX];
        matrix_mult_classical(2, A, B, C_classic, NULL);
        matrix_mult_strassen(2, A, B, C_strassen, NULL, 0);
        ASSERT_INT_EQ("Divide&Conquer", "Strassen 2x2 C[0][0]", C_strassen[0][0], C_classic[0][0]);
        ASSERT_INT_EQ("Divide&Conquer", "Strassen 2x2 C[1][1]", C_strassen[1][1], C_classic[1][1]);
    }

    /* ===================================================================== */
    /* 4. SHORTEST PATH TESTS                                                */
    /* ===================================================================== */
    {
        Graph g;
        g.num_vertices = 3;
        g.num_edges = 3;
        for (int i = 0; i < MAX_GRAPH_STOPS; i++) {
            for (int j = 0; j < MAX_GRAPH_STOPS; j++) g.adj_matrix[i][j] = (i == j) ? 0 : INF;
        }
        /* Triangle graph: 1->2 (3km), 2->3 (4km), 1->3 (10km) */
        g.adj_matrix[1][2] = 3; g.adj_matrix[2][1] = 3;
        g.adj_matrix[2][3] = 4; g.adj_matrix[3][2] = 4;
        g.adj_matrix[1][3] = 10; g.adj_matrix[3][1] = 10;

        g.edges[0] = (Edge){1, 2, 3, 9, 500};
        g.edges[1] = (Edge){2, 3, 4, 12, 500};
        g.edges[2] = (Edge){1, 3, 10, 30, 500};

        ShortestPathResult dij = dijkstra_shortest_path(&g, 1, NULL, 0);
        ShortestPathResult bf = bellman_ford_shortest_path(&g, 1, NULL, 0);

        int dist_fw[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
        int next_fw[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
        floyd_warshall_all_pairs(&g, dist_fw, next_fw, NULL, 0);

        ASSERT_INT_EQ("ShortestPath", "Dijkstra Triangle Shortest Dist", dij.dist[3], 7);
        ASSERT_INT_EQ("ShortestPath", "BellmanFord Triangle Shortest Dist", bf.dist[3], 7);
        ASSERT_INT_EQ("ShortestPath", "FloydWarshall All Pairs Dist 1->3", dist_fw[1][3], 7);
    }

    /* ===================================================================== */
    /* 5. SPANNING TREE TESTS                                                */
    /* ===================================================================== */
    {
        Graph g;
        g.num_vertices = 4;
        g.num_edges = 5;
        for (int i = 0; i < MAX_GRAPH_STOPS; i++) {
            for (int j = 0; j < MAX_GRAPH_STOPS; j++) g.adj_matrix[i][j] = (i == j) ? 0 : INF;
        }
        /* 4 nodes: 1-2 (1), 2-3 (2), 3-4 (3), 4-1 (4), 1-3 (5) */
        g.adj_matrix[1][2] = 1; g.adj_matrix[2][1] = 1;
        g.adj_matrix[2][3] = 2; g.adj_matrix[3][2] = 2;
        g.adj_matrix[3][4] = 3; g.adj_matrix[4][3] = 3;
        g.adj_matrix[4][1] = 4; g.adj_matrix[1][4] = 4;
        g.adj_matrix[1][3] = 5; g.adj_matrix[3][1] = 5;

        g.edges[0] = (Edge){1, 2, 1, 3, 400};
        g.edges[1] = (Edge){2, 3, 2, 6, 400};
        g.edges[2] = (Edge){3, 4, 3, 9, 400};
        g.edges[3] = (Edge){4, 1, 4, 12, 400};
        g.edges[4] = (Edge){1, 3, 5, 15, 400};

        MSTResult prim_res = prim_mst(&g, NULL, 0);
        MSTResult krusk_res = kruskal_mst(&g, NULL, 0);

        ASSERT_INT_EQ("SpanningTree", "Prim MST Total Weight", prim_res.total_weight, 6);
        ASSERT_INT_EQ("SpanningTree", "Kruskal MST Total Weight", krusk_res.total_weight, 6);
        ASSERT_INT_EQ("SpanningTree", "MST Edge Count (V-1)", krusk_res.edge_count, 3);
    }

    /* ===================================================================== */
    /* 6. SCHEDULING (GREEDY) TESTS                                          */
    /* ===================================================================== */
    {
        Trip trips[4] = {
            {1, 101, 60, 120, -1, 50},  /* 1:00 to 2:00 */
            {2, 102, 100, 150, -1, 60}, /* 1:40 to 2:30 (overlaps 1) */
            {3, 103, 130, 190, -1, 70}, /* 2:10 to 3:10 (compatible with 1) */
            {4, 104, 200, 250, -1, 80}  /* 3:20 to 4:10 (compatible with 3) */
        };

        int sel[4];
        int act_count = activity_selection_greedy(trips, 4, sel, NULL, 0);
        ASSERT_INT_EQ("Scheduling", "Activity Selection Optimal Trips", act_count, 3);

        int bus_assign[10] = {0};
        int min_buses = interval_partitioning_greedy(trips, 4, bus_assign, NULL, 0);
        ASSERT_INT_EQ("Scheduling", "Interval Partitioning Min Fleet", min_buses, 2);
    }

    /* ===================================================================== */
    /* 7. ROUTE SELECTION (ALL OPTIMIZATION METHODS AGREE)                   */
    /* ===================================================================== */
    {
        Route test_routes[4] = {
            {101, "R1", {0}, 0, 10.0, 20, 1000.0, 500},
            {102, "R2", {0}, 0, 12.0, 25, 2000.0, 700},
            {103, "R3", {0}, 0, 15.0, 30, 3000.0, 1200},
            {104, "R4", {0}, 0, 8.0, 15, 1500.0, 600}
        };
        int budget = 4500;

        RouteSelectionResult dp = route_selection_01_knapsack_dp(test_routes, 4, budget, NULL, 0);
        RouteSelectionResult bt = route_selection_backtracking(test_routes, 4, budget, NULL, 0);
        RouteSelectionResult fifo = route_selection_bb_fifo(test_routes, 4, budget, NULL, 0);
        RouteSelectionResult lifo = route_selection_bb_lifo(test_routes, 4, budget, NULL, 0);
        RouteSelectionResult lc = route_selection_bb_least_cost(test_routes, 4, budget, NULL, 0);

        ASSERT_INT_EQ("RouteSelection", "0/1 Knapsack DP Optimum", dp.max_passengers, 1800);
        ASSERT_INT_EQ("RouteSelection", "Backtracking Matches DP", bt.max_passengers, dp.max_passengers);
        ASSERT_INT_EQ("RouteSelection", "B&B FIFO Matches DP", fifo.max_passengers, dp.max_passengers);
        ASSERT_INT_EQ("RouteSelection", "B&B LIFO Matches DP", lifo.max_passengers, dp.max_passengers);
        ASSERT_INT_EQ("RouteSelection", "B&B Least-Cost Matches DP", lc.max_passengers, dp.max_passengers);

        /* N-Queens 4x4 */
        int board[20][20];
        int nq_solved = n_queens_solve(4, board, NULL, 0);
        ASSERT_INT_EQ("RouteSelection", "N-Queens 4x4 Solvable", nq_solved, 1);
    }

    /* ===================================================================== */
    /* 8. DP & TSP TESTS                                                     */
    /* ===================================================================== */
    {
        /* LCS */
        int s1[5] = {1, 2, 3, 4, 5};
        int s2[5] = {2, 4, 5, 6, 7};
        LCSResult lcs = lcs_routes(s1, 5, s2, 5, NULL, 0);
        ASSERT_INT_EQ("DP_TSP", "LCS Common Subsequence Length", lcs.length, 3);

        /* MCM */
        int dims[3] = {10, 20, 30};
        MCMResult mcm = matrix_chain_order(dims, 2, NULL, 0);
        ASSERT_INT_EQ("DP_TSP", "Matrix Chain Single Mult", mcm.min_scalar_mults, 6000);

        /* TSP 4 stops */
        int dist_tsp[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS] = {0};
        dist_tsp[1][2] = 10; dist_tsp[2][1] = 10;
        dist_tsp[2][3] = 15; dist_tsp[3][2] = 15;
        dist_tsp[3][4] = 20; dist_tsp[4][3] = 20;
        dist_tsp[4][1] = 25; dist_tsp[1][4] = 25;
        dist_tsp[1][3] = 35; dist_tsp[3][1] = 35;
        dist_tsp[2][4] = 30; dist_tsp[4][2] = 30;

        TSPResult tsp_bf = tsp_brute_force(dist_tsp, 4, NULL, 0);
        TSPResult tsp_dp = tsp_bitmask_dp(dist_tsp, 4, NULL, 0);
        ASSERT_INT_EQ("DP_TSP", "TSP Brute Force 4 Stops", tsp_bf.min_cost, 70);
        ASSERT_INT_EQ("DP_TSP", "TSP Bitmask DP Matches BF", tsp_dp.min_cost, 70);
    }

    /* ===================================================================== */
    /* 9. NETWORK FLOW & MATCHING                                            */
    /* ===================================================================== */
    {
        int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS] = {0};
        /* 4 nodes: 1 is src, 4 is sink */
        cap[1][2] = 10;
        cap[1][3] = 10;
        cap[2][4] = 10;
        cap[3][4] = 10;
        cap[2][3] = 2;

        FlowResult ff = ford_fulkerson_dfs(cap, 4, 1, 4, NULL, 0);
        FlowResult ek = edmonds_karp_bfs(cap, 4, 1, 4, NULL, 0);
        ASSERT_INT_EQ("FlowMatching", "Ford-Fulkerson Max Flow", ff.max_flow, 20);
        ASSERT_INT_EQ("FlowMatching", "Edmonds-Karp Matches FF", ek.max_flow, 20);

        /* Bipartite Matching */
        int bp[MAX_BIPARTITE][MAX_BIPARTITE] = {0};
        bp[0][0] = 1; bp[1][1] = 1; bp[2][2] = 1;
        BipartiteResult bpm = max_bipartite_matching(bp, 3, 3, NULL, 0);
        ASSERT_INT_EQ("FlowMatching", "Bipartite Matching Perfect 3", bpm.max_matches, 3);
    }

    /* ===================================================================== */
    /* 10. STRING MATCHING TESTS                                             */
    /* ===================================================================== */
    {
        const char *txt = "DEHRADUN_ISBT_CENTRAL_DEPOT_DEHRADUN";
        const char *pat = "DEHRADUN";

        StringMatchResult m_naive = naive_string_search(txt, pat, NULL, 0);
        StringMatchResult m_rk = rabin_karp_search(txt, pat, NULL, 0);
        StringMatchResult m_fa = finite_automaton_search(txt, pat, NULL, 0);
        StringMatchResult m_kmp = kmp_search(txt, pat, NULL, 0);

        ASSERT_INT_EQ("StringMatch", "Naive Occurrences", m_naive.match_count, 2);
        ASSERT_INT_EQ("StringMatch", "Rabin-Karp Matches Naive", m_rk.match_count, 2);
        ASSERT_INT_EQ("StringMatch", "Finite Automaton Matches Naive", m_fa.match_count, 2);
        ASSERT_INT_EQ("StringMatch", "KMP Matches Naive", m_kmp.match_count, 2);
    }

    /* ===================================================================== */
    /* 11. COMPRESSION & SYNCHRONIZATION                                     */
    /* ===================================================================== */
    {
        const char *sample = "AAAAABBBBCCCDDE";
        CompressionResult comp = huffman_compress(sample, NULL, 0);
        ASSERT_TRUE("Compression", "Huffman Bits Less Than Original", comp.compressed_bits < comp.original_bits);

        /* Chinese Remainder Theorem: x = 2 mod 3, x = 3 mod 5, x = 2 mod 7 -> x = 23 */
        int a[3] = {2, 3, 2};
        int m[3] = {3, 5, 7};
        CRTResult crt = solve_crt_bus_schedules(a, m, 3, NULL, 0);
        ASSERT_INT_EQ("Sync", "CRT Solution x = 23", (int)crt.synchronized_minute, 23);
    }

    test_print_summary();
    return (g_passed_tests == g_total_tests) ? 0 : 1;
}

#ifdef TEST_STANDALONE_RUNNER
int main(void) {
    return run_automated_tests();
}
#endif

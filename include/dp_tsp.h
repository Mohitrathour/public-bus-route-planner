/**
 * @file dp_tsp.h
 * @brief Dynamic Programming and Traveling Salesperson Problem (TSP):
 *        1. Longest Common Subsequence (LCS - Route Corridor Alignment)
 *        2. Matrix Chain Multiplication (MCM - Multi-transfer matrix optimization)
 *        3. Optimal Binary Search Tree (OBST - Stop lookup frequency optimization)
 *        4. Bounded TSP Brute Force
 *        5. Bounded TSP Bitmask DP (Held-Karp Algorithm)
 */

#ifndef DP_TSP_H
#define DP_TSP_H

#include "models.h"

/* LCS */
typedef struct {
    int length;
    int common_stops[MAX_ROUTE_STOPS];
} LCSResult;

LCSResult lcs_routes(const int seq1[], int n1, const int seq2[], int n2, Metrics *m, int verbose);

/* Matrix Chain Multiplication */
typedef struct {
    int min_scalar_mults;
    int split_table[20][20];
} MCMResult;

MCMResult matrix_chain_order(const int p[], int n, Metrics *m, int verbose);
void print_optimal_mcm_parens(int i, int j, const int s[20][20]);

/* Optimal Binary Search Tree */
typedef struct {
    double min_cost;
    int root_table[30][30];
} OBSTResult;

OBSTResult optimal_bst(const int keys[], const double p[], const double q[], int n, Metrics *m, int verbose);

/* Bounded TSP (max 12 stops) */
#define MAX_TSP_STOPS 12

typedef struct {
    int min_cost;
    int tour[MAX_TSP_STOPS + 1];
    int tour_len;
} TSPResult;

TSPResult tsp_brute_force(const int dist[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, Metrics *m, int verbose);
TSPResult tsp_bitmask_dp(const int dist[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, Metrics *m, int verbose);

/* Submenu and Comparison */
void run_dp_tsp_menu(TransitSystem *sys);
void compare_tsp_methods(const int dist[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n);

#endif /* DP_TSP_H */

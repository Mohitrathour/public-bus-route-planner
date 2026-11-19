/**
 * @file flow_matching.h
 * @brief Network Flow and Maximum Bipartite Matching algorithms:
 *        1. Ford-Fulkerson (DFS-based augmenting paths)
 *        2. Edmonds-Karp (BFS-based augmenting paths)
 *        3. Maximum Bipartite Matching (Buses to Routes / Drivers to Shifts)
 */

#ifndef FLOW_MATCHING_H
#define FLOW_MATCHING_H

#include "models.h"

/* Max Flow Result */
typedef struct {
    int max_flow;
    int augmentations_count;
    int residual[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
} FlowResult;

/* Maximum Flow Algorithms */
FlowResult ford_fulkerson_dfs(const int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, int src, int sink, Metrics *m, int verbose);
FlowResult edmonds_karp_bfs(const int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, int src, int sink, Metrics *m, int verbose);

/* Bipartite Matching */
#define MAX_BIPARTITE 50

typedef struct {
    int max_matches;
    int match_l_to_r[MAX_BIPARTITE];
    int match_r_to_l[MAX_BIPARTITE];
} BipartiteResult;

BipartiteResult max_bipartite_matching(const int bp_graph[MAX_BIPARTITE][MAX_BIPARTITE], int n_left, int n_right, Metrics *m, int verbose);

/* Submenu and Comparison */
void run_flow_matching_menu(TransitSystem *sys);
void compare_flow_methods(const int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, int src, int sink);

#endif /* FLOW_MATCHING_H */

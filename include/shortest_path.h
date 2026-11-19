/**
 * @file shortest_path.h
 * @brief Shortest path algorithms: Dijkstra, Bellman-Ford, Floyd-Warshall.
 *
 * Domain Use:
 *   Calculate minimum travel distance and time between bus stops and depots,
 *   supporting real-time bus rerouting and schedule generation.
 */

#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "models.h"

/* Result container for single-source shortest path */
typedef struct {
    int dist[MAX_GRAPH_STOPS];
    int parent[MAX_GRAPH_STOPS];
    int has_negative_cycle;
} ShortestPathResult;

/* Dijkstra's Algorithm */
ShortestPathResult dijkstra_shortest_path(const Graph *g, int src, Metrics *m, int verbose);

/* Bellman-Ford Algorithm */
ShortestPathResult bellman_ford_shortest_path(const Graph *g, int src, Metrics *m, int verbose);

/* Floyd-Warshall Algorithm */
void floyd_warshall_all_pairs(const Graph *g, int dist[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS],
                             int next[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS],
                             Metrics *m, int verbose);

/* Path reconstruction helper */
void print_reconstructed_path(int src, int dest, const int parent[]);

/* Submenu and Comparison */
void run_shortest_path_menu(TransitSystem *sys);
void compare_shortest_path_algorithms(const Graph *g, int src, int dest);

#endif /* SHORTEST_PATH_H */

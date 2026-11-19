/**
 * @file spanning_tree.h
 * @brief Minimum Spanning Tree algorithms: Prim's and Kruskal's (with Union-Find).
 *
 * Domain Use:
 *   Design minimum-cost transit backbone network connecting all bus depots
 *   and major passenger terminals with minimal road paving/maintenance expenditure.
 */

#ifndef SPANNING_TREE_H
#define SPANNING_TREE_H

#include "models.h"

/* Result container for MST */
typedef struct {
    Edge edges[MAX_ROADS];
    int edge_count;
    int total_weight;
} MSTResult;

/* Disjoint Set Union (Union-Find) struct */
typedef struct {
    int parent[MAX_GRAPH_STOPS];
    int rank[MAX_GRAPH_STOPS];
    int count;
} DSU;

void dsu_init(DSU *dsu, int n);
int dsu_find(DSU *dsu, int i);
int dsu_union(DSU *dsu, int i, int j);

/* Prim's Algorithm */
MSTResult prim_mst(const Graph *g, Metrics *m, int verbose);

/* Kruskal's Algorithm */
MSTResult kruskal_mst(const Graph *g, Metrics *m, int verbose);

/* Submenu and Comparison */
void run_spanning_tree_menu(TransitSystem *sys);
void compare_spanning_tree_algorithms(const Graph *g);

#endif /* SPANNING_TREE_H */

/**
 * @file spanning_tree.c
 * @brief Minimum Spanning Tree (MST) algorithms: Prim's and Kruskal's (with Union-Find).
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/spanning_tree.h"

/* ========================================================================= */
/*                    DISJOINT SET UNION (UNION-FIND)                        */
/* ========================================================================= */

/**
 * PATH COMPRESSION & UNION BY RANK:
 *   Find: amortized almost O(1) via alpha(n) (Inverse Ackermann function)
 *   Union: balances tree height by attaching shallower root to deeper root.
 */
void dsu_init(DSU *dsu, int n) {
    dsu->count = n;
    for (int i = 0; i <= n; i++) {
        dsu->parent[i] = i;
        dsu->rank[i] = 0;
    }
}

int dsu_find(DSU *dsu, int i) {
    if (dsu->parent[i] == i) {
        return i;
    }
    /* Path compression */
    dsu->parent[i] = dsu_find(dsu, dsu->parent[i]);
    return dsu->parent[i];
}

int dsu_union(DSU *dsu, int i, int j) {
    int root_i = dsu_find(dsu, i);
    int root_j = dsu_find(dsu, j);

    if (root_i == root_j) {
        return 0; /* Cycle detected */
    }

    /* Union by rank */
    if (dsu->rank[root_i] < dsu->rank[root_j]) {
        dsu->parent[root_i] = root_j;
    } else if (dsu->rank[root_i] > dsu->rank[root_j]) {
        dsu->parent[root_j] = root_i;
    } else {
        dsu->parent[root_j] = root_i;
        dsu->rank[root_i]++;
    }
    dsu->count--;
    return 1;
}

/* ========================================================================= */
/*                           1. PRIM'S ALGORITHM                             */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * PRIM_MST(G):
 *   key[v] = INF, parent[v] = -1, in_mst[v] = false for all v
 *   key[1] = 0
 *   for count = 0 to V - 1:
 *     u = min_key_vertex(not in_mst)
 *     in_mst[u] = true
 *     for each neighbor v of u:
 *       if weight(u, v) < key[v] and not in_mst[v]:
 *         key[v] = weight(u, v)
 *         parent[v] = u
 *
 * COMPLEXITY:
 *   Adjacency Matrix : O(V^2)
 *   Min-Heap         : O(E log V)
 *   Space            : O(V)
 *
 * DOMAIN USE:
 *   Connect all bus terminals with minimum total road maintenance expenditure.
 */
MSTResult prim_mst(const Graph *g, Metrics *m, int verbose) {
    MSTResult res;
    res.edge_count = 0;
    res.total_weight = 0;

    int V = g->num_vertices;
    if (V < 1) V = 15;

    int in_mst[MAX_GRAPH_STOPS] = {0};
    int key[MAX_GRAPH_STOPS];
    int parent[MAX_GRAPH_STOPS];

    for (int i = 0; i <= V; i++) {
        key[i] = INF;
        parent[i] = -1;
    }
    key[1] = 0;

    for (int count = 0; count < V; count++) {
        /* Find min key vertex not yet in MST */
        int min_val = INF, u = -1;
        for (int v = 1; v <= V; v++) {
            if (m) {
                m->comparisons++;
                m->iterations++;
            }
            if (!in_mst[v] && key[v] < min_val) {
                min_val = key[v];
                u = v;
            }
        }

        if (u == -1 || min_val == INF) break;
        in_mst[u] = 1;

        if (parent[u] != -1) {
            int idx = res.edge_count++;
            res.edges[idx].u = parent[u];
            res.edges[idx].v = u;
            res.edges[idx].distance = key[u];
            res.total_weight += key[u];

            if (verbose) {
                printf("    [Prim Add Edge] Added (%d <-> %d) | Weight: %d km | Running Total: %d km\n",
                       parent[u], u, key[u], res.total_weight);
            }
        }

        /* Update keys of adjacent vertices */
        for (int v = 1; v <= V; v++) {
            int w = g->adj_matrix[u][v];
            if (w > 0 && w < INF && !in_mst[v]) {
                if (m) {
                    m->comparisons++;
                    m->assignments++;
                }
                if (w < key[v]) {
                    key[v] = w;
                    parent[v] = u;
                }
            }
        }
    }

    return res;
}

/* ========================================================================= */
/*                          2. KRUSKAL'S ALGORITHM                           */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * KRUSKAL_MST(G):
 *   Sort all edges E in non-decreasing order of weight
 *   Initialize DSU for V vertices
 *   for each edge (u, v) in sorted E:
 *     if find(u) != find(v):
 *       Add (u, v) to MST
 *       union(u, v)
 *
 * COMPLEXITY:
 *   Time Complexity : O(E log E) or O(E log V) (dominated by edge sorting)
 *   Space Complexity: O(V + E)
 *
 * DOMAIN USE:
 *   Sparse regional transit network expansion across distant depot zones.
 */
static void sort_edges_by_weight(Edge edges[], int n, Metrics *m) {
    /* Simple in-place insertion sort for clean C99 */
    for (int i = 1; i < n; i++) {
        Edge key = edges[i];
        int j = i - 1;
        while (j >= 0 && edges[j].distance > key.distance) {
            if (m) {
                m->comparisons++;
                m->assignments++;
            }
            edges[j + 1] = edges[j];
            j--;
        }
        edges[j + 1] = key;
        if (m) m->assignments++;
    }
}

MSTResult kruskal_mst(const Graph *g, Metrics *m, int verbose) {
    MSTResult res;
    res.edge_count = 0;
    res.total_weight = 0;

    int E = g->num_edges;
    int V = g->num_vertices;
    if (V < 1) V = 15;

    Edge *edge_list = (Edge *)malloc(E * sizeof(Edge));
    if (!edge_list) return res;
    memcpy(edge_list, g->edges, E * sizeof(Edge));

    /* Sort edges by distance */
    sort_edges_by_weight(edge_list, E, m);

    DSU dsu;
    dsu_init(&dsu, V);

    if (verbose) printf("    [Kruskal Processing] Iterating over %d sorted road edges...\n", E);

    for (int i = 0; i < E && res.edge_count < V - 1; i++) {
        int u = edge_list[i].u;
        int v = edge_list[i].v;
        int w = edge_list[i].distance;

        if (m) {
            m->comparisons++;
            m->iterations++;
        }

        if (dsu_union(&dsu, u, v)) {
            /* Edge accepted */
            int idx = res.edge_count++;
            res.edges[idx] = edge_list[i];
            res.total_weight += w;

            if (verbose) {
                printf("    [Kruskal Accept] Edge (%d <-> %d) | Weight: %d km | Total MST: %d km\n",
                       u, v, w, res.total_weight);
            }
        } else {
            if (verbose && i < 6) {
                printf("    [Kruskal Reject] Edge (%d <-> %d) forms a cycle. Discarded.\n", u, v);
            }
        }
    }

    free(edge_list);
    return res;
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_spanning_tree_algorithms(const Graph *g) {
    Metrics m_prim, m_kruskal;
    RESET_METRICS(m_prim);
    RESET_METRICS(m_kruskal);

    clock_t t0 = timer_start();
    MSTResult res_prim = prim_mst(g, &m_prim, 0);
    m_prim.time_ms = timer_stop_ms(t0);

    clock_t t1 = timer_start();
    MSTResult res_kruskal = kruskal_mst(g, &m_kruskal, 0);
    m_kruskal.time_ms = timer_stop_ms(t1);

    print_banner("MINIMUM SPANNING TREE COMPARISON (SAME ROAD NETWORK)");
    printf("  Network Size: %d vertices, %d edges\n\n", g->num_vertices, g->num_edges);

    printf("  +-----------------+-----------+-------------+------------+-----------+----------------+\n");
    printf("  | Algorithm       | Time (ms) | Operations  | Memory KB  | MST Edges | Total Cost km  |\n");
    printf("  +-----------------+-----------+-------------+------------+-----------+----------------+\n");
    printf("  | Prim's MST      | %-9.4f | %-11llu | %-10.2f | %-9d | %-14d |\n",
           m_prim.time_ms, m_prim.comparisons + m_prim.assignments,
           (double)sizeof(MSTResult) / 1024.0, res_prim.edge_count, res_prim.total_weight);
    printf("  | Kruskal's MST   | %-9.4f | %-11llu | %-10.2f | %-9d | %-14d |\n",
           m_kruskal.time_ms, m_kruskal.comparisons + m_kruskal.assignments,
           (double)sizeof(MSTResult) / 1024.0, res_kruskal.edge_count, res_kruskal.total_weight);
    printf("  +-----------------+-----------+-------------+------------+-----------+----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    if (g->num_edges >= g->num_vertices * (g->num_vertices - 1) / 4) {
        printf("  >> Prim's Algorithm is preferred for dense networks (avoids edge sorting overhead).\n");
    } else {
        printf("  >> Kruskal's Algorithm is preferred for sparse networks (O(E log V) with Disjoint Set Union).\n");
    }
}

void run_spanning_tree_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 5: SPANNING TREE ALGORITHMS");
        printf("  1. Prim's Algorithm (Vertex-based MST Trace)\n");
        printf("  2. Kruskal's Algorithm (Union-Find Edge Sorting Trace)\n");
        printf("  3. Compare (Prim vs Kruskal on same network)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 3);
        if (choice == 0) break;

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. PRIM'S MINIMUM SPANNING TREE TRACE");
                printf("  [Time Complexity: O(V^2) matrix or O(E log V) heap | Space: O(V)]\n\n");
                MSTResult res = prim_mst(&sys->road_network, &m, 1);
                printf("\n  >> Total Minimum Network Road Length: %d km\n", res.total_weight);
                printf("  >> Edges selected: %d\n", res.edge_count);
                break;
            }
            case 2: {
                print_banner("2. KRUSKAL'S MINIMUM SPANNING TREE TRACE");
                printf("  [Time Complexity: O(E log E) with Union-Find | Space: O(V + E)]\n\n");
                MSTResult res = kruskal_mst(&sys->road_network, &m, 1);
                printf("\n  >> Total Minimum Network Road Length: %d km\n", res.total_weight);
                printf("  >> Edges selected: %d\n", res.edge_count);
                break;
            }
            case 3: {
                compare_spanning_tree_algorithms(&sys->road_network);
                break;
            }
        }

        press_enter_to_continue();
    }
}

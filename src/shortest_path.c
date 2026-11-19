/**
 * @file shortest_path.c
 * @brief Shortest Path Algorithms: Dijkstra, Bellman-Ford, Floyd-Warshall.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/shortest_path.h"

/* ========================================================================= */
/*                           1. DIJKSTRA'S ALGORITHM                         */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * DIJKSTRA(G, src):
 *   Initialize dist[v] = INF, parent[v] = -1 for all v
 *   dist[src] = 0
 *   visited[v] = false for all v
 *   for count = 0 to V - 1:
 *     u = extract_min(unvisited vertices)
 *     visited[u] = true
 *     for each neighbor v of u:
 *       if not visited[v] and dist[u] + weight(u, v) < dist[v]:
 *         dist[v] = dist[u] + weight(u, v)
 *         parent[v] = u
 *
 * COMPLEXITY:
 *   Time Complexity:
 *     With Min-Array  : O(V^2)
 *     With Min-Heap   : O((V + E) log V)
 *   Space Complexity : O(V)
 *   Limitation       : Cannot handle negative edge weights.
 *
 * DOMAIN USE:
 *   Find fastest emergency bus reroute or shortest passenger travel time.
 */
ShortestPathResult dijkstra_shortest_path(const Graph *g, int src, Metrics *m, int verbose) {
    ShortestPathResult res;
    int visited[MAX_GRAPH_STOPS] = {0};
    int V = g->num_vertices;
    if (V < 1) V = 15;

    for (int i = 0; i <= V; i++) {
        res.dist[i] = INF;
        res.parent[i] = -1;
    }
    res.dist[src] = 0;
    res.has_negative_cycle = 0;

    if (m) m->memory_bytes = sizeof(ShortestPathResult) + sizeof(visited);

    for (int count = 0; count < V; count++) {
        /* Extract-Min */
        int min_d = INF, u = -1;
        for (int v = 1; v <= V; v++) {
            if (m) {
                m->comparisons++;
                m->iterations++;
            }
            if (!visited[v] && res.dist[v] < min_d) {
                min_d = res.dist[v];
                u = v;
            }
        }

        if (u == -1 || min_d == INF) break;
        visited[u] = 1;

        if (verbose && (count < 5 || count == V - 1)) {
            printf("    [Extract-Min] Picked Stop %d (Current Dist: %d km)\n", u, res.dist[u]);
        }

        /* Relax neighbors */
        for (int v = 1; v <= V; v++) {
            if (g->adj_matrix[u][v] > 0 && g->adj_matrix[u][v] < INF && !visited[v]) {
                int weight = g->adj_matrix[u][v];
                if (m) {
                    m->comparisons++;
                    m->assignments++;
                }
                if (res.dist[u] + weight < res.dist[v]) {
                    res.dist[v] = res.dist[u] + weight;
                    res.parent[v] = u;
                    if (verbose && (count < 4)) {
                        printf("      -> Relaxed Edge (%d -> %d): New Dist = %d km via parent %d\n",
                               u, v, res.dist[v], u);
                    }
                }
            }
        }
    }

    return res;
}

/* ========================================================================= */
/*                          2. BELLMAN-FORD ALGORITHM                        */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * BELLMAN_FORD(G, src):
 *   Initialize dist[v] = INF, parent[v] = -1
 *   dist[src] = 0
 *   for i = 1 to V - 1:
 *     for each edge (u, v) in E:
 *       if dist[u] + weight(u, v) < dist[v]:
 *         dist[v] = dist[u] + weight(u, v)
 *         parent[v] = u
 *   Check negative cycle:
 *   for each edge (u, v) in E:
 *     if dist[u] + weight(u, v) < dist[v]:
 *       return "Negative Cycle Detected"
 *
 * COMPLEXITY:
 *   Time Complexity: O(V * E)
 *   Space Complexity: O(V)
 *   Advantage: Handles negative weights and detects negative weight cycles.
 *
 * DOMAIN USE:
 *   Subsidized green transit routes or toll discount loops where net cost can be negative.
 */
ShortestPathResult bellman_ford_shortest_path(const Graph *g, int src, Metrics *m, int verbose) {
    ShortestPathResult res;
    int V = g->num_vertices;
    int E = g->num_edges;
    if (V < 1) V = 15;

    for (int i = 0; i <= V; i++) {
        res.dist[i] = INF;
        res.parent[i] = -1;
    }
    res.dist[src] = 0;
    res.has_negative_cycle = 0;

    /* Relax all edges V - 1 times */
    for (int i = 1; i <= V - 1; i++) {
        int changed = 0;
        if (m) m->iterations++;

        for (int j = 0; j < E; j++) {
            int u = g->edges[j].u;
            int v = g->edges[j].v;
            int w = g->edges[j].distance;

            if (m) {
                m->comparisons++;
                m->assignments++;
            }

            if (res.dist[u] != INF && res.dist[u] + w < res.dist[v]) {
                res.dist[v] = res.dist[u] + w;
                res.parent[v] = u;
                changed = 1;
            }
            /* Undirected relaxation */
            if (res.dist[v] != INF && res.dist[v] + w < res.dist[u]) {
                res.dist[u] = res.dist[v] + w;
                res.parent[u] = v;
                changed = 1;
            }
        }

        if (verbose && (i <= 3 || !changed)) {
            printf("    [Pass %d] Edge relaxation pass complete. Changes detected: %d\n", i, changed);
        }

        if (!changed) {
            if (verbose) printf("    [Early Termination] No distances updated in pass %d.\n", i);
            break;
        }
    }

    /* Check for negative-weight cycles */
    for (int j = 0; j < E; j++) {
        int u = g->edges[j].u;
        int v = g->edges[j].v;
        int w = g->edges[j].distance;
        if (res.dist[u] != INF && res.dist[u] + w < res.dist[v]) {
            res.has_negative_cycle = 1;
            if (verbose) {
                printf("    [ALERT] Negative weight cycle detected on road link %d -> %d!\n", u, v);
            }
            break;
        }
    }

    return res;
}

/* ========================================================================= */
/*                          3. FLOYD-WARSHALL ALGORITHM                      */
/* ========================================================================= */

/**
 * RECURRENCE:
 *   D^(k)[i][j] = min(D^(k-1)[i][j], D^(k-1)[i][k] + D^(k-1)[k][j])
 *
 * COMPLEXITY:
 *   Time Complexity : O(V^3)
 *   Space Complexity: O(V^2) for distance and predecessor matrices
 *
 * DOMAIN USE:
 *   Compute complete all-pairs transit matrix for origin-destination trip tables.
 */
void floyd_warshall_all_pairs(const Graph *g, int dist[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS],
                             int next[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS],
                             Metrics *m, int verbose) {
    int V = g->num_vertices;
    if (V < 1) V = 15;

    /* Initialize distance matrix */
    for (int i = 1; i <= V; i++) {
        for (int j = 1; j <= V; j++) {
            dist[i][j] = g->adj_matrix[i][j];
            if (i == j) {
                dist[i][j] = 0;
                next[i][j] = j;
            } else if (dist[i][j] < INF) {
                next[i][j] = j;
            } else {
                next[i][j] = -1;
            }
        }
    }

    /* DP Triple Loop: k is the intermediate vertex */
    for (int k = 1; k <= V; k++) {
        if (m) m->iterations++;

        for (int i = 1; i <= V; i++) {
            for (int j = 1; j <= V; j++) {
                if (m) {
                    m->comparisons++;
                    m->assignments++;
                }

                if (dist[i][k] != INF && dist[k][j] != INF) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                    }
                }
            }
        }

        if (verbose && (k == 1 || k == 2 || k == V)) {
            printf("    [Floyd-Warshall Phase k=%d] Matrix paths updated using intermediate stop %d.\n", k, k);
        }
    }
}

void print_reconstructed_path(int src, int dest, const int parent[]) {
    if (src == dest) {
        printf("%d", src);
        return;
    }
    if (dest == -1 || parent[dest] == -1) {
        printf("No Path");
        return;
    }
    print_reconstructed_path(src, parent[dest], parent);
    printf(" -> %d", dest);
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_shortest_path_algorithms(const Graph *g, int src, int dest) {
    Metrics m_dijkstra, m_bellman, m_floyd;
    RESET_METRICS(m_dijkstra);
    RESET_METRICS(m_bellman);
    RESET_METRICS(m_floyd);

    /* 1. Dijkstra */
    clock_t t0 = timer_start();
    ShortestPathResult res_dijkstra = dijkstra_shortest_path(g, src, &m_dijkstra, 0);
    m_dijkstra.time_ms = timer_stop_ms(t0);

    /* 2. Bellman-Ford */
    clock_t t1 = timer_start();
    ShortestPathResult res_bellman = bellman_ford_shortest_path(g, src, &m_bellman, 0);
    m_bellman.time_ms = timer_stop_ms(t1);

    /* 3. Floyd-Warshall */
    int dist_fw[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    int next_fw[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    clock_t t2 = timer_start();
    floyd_warshall_all_pairs(g, dist_fw, next_fw, &m_floyd, 0);
    m_floyd.time_ms = timer_stop_ms(t2);

    print_banner("SHORTEST PATH ALGORITHM COMPARISON (SAME NETWORK)");
    printf("  Source Stop: %d | Destination Stop: %d\n", src, dest);
    printf("  Network Size: %d vertices, %d road edges\n\n", g->num_vertices, g->num_edges);

    printf("  +-----------------+-----------+-------------+------------+-------------------+----------------+\n");
    printf("  | Algorithm       | Time (ms) | Operations  | Space      | Negative Edges    | Shortest Dist  |\n");
    printf("  +-----------------+-----------+-------------+------------+-------------------+----------------+\n");
    printf("  | Dijkstra        | %-9.4f | %-11llu | O(V)       | No (Fails)        | %-14d |\n",
           m_dijkstra.time_ms, m_dijkstra.comparisons + m_dijkstra.assignments, res_dijkstra.dist[dest]);
    printf("  | Bellman-Ford    | %-9.4f | %-11llu | O(V)       | Yes (Cycle Check) | %-14d |\n",
           m_bellman.time_ms, m_bellman.comparisons + m_bellman.assignments, res_bellman.dist[dest]);
    printf("  | Floyd-Warshall  | %-9.4f | %-11llu | O(V^2)     | Yes (No Neg Cyc)  | %-14d |\n",
           m_floyd.time_ms, m_floyd.comparisons + m_floyd.assignments, dist_fw[src][dest]);
    printf("  +-----------------+-----------+-------------+------------+-------------------+----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> Dijkstra is best for single-source queries on road networks with non-negative distances (fastest O(V^2) or O(E log V)).\n");
    printf("  >> Floyd-Warshall is best when all-pairs origin-destination matrices are required for complete city schedule planning.\n");
}

void run_shortest_path_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 4: SHORTEST PATH ALGORITHMS");
        printf("  1. Dijkstra's Algorithm (Single-Source Step-by-Step)\n");
        printf("  2. Bellman-Ford Algorithm (Edge Relaxation & Negative Cycle Trace)\n");
        printf("  3. Floyd-Warshall Algorithm (All-Pairs Distance Matrix Trace)\n");
        printf("  4. Compare all (Dijkstra vs Bellman-Ford vs Floyd-Warshall)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 4);
        if (choice == 0) break;

        int src = get_int_input("Enter Source Stop ID (e.g. 1 for Ballupur, 3 for ISBT)", 1, 15);
        int dest = get_int_input("Enter Destination Stop ID (e.g. 5 for Rajpur Road)", 1, 15);

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. DIJKSTRA'S SHORTEST PATH TRACE");
                printf("  [Time Complexity: O(V^2) | Space: O(V)]\n\n");
                ShortestPathResult res = dijkstra_shortest_path(&sys->road_network, src, &m, 1);
                printf("\n  >> Shortest Distance from Stop %d to Stop %d: %d km\n", src, dest, res.dist[dest]);
                printf("  >> Path: ");
                print_reconstructed_path(src, dest, res.parent);
                printf("\n  >> Operations: %llu comparisons\n", m.comparisons);
                break;
            }
            case 2: {
                print_banner("2. BELLMAN-FORD ALGORITHM TRACE");
                printf("  [Time Complexity: O(V * E) | Space: O(V)]\n\n");
                ShortestPathResult res = bellman_ford_shortest_path(&sys->road_network, src, &m, 1);
                if (res.has_negative_cycle) {
                    printf("\n  [!] Warning: Road network contains a negative weight cycle!\n");
                } else {
                    printf("\n  >> Shortest Distance from Stop %d to Stop %d: %d km\n", src, dest, res.dist[dest]);
                    printf("  >> Path: ");
                    print_reconstructed_path(src, dest, res.parent);
                    printf("\n  >> Operations: %llu comparisons\n", m.comparisons);
                }
                break;
            }
            case 3: {
                print_banner("3. FLOYD-WARSHALL ALL-PAIRS SHORTEST PATH TRACE");
                printf("  [Time Complexity: O(V^3) | Space: O(V^2)]\n\n");
                int dist[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
                int next[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
                floyd_warshall_all_pairs(&sys->road_network, dist, next, &m, 1);
                printf("\n  >> Shortest Distance between Stop %d and Stop %d: %d km\n", src, dest, dist[src][dest]);
                printf("  >> Intermediate Next Hop: Stop %d\n", next[src][dest]);
                break;
            }
            case 4: {
                compare_shortest_path_algorithms(&sys->road_network, src, dest);
                break;
            }
        }

        press_enter_to_continue();
    }
}

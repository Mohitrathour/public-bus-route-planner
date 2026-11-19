/**
 * @file flow_matching.c
 * @brief Maximum Network Flow and Maximum Bipartite Matching:
 *        1. Ford-Fulkerson (DFS-based augmenting paths)
 *        2. Edmonds-Karp (BFS-based shortest augmenting paths)
 *        3. Maximum Bipartite Matching (Bus Fleet to Scheduled Routes)
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/flow_matching.h"

/* ========================================================================= */
/*                      1. FORD-FULKERSON (DFS-BASED)                        */
/* ========================================================================= */

/**
 * RECURSIVE DFS AUGMENTING PATH:
 *   Traverse residual graph using depth-first search until sink is reached.
 *
 * COMPLEXITY:
 *   Time Complexity: O(E * max_flow)
 *   Limitation     : Can suffer from poor path choices if capacities are large.
 *
 * DOMAIN USE:
 *   Calculate peak evacuation / hourly throughput capacity from residential
 *   stops to central ISBT terminal.
 */
static int dfs_augment(int r_graph[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int u, int sink,
                       int visited[], int min_cap, int parent[], int n, Metrics *m) {
    if (u == sink) return min_cap;
    visited[u] = 1;

    for (int v = 1; v <= n; v++) {
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        if (!visited[v] && r_graph[u][v] > 0) {
            parent[v] = u;
            int next_min = (min_cap < r_graph[u][v]) ? min_cap : r_graph[u][v];
            int bottleneck = dfs_augment(r_graph, v, sink, visited, next_min, parent, n, m);
            if (bottleneck > 0) return bottleneck;
        }
    }
    return 0;
}

FlowResult ford_fulkerson_dfs(const int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, int src, int sink, Metrics *m, int verbose) {
    FlowResult res;
    res.max_flow = 0;
    res.augmentations_count = 0;

    int r_graph[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            r_graph[i][j] = cap[i][j];
        }
    }

    int parent[MAX_GRAPH_STOPS];
    int visited[MAX_GRAPH_STOPS];

    while (1) {
        for (int i = 0; i <= n; i++) visited[i] = 0;
        int bottleneck = dfs_augment(r_graph, src, sink, visited, INF, parent, n, m);
        if (bottleneck <= 0) break;

        /* Augment residual flow */
        int v = sink;
        while (v != src) {
            int u = parent[v];
            r_graph[u][v] -= bottleneck;
            r_graph[v][u] += bottleneck;
            v = u;
        }

        res.max_flow += bottleneck;
        res.augmentations_count++;

        if (verbose && res.augmentations_count <= 5) {
            printf("    [DFS Augmentation #%d] Found path with bottleneck capacity: %d pax/hr (Total: %d)\n",
                   res.augmentations_count, bottleneck, res.max_flow);
        }
    }

    for (int i = 0; i <= n; i++) for (int j = 0; j <= n; j++) res.residual[i][j] = r_graph[i][j];
    return res;
}

/* ========================================================================= */
/*                       2. EDMONDS-KARP (BFS-BASED)                         */
/* ========================================================================= */

/**
 * BREADTH-FIRST SEARCH SHORTEST AUGMENTING PATH:
 *   Guarantees that the shortest path (in terms of number of edges) is chosen.
 *
 * COMPLEXITY:
 *   Time Complexity : O(V * E^2) (Polynomial bound, independent of capacity values!)
 *   Space Complexity: O(V^2) for residual graph
 *
 * DOMAIN USE:
 *   Robust maximum capacity planning on large multimodal transit networks.
 */
static int bfs_find_path(int r_graph[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int src, int sink,
                         int parent[], int n, Metrics *m) {
    int visited[MAX_GRAPH_STOPS] = {0};
    int queue[MAX_GRAPH_STOPS];
    int front = 0, rear = 0;

    queue[rear++] = src;
    visited[src] = 1;
    parent[src] = -1;

    while (front < rear) {
        int u = queue[front++];
        if (m) m->iterations++;

        for (int v = 1; v <= n; v++) {
            if (m) m->comparisons++;
            if (!visited[v] && r_graph[u][v] > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = 1;
                if (v == sink) return 1;
            }
        }
    }
    return 0;
}

FlowResult edmonds_karp_bfs(const int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, int src, int sink, Metrics *m, int verbose) {
    FlowResult res;
    res.max_flow = 0;
    res.augmentations_count = 0;

    int r_graph[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            r_graph[i][j] = cap[i][j];
        }
    }

    int parent[MAX_GRAPH_STOPS];

    while (bfs_find_path(r_graph, src, sink, parent, n, m)) {
        /* Find bottleneck capacity along the BFS path */
        int bottleneck = INF;
        for (int v = sink; v != src; v = parent[v]) {
            int u = parent[v];
            if (r_graph[u][v] < bottleneck) bottleneck = r_graph[u][v];
        }

        /* Update residual capacities */
        for (int v = sink; v != src; v = parent[v]) {
            int u = parent[v];
            r_graph[u][v] -= bottleneck;
            r_graph[v][u] += bottleneck;
        }

        res.max_flow += bottleneck;
        res.augmentations_count++;

        if (verbose && res.augmentations_count <= 5) {
            printf("    [BFS Augmentation #%d] Shortest path augmented by: %d pax/hr (Total: %d)\n",
                   res.augmentations_count, bottleneck, res.max_flow);
        }
    }

    for (int i = 0; i <= n; i++) for (int j = 0; j <= n; j++) res.residual[i][j] = r_graph[i][j];
    return res;
}

/* ========================================================================= */
/*                      3. MAXIMUM BIPARTITE MATCHING                        */
/* ========================================================================= */

/**
 * KUHN'S AUGMENTING PATH ALGORITHM FOR BIPARTITE MATCHING:
 *   Left set  : Available buses
 *   Right set : Scheduled routes / driver shifts
 *   Edge (u, v): Bus u is certified and capable of operating route v.
 *
 * COMPLEXITY:
 *   Time Complexity: O(V * E)
 *
 * DOMAIN USE:
 *   Assign available bus vehicles to routes or drivers to shift schedules.
 */
static int bpm_dfs(const int bp_graph[MAX_BIPARTITE][MAX_BIPARTITE], int u,
                   int visited[], int match_r[], int n_right, Metrics *m) {
    for (int v = 0; v < n_right; v++) {
        if (m) m->comparisons++;
        if (bp_graph[u][v] && !visited[v]) {
            visited[v] = 1;
            /* If right vertex is not matched, or previously matched vertex has an alternate */
            if (match_r[v] < 0 || bpm_dfs(bp_graph, match_r[v], visited, match_r, n_right, m)) {
                match_r[v] = u;
                return 1;
            }
        }
    }
    return 0;
}

BipartiteResult max_bipartite_matching(const int bp_graph[MAX_BIPARTITE][MAX_BIPARTITE], int n_left, int n_right, Metrics *m, int verbose) {
    BipartiteResult res;
    res.max_matches = 0;
    for (int i = 0; i < MAX_BIPARTITE; i++) {
        res.match_l_to_r[i] = -1;
        res.match_r_to_l[i] = -1;
    }

    for (int u = 0; u < n_left; u++) {
        int visited[MAX_BIPARTITE] = {0};
        if (m) m->iterations++;

        if (bpm_dfs(bp_graph, u, visited, res.match_r_to_l, n_right, m)) {
            res.max_matches++;
        }
    }

    /* Invert for left-to-right mapping */
    for (int v = 0; v < n_right; v++) {
        if (res.match_r_to_l[v] >= 0) {
            res.match_l_to_r[res.match_r_to_l[v]] = v;
            if (verbose) {
                printf("    [Matching Found] Bus #%d assigned to Route/Shift #%d\n",
                       res.match_r_to_l[v] + 101, v + 1);
            }
        }
    }

    return res;
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_flow_methods(const int cap[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, int src, int sink) {
    Metrics m_ff, m_ek;
    RESET_METRICS(m_ff);
    RESET_METRICS(m_ek);

    clock_t t0 = timer_start();
    FlowResult res_ff = ford_fulkerson_dfs(cap, n, src, sink, &m_ff, 0);
    m_ff.time_ms = timer_stop_ms(t0);

    clock_t t1 = timer_start();
    FlowResult res_ek = edmonds_karp_bfs(cap, n, src, sink, &m_ek, 0);
    m_ek.time_ms = timer_stop_ms(t1);

    print_banner("MAXIMUM NETWORK FLOW COMPARISON: FORD-FULKERSON VS EDMONDS-KARP");
    printf("  Source: Stop %d | Destination Sink: Stop %d | Vertices: %d\n\n", src, sink, n);

    printf("  +---------------------+-----------+---------------+--------------------+----------------+\n");
    printf("  | Flow Algorithm      | Time (ms) | Augmentations | Operations         | Max Flow (Pax) |\n");
    printf("  +---------------------+-----------+---------------+--------------------+----------------+\n");
    printf("  | Ford-Fulkerson (DFS)| %-9.4f | %-13d | %-18llu | %-14d |\n",
           m_ff.time_ms, res_ff.augmentations_count, m_ff.comparisons, res_ff.max_flow);
    printf("  | Edmonds-Karp (BFS)  | %-9.4f | %-13d | %-18llu | %-14d |\n",
           m_ek.time_ms, res_ek.augmentations_count, m_ek.comparisons, res_ek.max_flow);
    printf("  +---------------------+-----------+---------------+--------------------+----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> Edmonds-Karp is guaranteed polynomial O(V * E^2), using BFS to select fewest-hop paths and avoiding long DFS detours.\n");
}

void run_flow_matching_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 9: NETWORK FLOW & BIPARTITE MATCHING");
        printf("  1. Ford-Fulkerson (DFS-based Maximum Passenger Flow Trace)\n");
        printf("  2. Edmonds-Karp (BFS-based Shortest Augmenting Path Trace)\n");
        printf("  3. Maximum Bipartite Matching (Bus Fleet to Scheduled Routes)\n");
        printf("  4. Compare flow methods (Ford-Fulkerson DFS vs Edmonds-Karp BFS)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 4);
        if (choice == 0) break;

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. FORD-FULKERSON MAXIMUM FLOW (DFS)");
                int src = get_int_input("Enter Source Terminal ID", 1, 15);
                int sink = get_int_input("Enter Destination Station ID", 1, 15);
                FlowResult res = ford_fulkerson_dfs((const int (*)[MAX_GRAPH_STOPS])sys->road_network.cap_matrix,
                                                    sys->road_network.num_vertices, src, sink, &m, 1);
                printf("\n  >> Maximum Passenger Network Flow: %d passengers/hour\n", res.max_flow);
                printf("  >> Total Augmenting Paths: %d\n", res.augmentations_count);
                break;
            }
            case 2: {
                print_banner("2. EDMONDS-KARP MAXIMUM FLOW (BFS)");
                int src = get_int_input("Enter Source Terminal ID", 1, 15);
                int sink = get_int_input("Enter Destination Station ID", 1, 15);
                FlowResult res = edmonds_karp_bfs((const int (*)[MAX_GRAPH_STOPS])sys->road_network.cap_matrix,
                                                  sys->road_network.num_vertices, src, sink, &m, 1);
                printf("\n  >> Maximum Passenger Network Flow: %d passengers/hour\n", res.max_flow);
                printf("  >> Total Augmenting Paths: %d\n", res.augmentations_count);
                break;
            }
            case 3: {
                print_banner("3. MAXIMUM BIPARTITE MATCHING (BUSES TO ROUTES)");
                int bp_graph[MAX_BIPARTITE][MAX_BIPARTITE] = {0};
                /* Create sample bipartite compatibility matrix between 6 buses and 6 routes */
                bp_graph[0][0] = 1; bp_graph[0][1] = 1;
                bp_graph[1][1] = 1; bp_graph[1][2] = 1;
                bp_graph[2][0] = 1; bp_graph[2][3] = 1;
                bp_graph[3][2] = 1; bp_graph[3][4] = 1;
                bp_graph[4][3] = 1; bp_graph[4][5] = 1;
                bp_graph[5][4] = 1; bp_graph[5][5] = 1;

                printf("  Solving maximum assignment for 6 Buses (101-106) across 6 Routes (1-6)...\n\n");
                BipartiteResult res = max_bipartite_matching(bp_graph, 6, 6, &m, 1);
                printf("\n  >> Maximum Matched Pairs: %d out of 6 possible\n", res.max_matches);
                break;
            }
            case 4: {
                compare_flow_methods((const int (*)[MAX_GRAPH_STOPS])sys->road_network.cap_matrix,
                                     sys->road_network.num_vertices, 1, 3);
                break;
            }
        }

        press_enter_to_continue();
    }
}

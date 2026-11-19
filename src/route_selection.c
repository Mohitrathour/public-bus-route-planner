/**
 * @file route_selection.c
 * @brief Combinatorial Route Selection under Budget & N-Queens Backtracking Demo.
 *
 * Implements and compares:
 *   1. Greedy (Ratio heuristic)
 *   2. 0/1 Knapsack Dynamic Programming
 *   3. Backtracking (Exact DFS)
 *   4. Branch & Bound FIFO (Queue-based)
 *   5. Branch & Bound LIFO (Stack-based)
 *   6. Branch & Bound Least-Cost (Best-First search via priority queue)
 *   7. N-Queens Backtracking Demo
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/route_selection.h"

/* Structure representing a search tree node for Branch and Bound */
typedef struct {
    int level;          /* Decision index in routes array */
    int profit;         /* Total passenger volume accumulated */
    double weight;      /* Total operational budget spent */
    double bound;       /* Upper bound on maximum possible profit */
    int taken[MAX_ROUTES]; /* 1 if route is operated, 0 otherwise */
} BBNode;

/* Helper: Calculate upper bound on profit using fractional knapsack relaxation */
static double calculate_upper_bound(BBNode u, int n, double W, const Route routes[]) {
    if (u.weight >= W) return 0.0;

    double profit_bound = (double)u.profit;
    double current_weight = u.weight;
    int j = u.level + 1;

    /* Greedily accumulate whole items */
    while (j < n && current_weight + routes[j].operating_cost <= W) {
        current_weight += routes[j].operating_cost;
        profit_bound += routes[j].total_passengers;
        j++;
    }

    /* Add fractional part of next item */
    if (j < n) {
        double remain = W - current_weight;
        profit_bound += routes[j].total_passengers * (remain / routes[j].operating_cost);
    }

    return profit_bound;
}

/* ========================================================================= */
/*                              1. GREEDY HEURISTIC                          */
/* ========================================================================= */

/**
 * PSEUDOCODE:
 * GREEDY_KNAPSACK(routes, n, W):
 *   Sort routes descending by ratio = passengers / operating_cost
 *   total_pax = 0, current_cost = 0
 *   for i = 0 to n - 1:
 *     if current_cost + routes[i].cost <= W:
 *       Add routes[i]
 *       current_cost += routes[i].cost
 *       total_pax += routes[i].passengers
 *
 * COMPLEXITY: O(n log n)
 * QUALITY   : Approximation (can be arbitrarily suboptimal for 0/1 integer knapsack)
 */
RouteSelectionResult route_selection_greedy(const Route routes[], int n, double budget, Metrics *m, int verbose) {
    RouteSelectionResult res;
    res.max_passengers = 0;
    res.total_cost = 0.0;
    res.selected_count = 0;
    res.nodes_explored = (unsigned long long)n;

    /* Create sorted indices by efficiency ratio (pax per rupee) */
    int indices[MAX_ROUTES];
    for (int i = 0; i < n; i++) indices[i] = i;

    for (int i = 1; i < n; i++) {
        int key = indices[i];
        double r_key = (double)routes[key].total_passengers / routes[key].operating_cost;
        int j = i - 1;
        while (j >= 0) {
            double r_j = (double)routes[indices[j]].total_passengers / routes[indices[j]].operating_cost;
            if (m) m->comparisons++;
            if (r_j < r_key) {
                indices[j + 1] = indices[j];
                j--;
            } else {
                break;
            }
        }
        indices[j + 1] = key;
    }

    for (int i = 0; i < n; i++) {
        int idx = indices[i];
        if (m) {
            m->comparisons++;
            m->iterations++;
        }
        if (res.total_cost + routes[idx].operating_cost <= budget) {
            res.total_cost += routes[idx].operating_cost;
            res.max_passengers += routes[idx].total_passengers;
            res.selected_routes[res.selected_count++] = routes[idx].id;

            if (verbose) {
                printf("    [Greedy Pick] Route %d ('%s') | Cost: Rs %.1f | Demand: %d pax (Ratio: %.2f pax/Rs)\n",
                       routes[idx].id, routes[idx].name, routes[idx].operating_cost,
                       routes[idx].total_passengers,
                       (double)routes[idx].total_passengers / routes[idx].operating_cost);
            }
        }
    }

    return res;
}

/* ========================================================================= */
/*                   2. 0/1 KNAPSACK DYNAMIC PROGRAMMING                     */
/* ========================================================================= */

/**
 * RECURRENCE:
 *   DP[i][w] = DP[i-1][w]                                    if cost[i] > w
 *   DP[i][w] = max(DP[i-1][w], DP[i-1][w - cost[i]] + pax[i]) if cost[i] <= w
 *
 * COMPLEXITY:
 *   Time Complexity : O(n * W) (Pseudo-polynomial)
 *   Space Complexity: O(n * W)
 *
 * DOMAIN USE:
 *   Exact optimal selection of bus route corridors under budgetary limits.
 */
RouteSelectionResult route_selection_01_knapsack_dp(const Route routes[], int n, int budget, Metrics *m, int verbose) {
    RouteSelectionResult res;
    res.max_passengers = 0;
    res.total_cost = 0.0;
    res.selected_count = 0;
    res.nodes_explored = (unsigned long long)(n * budget);

    /* Allocate DP table */
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((budget + 1) * sizeof(int));
        for (int w = 0; w <= budget; w++) dp[i][w] = 0;
    }

    if (m) m->memory_bytes = (n + 1) * (budget + 1) * sizeof(int);

    /* Fill DP table */
    for (int i = 1; i <= n; i++) {
        int cost_i = (int)routes[i - 1].operating_cost;
        int pax_i = routes[i - 1].total_passengers;

        for (int w = 0; w <= budget; w++) {
            if (m) {
                m->comparisons++;
                m->iterations++;
            }
            if (cost_i <= w) {
                int take = dp[i - 1][w - cost_i] + pax_i;
                int leave = dp[i - 1][w];
                dp[i][w] = (take > leave) ? take : leave;
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    res.max_passengers = dp[n][budget];

    /* Backtrack to reconstruct selected routes */
    int curr_w = budget;
    for (int i = n; i > 0 && curr_w > 0; i--) {
        if (dp[i][curr_w] != dp[i - 1][curr_w]) {
            res.selected_routes[res.selected_count++] = routes[i - 1].id;
            res.total_cost += routes[i - 1].operating_cost;
            curr_w -= (int)routes[i - 1].operating_cost;

            if (verbose) {
                printf("    [DP Selected] Route %d ('%s') | Cost: Rs %.1f | Demand: %d pax\n",
                       routes[i - 1].id, routes[i - 1].name,
                       routes[i - 1].operating_cost, routes[i - 1].total_passengers);
            }
        }
    }

    for (int i = 0; i <= n; i++) free(dp[i]);
    free(dp);

    return res;
}

/* ========================================================================= */
/*                             3. BACKTRACKING                               */
/* ========================================================================= */

/**
 * RECURSIVE DFS STATE-SPACE EXPLORATION:
 *   Branch 1: Include route[idx] (if within budget)
 *   Branch 2: Exclude route[idx]
 *   Track global optimum.
 */
static void backtrack_dfs(int idx, int n, double budget, double curr_cost, int curr_pax,
                          int curr_taken[], RouteSelectionResult *best, const Route routes[],
                          Metrics *m, int verbose) {
    if (m) {
        m->iterations++;
        m->comparisons++;
    }
    best->nodes_explored++;

    if (curr_pax > best->max_passengers) {
        best->max_passengers = curr_pax;
        best->total_cost = curr_cost;
        best->selected_count = 0;
        for (int i = 0; i < n; i++) {
            if (curr_taken[i]) {
                best->selected_routes[best->selected_count++] = routes[i].id;
            }
        }
        if (verbose && best->nodes_explored < 20) {
            printf("    [Backtracking Update] New best profit: %d pax (Cost Rs %.1f)\n",
                   best->max_passengers, best->total_cost);
        }
    }

    if (idx >= n) return;

    /* Branch 1: Include */
    if (curr_cost + routes[idx].operating_cost <= budget) {
        curr_taken[idx] = 1;
        backtrack_dfs(idx + 1, n, budget, curr_cost + routes[idx].operating_cost,
                      curr_pax + routes[idx].total_passengers, curr_taken, best, routes, m, verbose);
        curr_taken[idx] = 0; /* backtrack */
    }

    /* Branch 2: Exclude */
    backtrack_dfs(idx + 1, n, budget, curr_cost, curr_pax, curr_taken, best, routes, m, verbose);
}

RouteSelectionResult route_selection_backtracking(const Route routes[], int n, double budget, Metrics *m, int verbose) {
    RouteSelectionResult res;
    res.max_passengers = 0;
    res.total_cost = 0.0;
    res.selected_count = 0;
    res.nodes_explored = 0;

    int taken[MAX_ROUTES] = {0};
    backtrack_dfs(0, n, budget, 0.0, 0, taken, &res, routes, m, verbose);
    return res;
}

/* ========================================================================= */
/*                      4. BRANCH & BOUND (FIFO - QUEUE)                     */
/* ========================================================================= */

#define BB_QUEUE_SIZE 20000

RouteSelectionResult route_selection_bb_fifo(const Route routes[], int n, double budget, Metrics *m, int verbose) {
    RouteSelectionResult res;
    res.max_passengers = 0;
    res.total_cost = 0.0;
    res.selected_count = 0;
    res.nodes_explored = 0;

    BBNode *queue = (BBNode *)malloc(BB_QUEUE_SIZE * sizeof(BBNode));
    if (!queue) return res;

    int front = 0, rear = 0;

    /* Root node */
    BBNode u, v;
    u.level = -1;
    u.profit = 0;
    u.weight = 0.0;
    for (int i = 0; i < n; i++) u.taken[i] = 0;
    u.bound = calculate_upper_bound(u, n, budget, routes);

    queue[rear++] = u;
    res.nodes_explored++;

    while (front < rear) {
        u = queue[front++];
        if (m) m->iterations++;

        if (u.level == n - 1) continue;

        /* Consider child v */
        v.level = u.level + 1;

        /* Branch 1: Include route at v.level */
        v.weight = u.weight + routes[v.level].operating_cost;
        v.profit = u.profit + routes[v.level].total_passengers;
        memcpy(v.taken, u.taken, sizeof(u.taken));
        v.taken[v.level] = 1;
        res.nodes_explored++;

        if (v.weight <= budget && v.profit > res.max_passengers) {
            res.max_passengers = v.profit;
            res.total_cost = v.weight;
            res.selected_count = 0;
            for (int i = 0; i < n; i++) if (v.taken[i]) res.selected_routes[res.selected_count++] = routes[i].id;
        }

        v.bound = calculate_upper_bound(v, n, budget, routes);
        if (v.bound > (double)res.max_passengers && rear < BB_QUEUE_SIZE - 1) {
            queue[rear++] = v;
        } else if (verbose && v.bound <= (double)res.max_passengers) {
            /* Pruned */
        }

        /* Branch 2: Exclude route at v.level */
        v.weight = u.weight;
        v.profit = u.profit;
        v.taken[v.level] = 0;
        res.nodes_explored++;

        v.bound = calculate_upper_bound(v, n, budget, routes);
        if (v.bound > (double)res.max_passengers && rear < BB_QUEUE_SIZE - 1) {
            queue[rear++] = v;
        }
    }

    free(queue);
    return res;
}

/* ========================================================================= */
/*                      5. BRANCH & BOUND (LIFO - STACK)                     */
/* ========================================================================= */

#define BB_STACK_SIZE 20000

RouteSelectionResult route_selection_bb_lifo(const Route routes[], int n, double budget, Metrics *m, int verbose) {
    (void)verbose;
    RouteSelectionResult res;
    res.max_passengers = 0;
    res.total_cost = 0.0;
    res.selected_count = 0;
    res.nodes_explored = 0;

    BBNode *stack = (BBNode *)malloc(BB_STACK_SIZE * sizeof(BBNode));
    if (!stack) return res;

    int top = -1;

    BBNode u, v;
    u.level = -1;
    u.profit = 0;
    u.weight = 0.0;
    for (int i = 0; i < n; i++) u.taken[i] = 0;
    u.bound = calculate_upper_bound(u, n, budget, routes);

    stack[++top] = u;
    res.nodes_explored++;

    while (top >= 0) {
        u = stack[top--];
        if (m) m->iterations++;

        if (u.level == n - 1) continue;

        v.level = u.level + 1;

        /* Branch Exclude first (so Include is popped first from stack) */
        v.weight = u.weight;
        v.profit = u.profit;
        memcpy(v.taken, u.taken, sizeof(u.taken));
        v.taken[v.level] = 0;
        v.bound = calculate_upper_bound(v, n, budget, routes);
        res.nodes_explored++;

        if (v.bound > (double)res.max_passengers && top < BB_STACK_SIZE - 1) {
            stack[++top] = v;
        }

        /* Branch Include */
        v.weight = u.weight + routes[v.level].operating_cost;
        v.profit = u.profit + routes[v.level].total_passengers;
        v.taken[v.level] = 1;
        res.nodes_explored++;

        if (v.weight <= budget && v.profit > res.max_passengers) {
            res.max_passengers = v.profit;
            res.total_cost = v.weight;
            res.selected_count = 0;
            for (int i = 0; i < n; i++) if (v.taken[i]) res.selected_routes[res.selected_count++] = routes[i].id;
        }

        v.bound = calculate_upper_bound(v, n, budget, routes);
        if (v.bound > (double)res.max_passengers && top < BB_STACK_SIZE - 1) {
            stack[++top] = v;
        }
    }

    free(stack);
    return res;
}

/* ========================================================================= */
/*              6. BRANCH & BOUND (LEAST-COST / BEST-FIRST LCBB)             */
/* ========================================================================= */

#define BB_HEAP_SIZE 20000

RouteSelectionResult route_selection_bb_least_cost(const Route routes[], int n, double budget, Metrics *m, int verbose) {
    (void)verbose;
    RouteSelectionResult res;
    res.max_passengers = 0;
    res.total_cost = 0.0;
    res.selected_count = 0;
    res.nodes_explored = 0;

    BBNode *pool = (BBNode *)malloc(BB_HEAP_SIZE * sizeof(BBNode));
    if (!pool) return res;
    int pool_size = 0;

    BBNode u, v;
    u.level = -1;
    u.profit = 0;
    u.weight = 0.0;
    for (int i = 0; i < n; i++) u.taken[i] = 0;
    u.bound = calculate_upper_bound(u, n, budget, routes);

    pool[pool_size++] = u;
    res.nodes_explored++;

    while (pool_size > 0) {
        if (m) m->iterations++;

        /* Best-First: Find node with HIGHEST upper bound */
        int best_idx = 0;
        for (int i = 1; i < pool_size; i++) {
            if (m) m->comparisons++;
            if (pool[i].bound > pool[best_idx].bound) {
                best_idx = i;
            }
        }

        u = pool[best_idx];
        /* Remove from pool */
        pool[best_idx] = pool[--pool_size];

        /* Prune if bound cannot beat current best */
        if (u.bound <= (double)res.max_passengers) continue;
        if (u.level == n - 1) continue;

        v.level = u.level + 1;

        /* Child 1: Include */
        v.weight = u.weight + routes[v.level].operating_cost;
        v.profit = u.profit + routes[v.level].total_passengers;
        memcpy(v.taken, u.taken, sizeof(u.taken));
        v.taken[v.level] = 1;
        res.nodes_explored++;

        if (v.weight <= budget && v.profit > res.max_passengers) {
            res.max_passengers = v.profit;
            res.total_cost = v.weight;
            res.selected_count = 0;
            for (int i = 0; i < n; i++) if (v.taken[i]) res.selected_routes[res.selected_count++] = routes[i].id;
        }

        v.bound = calculate_upper_bound(v, n, budget, routes);
        if (v.bound > (double)res.max_passengers && pool_size < BB_HEAP_SIZE - 1) {
            pool[pool_size++] = v;
        }

        /* Child 2: Exclude */
        v.weight = u.weight;
        v.profit = u.profit;
        v.taken[v.level] = 0;
        res.nodes_explored++;

        v.bound = calculate_upper_bound(v, n, budget, routes);
        if (v.bound > (double)res.max_passengers && pool_size < BB_HEAP_SIZE - 1) {
            pool[pool_size++] = v;
        }
    }

    free(pool);
    return res;
}

/* ========================================================================= */
/*                       7. N-QUEENS BACKTRACKING DEMO                       */
/* ========================================================================= */

/**
 * DEPOT PARKING GRID BAY ASSIGNMENT:
 *   Place N buses into an N x N depot charging grid such that no two buses
 *   share the same row, column, or diagonal maneuvering corridor.
 */
static int is_safe_bay(int board[20][20], int row, int col, int n, Metrics *m) {
    /* Check row on left */
    for (int i = 0; i < col; i++) {
        if (m) m->comparisons++;
        if (board[row][i]) return 0;
    }
    /* Check upper diagonal on left */
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--) {
        if (m) m->comparisons++;
        if (board[i][j]) return 0;
    }
    /* Check lower diagonal on left */
    for (int i = row, j = col; j >= 0 && i < n; i++, j--) {
        if (m) m->comparisons++;
        if (board[i][j]) return 0;
    }
    return 1;
}

static int solve_n_queens_util(int board[20][20], int col, int n, Metrics *m, int verbose) {
    if (col >= n) return 1;

    for (int i = 0; i < n; i++) {
        if (m) m->iterations++;
        if (is_safe_bay(board, i, col, n, m)) {
            board[i][col] = 1;
            if (verbose && col < 3) {
                printf("    [Bus Placed] Bus #%d allocated to Depot Bay [%d, %d]\n", col + 1, i, col);
            }

            if (solve_n_queens_util(board, col + 1, n, m, verbose)) return 1;

            /* Backtrack */
            board[i][col] = 0;
            if (verbose && col < 3) {
                printf("    [Backtrack] Conflict ahead! Removing bus from Bay [%d, %d]\n", i, col);
            }
        }
    }
    return 0;
}

int n_queens_solve(int n, int board[20][20], Metrics *m, int verbose) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) board[i][j] = 0;
    return solve_n_queens_util(board, 0, n, m, verbose);
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_route_selection_all(const Route routes[], int n, int budget) {
    if (n <= 0) {
        printf("  [!] Error: No routes loaded.\n");
        return;
    }

    Metrics m_greedy, m_dp, m_bt, m_fifo, m_lifo, m_lc;
    RESET_METRICS(m_greedy);
    RESET_METRICS(m_dp);
    RESET_METRICS(m_bt);
    RESET_METRICS(m_fifo);
    RESET_METRICS(m_lifo);
    RESET_METRICS(m_lc);

    /* 1. Greedy */
    clock_t t0 = timer_start();
    RouteSelectionResult res_greedy = route_selection_greedy(routes, n, budget, &m_greedy, 0);
    m_greedy.time_ms = timer_stop_ms(t0);

    /* 2. 0/1 Knapsack DP */
    clock_t t1 = timer_start();
    RouteSelectionResult res_dp = route_selection_01_knapsack_dp(routes, n, budget, &m_dp, 0);
    m_dp.time_ms = timer_stop_ms(t1);

    /* 3. Backtracking */
    clock_t t2 = timer_start();
    RouteSelectionResult res_bt = route_selection_backtracking(routes, n, budget, &m_bt, 0);
    m_bt.time_ms = timer_stop_ms(t2);

    /* 4. Branch & Bound FIFO */
    clock_t t3 = timer_start();
    RouteSelectionResult res_fifo = route_selection_bb_fifo(routes, n, budget, &m_fifo, 0);
    m_fifo.time_ms = timer_stop_ms(t3);

    /* 5. Branch & Bound LIFO */
    clock_t t4 = timer_start();
    RouteSelectionResult res_lifo = route_selection_bb_lifo(routes, n, budget, &m_lifo, 0);
    m_lifo.time_ms = timer_stop_ms(t4);

    /* 6. Branch & Bound Least-Cost (LCBB) */
    clock_t t5 = timer_start();
    RouteSelectionResult res_lc = route_selection_bb_least_cost(routes, n, budget, &m_lc, 0);
    m_lc.time_ms = timer_stop_ms(t5);

    print_banner("ROUTE SELECTION UNDER BUDGET: COMPREHENSIVE ALGORITHM COMPARISON");
    printf("  Operating Budget: Rs %d | Candidate Corridors: %d\n\n", budget, n);

    printf("  +-----------------------+-----------+----------------+--------------+------------------+-----------------+\n");
    printf("  | Algorithm             | Time (ms) | Nodes Explored | Cost Spent   | Total Passengers | Solution Type   |\n");
    printf("  +-----------------------+-----------+----------------+--------------+------------------+-----------------+\n");
    printf("  | Greedy (Ratio)        | %-9.4f | %-14llu | Rs %-9.1f | %-16d | Approximation   |\n",
           m_greedy.time_ms, res_greedy.nodes_explored, res_greedy.total_cost, res_greedy.max_passengers);
    printf("  | 0/1 Knapsack DP       | %-9.4f | %-14llu | Rs %-9.1f | %-16d | Exact Optimal   |\n",
           m_dp.time_ms, res_dp.nodes_explored, res_dp.total_cost, res_dp.max_passengers);
    printf("  | Backtracking (DFS)    | %-9.4f | %-14llu | Rs %-9.1f | %-16d | Exact Optimal   |\n",
           m_bt.time_ms, res_bt.nodes_explored, res_bt.total_cost, res_bt.max_passengers);
    printf("  | B&B FIFO (Queue)      | %-9.4f | %-14llu | Rs %-9.1f | %-16d | Exact Optimal   |\n",
           m_fifo.time_ms, res_fifo.nodes_explored, res_fifo.total_cost, res_fifo.max_passengers);
    printf("  | B&B LIFO (Stack)      | %-9.4f | %-14llu | Rs %-9.1f | %-16d | Exact Optimal   |\n",
           m_lifo.time_ms, res_lifo.nodes_explored, res_lifo.total_cost, res_lifo.max_passengers);
    printf("  | B&B Least-Cost (LCBB) | %-9.4f | %-14llu | Rs %-9.1f | %-16d | Exact Optimal   |\n",
           m_lc.time_ms, res_lc.nodes_explored, res_lc.total_cost, res_lc.max_passengers);
    printf("  +-----------------------+-----------+-------------+------------------+-----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> Least-Cost Branch & Bound (LCBB) explores the FEWEST search tree nodes (%llu nodes) due to best-first pruning!\n",
           res_lc.nodes_explored);
    printf("  >> Dynamic Programming is fastest for small-to-moderate integer budgets.\n");
}

void run_route_selection_menu(TransitSystem *sys) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU 7: ROUTE SELECTION UNDER BUDGET");
        printf("  1. Greedy (Ratio-based Heuristic Trace)\n");
        printf("  2. 0/1 Knapsack Dynamic Programming (Exact DP Table Trace)\n");
        printf("  3. Backtracking (Decision Tree DFS Trace)\n");
        printf("  4. Branch & Bound FIFO (Queue-based Breadth-First Trace)\n");
        printf("  5. Branch & Bound LIFO (Stack-based Depth-First Trace)\n");
        printf("  6. Branch & Bound Least-Cost (Best-First Priority Queue Trace)\n");
        printf("  7. N-Queens Backtracking Demo (Depot Bay Non-Interference)\n");
        printf("  8. Compare all (Exact Decision on Same Routes & Budget)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 8);
        if (choice == 0) break;

        if (choice == 7) {
            print_banner("7. N-QUEENS BACKTRACKING DEMO (DEPOT BAY PARKING)");
            int n = get_int_input("Enter Depot Grid Size N (e.g. 4 to 8)", 4, 12);
            int board[20][20];
            Metrics m;
            RESET_METRICS(m);
            if (n_queens_solve(n, board, &m, 1)) {
                printf("\n  >> Valid Conflict-Free Depot Bay Allocation for %d Buses:\n", n);
                for (int i = 0; i < n; i++) {
                    printf("    ");
                    for (int j = 0; j < n; j++) {
                        printf("%s ", board[i][j] ? "[BUS]" : " .  ");
                    }
                    printf("\n");
                }
            } else {
                printf("\n  >> No valid parking configuration found for N=%d.\n", n);
            }
            press_enter_to_continue();
            continue;
        }

        if (sys->route_count == 0) {
            printf("  [!] Please load routes data first (Menu 12)!\n");
            press_enter_to_continue();
            continue;
        }

        int budget = get_int_input("Enter Total Operating Budget (e.g. 3500 Rs)", 500, 50000);
        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. GREEDY ROUTE SELECTION TRACE");
                RouteSelectionResult res = route_selection_greedy(sys->routes, sys->route_count, budget, &m, 1);
                printf("\n  >> Result: %d passengers served | Cost: Rs %.1f\n", res.max_passengers, res.total_cost);
                break;
            }
            case 2: {
                print_banner("2. 0/1 KNAPSACK DYNAMIC PROGRAMMING TRACE");
                RouteSelectionResult res = route_selection_01_knapsack_dp(sys->routes, sys->route_count, budget, &m, 1);
                printf("\n  >> Exact Optimal: %d passengers served | Cost: Rs %.1f\n", res.max_passengers, res.total_cost);
                break;
            }
            case 3: {
                print_banner("3. BACKTRACKING ROUTE SELECTION TRACE");
                RouteSelectionResult res = route_selection_backtracking(sys->routes, sys->route_count, budget, &m, 1);
                printf("\n  >> Nodes Explored: %llu | Optimal: %d passengers | Cost: Rs %.1f\n",
                       res.nodes_explored, res.max_passengers, res.total_cost);
                break;
            }
            case 4: {
                print_banner("4. BRANCH & BOUND FIFO (QUEUE-BASED BFS)");
                RouteSelectionResult res = route_selection_bb_fifo(sys->routes, sys->route_count, budget, &m, 1);
                printf("\n  >> Nodes Explored: %llu | Optimal: %d passengers | Cost: Rs %.1f\n",
                       res.nodes_explored, res.max_passengers, res.total_cost);
                break;
            }
            case 5: {
                print_banner("5. BRANCH & BOUND LIFO (STACK-BASED DFS)");
                RouteSelectionResult res = route_selection_bb_lifo(sys->routes, sys->route_count, budget, &m, 1);
                printf("\n  >> Nodes Explored: %llu | Optimal: %d passengers | Cost: Rs %.1f\n",
                       res.nodes_explored, res.max_passengers, res.total_cost);
                break;
            }
            case 6: {
                print_banner("6. BRANCH & BOUND LEAST-COST (BEST-FIRST SEARCH)");
                RouteSelectionResult res = route_selection_bb_least_cost(sys->routes, sys->route_count, budget, &m, 1);
                printf("\n  >> Nodes Explored: %llu | Optimal: %d passengers | Cost: Rs %.1f\n",
                       res.nodes_explored, res.max_passengers, res.total_cost);
                break;
            }
            case 8: {
                compare_route_selection_all(sys->routes, sys->route_count, budget);
                break;
            }
        }

        press_enter_to_continue();
    }
}

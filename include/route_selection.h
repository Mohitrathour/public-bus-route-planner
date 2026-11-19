/**
 * @file route_selection.h
 * @brief Combinatorial optimization algorithms for route operating selection under budget:
 *        1. Greedy (Value-to-Cost heuristic)
 *        2. 0/1 Knapsack Dynamic Programming
 *        3. Backtracking
 *        4. Branch and Bound (FIFO - Queue based)
 *        5. Branch and Bound (LIFO - Stack based)
 *        6. Branch and Bound (Least-Cost / Best-First - Priority Queue)
 *        7. N-Queens Backtracking demo for depot bay allocation
 */

#ifndef ROUTE_SELECTION_H
#define ROUTE_SELECTION_H

#include "models.h"

/* Result container for route selection problem */
typedef struct {
    int max_passengers;
    double total_cost;
    int selected_routes[MAX_ROUTES];
    int selected_count;
    unsigned long long nodes_explored;
} RouteSelectionResult;

/* Algorithm implementations */
RouteSelectionResult route_selection_greedy(const Route routes[], int n, double budget, Metrics *m, int verbose);
RouteSelectionResult route_selection_01_knapsack_dp(const Route routes[], int n, int budget, Metrics *m, int verbose);
RouteSelectionResult route_selection_backtracking(const Route routes[], int n, double budget, Metrics *m, int verbose);
RouteSelectionResult route_selection_bb_fifo(const Route routes[], int n, double budget, Metrics *m, int verbose);
RouteSelectionResult route_selection_bb_lifo(const Route routes[], int n, double budget, Metrics *m, int verbose);
RouteSelectionResult route_selection_bb_least_cost(const Route routes[], int n, double budget, Metrics *m, int verbose);

/* N-Queens Backtracking Demo */
int n_queens_solve(int n, int board[20][20], Metrics *m, int verbose);

/* Submenu and Comparison */
void run_route_selection_menu(TransitSystem *sys);
void compare_route_selection_all(const Route routes[], int n, int budget);

#endif /* ROUTE_SELECTION_H */

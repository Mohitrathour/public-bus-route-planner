/**
 * @file divide_conquer.h
 * @brief Divide and Conquer algorithms applied to bus planning:
 *        1. Maximum Subarray (Peak demand interval)
 *        2. Fast Exponentiation (Compounding passenger delay factor)
 *        3. Celebrity Problem (Central Transit Hub identification)
 *        4. Classical vs Strassen Matrix Multiplication (Transit reachability)
 */

#ifndef DIVIDE_CONQUER_H
#define DIVIDE_CONQUER_H

#include "models.h"

/* 1. Maximum Subarray */
typedef struct {
    int low;
    int high;
    int sum;
} SubarrayResult;

SubarrayResult max_subarray_brute_force(const int arr[], int n, Metrics *m);
SubarrayResult max_subarray_dc(const int arr[], int low, int high, Metrics *m, int verbose);
SubarrayResult max_subarray_kadane(const int arr[], int n, Metrics *m);

/* 2. Fast Exponentiation */
long long power_brute_force(long long base, int exp, long long mod, Metrics *m);
long long power_fast_dc(long long base, int exp, long long mod, Metrics *m, int verbose);

/* 3. Celebrity Problem (Transit Hub) */
int find_transit_hub_brute(const int adj[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, Metrics *m);
int find_transit_hub_dc(const int adj[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS], int n, Metrics *m, int verbose);

/* 4. Matrix Multiplication: Classical vs Strassen */
#define MATRIX_MAX 4
void matrix_mult_classical(int n, const int A[MATRIX_MAX][MATRIX_MAX],
                           const int B[MATRIX_MAX][MATRIX_MAX],
                           int C[MATRIX_MAX][MATRIX_MAX],
                           Metrics *m);

void matrix_mult_strassen(int n, const int A[MATRIX_MAX][MATRIX_MAX],
                          const int B[MATRIX_MAX][MATRIX_MAX],
                          int C[MATRIX_MAX][MATRIX_MAX],
                          Metrics *m, int verbose);

/* Submenu and Comparison */
void run_divide_conquer_menu(TransitSystem *sys);
void compare_divide_conquer_all(void);

#endif /* DIVIDE_CONQUER_H */

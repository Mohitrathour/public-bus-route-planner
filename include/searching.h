/**
 * @file searching.h
 * @brief Searching algorithms: Linear, Binary Iterative, Binary Recursive.
 *
 * Domain Use:
 *   Instantly locate bus stops, transfer hubs, or depots by stop ID or name.
 */

#ifndef SEARCHING_H
#define SEARCHING_H

#include "models.h"

/* Search Functions */
int linear_search_stop(const Stop arr[], int n, int target_id, Metrics *m, int verbose);
int binary_search_stop_iterative(const Stop arr[], int n, int target_id, Metrics *m, int verbose);
int binary_search_stop_recursive(const Stop arr[], int low, int high, int target_id, Metrics *m, int verbose);

/* Name lookup */
int linear_search_stop_by_name(const Stop arr[], int n, const char *name, Metrics *m);

/* Submenu & Comparison Runner */
void run_searching_menu(TransitSystem *sys);
void compare_searching_algorithms(const Stop arr[], int n, int target_id);

/* Compatibility functions matching student's original signatures */
int linearSearch(struct node stops[], int count, int id);
int binarySearchIterative(struct node stops[], int count, int id);
int binarySearchRecursive(struct node stops[], int low, int high, int id);

#endif /* SEARCHING_H */

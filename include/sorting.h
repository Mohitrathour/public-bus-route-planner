/**
 * @file sorting.h
 * @brief Sorting algorithms: Bubble Sort, Merge Sort, Quick Sort.
 *
 * Domain Use:
 *   Sort bus stops by hourly passenger demand to prioritize frequency,
 *   or sort transit routes by operational cost / travel distance.
 */

#ifndef SORTING_H
#define SORTING_H

#include "models.h"

/* Sorting Functions on Stop Array */
void bubble_sort_stops_by_demand(Stop arr[], int n, Metrics *m, int verbose);
void merge_sort_stops_by_demand(Stop arr[], int l, int r, Metrics *m, int verbose);
void quick_sort_stops_by_demand(Stop arr[], int low, int high, Metrics *m, int verbose);

/* Sorting Functions on Route Array */
void quick_sort_routes_by_cost(Route arr[], int low, int high, Metrics *m, int verbose);

/* Interactive Submenu & Comparison Runner */
void run_sorting_menu(TransitSystem *sys);
void compare_sorting_algorithms(Stop arr[], int n);

#endif /* SORTING_H */

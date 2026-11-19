/**
 * @file scheduling.h
 * @brief Greedy Scheduling algorithms: Activity Selection & Interval Partitioning.
 *
 * Domain Use:
 *   1. Activity Selection: Maximize non-overlapping revenue trips a single bus can make.
 *   2. Interval Partitioning: Determine minimum fleet size / depot maintenance bays
 *      needed to service all scheduled bus trips without timetable collision.
 */

#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "models.h"

/* Activity Selection */
int activity_selection_greedy(Trip trips[], int n, int selected_ids[], Metrics *m, int verbose);
int activity_selection_fcfs(Trip trips[], int n, int selected_ids[], Metrics *m);

/* Interval Partitioning */
int interval_partitioning_greedy(Trip trips[], int n, int bus_assignment[], Metrics *m, int verbose);
int interval_partitioning_fcfs(Trip trips[], int n, int bus_assignment[], Metrics *m);

/* Submenu and Comparison */
void run_scheduling_menu(TransitSystem *sys);
void compare_scheduling_algorithms(Trip trips[], int n);

#endif /* SCHEDULING_H */

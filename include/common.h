/**
 * @file common.h
 * @brief Common definitions, metrics tracking, and CLI utilities for
 *        Public Bus Route Frequency and Depot Planner (Project 12).
 *
 * Designed for B.Tech DAA course (CSEG3053).
 * Compatible with C99 standard using only standard C libraries.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/* Capacity Limits */
#define MAX_STOPS 1200
#define MAX_GRAPH_STOPS 100
#define MAX_ROADS 3000
#define MAX_ROUTES 200
#define MAX_ROUTE_STOPS 20
#define MAX_BUSES 300
#define MAX_DEPOTS 50
#define MAX_TRIPS 2000
#define MAX_STRING 64
#define INF 1000000000

/**
 * @struct Metrics
 * @brief Performance and complexity tracking metrics.
 */
typedef struct {
    unsigned long long comparisons;
    unsigned long long assignments;
    unsigned long long iterations;
    size_t memory_bytes;
    double time_ms;
} Metrics;

/* Utility Macros */
#define RESET_METRICS(m) do { \
    (m).comparisons = 0; \
    (m).assignments = 0; \
    (m).iterations = 0; \
    (m).memory_bytes = 0; \
    (m).time_ms = 0.0; \
} while(0)

/* Timing helpers */
static inline clock_t timer_start(void) {
    return clock();
}

static inline double timer_stop_ms(clock_t start) {
    clock_t end = clock();
    return ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
}

/* UI Helper function declarations */
void print_banner(const char *title);
void print_separator(void);
void press_enter_to_continue(void);
int get_int_input(const char *prompt, int min_val, int max_val);
double get_double_input(const char *prompt, double min_val, double max_val);
void get_string_input(const char *prompt, char *dest, int max_size);

#endif /* COMMON_H */

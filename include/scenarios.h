/**
 * @file scenarios.h
 * @brief Demonstration scenarios, synthetic data generation, and benchmarking suite:
 *        1. Normal Operation Scenario
 *        2. Constraint Shock Scenario (depot closure, road blockage, 50% bus reduction)
 *        3. Large Stress Scenario (1000+ stops/trips)
 *        4. Comprehensive Benchmark Suite (100, 500, 1000, 5000, 10000 + Adversarial)
 */

#ifndef SCENARIOS_H
#define SCENARIOS_H

#include "models.h"

/* Scenarios */
void run_normal_scenario(TransitSystem *sys);
void run_constraint_shock_scenario(TransitSystem *sys);
void run_stress_scenario(TransitSystem *sys);

/* Data Generator */
void generate_synthetic_data(TransitSystem *sys, int num_stops, int num_routes, int num_buses);

/* Benchmark Suite */
void run_comprehensive_benchmark(const char *csv_output_path);

/* Submenu */
void run_scenarios_menu(TransitSystem *sys);

#endif /* SCENARIOS_H */

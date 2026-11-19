/**
 * @file models.h
 * @brief Domain models for the Public Bus Route Frequency and Depot Planner.
 *
 * Preserves compatibility with existing student code (struct node)
 * while expanding to full multimodal transit entities.
 */

#ifndef MODELS_H
#define MODELS_H

#include "common.h"

/**
 * @struct node / Stop
 * @brief Represents a bus stop / passenger pickup station.
 * Reused directly from student's stops.h.
 */
struct node {
    int id;
    char name[MAX_STRING];
    int passengers; /* Demand per hour */
};
typedef struct node Stop;

/**
 * @struct Route
 * @brief Represents a designated transit corridor connecting multiple stops.
 */
typedef struct {
    int id;
    char name[MAX_STRING];
    int stop_ids[MAX_ROUTE_STOPS];
    int stop_count;
    double distance_km;
    int travel_time_min;
    double operating_cost;
    int total_passengers;
} Route;

/**
 * @struct Bus
 * @brief Fleet vehicle details.
 * Reused from student's buses.csv.
 */
typedef struct {
    int id;
    int capacity;
    int depot_id;
    int assigned_route_id;
    int is_available;
} Bus;

/**
 * @struct Depot
 * @brief Parking and maintenance garage for bus fleet.
 * Reused from student's deports.csv.
 */
typedef struct {
    int id;
    char name[MAX_STRING];
    int capacity;        /* Maximum fleet slots */
    int available_buses; /* Ready for dispatch */
    int is_active;       /* 1 = active, 0 = closed (for shock scenarios) */
} Depot;

/**
 * @struct Trip
 * @brief Scheduled route run with specific time window.
 */
typedef struct {
    int trip_id;
    int route_id;
    int start_time; /* in minutes from midnight (e.g., 8:00 AM = 480) */
    int end_time;   /* in minutes from midnight */
    int bus_id;     /* assigned bus id (-1 if unassigned) */
    int demand;     /* expected passenger volume */
} Trip;

/**
 * @struct Edge
 * @brief Road segment between two stops.
 */
typedef struct {
    int u;              /* Source stop ID */
    int v;              /* Destination stop ID */
    int distance;       /* Distance in km */
    int travel_time;    /* Time in minutes */
    int capacity;       /* Passenger capacity per hour */
} Edge;

/**
 * @struct Graph
 * @brief Combined Adjacency Matrix and Edge List for bus transit road network.
 */
typedef struct {
    int num_vertices;
    int num_edges;
    int adj_matrix[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    int time_matrix[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    int cap_matrix[MAX_GRAPH_STOPS][MAX_GRAPH_STOPS];
    Edge edges[MAX_ROADS];
} Graph;

/**
 * @struct TransitSystem
 * @brief Unified container holding all transit assets.
 */
typedef struct {
    Stop stops[MAX_STOPS];
    int stop_count;

    Route routes[MAX_ROUTES];
    int route_count;

    Bus buses[MAX_BUSES];
    int bus_count;

    Depot depots[MAX_DEPOTS];
    int depot_count;

    Trip trips[MAX_TRIPS];
    int trip_count;

    Graph road_network;
} TransitSystem;

/* Global system state reference */
extern TransitSystem g_system;

/* Function Declarations */
void init_transit_system(TransitSystem *sys);

/* File Loading and Saving */
int load_all_data(TransitSystem *sys, const char *data_dir);
int load_stops(TransitSystem *sys, const char *filepath);
int load_roads(TransitSystem *sys, const char *filepath);
int load_buses(TransitSystem *sys, const char *filepath);
int load_depots(TransitSystem *sys, const char *filepath);
int load_routes(TransitSystem *sys, const char *filepath);
int load_trips(TransitSystem *sys, const char *filepath);

int export_routes_csv(const TransitSystem *sys, const char *filepath);
int export_benchmark_csv(const char *filepath, const char *data);

/* Display and UI Helpers */
void display_stops(const Stop stops[], int count);
void display_routes(const Route routes[], int count);
void display_buses(const Bus buses[], int count);
void display_depots(const Depot depots[], int count);
void display_trips(const Trip trips[], int count);
void display_network_summary(const TransitSystem *sys);

/* Manual Entry Helpers */
void manual_add_stop(TransitSystem *sys);
void manual_add_road(TransitSystem *sys);
void manual_add_bus(TransitSystem *sys);
void manual_add_route(TransitSystem *sys);

/* Original student compatibility wrappers */
int loadStops(struct node stops[], int maxStops);
void displayStops(struct node stops[], int count);

#endif /* MODELS_H */

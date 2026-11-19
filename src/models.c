/**
 * @file models.c
 * @brief Implementation of domain models, file I/O, validation, and UI helpers.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 * Preserves student's original struct node and CSV loading conventions.
 */

#include "../include/models.h"

/* Global Transit System instance */
TransitSystem g_system;

/* ========================================================================= */
/*                          CLI & UI HELPER FUNCTIONS                        */
/* ========================================================================= */

/**
 * @brief Prints a standardized header box.
 * @param title Header title text.
 */
void print_banner(const char *title) {
    printf("\n======================================================================\n");
    printf("  %s\n", title);
    printf("======================================================================\n");
}

/**
 * @brief Prints a clean divider line.
 */
void print_separator(void) {
    printf("----------------------------------------------------------------------\n");
}

/**
 * @brief Pauses execution until the user presses Enter.
 */
void press_enter_to_continue(void) {
    printf("\n[Press Enter to continue...]");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/**
 * @brief Reads a validated integer from user with bounds checking.
 * @param prompt Display prompt.
 * @param min_val Minimum acceptable value.
 * @param max_val Maximum acceptable value.
 * @return Validated integer.
 */
int get_int_input(const char *prompt, int min_val, int max_val) {
    int val;
    char buffer[128];
    while (1) {
        printf("%s (%d to %d): ", prompt, min_val, max_val);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0; /* On EOF return 0 to exit menus cleanly */
        }
        if (sscanf(buffer, "%d", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                return val;
            }
        }
        printf("  [!] Invalid input! Value must be an integer between %d and %d.\n", min_val, max_val);
    }
}

/**
 * @brief Reads a validated floating point number.
 */
double get_double_input(const char *prompt, double min_val, double max_val) {
    double val;
    char buffer[128];
    while (1) {
        printf("%s (%.2f to %.2f): ", prompt, min_val, max_val);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return min_val;
        }
        if (sscanf(buffer, "%lf", &val) == 1) {
            if (val >= min_val && val <= max_val) {
                return val;
            }
        }
        printf("  [!] Invalid input! Value must be between %.2f and %.2f.\n", min_val, max_val);
    }
}

/**
 * @brief Reads a trimmed non-empty string.
 */
void get_string_input(const char *prompt, char *dest, int max_size) {
    char buffer[256];
    while (1) {
        printf("%s: ", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            /* Remove trailing newline */
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                len--;
            }
            if (len > 0) {
                strncpy(dest, buffer, max_size - 1);
                dest[max_size - 1] = '\0';
                return;
            }
        } else {
            strncpy(dest, "default", max_size - 1);
            dest[max_size - 1] = '\0';
            return;
        }
        printf("  [!] Input cannot be empty! Please enter text.\n");
    }
}

/* ========================================================================= */
/*                      INITIALIZATION & GRAPH BUILD                         */
/* ========================================================================= */

/**
 * @brief Initializes transit system memory and graph adjacency matrices.
 * @param sys Pointer to TransitSystem.
 */
void init_transit_system(TransitSystem *sys) {
    if (!sys) return;
    sys->stop_count = 0;
    sys->route_count = 0;
    sys->bus_count = 0;
    sys->depot_count = 0;
    sys->trip_count = 0;

    sys->road_network.num_vertices = 0;
    sys->road_network.num_edges = 0;

    for (int i = 0; i < MAX_GRAPH_STOPS; i++) {
        for (int j = 0; j < MAX_GRAPH_STOPS; j++) {
            sys->road_network.adj_matrix[i][j] = (i == j) ? 0 : INF;
            sys->road_network.time_matrix[i][j] = (i == j) ? 0 : INF;
            sys->road_network.cap_matrix[i][j] = 0;
        }
    }
}

/* ========================================================================= */
/*                         DATA LOADING IMPLEMENTATION                       */
/* ========================================================================= */

/**
 * @brief Loads bus stops from CSV. Reuses student's stops.csv format.
 * Format: id,name,passengers
 */
int load_stops(TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("  [!] Notice: Could not open stops file '%s'\n", filepath);
        return 0;
    }

    char line[256];
    int loaded = 0;

    /* Skip header line */
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) && sys->stop_count < MAX_STOPS) {
        int id = 0, passengers = 0;
        char name[MAX_STRING] = {0};

        /* Parse CSV line */
        int res = sscanf(line, "%d,%63[^,\r\n],%d", &id, name, &passengers);
        if (res == 3 && id > 0 && passengers >= 0) {
            sys->stops[sys->stop_count].id = id;
            strncpy(sys->stops[sys->stop_count].name, name, MAX_STRING - 1);
            sys->stops[sys->stop_count].name[MAX_STRING - 1] = '\0';
            sys->stops[sys->stop_count].passengers = passengers;
            sys->stop_count++;
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

/**
 * @brief Loads road edges and populates graph adjacency matrices.
 * Format: from,to,distance[,travel_time,capacity]
 */
int load_roads(TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("  [!] Notice: Could not open roads file '%s'\n", filepath);
        return 0;
    }

    char line[256];
    int loaded = 0;

    /* Skip header */
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) && sys->road_network.num_edges < MAX_ROADS) {
        int from = 0, to = 0, dist = 0, time_val = 0, cap_val = 0;
        int res = sscanf(line, "%d,%d,%d,%d,%d", &from, &to, &dist, &time_val, &cap_val);

        if (res >= 3 && from > 0 && to > 0 && dist >= 0) {
            if (time_val <= 0) time_val = dist * 3; /* Fallback approx: 3 mins per km */
            if (cap_val <= 0) cap_val = 500;       /* Fallback approx: 500 passengers/hr */

            int idx = sys->road_network.num_edges;
            sys->road_network.edges[idx].u = from;
            sys->road_network.edges[idx].v = to;
            sys->road_network.edges[idx].distance = dist;
            sys->road_network.edges[idx].travel_time = time_val;
            sys->road_network.edges[idx].capacity = cap_val;
            sys->road_network.num_edges++;

            /* Update adjacency matrices (convert 1-based stop ID to 0-based matrix index) */
            if (from < MAX_GRAPH_STOPS && to < MAX_GRAPH_STOPS) {
                /* Undirected road connectivity */
                sys->road_network.adj_matrix[from][to] = dist;
                sys->road_network.adj_matrix[to][from] = dist;
                sys->road_network.time_matrix[from][to] = time_val;
                sys->road_network.time_matrix[to][from] = time_val;
                sys->road_network.cap_matrix[from][to] = cap_val;
                sys->road_network.cap_matrix[to][from] = cap_val;

                if (from > sys->road_network.num_vertices) sys->road_network.num_vertices = from;
                if (to > sys->road_network.num_vertices) sys->road_network.num_vertices = to;
            }
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

/**
 * @brief Loads bus fleet. Reused from student's buses.csv.
 * Format: bus_id,capacity,depot_id
 */
int load_buses(TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("  [!] Notice: Could not open buses file '%s'\n", filepath);
        return 0;
    }

    char line[256];
    int loaded = 0;

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) && sys->bus_count < MAX_BUSES) {
        int id = 0, cap = 0, depot = 0;
        int res = sscanf(line, "%d,%d,%d", &id, &cap, &depot);
        if (res == 3 && id > 0 && cap > 0) {
            sys->buses[sys->bus_count].id = id;
            sys->buses[sys->bus_count].capacity = cap;
            sys->buses[sys->bus_count].depot_id = depot;
            sys->buses[sys->bus_count].assigned_route_id = -1;
            sys->buses[sys->bus_count].is_available = 1;
            sys->bus_count++;
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

/**
 * @brief Loads bus depots. Reused from student's deports.csv / depots.csv.
 */
int load_depots(TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;
    }

    char line[256];
    int loaded = 0;

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) && sys->depot_count < MAX_DEPOTS) {
        int id = 0, cap = 0, avail = 0;
        char name[MAX_STRING] = {0};

        /* Format 1: depot_id,name,capacity,available_buses */
        int res = sscanf(line, "%d,%63[^,\r\n],%d,%d", &id, name, &cap, &avail);
        if (res == 4 && id > 0) {
            sys->depots[sys->depot_count].id = id;
            strncpy(sys->depots[sys->depot_count].name, name, MAX_STRING - 1);
            sys->depots[sys->depot_count].capacity = cap;
            sys->depots[sys->depot_count].available_buses = avail;
            sys->depots[sys->depot_count].is_active = 1;
            sys->depot_count++;
            loaded++;
            continue;
        }

        /* Format 2: depot_id,name,available_buses */
        res = sscanf(line, "%d,%63[^,\r\n],%d", &id, name, &avail);
        if (res == 3 && id > 0) {
            sys->depots[sys->depot_count].id = id;
            strncpy(sys->depots[sys->depot_count].name, name, MAX_STRING - 1);
            sys->depots[sys->depot_count].capacity = avail * 2;
            sys->depots[sys->depot_count].available_buses = avail;
            sys->depots[sys->depot_count].is_active = 1;
            sys->depot_count++;
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

/**
 * @brief Loads bus route definitions from CSV.
 * Format: route_id,name,distance,travel_time,cost,passengers
 */
int load_routes(TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;
    }

    char line[256];
    int loaded = 0;

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) && sys->route_count < MAX_ROUTES) {
        int id = 0, travel_time = 0, passengers = 0;
        double dist = 0.0, cost = 0.0;
        char name[MAX_STRING] = {0};

        int res = sscanf(line, "%d,%63[^,\r\n],%lf,%d,%lf,%d",
                         &id, name, &dist, &travel_time, &cost, &passengers);
        if (res == 6 && id > 0) {
            sys->routes[sys->route_count].id = id;
            strncpy(sys->routes[sys->route_count].name, name, MAX_STRING - 1);
            sys->routes[sys->route_count].distance_km = dist;
            sys->routes[sys->route_count].travel_time_min = travel_time;
            sys->routes[sys->route_count].operating_cost = cost;
            sys->routes[sys->route_count].total_passengers = passengers;
            sys->routes[sys->route_count].stop_count = 0;
            sys->route_count++;
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

/**
 * @brief Loads scheduled trips from CSV.
 * Format: trip_id,route_id,start_time,end_time,bus_id,demand
 */
int load_trips(TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;
    }

    char line[256];
    int loaded = 0;

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) && sys->trip_count < MAX_TRIPS) {
        int t_id = 0, r_id = 0, s_time = 0, e_time = 0, b_id = -1, demand = 0;
        int res = sscanf(line, "%d,%d,%d,%d,%d,%d",
                         &t_id, &r_id, &s_time, &e_time, &b_id, &demand);
        if (res >= 5 && t_id > 0) {
            sys->trips[sys->trip_count].trip_id = t_id;
            sys->trips[sys->trip_count].route_id = r_id;
            sys->trips[sys->trip_count].start_time = s_time;
            sys->trips[sys->trip_count].end_time = e_time;
            sys->trips[sys->trip_count].bus_id = b_id;
            sys->trips[sys->trip_count].demand = (res == 6) ? demand : 50;
            sys->trip_count++;
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

/**
 * @brief Loads all dataset CSV files from the designated directory.
 */
int load_all_data(TransitSystem *sys, const char *data_dir) {
    char path[512];
    init_transit_system(sys);

    sprintf(path, "%s/stops.csv", data_dir);
    int s = load_stops(sys, path);

    sprintf(path, "%s/roads.csv", data_dir);
    int r = load_roads(sys, path);

    sprintf(path, "%s/buses.csv", data_dir);
    int b = load_buses(sys, path);

    sprintf(path, "%s/depots.csv", data_dir);
    int d = load_depots(sys, path);
    if (d == 0) {
        /* Try student's spelling */
        sprintf(path, "%s/deports.csv", data_dir);
        d = load_depots(sys, path);
    }

    sprintf(path, "%s/routes.csv", data_dir);
    int rt = load_routes(sys, path);

    sprintf(path, "%s/trips.csv", data_dir);
    int tp = load_trips(sys, path);

    printf("  [+] Loaded Transit Data Successfully:\n");
    printf("      Stops: %d | Roads: %d | Buses: %d | Depots: %d | Routes: %d | Trips: %d\n",
           s, r, b, d, rt, tp);

    return s;
}

/* ========================================================================= */
/*                             EXPORT HELPERS                                */
/* ========================================================================= */

int export_routes_csv(const TransitSystem *sys, const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        printf("  [!] Error: Unable to open '%s' for writing.\n", filepath);
        return 0;
    }

    fprintf(fp, "route_id,name,distance_km,travel_time_min,operating_cost,total_passengers\n");
    for (int i = 0; i < sys->route_count; i++) {
        fprintf(fp, "%d,%s,%.2f,%d,%.2f,%d\n",
                sys->routes[i].id,
                sys->routes[i].name,
                sys->routes[i].distance_km,
                sys->routes[i].travel_time_min,
                sys->routes[i].operating_cost,
                sys->routes[i].total_passengers);
    }

    fclose(fp);
    printf("  [+] Routes exported successfully to '%s'\n", filepath);
    return 1;
}

int export_benchmark_csv(const char *filepath, const char *data) {
    FILE *fp = fopen(filepath, "a");
    if (!fp) return 0;
    fprintf(fp, "%s", data);
    fclose(fp);
    return 1;
}

/* ========================================================================= */
/*                          DISPLAY HELPERS                                  */
/* ========================================================================= */

void display_stops(const Stop stops[], int count) {
    if (count == 0) {
        printf("\n  [!] No bus stops available.\n");
        return;
    }

    printf("\n  +-----+--------------------------------+-----------------+\n");
    printf("  | ID  | Stop Name                      | Demand (Pax/Hr) |\n");
    printf("  +-----+--------------------------------+-----------------+\n");
    for (int i = 0; i < count; i++) {
        printf("  | %-3d | %-30s | %-15d |\n", stops[i].id, stops[i].name, stops[i].passengers);
    }
    printf("  +-----+--------------------------------+-----------------+\n");
    printf("  Total stops: %d\n", count);
}

void display_routes(const Route routes[], int count) {
    if (count == 0) {
        printf("\n  [!] No routes available.\n");
        return;
    }

    printf("\n  +-----+--------------------------------+---------+----------+----------+----------+\n");
    printf("  | ID  | Route Corridor Name            | Dist km | Time min | Cost Rs  | Demand   |\n");
    printf("  +-----+--------------------------------+---------+----------+----------+----------+\n");
    for (int i = 0; i < count; i++) {
        printf("  | %-3d | %-30s | %-7.1f | %-8d | %-8.1f | %-8d |\n",
               routes[i].id, routes[i].name, routes[i].distance_km,
               routes[i].travel_time_min, routes[i].operating_cost, routes[i].total_passengers);
    }
    printf("  +-----+--------------------------------+---------+----------+----------+----------+\n");
}

void display_buses(const Bus buses[], int count) {
    if (count == 0) {
        printf("\n  [!] No buses in fleet.\n");
        return;
    }

    printf("\n  +--------+----------+----------+----------------+\n");
    printf("  | Bus ID | Capacity | Depot ID | Status         |\n");
    printf("  +--------+----------+----------+----------------+\n");
    for (int i = 0; i < count; i++) {
        printf("  | %-6d | %-8d | %-8d | %-14s |\n",
               buses[i].id, buses[i].capacity, buses[i].depot_id,
               buses[i].is_available ? "Ready" : "Maintenance");
    }
    printf("  +--------+----------+----------+----------------+\n");
}

void display_depots(const Depot depots[], int count) {
    if (count == 0) {
        printf("\n  [!] No depots available.\n");
        return;
    }

    printf("\n  +----+--------------------------------+----------+-----------+--------+\n");
    printf("  | ID | Depot Name                     | Capacity | Available | Status |\n");
    printf("  +----+--------------------------------+----------+-----------+--------+\n");
    for (int i = 0; i < count; i++) {
        printf("  | %-2d | %-30s | %-8d | %-9d | %-6s |\n",
               depots[i].id, depots[i].name, depots[i].capacity,
               depots[i].available_buses, depots[i].is_active ? "Active" : "SHOCK_OFF");
    }
    printf("  +----+--------------------------------+----------+-----------+--------+\n");
}

void display_trips(const Trip trips[], int count) {
    if (count == 0) {
        printf("\n  [!] No trips scheduled.\n");
        return;
    }

    printf("\n  +---------+----------+------------+----------+--------+--------+\n");
    printf("  | Trip ID | Route ID | Start Time | End Time | Bus ID | Demand |\n");
    printf("  +---------+----------+------------+----------+--------+--------+\n");
    for (int i = 0; i < count && i < 25; i++) {
        int sh = trips[i].start_time / 60, sm = trips[i].start_time % 60;
        int eh = trips[i].end_time / 60, em = trips[i].end_time % 60;
        printf("  | %-7d | %-8d | %02d:%02d      | %02d:%02d    | %-6d | %-6d |\n",
               trips[i].trip_id, trips[i].route_id, sh, sm, eh, em,
               trips[i].bus_id, trips[i].demand);
    }
    if (count > 25) {
        printf("  | ... (showing 25 of %d total trips)                          |\n", count);
    }
    printf("  +---------+----------+------------+----------+--------+--------+\n");
}

void display_network_summary(const TransitSystem *sys) {
    print_banner("TRANSIT SYSTEM ASSET SUMMARY");
    printf("  Active Stops       : %d\n", sys->stop_count);
    printf("  Road Segments      : %d (Vertices: %d)\n", sys->road_network.num_edges, sys->road_network.num_vertices);
    printf("  Fleet Size         : %d buses\n", sys->bus_count);
    printf("  Depot Terminals    : %d depots\n", sys->depot_count);
    printf("  Operating Routes   : %d corridors\n", sys->route_count);
    printf("  Daily Timetable    : %d trips\n", sys->trip_count);
    print_separator();
}

/* ========================================================================= */
/*                          MANUAL ENTRY HELPERS                             */
/* ========================================================================= */

void manual_add_stop(TransitSystem *sys) {
    if (sys->stop_count >= MAX_STOPS) {
        printf("  [!] Error: Maximum stop capacity reached (%d)!\n", MAX_STOPS);
        return;
    }
    print_banner("MANUAL ENTRY: ADD NEW BUS STOP");
    int id = get_int_input("Enter Stop ID", 1, 9999);
    for (int i = 0; i < sys->stop_count; i++) {
        if (sys->stops[i].id == id) {
            printf("  [!] Error: Stop ID %d already exists!\n", id);
            return;
        }
    }
    char name[MAX_STRING];
    get_string_input("Enter Stop Name (e.g. Clock Tower)", name, MAX_STRING);
    int pax = get_int_input("Enter Passenger Demand (passengers/hour)", 0, 5000);

    sys->stops[sys->stop_count].id = id;
    strncpy(sys->stops[sys->stop_count].name, name, MAX_STRING - 1);
    sys->stops[sys->stop_count].passengers = pax;
    sys->stop_count++;

    printf("  [+] Stop added successfully! Total stops now: %d\n", sys->stop_count);
}

void manual_add_road(TransitSystem *sys) {
    if (sys->road_network.num_edges >= MAX_ROADS) {
        printf("  [!] Error: Maximum road edges reached!\n");
        return;
    }
    print_banner("MANUAL ENTRY: ADD ROAD LINK");
    int u = get_int_input("Enter From Stop ID", 1, MAX_GRAPH_STOPS - 1);
    int v = get_int_input("Enter To Stop ID", 1, MAX_GRAPH_STOPS - 1);
    if (u == v) {
        printf("  [!] Error: Self-loops not permitted between same stop ID!\n");
        return;
    }
    int dist = get_int_input("Enter Distance (km)", 1, 1000);
    int t = get_int_input("Enter Travel Time (minutes)", 1, 600);
    int cap = get_int_input("Enter Road Passenger Capacity (pax/hr)", 1, 10000);

    int idx = sys->road_network.num_edges;
    sys->road_network.edges[idx].u = u;
    sys->road_network.edges[idx].v = v;
    sys->road_network.edges[idx].distance = dist;
    sys->road_network.edges[idx].travel_time = t;
    sys->road_network.edges[idx].capacity = cap;
    sys->road_network.num_edges++;

    sys->road_network.adj_matrix[u][v] = dist;
    sys->road_network.adj_matrix[v][u] = dist;
    sys->road_network.time_matrix[u][v] = t;
    sys->road_network.time_matrix[v][u] = t;
    sys->road_network.cap_matrix[u][v] = cap;
    sys->road_network.cap_matrix[v][u] = cap;

    if (u > sys->road_network.num_vertices) sys->road_network.num_vertices = u;
    if (v > sys->road_network.num_vertices) sys->road_network.num_vertices = v;

    printf("  [+] Road segment %d <-> %d (%d km) added successfully!\n", u, v, dist);
}

void manual_add_bus(TransitSystem *sys) {
    if (sys->bus_count >= MAX_BUSES) {
        printf("  [!] Error: Fleet limit reached!\n");
        return;
    }
    print_banner("MANUAL ENTRY: ADD NEW BUS");
    int id = get_int_input("Enter Bus ID", 1, 9999);
    int cap = get_int_input("Enter Seating Capacity", 10, 120);
    int depot = get_int_input("Enter Assigned Depot ID", 1, 50);

    sys->buses[sys->bus_count].id = id;
    sys->buses[sys->bus_count].capacity = cap;
    sys->buses[sys->bus_count].depot_id = depot;
    sys->buses[sys->bus_count].assigned_route_id = -1;
    sys->buses[sys->bus_count].is_available = 1;
    sys->bus_count++;

    printf("  [+] Bus %d added to depot %d!\n", id, depot);
}

void manual_add_route(TransitSystem *sys) {
    if (sys->route_count >= MAX_ROUTES) {
        printf("  [!] Error: Route limit reached!\n");
        return;
    }
    print_banner("MANUAL ENTRY: ADD ROUTE CORRIDOR");
    int id = get_int_input("Enter Route ID", 1, 9999);
    char name[MAX_STRING];
    get_string_input("Enter Route Name (e.g. Express Line 1)", name, MAX_STRING);
    double dist = get_double_input("Enter Total Distance (km)", 1.0, 500.0);
    int t = get_int_input("Enter Travel Time (min)", 1, 600);
    double cost = get_double_input("Enter Operating Cost (Rs)", 100.0, 100000.0);
    int pax = get_int_input("Enter Expected Passengers", 10, 50000);

    sys->routes[sys->route_count].id = id;
    strncpy(sys->routes[sys->route_count].name, name, MAX_STRING - 1);
    sys->routes[sys->route_count].distance_km = dist;
    sys->routes[sys->route_count].travel_time_min = t;
    sys->routes[sys->route_count].operating_cost = cost;
    sys->routes[sys->route_count].total_passengers = pax;
    sys->routes[sys->route_count].stop_count = 0;
    sys->route_count++;

    printf("  [+] Route %d ('%s') created successfully!\n", id, name);
}

/* ========================================================================= */
/*                   ORIGINAL STUDENT CODE WRAPPERS                          */
/* ========================================================================= */

int loadStops(struct node stops[], int maxStops) {
    TransitSystem temp;
    temp.stop_count = 0;
    int loaded = load_stops(&temp, "data/stops.csv");
    if (loaded == 0) {
        loaded = load_stops(&temp, "bus planner/data/stops.csv");
    }
    for (int i = 0; i < loaded && i < maxStops; i++) {
        stops[i] = temp.stops[i];
    }
    return loaded;
}

void displayStops(struct node stops[], int count) {
    display_stops(stops, count);
}

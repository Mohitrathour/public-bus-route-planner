# System Architecture & Technical Design
## Public Bus Route Frequency and Depot Planner (Project 12)

---

### 1. High-Level System Architecture Diagram

```
+-----------------------------------------------------------------------------------------+
|                                    CLI USER INTERFACE                                   |
|                        12-Topic Menu Hierarchy + Direct CLI Flags                       |
+-----------------------------------------------------------------------------------------+
                                           |
                                           v
+-----------------------------------------------------------------------------------------+
|                                   CONTROLLER ENGINE                                     |
|                           (src/main.c & src/scenarios.c)                                |
+-----------------------------------------------------------------------------------------+
         |                                |                                   |
         v                                v                                   v
+------------------+            +-------------------+               +--------------------+
|  DATA ACCESS     |            | ALGORITHM ENGINE  |               | BENCHMARK & TESTS  |
|  (src/models.c)  |            | (10 DAA Modules)  |               | (tests/test_main)  |
+------------------+            +-------------------+               +--------------------+
  | CSV Parsers:                  | Sorting (M1)                      | 44 Automated Unit
  | - stops.csv                   | Searching (M2)                    | Assertions
  | - roads.csv                   | Divide & Conquer (M3)             | Benchmark Engine:
  | - buses.csv                   | Shortest Paths (M4)               | - 100 to 10000 N
  | - depots.csv                  | Spanning Trees (M5)               | - Adversarial sets
  | - routes.csv                  | Greedy Scheduling (M6)            | - Export to CSV
  | - trips.csv                   | Route Selection (M7)              |
  | Manual Entry                  | DP & TSP (M8)                     |
  | Data Generator                | Flow & Matching (M9)              |
                                  | String Search (M10)               |
                                  | Compression & Sync (M11)          |
                                  | Theory & Recurrences (M12)        |
+-----------------------------------------------------------------------------------------+
```

---

### 2. End-to-End Operational Pipeline

```
           +-----------------------+
           | 1. CSV Data Loader    | (data/*.csv -> TransitSystem struct)
           +-----------------------+
                       |
                       v
           +-----------------------+
           | 2. Network Graph Build| (Adjacency Matrix + Edge List)
           +-----------------------+
                       |
          +------------+------------+
          |                         |
          v                         v
+--------------------+   +---------------------+
| 3. Route Routing   |   | 4. Corridor Ranking |
| - Dijkstra (km)    |   | - Merge / Quick Sort|
| - Bellman-Ford     |   | - Peak Subarray D&C |
+--------------------+   +---------------------+
          |                         |
          +------------+------------+
                       |
                       v
           +-----------------------+
           | 5. Service Selection  | (Under Budget Constraint W)
           | - 0/1 Knapsack DP     |
           | - Branch & Bound LCBB |
           +-----------------------+
                       |
                       v
           +-----------------------+
           | 6. Timetable & Fleet  |
           | - Activity Selection  | (Trips per bus)
           | - Interval Partition  | (Total fleet size needed)
           | - Bipartite Matching  | (Buses to route lines)
           +-----------------------+
                       |
                       v
           +-----------------------+
           | 7. Output Synthesis   |
           | - Terminal Trace Log  |
           | - Results CSV Export  |
           +-----------------------+
```

---

### 3. Component Breakdown

| Module | Header | Source | Key Responsibilities |
| :--- | :--- | :--- | :--- |
| **Common & UI** | `include/common.h` | `src/models.c` | Metrics tracking, timer helpers, input validation, borders. |
| **Domain Models** | `include/models.h` | `src/models.c` | Struct definitions (`Stop`, `Route`, `Bus`, `Depot`, `Trip`, `Graph`), CSV parsing, manual entry. |
| **Sorting** | `include/sorting.h` | `src/sorting.c` | Bubble Sort, Merge Sort, Quick Sort (descending demand/cost), tracing, comparative table. |
| **Searching** | `include/searching.h` | `src/searching.c` | Linear Search, Binary Search Iterative, Binary Search Recursive, name search. |
| **Divide & Conquer** | `include/divide_conquer.h` | `src/divide_conquer.c`| Maximum Subarray, Fast Power, Celebrity Hub, Strassen vs Classical 2x2/4x4. |
| **Shortest Path** | `include/shortest_path.h`| `src/shortest_path.c`| Dijkstra, Bellman-Ford, Floyd-Warshall, path reconstruction. |
| **Spanning Tree** | `include/spanning_tree.h`| `src/spanning_tree.c`| Prim's MST, Kruskal's MST with Disjoint Set Union (path compression, union by rank). |
| **Scheduling** | `include/scheduling.h` | `src/scheduling.c` | Greedy Activity Selection, Greedy Interval Partitioning, comparison with FCFS. |
| **Route Selection**| `include/route_selection.h`| `src/route_selection.c`| Greedy ratio, 0/1 Knapsack DP, Backtracking, B&B FIFO, B&B LIFO, B&B Least-Cost, N-Queens. |
| **DP & TSP** | `include/dp_tsp.h` | `src/dp_tsp.c` | LCS corridor alignment, Matrix Chain Multiplication, Optimal BST, TSP Brute Force vs Bitmask DP. |
| **Flow & Matching**| `include/flow_matching.h`| `src/flow_matching.c`| Ford-Fulkerson (DFS), Edmonds-Karp (BFS), Maximum Bipartite Matching. |
| **String Match** | `include/string_matching.h`| `src/string_matching.c`| Naive, Rabin-Karp (rolling hash), Finite Automaton (DFA), KMP (prefix function LPS). |
| **Compression** | `include/compression_sync.h`| `src/compression_sync.c`| Huffman Coding (Min-Heap), Fixed-Length Coding, Chinese Remainder Theorem (CRT). |
| **Scenarios** | `include/scenarios.h` | `src/scenarios.c` | Normal, Shock, Stress scenarios, synthetic generator, multi-scale benchmark to CSV. |
| **Theory Notes** | `include/theory_notes.h` | `src/theory_notes.c` | Master Theorem, recurrence derivations, complexity table, P vs NP, Subset-Sum reduction. |
| **Test Suite** | `tests/test_runner.h` | `tests/test_main.c` | 44 automated unit assertions covering normal, boundary, invalid, and adversarial inputs. |

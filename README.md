# Public Bus Route Frequency and Depot Planner
### Project 12 | Design and Analysis of Algorithms (CSEG3053)
**Author:** MOHIT SINGH RATHOUR (SAP ID: 590025840)  
**Program:** B.Tech Computer Science & Engineering  
**Standard:** Pure C (C99), Standard C Library Only (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `limits.h`, `math.h`)  
**Zero External Dependencies:** No C++, no STL, no third-party libraries.

---

## 1. Project Overview
The **Public Bus Route Frequency and Depot Planner** is a full-featured, menu-driven command-line transit engineering application. It models an urban transit network (Dehradun city case study) and applies 28+ core algorithm implementations across 12 structured algorithmic modules to optimize passenger wait times, fleet utilization, and municipal operating budgets.

Every algorithm is implemented from scratch with:
- Detailed academic header comments containing pseudocode, recurrence relations, and time/space complexities.
- Interactive step-by-step trace mode printing intermediate array partitions, dynamic programming tables, and branch-and-bound pruning decisions.
- Side-by-side **Compare All** benchmarks evaluating runtime (`clock()`), operational step counters, and memory estimates on identical datasets.

---

## 2. Directory Structure

```
daa phase1/
├── Makefile                    # GCC build targets for Linux/macOS/MinGW
├── build.bat                   # Batch script for Windows GCC compilation
├── README.md                   # Complete offline setup & execution documentation
├── include/                    # C99 Header Declarations
│   ├── common.h                # System limits, Metrics struct, UI validation helpers
│   ├── models.h                # Domain models (Stop, Route, Bus, Depot, Trip, Graph)
│   ├── sorting.h               # Bubble Sort, Merge Sort, Quick Sort
│   ├── searching.h             # Linear Search, Binary Search Iterative & Recursive
│   ├── divide_conquer.h        # Max Subarray, Fast Power, Celebrity Hub, Strassen 2x2/4x4
│   ├── shortest_path.h         # Dijkstra, Bellman-Ford, Floyd-Warshall
│   ├── spanning_tree.h         # Prim's MST, Kruskal's MST with Disjoint Set Union
│   ├── scheduling.h            # Activity Selection, Interval Partitioning
│   ├── route_selection.h       # Greedy, 0/1 Knapsack DP, Backtracking, B&B (FIFO/LIFO/LC), N-Queens
│   ├── dp_tsp.h                # LCS, Matrix Chain Mult, Optimal BST, TSP (Brute Force & Bitmask DP)
│   ├── flow_matching.h         # Ford-Fulkerson (DFS), Edmonds-Karp (BFS), Bipartite Matching
│   ├── string_matching.h       # Naive, Rabin-Karp (hash), Finite Automaton (DFA), KMP (LPS)
│   ├── compression_sync.h      # Huffman Coding, Fixed-Length Coding, Chinese Remainder Theorem
│   ├── scenarios.h             # Normal, Shock, Stress scenarios & Benchmark suite
│   └── theory_notes.h          # Recurrences, Master Theorem, Complexity table, P vs NP
├── src/                        # C99 Source Implementations
│   ├── main.c                  # Main menu loop & CLI argument dispatch
│   ├── models.c                # CSV data parsing, manual entry, network graph builder
│   ├── sorting.c               # Sorting implementations & comparator runner
│   ├── searching.c             # Searching implementations & comparator runner
│   ├── divide_conquer.c        # D&C implementations & comparison vs brute-force
│   ├── shortest_path.c         # Shortest path algorithms & comparison runner
│   ├── spanning_tree.c         # MST algorithms & comparison runner
│   ├── scheduling.c            # Greedy scheduling & comparison vs FCFS
│   ├── route_selection.c       # Combinatorial route selection & B&B node exploration
│   ├── dp_tsp.c                # Dynamic programming & Held-Karp Bitmask TSP
│   ├── flow_matching.c         # Max flow & bipartite matching algorithms
│   ├── string_matching.c       # String matching algorithms on transit incident logs
│   ├── compression_sync.c      # Huffman compression & CRT timetable sync
│   ├── theory_notes.c          # Interactive academic theory notes
│   └── scenarios.c             # Scenarios A, B, C, data generator & CSV exporter
├── data/                       # CSV Datasets
│   ├── stops.csv               # 15 Dehradun bus stops with hourly passenger demand
│   ├── roads.csv               # Road network connections, distances, times, capacities
│   ├── buses.csv               # Fleet vehicles, seating capacities, assigned depots
│   ├── depots.csv              # Depot terminals, bay capacities, available vehicles
│   ├── routes.csv              # Operating corridors, distances, costs, expected passengers
│   └── trips.csv               # Timetable trip schedules with start/end time windows
├── tests/                      # Automated Verification
│   ├── test_runner.h           # Assert-style testing macros and result registry
│   └── test_main.c             # 44 automated tests covering all modules & edge cases
├── results/                    # Generated Outputs & Exports
│   ├── benchmark.csv           # Multi-scale benchmark results (100 to 10000 + adversarial)
│   └── route_plan.csv          # Exported route plans
└── docs/                       # Project Documentation
    ├── PROBLEM_STATEMENT.md    # Target user, requirements, inputs, outputs, constraints
    ├── ARCHITECTURE.md         # ASCII system workflow & component relationships
    ├── ALGORITHMS.md           # Pseudocode, recurrences, and correctness proofs
    ├── COMPLEXITY_TABLE.md     # Comprehensive complexity cheat-sheet
    ├── COMPARISON_GUIDE.md     # Comparative algorithm selection guide
    ├── LIMITATIONS_AND_ERRORS.md# Defensive handling of edge cases & boundary conditions
    ├── TEST_CASES.md           # Complete 44-test execution matrix
    └── VIVA_QUESTIONS.md       # 40 comprehensive viva questions with detailed answers
```

---

## 3. Reused Components from Student's Initial Workspace
We preserved the student's foundation while expanding it into an enterprise-grade academic project:
1. **`struct node` (alias `Stop`):** Maintained original field names (`id`, `name`, `passengers`) in `include/models.h`, preserving 100% compatibility with the student's original `stops.h`.
2. **Original Function Signatures:** Provided compatibility wrappers for `loadStops`, `displayStops`, `linearSearch`, `binarySearchIterative`, and `binarySearchRecursive`.
3. **Domain Datasets:** Reused and enriched the original CSV data (`stops.csv`, `roads.csv`, `buses.csv`, `deports.csv`) centered around Dehradun transit landmarks (Ballupur, Clock Tower, ISBT, Prem Nagar, Rajpur Road).
4. **Menu Style:** Retained the clean, simple, student-friendly `printf`/`scanf` menu navigation loop with clear ASCII borders and input validation.

---

## 4. Compilation & Execution Instructions

### A. Windows (Command Prompt or PowerShell)
Ensure `gcc` is installed (MinGW, TDM-GCC, or MSYS2). Run:

```cmd
build.bat
```

Or compile manually with a single plain `gcc` command:
```cmd
gcc -std=c99 -Wall -Wextra -O2 -Iinclude src/*.c tests/test_main.c -o bus_planner.exe -lm
gcc -std=c99 -Wall -Wextra -O2 -Iinclude -DTEST_STANDALONE_RUNNER tests/test_main.c src/models.c src/sorting.c src/searching.c src/divide_conquer.c src/shortest_path.c src/spanning_tree.c src/scheduling.c src/route_selection.c src/dp_tsp.c src/flow_matching.c src/string_matching.c src/compression_sync.c -o test_runner.exe -lm
```

To run:
```cmd
bus_planner.exe
test_runner.exe
```

### B. Linux / macOS
Using `make`:
```bash
make
./bus_planner.exe
./test_runner.exe
```

Or with plain `gcc`:
```bash
gcc -std=c99 -Wall -Wextra -O2 -Iinclude src/*.c tests/test_main.c -o bus_planner -lm
./bus_planner
```

---

## 5. Direct Command-Line Flags (Headless Mode)
In addition to the interactive menu, `bus_planner.exe` supports direct command-line arguments:
- `.\bus_planner.exe --test` : Runs all 44 unit tests and prints the pass/fail summary table.
- `.\bus_planner.exe --benchmark` : Runs multi-scale benchmarks ($N=100 \dots 10000$) and exports to `results/benchmark.csv`.
- `.\bus_planner.exe --normal` : Executes Demo Scenario A (Normal Operations).
- `.\bus_planner.exe --shock` : Executes Demo Scenario B (Depot Outage, Blocked Road, 50% Fleet Cut).
- `.\bus_planner.exe --stress` : Executes Demo Scenario C (1000+ Stops/Trips Synthetic Metropolitan Load).

---

## 6. Menu Structure & Algorithm Navigation

```text
======================================================================
                    PUBLIC BUS PLANNER - MAIN MENU                    
======================================================================
  1.  Sorting (Bubble, Merge, Quick, Compare all)
  2.  Searching (Linear, Binary iterative, Binary recursive, Compare)
  3.  Divide & Conquer (Max subarray, Fast power, Celebrity, Strassen)
  4.  Shortest Path (Dijkstra, Bellman-Ford, Floyd-Warshall, Compare)
  5.  Spanning Tree (Prim, Kruskal with Union-Find, Compare)
  6.  Scheduling (Greedy Activity Selection, Interval Partitioning)
  7.  Route Selection (Greedy, DP, Backtracking, B&B FIFO/LIFO/LC, N-Queens)
  8.  DP & TSP (LCS, Matrix Chain, OBST, TSP Brute Force vs Bitmask DP)
  9.  Flow & Matching (Ford-Fulkerson, Edmonds-Karp, Bipartite Matching)
  10. String Matching (Naive, Rabin-Karp, Finite Automaton, KMP)
  11. Compression & Sync (Huffman, Fixed-length, CRT Schedule Sync)
  12. Data, Scenarios & Tests (Load, Generate, Normal/Shock/Stress, Benchmark)
  0.  Exit Program
======================================================================
```

Every sub-menu provides:
- Algorithm options with mathematical step-by-step traces.
- A **Compare all** option as the last item running all methods on the exact same dataset and displaying runtime, operation count, memory estimate, and best choice recommendation.

---

## 7. Three Demonstration Scenarios
1. **Normal Scenario (Menu 12, Option 4):**
   - Full baseline city network with 15 stops, 3 depots, and 15 buses.
   - Computes arterial shortest paths, optimal fleet sizing, and budgeted route selection without delays.
2. **Constraint Shock Scenario (Menu 12, Option 5):**
   - Simulates emergency disruptions: Depot 1 (ISBT) closes, road link between Stop 2 and Stop 3 is blocked, and available fleet is cut by 50%.
   - Re-routes traffic via the Rispana bypass (increasing path from 10 km to 17 km) and applies emergency budget austerity.
3. **Stress Scenario (Menu 12, Option 6):**
   - Generates a 1000-stop, 1000-trip synthetic metropolitan transit network.
   - Executes Merge Sort, Quick Sort, Binary Search, and Maximum Subarray in sub-second time with 100% correctness.

---

## 8. Test Suite Verification
The automated test runner (`tests/test_main.c`) verifies 44 assertion test cases across all algorithmic categories:
```text
Test Verdict: 44 / 44 Tests Passed (100.0% Success Rate)
```
See `docs/TEST_CASES.md` for the complete expected vs. actual output matrix.

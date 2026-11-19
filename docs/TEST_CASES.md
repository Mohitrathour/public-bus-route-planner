# Test Case Specification & Execution Table
## Public Bus Route Frequency and Depot Planner (Project 12)

All 44 automated tests are implemented in `tests/test_main.c` and executed via `test_runner.exe` or Menu 12 Option 8.

---

### Test Execution Matrix

| # | Category | Test Case Description | Test Input / Setup | Expected Result | Actual Output | Status |
| :---: | :--- | :--- | :--- | :--- | :--- | :---: |
| 1 | **Sorting** | BubbleSort Max Demand Stop | Unsorted stops [120, 150, 200, 90, 180] | ID 3 (ISBT, 200 pax) at index 0 | ID 3 | **PASS** |
| 2 | **Sorting** | BubbleSort Min Demand Stop | Unsorted stops [120, 150, 200, 90, 180] | ID 4 (Prem Nagar, 90 pax) at index 4 | ID 4 | **PASS** |
| 3 | **Sorting** | MergeSort Max Demand Stop | Unsorted stops [120, 150, 200, 90, 180] | ID 3 at index 0 | ID 3 | **PASS** |
| 4 | **Sorting** | QuickSort Max Demand Stop | Unsorted stops [120, 150, 200, 90, 180] | ID 3 at index 0 | ID 3 | **PASS** |
| 5 | **Sorting** | Boundary: Single Element | Single stop array [100 pax] | Remains 100 pax | 100 | **PASS** |
| 6 | **Sorting** | Adversarial: All Equal Keys | Array [50, 50, 50] | Elements remain 50 | 50 | **PASS** |
| 7 | **Searching** | LinearSearch Target in Middle | Sorted stops, target ID 3 | Index 2 | Index 2 | **PASS** |
| 8 | **Searching** | BinarySearch Iterative Middle | Sorted stops, target ID 3 | Index 2 | Index 2 | **PASS** |
| 9 | **Searching** | BinarySearch Recursive Middle | Sorted stops, target ID 3 | Index 2 | Index 2 | **PASS** |
| 10 | **Searching** | Invalid: Target Missing | Sorted stops, search target ID 999 | `-1` (Not Found) | `-1` | **PASS** |
| 11 | **Divide&Conquer**| MaxSubarray D&C Sum | `[-2, 1, -3, 4, -1, 2, 1, -5, 4]` | Sum = 6 `[4, -1, 2, 1]` | 6 | **PASS** |
| 12 | **Divide&Conquer**| MaxSubarray Kadane Sum | Same surge array | Sum = 6 | 6 | **PASS** |
| 13 | **Divide&Conquer**| FastPower $2^{10}$ | Base 2, exponent 10, mod $10^9+7$ | 1024 | 1024 | **PASS** |
| 14 | **Divide&Conquer**| FastPower Base 0 Exponent | Base 3, exponent 0 | 1 | 1 | **PASS** |
| 15 | **Divide&Conquer**| Celebrity Transit Hub | 4 stops, all route into Stop 2 | Stop ID 2 | Stop ID 2 | **PASS** |
| 16 | **Divide&Conquer**| Strassen 2x2 C[0][0] | Matrix A x B | Matches Classical C[0][0] (4) | 4 | **PASS** |
| 17 | **Divide&Conquer**| Strassen 2x2 C[1][1] | Matrix A x B | Matches Classical C[1][1] (8) | 8 | **PASS** |
| 18 | **ShortestPath** | Dijkstra Triangle Shortest Dist | Triangle graph 1->2 (3), 2->3 (4), 1->3 (10) | Dist 1->3 = 7 km (via 2) | 7 | **PASS** |
| 19 | **ShortestPath** | BellmanFord Triangle Shortest Dist | Same graph | Dist 1->3 = 7 km | 7 | **PASS** |
| 20 | **ShortestPath** | FloydWarshall All Pairs Dist 1->3 | Full matrix calculation | Dist 1->3 = 7 km | 7 | **PASS** |
| 21 | **SpanningTree** | Prim MST Total Weight | 4-node diamond graph with weights 1, 2, 3, 4, 5 | Total MST = 6 km | 6 | **PASS** |
| 22 | **SpanningTree** | Kruskal MST Total Weight | Same graph | Total MST = 6 km | 6 | **PASS** |
| 23 | **SpanningTree** | MST Edge Count ($V-1$) | 4 vertices | 3 edges selected | 3 | **PASS** |
| 24 | **Scheduling** | Activity Selection Optimal Trips | 4 trips with overlaps | 3 non-overlapping trips | 3 | **PASS** |
| 25 | **Scheduling** | Interval Partitioning Min Fleet | Same 4 trips | Minimum 2 buses/bays | 2 | **PASS** |
| 26 | **RouteSelection**| 0/1 Knapsack DP Optimum | 4 routes, budget Rs 4500 | 1800 passengers | 1800 | **PASS** |
| 27 | **RouteSelection**| Backtracking Matches DP | Same routes and budget | 1800 passengers | 1800 | **PASS** |
| 28 | **RouteSelection**| B&B FIFO Matches DP | Same routes and budget | 1800 passengers | 1800 | **PASS** |
| 29 | **RouteSelection**| B&B LIFO Matches DP | Same routes and budget | 1800 passengers | 1800 | **PASS** |
| 30 | **RouteSelection**| B&B Least-Cost Matches DP | Same routes and budget | 1800 passengers | 1800 | **PASS** |
| 31 | **RouteSelection**| N-Queens 4x4 Solvable | $4 \times 4$ depot bay grid | 1 (Solvable without conflict) | 1 | **PASS** |
| 32 | **DP & TSP** | LCS Common Subsequence Length | Sequences `[1, 2, 3, 4, 5]` vs `[2, 4, 5, 6, 7]` | Length = 3 (`[2, 4, 5]`) | 3 | **PASS** |
| 33 | **DP & TSP** | Matrix Chain Single Mult | Dimensions {10, 20, 30} | 6000 scalar multiplications | 6000 | **PASS** |
| 34 | **DP & TSP** | TSP Brute Force 4 Stops | 4-stop feeder graph | Tour cost = 70 km | 70 | **PASS** |
| 35 | **DP & TSP** | TSP Bitmask DP Matches BF | Same 4-stop graph | Tour cost = 70 km | 70 | **PASS** |
| 36 | **FlowMatching** | Ford-Fulkerson Max Flow | 4-node network with capacity bottleneck | 20 passengers/hour | 20 | **PASS** |
| 37 | **FlowMatching** | Edmonds-Karp Matches FF | Same flow network | 20 passengers/hour | 20 | **PASS** |
| 38 | **FlowMatching** | Bipartite Matching Perfect 3 | 3 buses, 3 compatible routes | 3 matched pairs | 3 | **PASS** |
| 39 | **StringMatch** | Naive Occurrences | Search "DEHRADUN" in incident log | 2 occurrences | 2 | **PASS** |
| 40 | **StringMatch** | Rabin-Karp Matches Naive | Same text and keyword | 2 occurrences | 2 | **PASS** |
| 41 | **StringMatch** | Finite Automaton Matches Naive | Same text and keyword | 2 occurrences | 2 | **PASS** |
| 42 | **StringMatch** | KMP Matches Naive | Same text and keyword | 2 occurrences | 2 | **PASS** |
| 43 | **Compression** | Huffman Bits Less Than Original | String "AAAAABBBBCCCDDE" | Compressed bits < Original bits | TRUE | **PASS** |
| 44 | **Sync** | CRT Solution $x = 23$ | $x \equiv 2 \pmod 3, 3 \pmod 5, 2 \pmod 7$ | Synchronized minute = 23 | 23 | **PASS** |

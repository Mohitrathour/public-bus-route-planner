# Algorithm Complexity Reference Guide
## Public Bus Route Frequency and Depot Planner (Project 12)

---

### Complete Complexity Matrix

| Algorithm | Paradigm | Best Time | Average Time | Worst Time | Space | Recurrence / Bound | Optimal Domain Scenario |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Bubble Sort** | Brute Force | $\Omega(n)$ | $\Theta(n^2)$ | $O(n^2)$ | $O(1)$ | $T(n) = T(n-1) + O(n)$ | Small lists of bus stops with swapped early exit. |
| **Merge Sort** | Divide & Conquer | $\Omega(n \log n)$ | $\Theta(n \log n)$ | $O(n \log n)$ | $O(n)$ | $T(n) = 2T(n/2) + \Theta(n)$ | Guaranteed predictable runtime for sorting timetables. |
| **Quick Sort** | Divide & Conquer | $\Omega(n \log n)$ | $\Theta(n \log n)$ | $O(n^2)$ | $O(\log n)$ | $T(n) = 2T(n/2) + \Theta(n)$ | In-place, cache-friendly stop ranking by passenger demand. |
| **Linear Search** | Brute Force | $\Omega(1)$ | $\Theta(n)$ | $O(n)$ | $O(1)$ | $T(n) = T(n-1) + O(1)$ | Searching unsorted arrays or stop names. |
| **Binary Search (Iterative)** | Decrease & Conquer | $\Omega(1)$ | $\Theta(\log n)$ | $O(\log n)$ | $O(1)$ | $T(n) = T(n/2) + \Theta(1)$ | Sub-millisecond lookup on sorted stop/route indexes. |
| **Binary Search (Recursive)** | Decrease & Conquer | $\Omega(1)$ | $\Theta(\log n)$ | $O(\log n)$ | $O(\log n)$ | $T(n) = T(n/2) + \Theta(1)$ | Educational divide-and-conquer demonstration. |
| **Max Subarray (D&C)** | Divide & Conquer | $\Omega(n \log n)$ | $\Theta(n \log n)$ | $O(n \log n)$ | $O(\log n)$ | $T(n) = 2T(n/2) + \Theta(n)$ | Finding peak 24-hr passenger surge window. |
| **Max Subarray (Kadane)**| Dynamic Programming| $\Omega(n)$ | $\Theta(n)$ | $O(n)$ | $O(1)$ | $DP[i] = \max(A[i], DP[i-1] + A[i])$ | Real-time single-pass passenger surge detection. |
| **Fast Exponentiation** | Divide & Conquer | $\Omega(\log e)$ | $\Theta(\log e)$ | $O(\log e)$ | $O(1)$ | $T(e) = T(e/2) + O(1)$ | Compounding delay calculations across route stages. |
| **Celebrity Problem** | Elimination D&C | $\Omega(n)$ | $\Theta(n)$ | $O(n)$ | $O(1)$ | Eliminates 1 vertex per check | Identifying central transit hub station. |
| **Strassen Matrix Mult**| Divide & Conquer | $\Omega(n^{2.807})$| $\Theta(n^{2.807})$| $O(n^{2.807})$| $O(n^2)$ | $T(n) = 7T(n/2) + \Theta(n^2)$ | Asymptotically faster large-scale network reachability. |
| **Classical Matrix Mult**| Iterative | $\Omega(n^3)$ | $\Theta(n^3)$ | $O(n^3)$ | $O(n^2)$ | $n^3$ multiplications | Best for small matrix multiplication ($n \le 64$). |
| **Dijkstra** | Greedy | $\Omega(V^2)$ | $\Theta(V^2)$ | $O(V^2)$ | $O(V)$ | Array extract-min | Single-source shortest path with non-negative road lengths. |
| **Bellman-Ford** | Dynamic Programming| $\Omega(E)$ | $\Theta(V \cdot E)$ | $O(V \cdot E)$ | $O(V)$ | Relaxes $V-1$ times | Transit networks with discounts/rebates (negative edges). |
| **Floyd-Warshall** | Dynamic Programming| $\Omega(V^3)$ | $\Theta(V^3)$ | $O(V^3)$ | $O(V^2)$ | $D^{(k)}[i][j]$ recurrence | Full all-pairs origin-destination travel time matrix. |
| **Prim's Algorithm** | Greedy | $\Omega(V^2)$ | $\Theta(V^2)$ | $O(V^2)$ | $O(V)$ | Cut property vertex loop | Minimum-cost backbone road network on dense graphs. |
| **Kruskal's Algorithm** | Greedy (Union-Find) | $\Omega(E \log E)$| $\Theta(E \log E)$| $O(E \log E)$| $O(V + E)$| Edge sorting + DSU $\alpha(V)$ | Minimum-cost backbone network on sparse graphs. |
| **Activity Selection** | Greedy | $\Omega(n \log n)$ | $\Theta(n \log n)$ | $O(n \log n)$ | $O(1)$ | Sort finish times + scan | Maximizing non-overlapping trips for a single bus. |
| **Interval Partitioning**| Greedy | $\Omega(n \log n)$ | $\Theta(n \log n)$ | $O(n \log n)$ | $O(d)$ | Sort start times + scan | Calculating minimum bus fleet / depot maintenance bays. |
| **0/1 Knapsack DP** | Dynamic Programming| $\Omega(n \cdot W)$| $\Theta(n \cdot W)$| $O(n \cdot W)$| $O(n \cdot W)$| Pseudo-polynomial | Exact optimal route selection under integer budget $W$. |
| **Backtracking (Knapsack)**| Depth-First Search| $\Omega(n)$ | $O(2^n)$ | $O(2^n)$ | $O(n)$ | State-space tree search | Exact route subset selection for small $n$. |
| **Branch & Bound (FIFO)**| Breadth-First Search| $\Omega(n)$ | $O(2^n)$ | $O(2^n)$ | $O(2^n)$ | Queue + fractional UB | Exact route selection with level-by-level pruning. |
| **Branch & Bound (LIFO)**| Depth-First Search| $\Omega(n)$ | $O(2^n)$ | $O(2^n)$ | $O(n)$ | Stack + fractional UB | Memory-conservative branch and bound search. |
| **Branch & Bound (LCBB)**| Best-First Search | $\Omega(n)$ | Minimal nodes | $O(2^n)$ | $O(2^n)$ | Priority queue on highest UB | Explores fewest search tree nodes among exact solvers! |
| **N-Queens Backtracking**| Backtracking | $\Omega(n)$ | $O(n!)$ | $O(n!)$ | $O(n^2)$ | Safe bay positioning | Conflict-free bus parking in depot charging grids. |
| **LCS** | Dynamic Programming| $\Omega(nm)$ | $\Theta(nm)$ | $O(nm)$ | $O(nm)$ | $c[i][j]$ table | Finding shared transit corridors between two routes. |
| **Matrix Chain Mult** | Dynamic Programming| $\Omega(n^3)$ | $\Theta(n^3)$ | $O(n^3)$ | $O(n^2)$ | $m[i][j]$ split table | Optimizing multi-stage passenger transition matrices. |
| **Optimal BST** | Dynamic Programming| $\Omega(n^3)$ | $\Theta(n^3)$ | $O(n^3)$ | $O(n^2)$ | $e[i][j]$ probability tree | Minimizing search keystrokes in passenger kiosks. |
| **TSP Brute Force** | Exhaustive Search | $\Omega(n!)$ | $\Theta(n!)$ | $O(n!)$ | $O(n)$ | $(n-1)!$ permutations | Baseline exact tour for small feeder loops ($n \le 8$). |
| **TSP Bitmask DP** | Dynamic Programming| $\Omega(n^2 2^n)$ | $\Theta(n^2 2^n)$| $O(n^2 2^n)$ | $O(n 2^n)$ | Held-Karp $dp[mask][u]$ | Exact optimal feeder loop up to 12 stops. |
| **Ford-Fulkerson (DFS)** | Network Flow | $\Omega(E)$ | $O(E \cdot \text{max\_flow})$ | $O(E \cdot f)$ | $O(V^2)$ | DFS augmenting paths | Peak passenger evacuation flow through transit grid. |
| **Edmonds-Karp (BFS)** | Network Flow | $\Omega(V \cdot E)$ | $\Theta(V \cdot E^2)$| $O(V \cdot E^2)$ | $O(V^2)$ | BFS shortest path augments | Guaranteed polynomial capacity flow optimization. |
| **Bipartite Matching** | Flow Reduction | $\Omega(V)$ | $O(V \cdot E)$ | $O(V \cdot E)$ | $O(V)$ | Kuhn's alternating paths | Optimal assignment of buses to routes / drivers to shifts. |
| **Naive String Search** | Brute Force | $\Omega(n)$ | $O(nm)$ | $O(nm)$ | $O(1)$ | Slide pattern over text | Simple keyword lookups in short strings. |
| **Rabin-Karp** | Rolling Hash | $\Omega(n+m)$ | $\Theta(n+m)$ | $O(nm)$ | $O(1)$ | Modulo 101 prime hash | Multiple pattern searching in incident event logs. |
| **Finite Automaton** | Deterministic FA | $\Omega(n)$ | $\Theta(n)$ | $\Theta(n)$ | $O(m \|\Sigma\|)$| $\delta(q, a)$ state table | Real-time streaming incident log monitoring (no backtracking). |
| **Knuth-Morris-Pratt** | Prefix Matching | $\Omega(n+m)$ | $\Theta(n+m)$ | $O(n+m)$ | $O(m)$ | LPS table $\pi[q]$ | High-speed, memory-efficient string matching. |
| **Huffman Coding** | Greedy (Min-Heap) | $\Omega(k \log k)$ | $\Theta(k \log k)$ | $O(k \log k)$ | $O(k)$ | Min-heap code tree | Compressing GPS telemetry packets for IoT modems. |
| **Chinese Remainder Thm**| Number Theory | $\Omega(k \log m)$ | $\Theta(k \log m)$ | $O(k \log m)$ | $O(k)$ | Extended Euclidean inverse | Synchronizing multi-route bus arrival headways. |

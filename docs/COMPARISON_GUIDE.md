# Algorithm Comparison & Selection Guide
## Public Bus Route Frequency and Depot Planner (Project 12)

This guide summarizes how to select the right algorithm for specific bus transit operational constraints.

---

### 1. Sorting Selection Guide: Bubble vs Merge vs Quick Sort

| Criterion | Bubble Sort | Merge Sort | Quick Sort |
| :--- | :--- | :--- | :--- |
| **Time Complexity (Avg)** | $O(n^2)$ | $O(n \log n)$ | $O(n \log n)$ |
| **Worst-Case Guarantee** | $O(n^2)$ | **Guaranteed $O(n \log n)$** | $O(n^2)$ (degenerate pivot) |
| **Auxiliary Memory** | **$O(1)$ (In-place)** | $O(n)$ (Requires buffer) | $O(\log n)$ (Stack frames) |
| **Stability** | Stable | Stable | Unstable |
| **Cache Locality** | Poor | Moderate | **Excellent (Sequential array partition)** |
| **Recommendation** | Only for small ($n \le 20$) or already sorted stop lists. | **Best for mission-critical timetable sorting** where worst-case predictability is mandatory. | **Best general-purpose sort for passenger demand rankings** due to low constant factor and in-place sorting. |

---

### 2. Searching Selection Guide: Linear vs Binary Search

| Criterion | Linear Search | Binary Search (Iterative) | Binary Search (Recursive) |
| :--- | :--- | :--- | :--- |
| **Prerequisite** | None (Unsorted array OK) | **Must be sorted by search key** | **Must be sorted by search key** |
| **Time Complexity** | $O(n)$ | **$O(\log n)$** | **$O(\log n)$** |
| **Auxiliary Space** | $O(1)$ | **$O(1)$** | $O(\log n)$ stack frames |
| **Recommendation** | Use for stop name substring search. | **Best for stop ID lookup** in sorted master registries. | Use for academic recursion tracing in viva voce. |

---

### 3. Shortest Path Selection Guide: Dijkstra vs Bellman-Ford vs Floyd-Warshall

| Criterion | Dijkstra's Algorithm | Bellman-Ford Algorithm | Floyd-Warshall Algorithm |
| :--- | :--- | :--- | :--- |
| **Source Type** | Single-Source | Single-Source | **All-Pairs** |
| **Time Complexity** | **$O(V^2)$ or $O(E \log V)$** | $O(V \cdot E)$ | $O(V^3)$ |
| **Negative Road Weights**| **FAILS (Requires $w \ge 0$)** | **Supported** | Supported (No negative cycles) |
| **Negative Cycle Check**| None | **Detects & Reports Cycles** | Can detect via $D[i][i] < 0$ |
| **Recommendation** | **Best for real-time emergency rerouting** on standard physical roads. | Use when subsidized green transit corridors introduce net negative tolls. | **Best for offline schedule compilation** requiring full origin-destination distance matrix. |

---

### 4. Minimum Spanning Tree Selection: Prim's vs Kruskal's Algorithm

| Criterion | Prim's Algorithm | Kruskal's Algorithm |
| :--- | :--- | :--- |
| **Graph Representation** | Adjacency Matrix / Priority Queue | Edge List |
| **Time Complexity** | $O(V^2)$ matrix, $O(E \log V)$ heap | $O(E \log E)$ |
| **Data Structures** | Key array / Min-Heap | Disjoint Set Union (Union-Find) |
| **Optimal Graph Density** | **Dense graphs ($E \approx V^2$)** | **Sparse graphs ($E \ll V^2$)** |
| **Recommendation** | Best for tightly connected urban depot clusters. | Best for regional highway networks with distant stops. |

---

### 5. Route Selection under Budget: 6 Approaches Compared

For the exact same problem instance (operating routes under budget $W$ to maximize total passengers):

| Method | Optimality | Time Complexity | Nodes Explored | Memory | Best Scenario |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Greedy Ratio** | Approximation | $O(n \log n)$ | $n$ items | $O(1)$ | Rapid approximate feasibility checks. |
| **0/1 Knapsack DP** | **Exact Optimal**| $O(n \cdot W)$ | Full $n \times W$ table | $O(n \cdot W)$ | **Fastest exact solution** for integer budgets ($W \le 50,000$). |
| **Backtracking (DFS)**| **Exact Optimal**| $O(2^n)$ | Up to $2^{n+1}-1$ | $O(n)$ | Exact solutions when memory is strictly limited. |
| **B&B FIFO (Queue)** | **Exact Optimal**| $O(2^n)$ | Reduced by upper bound | $O(2^n)$ | Level-by-level breadth-first search. |
| **B&B LIFO (Stack)** | **Exact Optimal**| $O(2^n)$ | Reduced by upper bound | $O(n)$ | Combines DFS memory bounds with pruning. |
| **B&B Least-Cost (LCBB)**| **Exact Optimal**| $O(2^n)$ | **FEWEST SEARCH NODES!** | $O(2^n)$ | **Best exact solver when sub-problem evaluation is computationally expensive.** |

---

### 6. String Matching Selection: Naive vs Rabin-Karp vs Finite Automaton vs KMP

| Criterion | Naive | Rabin-Karp | Finite Automaton | KMP |
| :--- | :--- | :--- | :--- | :--- |
| **Preprocessing** | None ($0$) | $O(m)$ hash computation | $O(m \cdot \|\Sigma\|)$ table build | **$O(m)$ LPS table** |
| **Matching Time** | $O((n-m+1)m)$ | $O(n+m)$ average | **$\Theta(n)$ strict** | **$\Theta(n)$ strict** |
| **Text Backtracking**| Yes | Only on hash collision | **Never** | **Never** |
| **Recommendation**| Short patterns. | Multi-pattern or hashing demo. | Hardware regex engines. | **Best software matcher for streaming logs.** |

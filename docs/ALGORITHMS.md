# Algorithm Reference & Mathematical Derivations
## Public Bus Route Frequency and Depot Planner (Project 12)

---

### Module 1: Sorting Algorithms

#### 1. Bubble Sort
- **Pseudocode:**
  ```text
  procedure BubbleSort(A[0..n-1]):
      for i from 0 to n-2 do:
          swapped <- false
          for j from 0 to n-i-2 do:
              if A[j].passengers < A[j+1].passengers then:
                  swap(A[j], A[j+1])
                  swapped <- true
          if not swapped then break
  ```
- **Correctness:** Loop invariant: after iteration $i$, the $i+1$ largest elements are placed in their final sorted positions at $A[0..i]$. The swapped flag ensures $O(n)$ termination when already sorted.

#### 2. Merge Sort
- **Recurrence:** $T(n) = 2T(n/2) + \Theta(n)$  
  By Master Theorem ($a=2, b=2, d=1$): $\log_2 2 = 1 = d \implies T(n) = \Theta(n \log n)$.
- **Pseudocode:**
  ```text
  procedure MergeSort(A, l, r):
      if l < r then:
          m <- l + (r - l) / 2
          MergeSort(A, l, m)
          MergeSort(A, m + 1, r)
          Merge(A, l, m, r)
  ```
- **Correctness:** Inductive hypothesis: recursive calls correctly sort subarrays $A[l..m]$ and $A[m+1..r]$. The `Merge` procedure scans both in non-increasing order and combines them into sorted $A[l..r]$ in linear $\Theta(n)$ time.

#### 3. Quick Sort
- **Recurrence:**
  - Average Case: $T(n) = 2T(n/2) + \Theta(n) \implies \Theta(n \log n)$
  - Worst Case: $T(n) = T(n-1) + \Theta(n) \implies \Theta(n^2)$ (when already sorted and pivot is extreme)
- **Pseudocode (Lomuto Partition):**
  ```text
  procedure Partition(A, low, high):
      pivot <- A[high].passengers
      i <- low - 1
      for j from low to high - 1 do:
          if A[j].passengers >= pivot then:
              i <- i + 1
              swap(A[i], A[j])
      swap(A[i+1], A[high])
      return i + 1
  ```

---

### Module 2: Searching Algorithms

#### 1. Binary Search (Iterative & Recursive)
- **Recurrence:** $T(n) = T(n/2) + \Theta(1) \implies \Theta(\log n)$
- **Iterative Invariant:** Target ID $k$ lies within $A[\text{low}..\text{high}]$ if present. Midpoint check halves search space each iteration.
- **Recursive Invariant:** Base cases $\text{low} > \text{high}$ (not found) and $A[\text{mid}] = k$ terminate depth $\le \lfloor \log_2 n \rfloor + 1$.

---

### Module 3: Divide & Conquer

#### 1. Maximum Subarray (Peak Demand Period)
- **Recurrence:** $T(n) = 2T(n/2) + \Theta(n) \implies \Theta(n \log n)$
- **Explanation:** Max contiguous subarray either lies entirely in the left half, entirely in the right half, or crosses the midpoint. Crossing subarray is computed in $O(n)$ by expanding left from $\text{mid}$ and right from $\text{mid}+1$.

#### 2. Fast Exponentiation (Repeated Squaring)
- **Recurrence:** $T(n) = T(n/2) + \Theta(1) \implies \Theta(\log n)$
- **Formula:**
  $$a^n = \begin{cases} (a^{n/2})^2 & \text{if } n \text{ is even} \\ a \cdot (a^{(n-1)/2})^2 & \text{if } n \text{ is odd} \end{cases}$$

#### 3. Celebrity Hub Problem
- **Algorithm:** Two-pointer elimination in $\Theta(n)$ time with $3(n-1)$ edge queries.
- **Logic:** If Stop $a$ routes to Stop $b$, $a$ cannot be a central terminal hub (hub has 0 outgoing routes). If $a$ does not route to $b$, $b$ cannot be a hub (all stops must route into hub). Eliminates 1 stop per check.

#### 4. Strassen's Matrix Multiplication
- **Recurrence:** $T(n) = 7T(n/2) + \Theta(n^2)$
- **Master Theorem:** $a=7, b=2 \implies \log_2 7 \approx 2.807 > 2 \implies T(n) = \Theta(n^{\log_2 7}) \approx \Theta(n^{2.807})$.
- **Formulas:**
  $$\begin{aligned}
  M_1 &= (A_{11} + A_{22})(B_{11} + B_{22}) \\
  M_2 &= (A_{21} + A_{22})B_{11} \\
  M_3 &= A_{11}(B_{12} - B_{22}) \\
  M_4 &= A_{22}(B_{21} - B_{11}) \\
  M_5 &= (A_{11} + A_{12})B_{22} \\
  M_6 &= (A_{21} - A_{11})(B_{11} + B_{12}) \\
  M_7 &= (A_{12} - A_{22})(B_{21} + B_{22})
  \end{aligned}$$

---

### Module 4: Shortest Paths

#### 1. Dijkstra's Algorithm
- **Greedy Choice:** Always extract vertex $u$ with minimum tentative distance $\text{dist}[u]$.
- **Correctness:** By contradiction: since edge weights are non-negative, any alternative path to $u$ via unvisited vertices must have length $\ge \text{dist}[u]$.

#### 2. Bellman-Ford Algorithm
- **Relaxation Invariant:** After $k$ passes over all edges, $\text{dist}[v]$ is at most the weight of the shortest path from $\text{src}$ to $v$ containing $\le k$ edges.
- **Negative Cycle Detection:** A path without cycles has at most $V-1$ edges. If any edge can still be relaxed on pass $V$, a negative weight cycle exists.

#### 3. Floyd-Warshall Algorithm
- **Recurrence:** $D^{(k)}[i][j] = \min\left(D^{(k-1)}[i][j], D^{(k-1)}[i][k] + D^{(k-1)}[k][j]\right)$
- **Time/Space:** $O(V^3)$ time, $O(V^2)$ space.

---

### Module 5: Spanning Trees

#### 1. Prim's Algorithm
- **Cut Property:** Let $S$ be vertices currently spanned. The minimum weight edge crossing cut $(S, V \setminus S)$ is safe and belongs to the MST.

#### 2. Kruskal's Algorithm with Disjoint Set Union
- **Cycle Prevention:** Edge $(u, v)$ is accepted iff $\text{Find}(u) \ne \text{Find}(v)$.
- **Amortized Time:** Path compression + union by rank yields $\alpha(V)$ per operation (Inverse Ackermann), making total time $O(E \log E)$.

---

### Module 6: Scheduling (Greedy)

#### 1. Activity Selection
- **Greedy Rule:** Sort trips by finish time $\text{end}_i$, pick next compatible trip.
- **Proof of Optimality:** Greedy choice leaves maximum possible remaining time for subsequent trips (exchange argument).

#### 2. Interval Partitioning
- **Greedy Rule:** Sort trips by start time $\text{start}_i$, assign to earliest free bus/bay.
- **Optimality:** Number of buses allocated equals the maximum number of concurrent overlapping intervals (the depth $d$ of the schedule). Provably minimum possible fleet size!

---

### Module 7: Route Selection & Optimization

#### 1. 0/1 Knapsack Dynamic Programming
- **Recurrence:**
  $$\text{DP}[i][w] = \begin{cases} \text{DP}[i-1][w] & \text{if } \text{cost}_i > w \\ \max(\text{DP}[i-1][w], \text{DP}[i-1][w - \text{cost}_i] + \text{pax}_i) & \text{if } \text{cost}_i \le w \end{cases}$$

#### 2. Branch and Bound (Least-Cost LCBB)
- **Bounding Function:** Fractional knapsack linear relaxation provides an admissible upper bound $UB$.
- **Pruning Rule:** If node's $UB \le \text{best\_profit\_so\_far}$, prune child subtree.
- **Exploration:** Priority queue ordered by highest $UB$ visits fewest search tree nodes among all exact methods.

---

### Module 8: DP & Traveling Salesperson Problem

#### 1. Longest Common Subsequence (LCS)
- **Recurrence:**
  $$c[i][j] = \begin{cases} 0 & \text{if } i=0 \text{ or } j=0 \\ c[i-1][j-1] + 1 & \text{if } X[i] = Y[j] \\ \max(c[i-1][j], c[i][j-1]) & \text{if } X[i] \ne Y[j] \end{cases}$$

#### 2. Matrix Chain Multiplication (MCM)
- **Recurrence:** $m[i][j] = \min_{i \le k < j} \left(m[i][k] + m[k+1][j] + p_{i-1} p_k p_j\right)$

#### 3. Held-Karp Bitmask DP for TSP
- **State:** $\text{dp}[\text{mask}][u]$ = shortest path visiting subset of vertices in `mask` ending at vertex $u$.
- **Recurrence:** $\text{dp}[\text{mask}][u] = \min_{v \in \text{mask}, v \ne u} \left(\text{dp}[\text{mask} \setminus \{u\}][v] + \text{dist}[v][u]\right)$
- **Complexity:** $O(n^2 2^n)$ time, reducing $O(n!)$ factorial brute force to exponential.

---

### Module 9: Network Flow & Matching

#### 1. Ford-Fulkerson & Edmonds-Karp
- **Max-Flow Min-Cut Theorem:** The value of maximum flow equals the capacity of minimum $s$-$t$ cut.
- **Edmonds-Karp Guarantee:** Using BFS for augmenting paths guarantees shortest augmenting path in edge count. Running time is strictly bounded by $O(V \cdot E^2)$, regardless of edge capacities.

#### 2. Bipartite Matching
- **Kuhn's Augmenting Path:** Alternating paths in bipartite graph $G = (L \cup R, E)$ augment matching cardinality in $O(V \cdot E)$ time.

---

### Module 10: String Matching

| Algorithm | Preprocessing Time | Matching Time | Space | Character Backtracking in Text? |
| :--- | :--- | :--- | :--- | :--- |
| **Naive Search** | $0$ | $O((n-m+1)m)$ | $O(1)$ | Yes |
| **Rabin-Karp** | $O(m)$ | $O(n+m)$ avg, $O(nm)$ worst | $O(1)$ | On hash collision only |
| **Finite Automaton**| $O(m \cdot \|\Sigma\|)$ | $\Theta(n)$ strict | $O(m \cdot \|\Sigma\|)$| Never (1 state per char) |
| **Knuth-Morris-Pratt**| $O(m)$ (LPS $\pi$) | $\Theta(n)$ strict | $O(m)$ | Never (uses prefix table) |

---

### Module 11: Compression & Synchronization

#### 1. Huffman Coding
- **Prefix-Free Property:** No code is a prefix of another, enabling unambiguous single-pass stream decoding.
- **Optimality:** Produces minimum redundancy variable-length code bounded by Shannon's source coding theorem:
  $$H(X) \le L_{\text{avg}} < H(X) + 1$$

#### 2. Chinese Remainder Theorem (CRT)
- **System:** $x \equiv a_i \pmod{m_i}$ where $\gcd(m_i, m_j) = 1$ for all $i \ne j$.
- **Solution:** $x = \sum_{i=1}^k a_i M_i y_i \pmod M$, where $M = \prod m_i$, $M_i = M / m_i$, and $M_i y_i \equiv 1 \pmod{m_i}$.
- **Extended Euclidean Algorithm:** Computes $y_i = M_i^{-1} \pmod{m_i}$ in logarithmic time.

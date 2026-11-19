# Comprehensive Viva Voce Examination Guide (40 Questions & Answers)
## Public Bus Route Frequency and Depot Planner (Project 12)
**Course:** Design and Analysis of Algorithms (CSEG3053)

---

### Category 1: General Project Architecture & Domain Modeling

#### Q1: What is the core engineering objective of this project?
**Answer:** The project designs a bus transit frequency and depot allocation system in pure C (C99). It optimizes passenger waiting times and municipal operating costs under hard real-world constraints: fleet vehicle availability, depot parking/charging bay limits, route operating budgets, and non-overlapping timetable windows.

#### Q2: What structs did you reuse from the original code and why?
**Answer:** We reused `struct node` (aliased to `Stop`) with fields `id`, `name`, and `passengers` (demand per hour), maintaining 100% backward compatibility with `stops.h`. We also preserved the CSV schemas for `buses.csv` (`bus_id, capacity, depot_id`), `deports.csv` (`depot_id, name, available_buses`), and `roads.csv` (`from, to, distance`).

#### Q3: Why did you choose pure standard C (C99) without external libraries?
**Answer:** Standard C provides maximum transparency, portability, and zero-dependency compilation on any college lab machine. It allows the student to trace, explain, and debug every low-level memory layout, pointer movement, call stack frame, and data structure from first principles during viva voce.

#### Q4: How does the system handle adversarial or corrupt inputs?
**Answer:** All user and file inputs pass through defensive functions (`get_int_input`, `get_double_input`, `get_string_input`). File reading checks `fgets != NULL` and validates `sscanf` column counts. Array bounds are checked against `MAX_` constants, negative values are rejected, and EOF signals trigger graceful exits.

---

### Category 2: Sorting & Searching

#### Q5: What is the recurrence relation for Merge Sort, and how does the Master Theorem solve it?
**Answer:** $T(n) = 2T(n/2) + \Theta(n)$.  
Here $a=2, b=2, f(n) = \Theta(n^1)$. The critical exponent is $\log_b a = \log_2 2 = 1$. Since $f(n) = \Theta(n^1)$, Case 2 of the Master Theorem applies:
$$T(n) = \Theta(n^{\log_b a} \log n) = \Theta(n \log n)$$

#### Q6: Why can Quick Sort degrade to $O(n^2)$ time, and how does your implementation behave?
**Answer:** If the chosen pivot is always the smallest or largest element (e.g. Lomuto partition on an already sorted or reverse-sorted array), the problem size reduces by only 1 instead of halving ($T(n) = T(n-1) + \Theta(n) \implies O(n^2)$). Our benchmark explicitly verifies this adversarial degradation on pre-sorted arrays.

#### Q7: What is the difference between an in-place sort and a stable sort?
**Answer:**
- **In-place:** Uses $O(1)$ auxiliary memory beyond the input array (e.g. Bubble Sort, Quick Sort with $O(\log n)$ stack).
- **Stable:** Preserves the relative order of elements with equal keys. Merge Sort and Bubble Sort are stable; Quick Sort is unstable.

#### Q8: Why is Iterative Binary Search preferred over Recursive Binary Search in production?
**Answer:** Both achieve $O(\log n)$ time, but the iterative version requires $O(1)$ auxiliary space, whereas the recursive version consumes $O(\log n)$ stack frames, creating function call overhead and risking stack overflow on extremely large datasets.

---

### Category 3: Divide & Conquer

#### Q9: Explain the Maximum Subarray problem in the context of bus transit.
**Answer:** Hourly passenger demand fluctuates above and below average (e.g. $[-15, +30, +45, -20, +85]$). The maximum subarray identifies the contiguous peak surge interval of the day where passenger volume is greatest, allowing the planner to inject express feeder buses.

#### Q10: How does Divide & Conquer solve the Maximum Subarray problem in $O(n \log n)$?
**Answer:** The array is halved. The maximum subarray either lies entirely in the left half, entirely in the right half, or crosses the midpoint. The crossing subarray is found in linear $\Theta(n)$ time by finding max left sum from midpoint and max right sum. Recurrence: $T(n) = 2T(n/2) + \Theta(n) \implies O(n \log n)$.

#### Q11: How does Kadane's algorithm achieve $O(n)$ for Maximum Subarray?
**Answer:** Kadane uses dynamic programming: at each index $i$, the maximum subarray ending at $i$ is $\max(A[i], \text{max\_ending\_here} + A[i])$. It tracks the global maximum in a single pass with $O(1)$ space.

#### Q12: How does Fast Exponentiation achieve $O(\log e)$ time?
**Answer:** It uses repeated squaring: if exponent $e$ is even, $a^e = (a^{e/2})^2$; if odd, $a^e = a \cdot (a^{(e-1)/2})^2$. It halves the exponent at each step, reducing multiplications from $e$ to $\lfloor \log_2 e \rfloor$.

#### Q13: How is the Celebrity Problem solved in $O(n)$ time for a transit hub?
**Answer:** A central transit hub has incoming edges from all stops but no outgoing edges. Using two pointers $a$ and $b$: if $a \to b$, $a$ cannot be the hub (hub has no outgoing routes), so $a++$; if $a \not\to b$, $b$ cannot be the hub, so $b--$. After $n-1$ queries, exactly one candidate remains, verified with $2(n-1)$ checks. Total time: $\Theta(n)$ with $\le 3(n-1)$ questions.

#### Q14: Why does Strassen's matrix multiplication achieve $O(n^{2.807})$?
**Answer:** Classical matrix multiplication requires 8 recursive sub-matrix multiplications ($T(n) = 8T(n/2) + \Theta(n^2) \implies O(n^3)$). Strassen cleverly combines submatrices with 18 additions to compute the product using only 7 multiplications ($T(n) = 7T(n/2) + \Theta(n^2)$). By Master Theorem, $\log_2 7 \approx 2.80735 \implies O(n^{2.807})$.

---

### Category 4: Shortest Paths & Spanning Trees

#### Q15: Why does Dijkstra's algorithm fail on negative edge weights?
**Answer:** Dijkstra makes a greedy choice: once a vertex $u$ is extracted with minimum tentative distance, it is marked permanently visited. If a negative edge later reduces the path to an already visited vertex, Dijkstra cannot reconsider it, producing incorrect shortest paths.

#### Q16: How does Bellman-Ford handle negative edges and detect negative cycles?
**Answer:** It relaxes all $E$ edges $V-1$ times. In a graph without negative cycles, a simple shortest path has at most $V-1$ edges. If any edge can still be relaxed on the $V$-th pass, the graph must contain a reachable negative weight cycle.

#### Q17: What is the recurrence relation in Floyd-Warshall?
**Answer:**
$$D^{(k)}[i][j] = \min\left(D^{(k-1)}[i][j], D^{(k-1)}[i][k] + D^{(k-1)}[k][j]\right)$$
where $k$ is the intermediate vertex allowed on the path from $i$ to $j$. It computes all-pairs shortest paths in $\Theta(V^3)$ time and $\Theta(V^2)$ space.

#### Q18: What is the Cut Property in Minimum Spanning Trees?
**Answer:** For any cut $(S, V \setminus S)$ of a connected graph, the minimum weight edge crossing the cut belongs to every Minimum Spanning Tree of the graph. Prim's algorithm repeatedly picks the minimum cut-edge to grow the tree.

#### Q19: Explain Disjoint Set Union (Union-Find) with Path Compression and Union by Rank.
**Answer:** DSU maintains disjoint sets of vertices to detect cycles in Kruskal's algorithm. Union by rank keeps trees shallow by attaching the shorter tree under the taller root. Path compression flattens the tree during `find()`. Together they achieve near-constant amortized time: $O(\alpha(V))$ per operation, where $\alpha$ is the Inverse Ackermann function.

#### Q20: When should an engineer choose Prim's over Kruskal's algorithm?
**Answer:** Choose **Prim's** for dense graphs ($E \approx V^2$), where matrix-based Prim runs in $O(V^2)$ without edge sorting overhead. Choose **Kruskal's** for sparse graphs ($E \ll V^2$), where sorting $E$ edges takes $O(E \log E) \approx O(E \log V)$.

---

### Category 5: Scheduling & Combinatorial Optimization

#### Q21: What is the Greedy Choice Property in Activity Selection?
**Answer:** Sorting bus trips by earliest finish time ($\text{end}_i$) guarantees that the first compatible trip selected leaves the maximum remaining time available for subsequent trips. This ensures the maximum number of non-overlapping trips are scheduled.

#### Q22: What is Interval Partitioning, and what is its optimal guarantee?
**Answer:** Interval partitioning finds the minimum number of buses/depot bays needed to schedule all trips without overlap. By sorting by start time and assigning to the earliest available resource, the number of allocated buses equals the depth $d$ (maximum concurrent overlapping trips). It is provably optimal.

#### Q23: Why is 0/1 Knapsack termed "pseudo-polynomial"?
**Answer:** Dynamic Programming solves 0/1 Knapsack in $O(n \cdot W)$ time. $W$ is the numeric value of the budget. In computational complexity, input size is measured in bits ($b = \lceil \log_2 W \rceil$). Thus $W = 2^b$, meaning the runtime is exponential in the input bit length. It is polynomial only when $W$ is bounded by a polynomial in $n$.

#### Q24: How does Branch & Bound differ from Backtracking?
**Answer:** Both explore a state-space tree. Backtracking explores depth-first (DFS) with simple constraint pruning. Branch & Bound uses an explicit bounding function (e.g. fractional knapsack upper bound) to prune subtrees that cannot beat the current best solution, and can explore via FIFO (queue), LIFO (stack), or Least-Cost (priority queue).

#### Q25: Why does Least-Cost Branch & Bound (LCBB) explore fewer nodes than FIFO or LIFO?
**Answer:** LCBB is a Best-First search: it always expands the live node with the highest upper bound (greatest potential). By quickly finding near-optimal solutions, it raises the global best lower bound early, enabling aggressive pruning of all subsequent subtrees.

#### Q26: How does the N-Queens problem relate to bus depot management?
**Answer:** In an $N \times N$ depot parking/charging bay grid, placing $N$ buses such that no two share the same row, column, or diagonal ensures conflict-free bus positioning where no two vehicles block each other's entry, exit, or charging cables.

---

### Category 6: Dynamic Programming & TSP

#### Q27: How does Longest Common Subsequence (LCS) apply to transit routes?
**Answer:** Representing routes as sequences of stop IDs (e.g. Route A: $[1, 2, 4, 5, 7]$ vs Route B: $[2, 3, 4, 7, 8]$), LCS finds the longest shared corridor ($[2, 4, 7]$). This identifies optimal candidates for consolidated high-frequency bus rapid transit (BRT) lanes.

#### Q28: What is the optimal substructure in Matrix Chain Multiplication?
**Answer:** In parenthesizing matrices $A_i \dots A_j$, an optimal split at $k$ ($i \le k < j$) requires that $A_i \dots A_k$ and $A_{k+1} \dots A_j$ are each parenthesized optimally. Recurrence:
$$m[i][j] = \min_{i \le k < j} \left(m[i][k] + m[k+1][j] + p_{i-1} p_k p_j\right)$$

#### Q29: What is the time complexity difference between TSP Brute Force and Held-Karp Bitmask DP?
**Answer:**
- **Brute Force:** Evaluates all $(n-1)!$ permutations $\implies O(n!)$ time. For $n=12$, $11! \approx 39.9 \times 10^6$ operations.
- **Held-Karp Bitmask DP:** State $dp[\text{mask}][u]$ represents subset of visited stops ending at $u$. Time complexity: $O(n^2 2^n)$. For $n=12$, $12^2 \times 4096 \approx 5.9 \times 10^5$ operations (a 67x speedup!).

---

### Category 7: Network Flow & Matching

#### Q30: What is an augmenting path in network flow?
**Answer:** A simple directed path from source $s$ to sink $t$ in the residual graph along which all edges have residual capacity $c_f(u, v) = c(u, v) - f(u, v) > 0$. The flow can be increased by the bottleneck capacity of this path.

#### Q31: Why is Edmonds-Karp superior to standard Ford-Fulkerson?
**Answer:** Ford-Fulkerson uses arbitrary DFS to find augmenting paths, which can take $O(E \cdot \text{max\_flow})$ time and make tiny augmentations on large capacities. Edmonds-Karp uses BFS to always choose the shortest augmenting path (fewest edges), guaranteeing polynomial termination in $O(V \cdot E^2)$ regardless of capacities.

#### Q32: How is Maximum Bipartite Matching reduced to a Maximum Flow problem?
**Answer:** Add a supersource $s$ with capacity 1 edges to all left vertices (buses). Direct all bipartite compatibility edges from left to right with capacity 1. Add a supersink $t$ with capacity 1 edges from all right vertices (routes). The maximum $s$-$t$ flow equals the maximum bipartite matching cardinality.

---

### Category 8: String Matching & Compression

#### Q33: How does the Knuth-Morris-Pratt (KMP) algorithm achieve zero backtracking in the text?
**Answer:** KMP precomputes the Longest Proper Prefix which is also a Suffix (LPS array $\pi$) for the pattern in $O(m)$ time. When a mismatch occurs at pattern index $j$, KMP slides the pattern to $j = \pi[j-1]$ without rewinding the text pointer $i$, achieving strictly linear $\Theta(n)$ search time.

#### Q34: What is a spurious hit in the Rabin-Karp algorithm?
**Answer:** When the rolling hash of the current text window matches the pattern hash ($h(T) = h(P)$), but the actual substring characters do not match ($T[s..s+m-1] \ne P$). It occurs due to hash collisions modulo $q$. The algorithm verifies character-by-character to eliminate false positives.

#### Q35: How does a Finite Automaton string matcher work?
**Answer:** It builds a state transition table $\delta(q, a)$ where states $0 \dots m$ represent the length of the longest pattern prefix matched so far. Reading each text character takes exactly 1 state transition ($\Theta(1)$ per char, $\Theta(n)$ total).

#### Q36: Why is Huffman coding considered prefix-free and optimal?
**Answer:**
- **Prefix-free:** No character code is a prefix of any other code because every character resides strictly at a leaf node of the binary tree. This eliminates ambiguous delimiter tokens.
- **Optimal:** By greedily merging the two lowest-frequency trees in a Min-Heap, it minimizes the average weighted path length $\sum f_i \cdot \text{length}(c_i)$, approaching Shannon entropy.

#### Q37: How does the Chinese Remainder Theorem synchronize periodic bus headways?
**Answer:** If Route 1 arrives every $m_1$ minutes at offset $a_1$, and Route 2 arrives every $m_2$ minutes at offset $a_2$, CRT solves $x \equiv a_1 \pmod{m_1}$ and $x \equiv a_2 \pmod{m_2}$. Using the Extended Euclidean Algorithm to compute modular inverses, it determines the exact minute $x$ when both bus lines arrive simultaneously for timed passenger transfers.

---

### Category 9: Computational Complexity & Theory

#### Q38: Define the complexity classes P, NP, NP-Hard, and NP-Complete.
**Answer:**
- **P:** Problems solvable in polynomial time $O(n^k)$ by a deterministic Turing Machine.
- **NP:** Decision problems whose YES-instance certificate can be verified in polynomial time.
- **NP-Hard:** Problems at least as hard as any problem in NP; every problem in NP reduces to it in polynomial time.
- **NP-Complete:** Problems that are both in NP and NP-Hard (e.g. TSP, 0/1 Knapsack decision).

#### Q39: What is a Polynomial-Time Reduction ($A \le_p B$)?
**Answer:** An algorithm that transforms any instance of problem $A$ into an instance of problem $B$ in polynomial time, such that instance $A$ has answer YES if and only if instance $B$ has answer YES. It proves that problem $B$ is at least as hard as $A$.

#### Q40: How does your project prove that 0/1 Knapsack is NP-Complete?
**Answer:** We demonstrate a polynomial reduction from **Subset-Sum** to **0/1 Knapsack**: given a set of integers $S$ and target $T$, map each integer $s_i$ to a transit route with cost $w_i = s_i$ and passenger value $v_i = s_i$, with budget $W = T$ and demand target $P = T$. Because $w_i = v_i$, total cost equals total demand. A valid route plan exists iff a subset sums to $T$. Since Subset-Sum is NP-complete, 0/1 Knapsack is NP-complete.

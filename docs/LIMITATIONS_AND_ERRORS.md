# Limitations, Boundary Conditions & Error Handling Analysis
## Public Bus Route Frequency and Depot Planner (Project 12)

---

### 1. Architectural & Theoretical Limitations

1. **Fixed Memory Static Allocation Limits:**
   - Constants defined in `include/common.h`: `MAX_STOPS = 1200`, `MAX_GRAPH_STOPS = 100`, `MAX_ROADS = 3000`, `MAX_ROUTES = 200`, `MAX_BUSES = 300`, `MAX_DEPOTS = 50`, `MAX_TRIPS = 2000`.
   - *Rationale:* Standard C99 compatibility without dynamic realloc thrashing on resource-constrained college lab machines. Dense $V \times V$ matrices (e.g. Floyd-Warshall) are capped at $100 \times 100$ to prevent stack overflows on 32-bit Windows MinGW.
2. **0/1 Knapsack Integer Budget Assumption:**
   - The Dynamic Programming solution operates on integer budget values ($W$). Floating-point costs are scaled to integer rupees (`int cost = (int)routes[i].operating_cost`).
   - For continuous budgets, Branch & Bound LCBB or linear programming relaxation must be used.
3. **Traveling Salesperson Problem (TSP) Vertex Limit:**
   - Bounded TSP Brute Force is capped at $N=10$ stops ($9! = 362,880$ permutations) to ensure viva demonstration finishes in $< 1$ second.
   - Held-Karp Bitmask DP is bounded to $N=12$ stops ($12^2 \times 2^{12} = 589,824$ operations) to prevent exceeding standard memory allocation limits.
4. **Chinese Remainder Theorem Coprimality Condition:**
   - Standard CRT requires pairwise coprime moduli ($\gcd(m_i, m_j) = 1$). If transit headways share common divisors (e.g. 15 mins and 20 mins, $\gcd=5$), the system reports non-coprime error and prompts for relative prime headways (e.g. 11, 13, 17 mins).

---

### 2. Boundary & Edge Case Handling

| Edge Case Scenario | Potential Failure Mode | System Defensive Handling Strategy |
| :--- | :--- | :--- |
| **Empty CSV File** | Null pointer dereference / crash | Checked via `fgets()` returning NULL immediately; logs user-friendly warning and returns `0` loaded entities. |
| **Corrupted / Truncated CSV Row** | Misaligned struct attributes | `sscanf` return value verified ($== 3, 4, 6$). Incomplete rows are safely discarded with counter increment skipped. |
| **Target Stop Not in Network** | Infinite loop or segfault | Search algorithms return `-1` with a friendly "Stop ID not found" notification. |
| **Disconnected Road Graph** | Infinite distance loops | Distances initialized to `INF = 1,000,000,000`. Shortest path and MST algorithms check `dist < INF` before relaxing. |
| **Negative Weight Cycles** | Infinite loop in shortest paths | Dijkstra cannot be run on negative weights; Bellman-Ford performs $V$-th pass cycle detection and issues alert. |
| **All-Negative Subarray** | Sum initialized to 0 returns 0 | Kadane and D&C initialized to `arr[0]` or `-INF`, correctly returning the single largest negative element. |
| **Identical Elements in Quick Sort**| $O(n^2)$ recursion explosion | Lomuto partition handles equal elements gracefully; test suite includes adversarial all-equal key verification. |
| **Single Element Arrays ($N=1$)** | Out-of-bounds array access | Base case checks (`if (low >= high) return;`) handle single-element and empty arrays immediately. |
| **Division by Zero** | Hardware arithmetic trap | Modular inverse checks $\gcd(a, m) == 1$; percentage denominators check `original_bits > 0`. |
| **EOF / Stdin Redirection** | Infinite loop in input prompts | `fgets == NULL` detected; system safely returns default `0` to exit menus gracefully without hanging. |

---

### 3. Error Handling Implementation Summary

All interactive input collection is routed through defensive helper functions in `src/models.c`:
- `get_int_input(prompt, min, max)`: Re-prompts repeatedly on non-numeric characters or out-of-range values.
- `get_double_input(prompt, min, max)`: Validates double floating-point inputs within bounded domain ranges.
- `get_string_input(prompt, dest, max_size)`: Automatically strips trailing newlines (`\r\n`) and rejects blank strings.

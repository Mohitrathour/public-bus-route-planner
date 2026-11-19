/**
 * @file theory_notes.c
 * @brief Interactive Academic Theory Notes, Recurrences, and P vs NP for Viva Voce.
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/theory_notes.h"
#include "../include/common.h"

void show_recurrences_and_master_theorem(void) {
    print_banner("THEORY: MASTER THEOREM & RECURRENCE RELATIONS");
    printf("  MASTER THEOREM FORMULA:\n");
    printf("    T(n) = a * T(n/b) + f(n), where a >= 1, b > 1, f(n) = Theta(n^d)\n\n");
    printf("    Critical Threshold: c_crit = log_b(a)\n");
    printf("    - Case 1: If f(n) = O(n^(log_b(a) - eps))  => T(n) = Theta(n^(log_b(a)))   [Leaf heavy]\n");
    printf("    - Case 2: If f(n) = Theta(n^(log_b(a)))    => T(n) = Theta(n^(log_b(a)) * log n) [Balanced]\n");
    printf("    - Case 3: If f(n) = Omega(n^(log_b(a) + eps)) => T(n) = Theta(f(n))        [Root heavy]\n\n");

    print_separator();
    printf("  DIVIDE & CONQUER RECURRENCES IN THIS PROJECT:\n\n");
    printf("  1. Merge Sort:\n");
    printf("     T(n) = 2T(n/2) + Theta(n)\n");
    printf("     a=2, b=2 -> log_2(2) = 1. f(n) = Theta(n^1). Case 2 applies!\n");
    printf("     Solution: T(n) = Theta(n log n)\n\n");

    printf("  2. Binary Search (Recursive):\n");
    printf("     T(n) = T(n/2) + Theta(1)\n");
    printf("     a=1, b=2 -> log_2(1) = 0. f(n) = Theta(n^0 = 1). Case 2 applies!\n");
    printf("     Solution: T(n) = Theta(log n)\n\n");

    printf("  3. Maximum Subarray (Crossing Midpoint):\n");
    printf("     T(n) = 2T(n/2) + Theta(n)\n");
    printf("     a=2, b=2 -> log_2(2) = 1. Crossing step takes linear O(n) scan. Case 2!\n");
    printf("     Solution: T(n) = Theta(n log n)\n\n");

    printf("  4. Fast Exponentiation (Repeated Squaring):\n");
    printf("     T(n) = T(n/2) + Theta(1)\n");
    printf("     Halves exponent in 1 multiply per level -> T(n) = Theta(log n)\n\n");

    printf("  5. Strassen's Matrix Multiplication:\n");
    printf("     T(n) = 7T(n/2) + Theta(n^2)\n");
    printf("     a=7, b=2 -> log_2(7) ~ 2.80735. f(n) = Theta(n^2). log_2(7) > 2 -> Case 1!\n");
    printf("     Solution: T(n) = Theta(n^2.807) (vs Classical O(n^3))\n");
    print_separator();
}

void show_complexity_reference_table(void) {
    print_banner("ALGORITHM COMPLEXITY REFERENCE GUIDE (VIVA CHEAT-SHEET)");
    printf("  +-----------------------------+--------------+--------------+--------------+------------+\n");
    printf("  | Algorithm                   | Best Case    | Average Case | Worst Case   | Space      |\n");
    printf("  +-----------------------------+--------------+--------------+--------------+------------+\n");
    printf("  | Bubble Sort                 | Omega(n)     | Theta(n^2)   | O(n^2)       | O(1)       |\n");
    printf("  | Merge Sort                  | Omega(n logn)| Theta(n logn)| O(n log n)   | O(n)       |\n");
    printf("  | Quick Sort                  | Omega(n logn)| Theta(n logn)| O(n^2)       | O(log n)   |\n");
    printf("  | Linear Search               | Omega(1)     | Theta(n)     | O(n)         | O(1)       |\n");
    printf("  | Binary Search               | Omega(1)     | Theta(log n) | O(log n)     | O(1)       |\n");
    printf("  | Max Subarray (D&C)          | Omega(n logn)| Theta(n logn)| O(n log n)   | O(log n)   |\n");
    printf("  | Fast Exponentiation         | Omega(log e) | Theta(log e) | O(log e)     | O(1)       |\n");
    printf("  | Celebrity Problem (D&C)     | Omega(n)     | Theta(n)     | O(n)         | O(1)       |\n");
    printf("  | Strassen Matrix Mult        | Omega(n^2.8) | Theta(n^2.8) | O(n^2.807)   | O(n^2)     |\n");
    printf("  | Dijkstra (Min-Array)        | Omega(V^2)   | Theta(V^2)   | O(V^2)       | O(V)       |\n");
    printf("  | Bellman-Ford                | Omega(E)     | Theta(V * E) | O(V * E)     | O(V)       |\n");
    printf("  | Floyd-Warshall              | Omega(V^3)   | Theta(V^3)   | O(V^3)       | O(V^2)     |\n");
    printf("  | Prim's MST (Matrix)         | Omega(V^2)   | Theta(V^2)   | O(V^2)       | O(V)       |\n");
    printf("  | Kruskal's MST (DSU)         | Omega(E logE)| Theta(E logE)| O(E log E)   | O(V + E)   |\n");
    printf("  | Activity Selection (Greedy) | Omega(n logn)| Theta(n logn)| O(n log n)   | O(1)       |\n");
    printf("  | Interval Partitioning       | Omega(n logn)| Theta(n logn)| O(n log n)   | O(depth)   |\n");
    printf("  | 0/1 Knapsack DP             | Omega(n * W) | Theta(n * W) | O(n * W)     | O(n * W)   |\n");
    printf("  | LCS Dynamic Programming     | Omega(n * m) | Theta(n * m) | O(n * m)     | O(n * m)   |\n");
    printf("  | Matrix Chain Mult (DP)      | Omega(n^3)   | Theta(n^3)   | O(n^3)       | O(n^2)     |\n");
    printf("  | TSP Bitmask DP (Held-Karp)  | Omega(n^2 2^n)| Theta(n^2 2^n)| O(n^2 2^n) | O(n 2^n)   |\n");
    printf("  | Edmonds-Karp Max Flow       | Omega(V * E) | Theta(V * E^2)| O(V * E^2)  | O(V^2)     |\n");
    printf("  | KMP String Search           | Omega(n + m) | Theta(n + m) | O(n + m)     | O(m)       |\n");
    printf("  | Huffman Coding              | Omega(k logk)| Theta(k logk)| O(k log k)   | O(k)       |\n");
    printf("  +-----------------------------+--------------+--------------+--------------+------------+\n");
}

void show_p_vs_np_analysis(void) {
    print_banner("P vs NP vs NP-COMPLETE & NP-HARD CLASSIFICATION");
    printf("  1. CLASS P (Deterministic Polynomial Time):\n");
    printf("     - Problems solvable by a deterministic algorithm in O(n^k) polynomial time.\n");
    printf("     - Examples in this project: Shortest Path (Dijkstra), MST (Kruskal), Max Flow (Edmonds-Karp),\n");
    printf("       Activity Selection, Interval Partitioning, KMP String Matching, Sorting.\n\n");

    printf("  2. CLASS NP (Nondeterministic Polynomial Time):\n");
    printf("     - Decision problems whose proposed solution can be VERIFIED in polynomial time.\n");
    printf("     - Verification: Given a certificate (e.g. subset of routes), can check cost <= W in O(n).\n\n");

    printf("  3. CLASS NP-HARD:\n");
    printf("     - Problems at least as hard as the hardest problems in NP.\n");
    printf("     - Every problem in NP can be polynomial-time reduced (Cook-Karp reduction) to it.\n");
    printf("     - Does NOT need to be in NP itself (can be optimization problem).\n\n");

    printf("  4. CLASS NP-COMPLETE:\n");
    printf("     - The intersection of NP and NP-Hard: (1) Problem is in NP, AND (2) Problem is NP-Hard.\n");
    printf("     - In this project:\n");
    printf("       * Traveling Salesperson Decision Problem (Is there a feeder tour <= L km?)\n");
    printf("       * 0/1 Knapsack Decision Problem (Can we serve >= P passengers with budget <= W?)\n");
    printf("       * Subset Sum Problem (Can an exact subset of route costs sum to target B?)\n\n");

    printf("  5. PSEUDO-POLYNOMIAL TIME:\n");
    printf("     - 0/1 Knapsack DP runs in O(n * W). W is the numeric value of budget, represented in\n");
    printf("       log_2(W) bits. The runtime is exponential in the input BIT LENGTH (2^b), hence\n");
    printf("       0/1 Knapsack is WEAKLY NP-complete, solvable pseudo-polynomially by Dynamic Programming.\n");
    print_separator();
}

void show_polynomial_reduction_example(void) {
    print_banner("POLYNOMIAL-TIME REDUCTION: SUBSET-SUM -> 0/1 KNAPSACK");
    printf("  DEFINITION OF REDUCTION (A <=_p B):\n");
    printf("    Problem A reduces to B in polynomial time if any instance of A can be transformed\n");
    printf("    into an instance of B in polynomial time, such that A has answer YES iff B has answer YES.\n\n");

    printf("  REDUCTION WALKTHROUGH:\n");
    printf("    Input to SUBSET-SUM: A set of integers S = {s_1, s_2, ..., s_n} and target integer T.\n");
    printf("    Question: Does there exist a subset S' subset S such that sum(s_i in S') = T?\n\n");

    printf("    CONSTRUCTION TO 0/1 KNAPSACK (Transit Route Mapping):\n");
    printf("    For each integer s_i in S, create a transit route corridor i with:\n");
    printf("      - Operating Cost  w_i = s_i\n");
    printf("      - Passenger Value v_i = s_i\n");
    printf("    Set Knapsack Budget Capacity W = T.\n");
    printf("    Set Target Passenger Demand  P = T.\n\n");

    printf("    CORRECTNESS PROOF:\n");
    printf("    (=>) Suppose Subset-Sum has solution S' with sum = T.\n");
    printf("         Then operating the corresponding routes gives:\n");
    printf("           Total Cost   = sum_{i in S'} w_i = sum_{i in S'} s_i = T <= W  (Valid budget)\n");
    printf("           Total Demand = sum_{i in S'} v_i = sum_{i in S'} s_i = T >= P  (Satisfies demand)\n");
    printf("         Therefore, 0/1 Knapsack returns YES.\n\n");

    printf("    (<=) Suppose 0/1 Knapsack has solution with Total Cost <= W and Total Demand >= P.\n");
    printf("         Since w_i = v_i = s_i for all items, Total Cost = Total Demand.\n");
    printf("         Hence: T <= Total Demand = Total Cost <= T  ==> Total Cost must EXACTLY equal T!\n");
    printf("         Therefore, the corresponding subset of numbers sums to T.\n\n");

    printf("    Complexity of transformation: O(n) linear time.\n");
    printf("    Conclusion: Subset-Sum <=_p 0/1 Knapsack. Proves 0/1 Knapsack is NP-Complete!\n");
    print_separator();
}

void run_theory_notes_menu(void) {
    int choice = -1;
    while (choice != 0) {
        print_banner("MENU: ACADEMIC THEORY & VIVA NOTES");
        printf("  1. Master Theorem & D&C Recurrence Solutions\n");
        printf("  2. Algorithm Complexity Table (Best/Avg/Worst & Space)\n");
        printf("  3. P vs NP vs NP-Complete Classifications\n");
        printf("  4. Polynomial-Time Reduction (Subset-Sum -> 0/1 Knapsack)\n");
        printf("  0. Back to Data & Tests Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 4);
        if (choice == 0) break;

        switch (choice) {
            case 1: show_recurrences_and_master_theorem(); break;
            case 2: show_complexity_reference_table(); break;
            case 3: show_p_vs_np_analysis(); break;
            case 4: show_polynomial_reduction_example(); break;
        }

        press_enter_to_continue();
    }
}

# Problem Statement & Requirements Specification
## Public Bus Route Frequency and Depot Planner
**Project ID:** 12  
**Course:** Design and Analysis of Algorithms (CSEG3053)  
**Target Level:** B.Tech Computer Science & Engineering  

---

### 1. Executive Summary & Domain Context
Urban public transit networks face acute operational challenges: fluctuating passenger travel demand, congested road corridors, fleet vehicle and charging slot constraints, and stringent operational budgets. 

The **Public Bus Route Frequency and Depot Planner** is an integrated algorithm-driven transit planning suite built entirely in pure C (C99). It automates the synthesis of optimal bus transit service plans, minimizing passenger wait times, maximizing passenger ridership, and reducing operational expenses under hard real-world constraints.

---

### 2. Target Users
1. **Municipal Transit Authorities (e.g. UTC, DTC, BEST):** For long-term capital planning, determining optimal fleet acquisition numbers and depot bay capacities.
2. **Depot Operations Managers:** For daily scheduling, bus-to-route assignment, conflict-free maintenance bay parking, and real-time emergency disruption rerouting.
3. **Academic Evaluators & DAA Viva Examiners:** To inspect, trace, benchmark, and mathematically verify fundamental algorithmic paradigms (Divide & Conquer, Greedy, Dynamic Programming, Branch & Bound, Network Flows, String Matching) implemented on a tangible engineering domain.

---

### 3. System Inputs
1. **Bus Stops (`data/stops.csv`):**
   - Identifier (`id`: Integer, $1 \dots 9999$)
   - Station Name (`name`: Alphanumeric, up to 64 chars)
   - Passenger Demand per hour (`passengers`: Non-negative integer)
2. **Road Infrastructure (`data/roads.csv`):**
   - Origin Stop (`from`: Integer stop ID)
   - Destination Stop (`to`: Integer stop ID)
   - Physical Distance (`distance`: Integer kilometers)
   - Transit Travel Time (`travel_time`: Minutes under normal traffic)
   - Flow Capacity (`capacity`: Maximum passengers per hour)
3. **Bus Fleet (`data/buses.csv`):**
   - Bus Identifier (`bus_id`: Integer)
   - Passenger Capacity (`capacity`: Seats + standing, e.g. 35 to 60)
   - Home Depot Garage (`depot_id`: Integer)
4. **Depot Garages (`data/depots.csv`):**
   - Depot Identifier (`depot_id`: Integer)
   - Terminal Name (`name`: Alphanumeric)
   - Bay Capacity (`capacity`: Total simultaneous bus parking/maintenance bays)
   - Available Vehicles (`available_buses`: Ready for dispatch)
5. **Operating Corridors (`data/routes.csv`):**
   - Route ID (`route_id`: Integer)
   - Name (`name`: Corridor description)
   - Distance (`distance_km`: Float km)
   - Travel Time (`travel_time_min`: Integer minutes)
   - Operating Cost (`operating_cost`: Indian Rupees per shift)
   - Expected Passenger Demand (`passengers`: Hourly passenger volume)
6. **Timetable Trips (`data/trips.csv`):**
   - Scheduled Trip ID, Route ID, Start Time (minutes from midnight), End Time, Demand.

---

### 4. System Outputs
1. **Interactive Console Trace Tables:** Step-by-step traces for all 28+ algorithms showing intermediate partition states, DP matrix transitions, and bounding decisions.
2. **Algorithm Comparison Matrices:** On-screen benchmark tables for every module comparing execution runtime (`clock()`), operation counts (comparisons, assignments, iterations), memory footprint, and solution optimality.
3. **Emergency Rerouting Plans:** Outputting alternative non-intersecting road detours and impact reports under sudden constraint shock scenarios.
4. **Standardized CSV Exports:**
   - `results/benchmark.csv`: Input scaling ($N=100 \dots 10000$) execution data for external plotting.
   - `results/route_plan.csv`: Selected route corridors and timetables.

---

### 5. Constraints
1. **Fleet Constraint:** The number of simultaneous buses active on roads cannot exceed available depot vehicles:
   $$\sum_{i} \text{ActiveBuses}(i) \le \text{TotalFleetSize}$$
2. **Depot Capacity Constraint:** Buses resting between shifts cannot exceed garage parking slots.
3. **Budgetary Constraint:** The total operational expenditure of selected routes cannot exceed municipal budget $W$:
   $$\sum_{i \in \text{Selected}} \text{Cost}_i \le W$$
4. **Time Window Non-Overlapping Constraint:** A single bus cannot service overlapping trip intervals:
   $$\text{Trip}_a.\text{endTime} \le \text{Trip}_b.\text{startTime} \quad \forall a, b \text{ assigned to the same bus}$$
5. **Architectural & Language Constraint:** Standard C (C99) using only `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<time.h>`, `<limits.h>`. No external libraries, no C++, no STL.

---

### 6. Success & Evaluation Criteria
1. **100% Algorithmic Correctness:** All sorting, shortest path, MST, DP, branch-and-bound, flow, and string matching algorithms pass strict automated assertions.
2. **Traceability:** Every algorithm provides an educational step-by-step trace mode for viva voce interrogation.
3. **Graceful Error Handling:** Strict input validation preventing negative inputs, division by zero, buffer overflows, and corrupted files.
4. **Scalability:** Proven capability to handle metropolitan networks ($1000+$ stops/trips) under sub-second execution times.

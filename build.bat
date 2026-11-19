@echo off
echo ========================================================
echo Building Public Bus Route Frequency and Depot Planner...
echo ========================================================

REM Create results directory if it doesn't exist
if not exist results mkdir results

REM Compile Main Application
gcc -std=c99 -Wall -Wextra -O2 -Iinclude ^
    src/main.c ^
    src/models.c ^
    src/sorting.c ^
    src/searching.c ^
    src/divide_conquer.c ^
    src/shortest_path.c ^
    src/spanning_tree.c ^
    src/scheduling.c ^
    src/route_selection.c ^
    src/dp_tsp.c ^
    src/flow_matching.c ^
    src/string_matching.c ^
    src/compression_sync.c ^
    src/theory_notes.c ^
    src/scenarios.c ^
    tests/test_main.c ^
    -o bus_planner.exe -lm

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation of bus_planner.exe failed!
    exit /b %ERRORLEVEL%
)

echo [+] bus_planner.exe compiled successfully!

REM Compile Standalone Test Suite
gcc -std=c99 -Wall -Wextra -O2 -Iinclude -DTEST_STANDALONE_RUNNER ^
    tests/test_main.c ^
    src/models.c ^
    src/sorting.c ^
    src/searching.c ^
    src/divide_conquer.c ^
    src/shortest_path.c ^
    src/spanning_tree.c ^
    src/scheduling.c ^
    src/route_selection.c ^
    src/dp_tsp.c ^
    src/flow_matching.c ^
    src/string_matching.c ^
    src/compression_sync.c ^
    -o test_runner.exe -lm

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation of test_runner.exe failed!
    exit /b %ERRORLEVEL%
)

echo [+] test_runner.exe compiled successfully!
echo ========================================================
echo Build complete. Run with: bus_planner.exe or test_runner.exe
echo ========================================================

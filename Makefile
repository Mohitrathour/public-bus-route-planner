# Makefile for Public Bus Route Frequency and Depot Planner (Project 12)
# CSEG3053 - Design and Analysis of Algorithms

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lm

SRC = src/main.c \
      src/models.c \
      src/sorting.c \
      src/searching.c \
      src/divide_conquer.c \
      src/shortest_path.c \
      src/spanning_tree.c \
      src/scheduling.c \
      src/route_selection.c \
      src/dp_tsp.c \
      src/flow_matching.c \
      src/string_matching.c \
      src/compression_sync.c \
      src/theory_notes.c \
      src/scenarios.c \
      tests/test_main.c

TEST_SRC = tests/test_main.c \
           src/models.c \
           src/sorting.c \
           src/searching.c \
           src/divide_conquer.c \
           src/shortest_path.c \
           src/spanning_tree.c \
           src/scheduling.c \
           src/route_selection.c \
           src/dp_tsp.c \
           src/flow_matching.c \
           src/string_matching.c \
           src/compression_sync.c

TARGET = bus_planner.exe
TEST_TARGET = test_runner.exe

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) -DTEST_STANDALONE_RUNNER $(TEST_SRC) -o $(TEST_TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

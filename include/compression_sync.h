/**
 * @file compression_sync.h
 * @brief Compression and Periodic Schedule Synchronization:
 *        1. Huffman Coding (Telemetry and Stop Code Compression)
 *        2. Fixed-Length Coding
 *        3. Chinese Remainder Theorem (Synchronize multi-route bus cycles)
 */

#ifndef COMPRESSION_SYNC_H
#define COMPRESSION_SYNC_H

#include "models.h"

/* Huffman Tree Node */
typedef struct HuffmanNode {
    char data;
    unsigned freq;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

/* Huffman Code Table Entry */
typedef struct {
    char ch;
    char code[32];
    int freq;
} HuffmanCode;

typedef struct {
    HuffmanCode table[256];
    int unique_chars;
    int original_bits;
    int compressed_bits;
    double compression_ratio;
    double entropy;
} CompressionResult;

/* Compression Algorithms */
CompressionResult huffman_compress(const char *text, Metrics *m, int verbose);
CompressionResult fixed_length_encode(const char *text, Metrics *m);

/* Chinese Remainder Theorem */
typedef struct {
    int count;
    int remainders[10];
    int moduli[10];
    long long synchronized_minute;
} CRTResult;

CRTResult solve_crt_bus_schedules(const int a[], const int m_arr[], int k, Metrics *m, int verbose);

/* Submenu and Comparison */
void run_compression_sync_menu(TransitSystem *sys);
void compare_compression_methods(const char *text);

#endif /* COMPRESSION_SYNC_H */

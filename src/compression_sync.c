/**
 * @file compression_sync.c
 * @brief Compression and Periodic Schedule Synchronization:
 *        1. Huffman Coding (Telemetry and Stop Code Compression)
 *        2. Fixed-Length Coding
 *        3. Chinese Remainder Theorem (Bus Headway Alignment)
 *
 * Designed for B.Tech DAA Course (CSEG3053).
 */

#include "../include/compression_sync.h"
#include <math.h>

/* ========================================================================= */
/*                          1. HUFFMAN CODING                                */
/* ========================================================================= */

/**
 * GREEDY HUFFMAN ALGORITHM:
 *   Builds optimal prefix-free binary tree using min-priority queue (Min-Heap).
 *
 * COMPLEXITY:
 *   Time Complexity : O(k log k) where k is number of unique characters
 *   Space Complexity: O(k) for binary tree
 *
 * DOMAIN USE:
 *   Compress high-volume GPS telemetry packets and digital stop codes
 *   before transmitting over low-bandwidth cellular IoT modems.
 */
typedef struct {
    int size;
    int capacity;
    HuffmanNode **array;
} MinHeap;

static HuffmanNode *new_node(char data, unsigned freq) {
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static MinHeap *create_min_heap(int capacity) {
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode **)malloc(capacity * sizeof(HuffmanNode *));
    return minHeap;
}

static void min_heapify(MinHeap *minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        HuffmanNode *t = minHeap->array[smallest];
        minHeap->array[smallest] = minHeap->array[idx];
        minHeap->array[idx] = t;
        min_heapify(minHeap, smallest);
    }
}

static HuffmanNode *extract_min_node(MinHeap *minHeap) {
    HuffmanNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    min_heapify(minHeap, 0);
    return temp;
}

static void insert_min_heap(MinHeap *minHeap, HuffmanNode *node) {
    minHeap->size++;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

static void store_codes(HuffmanNode *root, int arr[], int top, CompressionResult *res) {
    if (root->left) {
        arr[top] = 0;
        store_codes(root->left, arr, top + 1, res);
    }
    if (root->right) {
        arr[top] = 1;
        store_codes(root->right, arr, top + 1, res);
    }
    if (!root->left && !root->right) {
        int idx = res->unique_chars++;
        res->table[idx].ch = root->data;
        res->table[idx].freq = (int)root->freq;
        for (int i = 0; i < top; i++) {
            res->table[idx].code[i] = (char)('0' + arr[i]);
        }
        res->table[idx].code[top] = '\0';
    }
}

static void free_huffman_tree(HuffmanNode *root) {
    if (!root) return;
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}

CompressionResult huffman_compress(const char *text, Metrics *m, int verbose) {
    CompressionResult res;
    res.unique_chars = 0;
    res.original_bits = 0;
    res.compressed_bits = 0;
    res.compression_ratio = 0.0;
    res.entropy = 0.0;

    int freq[256] = {0};
    int len = (int)strlen(text);
    for (int i = 0; i < len; i++) {
        freq[(unsigned char)text[i]]++;
    }

    int distinct = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) distinct++;
    }

    if (distinct == 0) return res;

    MinHeap *minHeap = create_min_heap(distinct);
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            insert_min_heap(minHeap, new_node((char)i, freq[i]));
        }
    }

    /* Build Huffman tree */
    while (minHeap->size > 1) {
        if (m) {
            m->iterations++;
            m->assignments += 2;
        }
        HuffmanNode *left = extract_min_node(minHeap);
        HuffmanNode *right = extract_min_node(minHeap);
        HuffmanNode *top = new_node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insert_min_heap(minHeap, top);
    }

    HuffmanNode *root = extract_min_node(minHeap);

    int code_buffer[64];
    store_codes(root, code_buffer, 0, &res);

    /* Compute bits and entropy */
    res.original_bits = len * 8;
    for (int i = 0; i < res.unique_chars; i++) {
        res.compressed_bits += res.table[i].freq * (int)strlen(res.table[i].code);
        double p = (double)res.table[i].freq / len;
        /* Shannon entropy approx */
        if (p > 0.0) {
            double lg2 = (double)(p > 0 ? (log(p) / log(2.0)) : 0.0);
            res.entropy -= p * lg2;
        }
    }

    res.compression_ratio = 100.0 * (1.0 - (double)res.compressed_bits / res.original_bits);

    if (verbose) {
        printf("    [Huffman Codebook Generated]:\n");
        printf("    +-------+------+------------+------------------+\n");
        printf("    | Char  | Freq | Bit Length | Prefix Code      |\n");
        printf("    +-------+------+------------+------------------+\n");
        for (int i = 0; i < res.unique_chars && i < 15; i++) {
            char display_ch = res.table[i].ch;
            if (display_ch == ' ') display_ch = '_';
            else if (display_ch == '\n') display_ch = '\\';
            printf("    | %-5c | %-4d | %-10zu | %-16s |\n",
                   display_ch, res.table[i].freq, strlen(res.table[i].code), res.table[i].code);
        }
        printf("    +-------+------+------------+------------------+\n");
    }

    free(minHeap->array);
    free(minHeap);
    free_huffman_tree(root);

    return res;
}

/* ========================================================================= */
/*                          2. FIXED-LENGTH CODING                           */
/* ========================================================================= */

CompressionResult fixed_length_encode(const char *text, Metrics *m) {
    (void)m;
    CompressionResult res;
    res.unique_chars = 0;
    int freq[256] = {0};
    int len = (int)strlen(text);
    for (int i = 0; i < len; i++) freq[(unsigned char)text[i]]++;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) res.unique_chars++;
    }

    /* Calculate minimum bits needed: ceil(log2(unique_chars)) */
    int bits_per_char = 1;
    while ((1 << bits_per_char) < res.unique_chars) bits_per_char++;

    res.original_bits = len * 8;
    res.compressed_bits = len * bits_per_char;
    res.compression_ratio = 100.0 * (1.0 - (double)res.compressed_bits / res.original_bits);
    return res;
}

/* ========================================================================= */
/*                     3. CHINESE REMAINDER THEOREM (CRT)                    */
/* ========================================================================= */

/**
 * PERIODIC HEADWAY SYNCHRONIZATION:
 *   Route 1 arrives at minute: x = a1 (mod m1)  (e.g., every 15 mins starting at min 3)
 *   Route 2 arrives at minute: x = a2 (mod m2)  (e.g., every 20 mins starting at min 7)
 *   Route 3 arrives at minute: x = a3 (mod m3)  (e.g., every 25 mins starting at min 12)
 *
 * Find smallest positive x representing the exact time all buses align at central hub.
 *
 * EXTENDED EUCLIDEAN ALGORITHM:
 *   Computes modular inverse M_i^(-1) mod m_i.
 */
static long long ext_gcd(long long a, long long b, long long *x, long long *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long x1, y1;
    long long gcd = ext_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return gcd;
}

static long long mod_inverse(long long a, long long m) {
    long long x, y;
    long long g = ext_gcd(a, m, &x, &y);
    if (g != 1) return -1;
    return (x % m + m) % m;
}

CRTResult solve_crt_bus_schedules(const int a[], const int m_arr[], int k, Metrics *m, int verbose) {
    CRTResult res;
    res.count = k;
    res.synchronized_minute = -1;

    long long M = 1;
    for (int i = 0; i < k; i++) {
        res.remainders[i] = a[i];
        res.moduli[i] = m_arr[i];
        M *= m_arr[i];
    }

    long long x = 0;
    for (int i = 0; i < k; i++) {
        if (m) {
            m->iterations++;
            m->assignments++;
        }
        long long M_i = M / m_arr[i];
        long long y_i = mod_inverse(M_i, m_arr[i]);

        if (y_i == -1) {
            if (verbose) printf("    [CRT Error] Moduli m[%d]=%d are not pairwise coprime!\n", i, m_arr[i]);
            return res;
        }

        x = (x + a[i] * M_i * y_i) % M;

        if (verbose) {
            printf("    [CRT Stage %d] a_%d=%d, m_%d=%d, M_%d=%lld, Inverse y_%d=%lld\n",
                   i + 1, i + 1, a[i], i + 1, m_arr[i], i + 1, M_i, i + 1, y_i);
        }
    }

    res.synchronized_minute = (x % M + M) % M;

    if (verbose) {
        printf("    [CRT Solution] Periodic alignment every %lld minutes.\n", M);
        printf("    First Simultaneous Transfer Window: Minute %lld (e.g. %02lld:%02lld)\n",
               res.synchronized_minute, res.synchronized_minute / 60, res.synchronized_minute % 60);
    }

    return res;
}

/* ========================================================================= */
/*                          COMPARISON & SUBMENU                             */
/* ========================================================================= */

void compare_compression_methods(const char *text) {
    Metrics m_huff, m_fixed;
    RESET_METRICS(m_huff);
    RESET_METRICS(m_fixed);

    clock_t t0 = timer_start();
    CompressionResult res_huff = huffman_compress(text, &m_huff, 0);
    m_huff.time_ms = timer_stop_ms(t0);

    clock_t t1 = timer_start();
    CompressionResult res_fixed = fixed_length_encode(text, &m_fixed);
    m_fixed.time_ms = timer_stop_ms(t1);

    print_banner("COMPRESSION METHODS COMPARISON (HUFFMAN VS FIXED-LENGTH)");
    printf("  Corpus Size: %zu characters | Unique Symbols: %d\n\n", strlen(text), res_huff.unique_chars);

    printf("  +---------------------+-----------+---------------+-----------------+------------------+-----------------+\n");
    printf("  | Encoding Scheme     | Time (ms) | Original Bits | Compressed Bits | Space Saved (%%)  | Shannon Entropy |\n");
    printf("  +---------------------+-----------+---------------+-----------------+------------------+-----------------+\n");
    printf("  | Fixed-Length        | %-9.4f | %-13d | %-15d | %-15.2f%% | %-15.3f |\n",
           m_fixed.time_ms, res_fixed.original_bits, res_fixed.compressed_bits,
           res_fixed.compression_ratio, res_huff.entropy);
    printf("  | Huffman (Variable)  | %-9.4f | %-13d | %-15d | %-15.2f%% | %-15.3f |\n",
           m_huff.time_ms, res_huff.original_bits, res_huff.compressed_bits,
           res_huff.compression_ratio, res_huff.entropy);
    printf("  +---------------------+-----------+---------------+-----------------+------------------+-----------------+\n");

    printf("\n  [VERDICT & BEST CHOICE]:\n");
    printf("  >> Huffman coding achieves superior compression (%.2f%% savings) by assigning shorter bit strings to frequent characters!\n",
           res_huff.compression_ratio);
}

void run_compression_sync_menu(TransitSystem *sys) {
    (void)sys;
    int choice = -1;
    const char *telemetry_sample =
        "STOP_ISBT_ARR:101;STOP_CLOCK_DEP:102;STOP_BALLUPUR_ARR:103;PASSENGER_SURGE:150;"
        "BATTERY_VOLT:380V;DELAY_SEC:0;DISPATCH_ACK:OK;STATUS:NOMINAL;BUS_101_ONLINE;";

    while (choice != 0) {
        print_banner("MENU 11: COMPRESSION & SCHEDULE SYNCHRONIZATION");
        printf("  1. Huffman Coding (Telemetry & Code Compression Trace)\n");
        printf("  2. Fixed-Length Coding (Baseline Bit Rate Trace)\n");
        printf("  3. Chinese Remainder Theorem (Bus Headway Sync Trace)\n");
        printf("  4. Compare Huffman vs Fixed-Length (Bits & Savings Table)\n");
        printf("  0. Back to Main Menu\n");
        print_separator();

        choice = get_int_input("Select an option", 0, 4);
        if (choice == 0) break;

        Metrics m;
        RESET_METRICS(m);

        switch (choice) {
            case 1: {
                print_banner("1. HUFFMAN VARIABLE-LENGTH CODING TRACE");
                printf("  Telemetry Corpus: \"%s\"\n\n", telemetry_sample);
                CompressionResult res = huffman_compress(telemetry_sample, &m, 1);
                printf("\n  >> Original: %d bits | Compressed: %d bits\n", res.original_bits, res.compressed_bits);
                printf("  >> Compression Ratio: %.2f%% space reduction\n", res.compression_ratio);
                break;
            }
            case 2: {
                print_banner("2. FIXED-LENGTH CODING BASELINE");
                CompressionResult res = fixed_length_encode(telemetry_sample, &m);
                printf("\n  >> Original (8-bit ASCII) : %d bits\n", res.original_bits);
                printf("  >> Fixed-Length Encoded   : %d bits\n", res.compressed_bits);
                printf("  >> Space Reduction        : %.2f%%\n", res.compression_ratio);
                break;
            }
            case 3: {
                print_banner("3. CHINESE REMAINDER THEOREM: MULTI-ROUTE SYNCHRONIZATION");
                printf("  Synchronizing 3 feeder lines arriving at transfer terminal:\n");
                printf("    Line 101: Headway 11 min, offset 3 min (x = 3 mod 11)\n");
                printf("    Line 102: Headway 13 min, offset 5 min (x = 5 mod 13)\n");
                printf("    Line 103: Headway 17 min, offset 7 min (x = 7 mod 17)\n\n");
                int a[3] = {3, 5, 7};
                int mod[3] = {11, 13, 17};
                CRTResult res = solve_crt_bus_schedules(a, mod, 3, &m, 1);
                printf("\n  >> First Simultaneous Meeting Minute: Minute %lld\n", res.synchronized_minute);
                break;
            }
            case 4: {
                compare_compression_methods(telemetry_sample);
                break;
            }
        }

        press_enter_to_continue();
    }
}

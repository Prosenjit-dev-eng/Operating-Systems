/*
 * Operating Systems Laboratory - Assignment III
 * Question 4: Least Frequently Used (LFU) Page Replacement Simulation
 *
 * Page Reference String (from Q2):
 * 9, 10, 11, 7, 12, 8, 7, 6, 12, 5, 4, 3, 10, 11, 12, 4, 5, 6, 9, 4, 5
 *
 * Requirements:
 * Simulate LFU for a memory with 4 frames.
 * Implement using TWO different methods for finding out the Least Frequently Used victim page:
 * - Method 1: LFU with FIFO Tie-Breaking (if frequencies tie, the page that arrived earliest is replaced)
 * - Method 2: LFU with LRU Tie-Breaking (if frequencies tie, the page used least recently is replaced)
 */

#include <stdio.h>
#include <stdlib.h>

#define REF_LEN 21
#define NUM_FRAMES 4

// Reference string from Q2
const int ref_str[REF_LEN] = {9, 10, 11, 7, 12, 8, 7, 6, 12, 5, 4, 3, 10, 11, 12, 4, 5, 6, 9, 4, 5};

/* =========================================================================
   METHOD 1: LFU with FIFO Tie-Breaking
   Tracks frequency of each frame. When multiple pages have the same minimum
   frequency, the page that entered the memory earliest (arrival time) is selected.
   ========================================================================= */
void simulate_lfu_fifo(int *total_faults, int *total_hits) {
    int frames[NUM_FRAMES];
    int freq[NUM_FRAMES];
    int arrival[NUM_FRAMES];
    int faults = 0;
    int hits = 0;

    for (int i = 0; i < NUM_FRAMES; i++) {
        frames[i] = -1;
        freq[i] = 0;
        arrival[i] = -1;
    }

    printf("\n--- Method 1: LFU with FIFO Tie-Breaker (4 Frames) ---\n");
    printf("+------+------------+------------+------------+------------+----------+\n");
    printf("| Ref  | Frame 1(f) | Frame 2(f) | Frame 3(f) | Frame 4(f) | Status   |\n");
    printf("+------+------------+------------+------------+------------+----------+\n");

    for (int t = 0; t < REF_LEN; t++) {
        int page = ref_str[t];
        int hit_idx = -1;

        // Check for page hit
        for (int i = 0; i < NUM_FRAMES; i++) {
            if (frames[i] == page) {
                hit_idx = i;
                break;
            }
        }

        if (hit_idx != -1) {
            hits++;
            freq[hit_idx]++; // increment frequency count

            printf("| %-4d |", page);
            for (int i = 0; i < NUM_FRAMES; i++) {
                if (frames[i] != -1) printf(" %2d(f=%d)   |", frames[i], freq[i]);
                else printf("            |");
            }
            printf(" HIT      |\n");
        } else {
            faults++;
            int victim_idx = -1;

            // Check if any frame is empty
            for (int i = 0; i < NUM_FRAMES; i++) {
                if (frames[i] == -1) {
                    victim_idx = i;
                    break;
                }
            }

            // If memory full, find page with lowest frequency
            // Break ties using earliest arrival time (FIFO)
            if (victim_idx == -1) {
                int min_freq = 999999;
                int min_arrival = 999999;

                for (int i = 0; i < NUM_FRAMES; i++) {
                    if (freq[i] < min_freq) {
                        min_freq = freq[i];
                        min_arrival = arrival[i];
                        victim_idx = i;
                    } else if (freq[i] == min_freq) {
                        // Tie: pick the one that arrived earlier
                        if (arrival[i] < min_arrival) {
                            min_arrival = arrival[i];
                            victim_idx = i;
                        }
                    }
                }
            }

            // Replace page in victim frame
            frames[victim_idx] = page;
            freq[victim_idx] = 1;      // reset frequency for new page
            arrival[victim_idx] = t;   // record arrival time

            printf("| %-4d |", page);
            for (int i = 0; i < NUM_FRAMES; i++) {
                if (frames[i] != -1) printf(" %2d(f=%d)   |", frames[i], freq[i]);
                else printf("            |");
            }
            printf(" FAULT    |\n");
        }
    }

    printf("+------+------------+------------+------------+------------+----------+\n");

    *total_faults = faults;
    *total_hits = hits;

    printf("Total References : %d\n", REF_LEN);
    printf("Total Page Faults: %d\n", faults);
    printf("Total Page Hits  : %d\n", hits);
    printf("Hit Ratio        : %.2f%%\n", (hits * 100.0) / REF_LEN);
    printf("Fault Ratio      : %.2f%%\n", (faults * 100.0) / REF_LEN);
}

/* =========================================================================
   METHOD 2: LFU with LRU Tie-Breaking
   Tracks frequency of each frame and the time of last access.
   When multiple pages have the same minimum frequency, the page used least
   recently (smallest last_access time) is selected as victim.
   ========================================================================= */
void simulate_lfu_lru(int *total_faults, int *total_hits) {
    int frames[NUM_FRAMES];
    int freq[NUM_FRAMES];
    int last_used[NUM_FRAMES];
    int faults = 0;
    int hits = 0;

    for (int i = 0; i < NUM_FRAMES; i++) {
        frames[i] = -1;
        freq[i] = 0;
        last_used[i] = -1;
    }

    printf("\n--- Method 2: LFU with LRU Tie-Breaker (4 Frames) ---\n");
    printf("+------+------------+------------+------------+------------+----------+\n");
    printf("| Ref  | Frame 1(f) | Frame 2(f) | Frame 3(f) | Frame 4(f) | Status   |\n");
    printf("+------+------------+------------+------------+------------+----------+\n");

    for (int t = 0; t < REF_LEN; t++) {
        int page = ref_str[t];
        int hit_idx = -1;

        // Check for page hit
        for (int i = 0; i < NUM_FRAMES; i++) {
            if (frames[i] == page) {
                hit_idx = i;
                break;
            }
        }

        if (hit_idx != -1) {
            hits++;
            freq[hit_idx]++;         // increment frequency
            last_used[hit_idx] = t;  // update last used time

            printf("| %-4d |", page);
            for (int i = 0; i < NUM_FRAMES; i++) {
                if (frames[i] != -1) printf(" %2d(f=%d)   |", frames[i], freq[i]);
                else printf("            |");
            }
            printf(" HIT      |\n");
        } else {
            faults++;
            int victim_idx = -1;

            // Check if any frame is empty
            for (int i = 0; i < NUM_FRAMES; i++) {
                if (frames[i] == -1) {
                    victim_idx = i;
                    break;
                }
            }

            // If memory full, find page with lowest frequency
            // Break ties using least recently used (LRU)
            if (victim_idx == -1) {
                int min_freq = 999999;
                int min_last_used = 999999;

                for (int i = 0; i < NUM_FRAMES; i++) {
                    if (freq[i] < min_freq) {
                        min_freq = freq[i];
                        min_last_used = last_used[i];
                        victim_idx = i;
                    } else if (freq[i] == min_freq) {
                        // Tie: pick the least recently accessed page
                        if (last_used[i] < min_last_used) {
                            min_last_used = last_used[i];
                            victim_idx = i;
                        }
                    }
                }
            }

            // Replace page in victim frame
            frames[victim_idx] = page;
            freq[victim_idx] = 1;        // reset frequency for incoming page
            last_used[victim_idx] = t;   // record access time

            printf("| %-4d |", page);
            for (int i = 0; i < NUM_FRAMES; i++) {
                if (frames[i] != -1) printf(" %2d(f=%d)   |", frames[i], freq[i]);
                else printf("            |");
            }
            printf(" FAULT    |\n");
        }
    }

    printf("+------+------------+------------+------------+------------+----------+\n");

    *total_faults = faults;
    *total_hits = hits;

    printf("Total References : %d\n", REF_LEN);
    printf("Total Page Faults: %d\n", faults);
    printf("Total Page Hits  : %d\n", hits);
    printf("Hit Ratio        : %.2f%%\n", (hits * 100.0) / REF_LEN);
    printf("Fault Ratio      : %.2f%%\n", (faults * 100.0) / REF_LEN);
}

int main() {
    printf("======================================================================\n");
    printf("     LEAST FREQUENTLY USED (LFU) PAGE REPLACEMENT SIMULATION          \n");
    printf("======================================================================\n");
    printf("Page Reference String: ");
    for (int i = 0; i < REF_LEN; i++) {
        printf("%d%s", ref_str[i], (i == REF_LEN - 1) ? "\n" : ", ");
    }
    printf("Memory Frames: %d\n", NUM_FRAMES);

    int fifo_faults = 0, fifo_hits = 0;
    int lru_faults = 0, lru_hits = 0;

    int choice;
    printf("\nChoose Simulation Option:\n");
    printf("1. Method 1: LFU with FIFO Tie-Breaking\n");
    printf("2. Method 2: LFU with LRU Tie-Breaking\n");
    printf("3. Run BOTH Methods and Compare\n");
    printf("Enter choice (1-3): ");
    if (scanf("%d", &choice) != 1) choice = 3;

    if (choice == 1 || choice == 3) {
        simulate_lfu_fifo(&fifo_faults, &fifo_hits);
    }
    if (choice == 2 || choice == 3) {
        simulate_lfu_lru(&lru_faults, &lru_hits);
    }

    if (choice == 3) {
        printf("\n======================================================================\n");
        printf("              COMPARISON OF LFU TIE-BREAKING METHODS                  \n");
        printf("======================================================================\n");
        printf("+---------------------------+-------------+-----------+-----------+-------------+\n");
        printf("| Method                    | Page Faults | Page Hits | Hit Ratio | Fault Ratio |\n");
        printf("+---------------------------+-------------+-----------+-----------+-------------+\n");
        printf("| Method 1 (LFU with FIFO)  | %-11d | %-9d | %-8.2f%% | %-10.2f%% |\n",
               fifo_faults, fifo_hits, (fifo_hits * 100.0) / REF_LEN, (fifo_faults * 100.0) / REF_LEN);
        printf("| Method 2 (LFU with LRU)   | %-11d | %-9d | %-8.2f%% | %-10.2f%% |\n",
               lru_faults, lru_hits, (lru_hits * 100.0) / REF_LEN, (lru_faults * 100.0) / REF_LEN);
        printf("+---------------------------+-------------+-----------+-----------+-------------+\n");
    }

    return 0;
}

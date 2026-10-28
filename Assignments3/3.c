/*
 * Operating Systems Laboratory - Assignment III
 * Question 3: First In First Out (FIFO) Page Replacement Simulation
 *
 * Page Reference String (from Q2):
 * 9, 10, 11, 7, 12, 8, 7, 6, 12, 5, 4, 3, 10, 11, 12, 4, 5, 6, 9, 4, 5
 *
 * Requirements:
 * Simulate FIFO page replacement for:
 * (i) 4 frames
 * (ii) 7 frames
 * Compare the results.
 */

#include <stdio.h>
#include <stdlib.h>

#define REF_LEN 21
#define MAX_FRAMES 10

// Reference string from Q2
const int ref_str[REF_LEN] = {9, 10, 11, 7, 12, 8, 7, 6, 12, 5, 4, 3, 10, 11, 12, 4, 5, 6, 9, 4, 5};

void simulate_fifo(int num_frames, int *total_faults, int *total_hits) {
    int frames[MAX_FRAMES];
    int fifo_ptr = 0; // points to the oldest page to be replaced
    int faults = 0;
    int hits = 0;

    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1; // -1 represents empty frame
    }

    printf("\n--- FIFO Simulation for %d Frames ---\n", num_frames);
    printf("+------+");
    for (int i = 1; i <= num_frames; i++) printf("---------+");
    printf("----------+\n");

    printf("| Ref  |");
    for (int i = 1; i <= num_frames; i++) printf(" Frame %d |", i);
    printf(" Status   |\n");

    printf("+------+");
    for (int i = 1; i <= num_frames; i++) printf("---------+");
    printf("----------+\n");

    for (int t = 0; t < REF_LEN; t++) {
        int page = ref_str[t];
        int hit = 0;

        // Check if page already exists in one of the frames
        for (int i = 0; i < num_frames; i++) {
            if (frames[i] == page) {
                hit = 1;
                break;
            }
        }

        if (hit) {
            hits++;
            printf("| %-4d |", page);
            for (int i = 0; i < num_frames; i++) {
                if (frames[i] != -1) printf(" %-7d |", frames[i]);
                else printf("         |");
            }
            printf(" HIT      |\n");
        } else {
            faults++;
            // Replace the oldest frame (FIFO)
            frames[fifo_ptr] = page;
            fifo_ptr = (fifo_ptr + 1) % num_frames;

            printf("| %-4d |", page);
            for (int i = 0; i < num_frames; i++) {
                if (frames[i] != -1) printf(" %-7d |", frames[i]);
                else printf("         |");
            }
            printf(" FAULT    |\n");
        }
    }

    printf("+------+");
    for (int i = 1; i <= num_frames; i++) printf("---------+");
    printf("----------+\n");

    *total_faults = faults;
    *total_hits = hits;

    printf("Total References: %d\n", REF_LEN);
    printf("Total Page Faults: %d\n", faults);
    printf("Total Page Hits  : %d\n", hits);
    printf("Hit Ratio        : %.2f%%\n", (hits * 100.0) / REF_LEN);
    printf("Fault Ratio      : %.2f%%\n", (faults * 100.0) / REF_LEN);
}

void compare_results(int f4_faults, int f4_hits, int f7_faults, int f7_hits) {
    printf("\n======================================================================\n");
    printf("                 COMPARISON: 4 FRAMES vs 7 FRAMES                     \n");
    printf("======================================================================\n");
    printf("+---------------+-------------+-----------+-----------+-------------+\n");
    printf("| Frame Count   | Page Faults | Page Hits | Hit Ratio | Fault Ratio |\n");
    printf("+---------------+-------------+-----------+-----------+-------------+\n");
    printf("| 4 Frames      | %-11d | %-9d | %-8.2f%% | %-10.2f%% |\n",
           f4_faults, f4_hits, (f4_hits * 100.0) / REF_LEN, (f4_faults * 100.0) / REF_LEN);
    printf("| 7 Frames      | %-11d | %-9d | %-8.2f%% | %-10.2f%% |\n",
           f7_faults, f7_hits, (f7_hits * 100.0) / REF_LEN, (f7_faults * 100.0) / REF_LEN);
    printf("+---------------+-------------+-----------+-----------+-------------+\n");

    if (f7_faults < f4_faults) {
        printf("Observation: Increasing frame count from 4 to 7 reduced page faults by %d.\n", f4_faults - f7_faults);
    } else if (f7_faults > f4_faults) {
        printf("Observation: Belady's Anomaly encountered (page faults increased with more frames).\n");
    } else {
        printf("Observation: Both 4 frames and 7 frames resulted in equal page faults (%d).\n", f4_faults);
    }
}

int main() {
    printf("======================================================================\n");
    printf("       FIRST IN FIRST OUT (FIFO) PAGE REPLACEMENT SIMULATION          \n");
    printf("======================================================================\n");
    printf("Page Reference String: ");
    for (int i = 0; i < REF_LEN; i++) {
        printf("%d%s", ref_str[i], (i == REF_LEN - 1) ? "\n" : ", ");
    }

    int f4_faults = 0, f4_hits = 0;
    int f7_faults = 0, f7_hits = 0;

    // Simulate for 4 frames
    simulate_fifo(4, &f4_faults, &f4_hits);

    // Simulate for 7 frames
    simulate_fifo(7, &f7_faults, &f7_hits);

    // Compare results
    compare_results(f4_faults, f4_hits, f7_faults, f7_hits);

    return 0;
}

/*
 * Operating Systems Laboratory - Assignment III
 * Question 2: Least Recently Used (LRU) Page Replacement Simulation
 *
 * Page Reference String:
 * 9, 10, 11, 7, 12, 8, 7, 6, 12, 5, 4, 3, 10, 11, 12, 4, 5, 6, 9, 4, 5
 *
 * Requirements:
 * 1. Simulate for 5 frames and 6 frames.
 * 2. Compare the results.
 * 3. Implement using TWO different methods for finding the LRU victim page:
 *    - Method 1: Counter / Timestamp Method (associating time of last access with each frame)
 *    - Method 2: Stack / Aging Order Method (maintaining MRU-to-LRU order list)
 */

#include <stdio.h>
#include <stdlib.h>

#define REF_LEN 21
#define MAX_FRAMES 10

// Given reference string
const int ref_str[REF_LEN] = {9, 10, 11, 7, 12, 8, 7, 6, 12, 5, 4, 3, 10, 11, 12, 4, 5, 6, 9, 4, 5};

/* =========================================================================
   METHOD 1: Counter / Timestamp Method
   Each frame keeps track of the time step when it was last accessed.
   On a page fault when frames are full, the frame with the smallest timestamp
   is chosen as the LRU victim.
   ========================================================================= */
void simulate_lru_counter(int num_frames, int *total_faults, int *total_hits) {
    int frames[MAX_FRAMES];
    int last_used[MAX_FRAMES];
    int faults = 0;
    int hits = 0;

    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1; // -1 denotes empty frame
        last_used[i] = -1;
    }

    printf("\n--- Simulation using Method 1 (Counter / Timestamp Method) [%d Frames] ---\n", num_frames);
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
        int found_idx = -1;

        // Check if page is already present (Hit)
        for (int i = 0; i < num_frames; i++) {
            if (frames[i] == page) {
                found_idx = i;
                break;
            }
        }

        if (found_idx != -1) {
            // Page Hit
            hits++;
            last_used[found_idx] = t; // update timestamp

            printf("| %-4d |", page);
            for (int i = 0; i < num_frames; i++) {
                if (frames[i] != -1) printf(" %-7d |", frames[i]);
                else printf("         |");
            }
            printf(" HIT      |\n");
        } else {
            // Page Fault (Miss)
            faults++;
            int victim_idx = -1;

            // Check if any frame is empty
            for (int i = 0; i < num_frames; i++) {
                if (frames[i] == -1) {
                    victim_idx = i;
                    break;
                }
            }

            // If no frame is empty, find frame with oldest timestamp (LRU)
            if (victim_idx == -1) {
                int min_time = 999999;
                for (int i = 0; i < num_frames; i++) {
                    if (last_used[i] < min_time) {
                        min_time = last_used[i];
                        victim_idx = i;
                    }
                }
            }

            // Replace page in victim frame
            frames[victim_idx] = page;
            last_used[victim_idx] = t;

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

/* =========================================================================
   METHOD 2: Stack / Order List Method
   Maintains a list of pages in memory ordered by recency.
   Whenever a page is referenced:
   - If present, move it to the top (MRU position).
   - If missing, the bottom element (index 0) is the LRU page and is replaced.
   ========================================================================= */
void simulate_lru_stack(int num_frames, int *total_faults, int *total_hits) {
    int frames[MAX_FRAMES];
    int stack[MAX_FRAMES]; // stack[0] is LRU, stack[count-1] is MRU
    int stack_count = 0;
    int faults = 0;
    int hits = 0;

    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }

    printf("\n--- Simulation using Method 2 (Stack / Order List Method) [%d Frames] ---\n", num_frames);
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
        int found_in_stack = -1;

        // Check if page is already in the stack
        for (int i = 0; i < stack_count; i++) {
            if (stack[i] == page) {
                found_in_stack = i;
                break;
            }
        }

        if (found_in_stack != -1) {
            // Page Hit: Move the hit page to MRU position (top of stack)
            hits++;
            int hit_page = stack[found_in_stack];
            for (int i = found_in_stack; i < stack_count - 1; i++) {
                stack[i] = stack[i + 1];
            }
            stack[stack_count - 1] = hit_page;

            printf("| %-4d |", page);
            for (int i = 0; i < num_frames; i++) {
                if (frames[i] != -1) printf(" %-7d |", frames[i]);
                else printf("         |");
            }
            printf(" HIT      |\n");
        } else {
            // Page Fault
            faults++;
            if (stack_count < num_frames) {
                // Empty frame available
                frames[stack_count] = page;
                stack[stack_count] = page;
                stack_count++;
            } else {
                // Memory full: victim is stack[0] (the least recently used page)
                int victim_page = stack[0];

                // Shift stack left to drop the victim
                for (int i = 0; i < stack_count - 1; i++) {
                    stack[i] = stack[i + 1];
                }
                stack[stack_count - 1] = page; // new page at MRU position

                // Find victim in frames array and replace it
                for (int i = 0; i < num_frames; i++) {
                    if (frames[i] == victim_page) {
                        frames[i] = page;
                        break;
                    }
                }
            }

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

void print_comparison(int f5_faults, int f5_hits, int f6_faults, int f6_hits) {
    printf("\n======================================================================\n");
    printf("                 COMPARISON: 5 FRAMES vs 6 FRAMES                     \n");
    printf("======================================================================\n");
    printf("+---------------+-------------+-----------+-----------+-------------+\n");
    printf("| Frame Count   | Page Faults | Page Hits | Hit Ratio | Fault Ratio |\n");
    printf("+---------------+-------------+-----------+-----------+-------------+\n");
    printf("| 5 Frames      | %-11d | %-9d | %-8.2f%% | %-10.2f%% |\n",
           f5_faults, f5_hits, (f5_hits * 100.0) / REF_LEN, (f5_faults * 100.0) / REF_LEN);
    printf("| 6 Frames      | %-11d | %-9d | %-8.2f%% | %-10.2f%% |\n",
           f6_faults, f6_hits, (f6_hits * 100.0) / REF_LEN, (f6_faults * 100.0) / REF_LEN);
    printf("+---------------+-------------+-----------+-----------+-------------+\n");

    if (f6_faults < f5_faults) {
        printf("Observation: Increasing frames from 5 to 6 reduces page faults by %d.\n", f5_faults - f6_faults);
    } else if (f6_faults == f5_faults) {
        printf("Observation: Both 5 and 6 frames resulted in the same number of page faults (%d).\n", f5_faults);
    } else {
        printf("Observation: Belady's anomaly does not occur in LRU (LRU is a stack algorithm).\n");
    }
}

int main() {
    printf("======================================================================\n");
    printf("       LEAST RECENTLY USED (LRU) PAGE REPLACEMENT SIMULATION          \n");
    printf("======================================================================\n");
    printf("Page Reference String: ");
    for (int i = 0; i < REF_LEN; i++) {
        printf("%d%s", ref_str[i], (i == REF_LEN - 1) ? "\n" : ", ");
    }

    int f5_faults = 0, f5_hits = 0;
    int f6_faults = 0, f6_hits = 0;

    int method_choice;
    printf("\nSelect Implementation Method:\n");
    printf("1. Method 1 (Counter / Timestamp Method)\n");
    printf("2. Method 2 (Stack / Order List Method)\n");
    printf("3. Run BOTH Methods and Compare\n");
    printf("Enter choice (1-3): ");
    if (scanf("%d", &method_choice) != 1) method_choice = 3;

    if (method_choice == 1 || method_choice == 3) {
        printf("\n======================================================================\n");
        printf("           METHOD 1: COUNTER / TIMESTAMP IMPLEMENTATION              \n");
        printf("======================================================================\n");
        simulate_lru_counter(5, &f5_faults, &f5_hits);
        simulate_lru_counter(6, &f6_faults, &f6_hits);
        print_comparison(f5_faults, f5_hits, f6_faults, f6_hits);
    }

    if (method_choice == 2 || method_choice == 3) {
        printf("\n======================================================================\n");
        printf("           METHOD 2: STACK / ORDER LIST IMPLEMENTATION               \n");
        printf("======================================================================\n");
        simulate_lru_stack(5, &f5_faults, &f5_hits);
        simulate_lru_stack(6, &f6_faults, &f6_hits);
        print_comparison(f5_faults, f5_hits, f6_faults, f6_hits);
    }

    return 0;
}

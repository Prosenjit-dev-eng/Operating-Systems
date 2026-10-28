/*
 * Operating Systems Laboratory - Assignment III
 * Question 1: Memory Allocation Simulation (First Fit, Best Fit, Worst Fit)
 *
 * Problem:
 * Pre-existing processes:
 * P0: size 800 bytes, starting at 100
 * P1: size 300 bytes, starting at 1210
 * P2: size 550 bytes, starting at 1870
 * P3: size 700 bytes, starting at 2520
 * Total memory: 0001 to 9999 (1 byte per address).
 *
 * Incoming processes:
 * P4: 350 bytes, P5: 500 bytes, P6: 150 bytes
 *
 * Cases:
 * a) Order: P4 -> P5 -> P6
 * b) Order: P6 -> P5 -> P4
 * c) Order: P5 -> P4 -> P6
 * d) Order: P6 -> P4 -> P5
 *
 * Show memory map (locations and sizes of processes and holes) after each accommodation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 50

typedef struct {
    char name[20];
    int start;
    int end;
    int size;
    int is_process; // 1 = process, 0 = hole
} Block;

typedef struct {
    char name[10];
    int size;
} Process;

// Memory map representation
Block memory[MAX_BLOCKS];
int block_count = 0;

// Function to initialize memory with P0, P1, P2, P3 and initial holes
void reset_memory() {
    block_count = 0;

    // Hole: 1 to 99 (size 99)
    strcpy(memory[block_count].name, "Hole");
    memory[block_count].start = 1;
    memory[block_count].end = 99;
    memory[block_count].size = 99;
    memory[block_count].is_process = 0;
    block_count++;

    // P0: 100 to 899 (size 800)
    strcpy(memory[block_count].name, "P0");
    memory[block_count].start = 100;
    memory[block_count].end = 899;
    memory[block_count].size = 800;
    memory[block_count].is_process = 1;
    block_count++;

    // Hole: 900 to 1209 (size 310)
    strcpy(memory[block_count].name, "Hole");
    memory[block_count].start = 900;
    memory[block_count].end = 1209;
    memory[block_count].size = 310;
    memory[block_count].is_process = 0;
    block_count++;

    // P1: 1210 to 1509 (size 300)
    strcpy(memory[block_count].name, "P1");
    memory[block_count].start = 1210;
    memory[block_count].end = 1509;
    memory[block_count].size = 300;
    memory[block_count].is_process = 1;
    block_count++;

    // Hole: 1510 to 1869 (size 360)
    strcpy(memory[block_count].name, "Hole");
    memory[block_count].start = 1510;
    memory[block_count].end = 1869;
    memory[block_count].size = 360;
    memory[block_count].is_process = 0;
    block_count++;

    // P2: 1870 to 2419 (size 550)
    strcpy(memory[block_count].name, "P2");
    memory[block_count].start = 1870;
    memory[block_count].end = 2419;
    memory[block_count].size = 550;
    memory[block_count].is_process = 1;
    block_count++;

    // Hole: 2420 to 2519 (size 100)
    strcpy(memory[block_count].name, "Hole");
    memory[block_count].start = 2420;
    memory[block_count].end = 2519;
    memory[block_count].size = 100;
    memory[block_count].is_process = 0;
    block_count++;

    // P3: 2520 to 3219 (size 700)
    strcpy(memory[block_count].name, "P3");
    memory[block_count].start = 2520;
    memory[block_count].end = 3219;
    memory[block_count].size = 700;
    memory[block_count].is_process = 1;
    block_count++;

    // Hole: 3220 to 9999 (size 6780)
    strcpy(memory[block_count].name, "Hole");
    memory[block_count].start = 3220;
    memory[block_count].end = 9999;
    memory[block_count].size = 6780;
    memory[block_count].is_process = 0;
    block_count++;
}

// Function to print the entire memory map
void display_memory(const char *title) {
    printf("\n--- %s ---\n", title);
    printf("+-------+---------------+---------------+---------------+---------------+\n");
    printf("| Type  | Name          | Start Address | End Address   | Size (bytes)  |\n");
    printf("+-------+---------------+---------------+---------------+---------------+\n");
    for (int i = 0; i < block_count; i++) {
        const char *type = memory[i].is_process ? "Process" : "Hole   ";
        printf("| %s | %-13s | %13d | %13d | %13d |\n",
               type, memory[i].name, memory[i].start, memory[i].end, memory[i].size);
    }
    printf("+-------+---------------+---------------+---------------+---------------+\n");
}

// Allocate a process into a hole at index 'hole_idx'
void allocate_at_hole(int hole_idx, const char *pname, int psize) {
    int original_start = memory[hole_idx].start;
    int original_size = memory[hole_idx].size;

    if (original_size == psize) {
        // Perfect fit: hole turns completely into the process
        strcpy(memory[hole_idx].name, pname);
        memory[hole_idx].is_process = 1;
    } else {
        // Process takes front of the hole, leftover remains as a smaller hole
        // Shift elements to make space for the new hole
        for (int i = block_count; i > hole_idx + 1; i--) {
            memory[i] = memory[i - 1];
        }
        block_count++;

        // Current block becomes the process
        strcpy(memory[hole_idx].name, pname);
        memory[hole_idx].start = original_start;
        memory[hole_idx].end = original_start + psize - 1;
        memory[hole_idx].size = psize;
        memory[hole_idx].is_process = 1;

        // Next block is the remaining hole
        strcpy(memory[hole_idx + 1].name, "Hole");
        memory[hole_idx + 1].start = original_start + psize;
        memory[hole_idx + 1].end = original_start + original_size - 1;
        memory[hole_idx + 1].size = original_size - psize;
        memory[hole_idx + 1].is_process = 0;
    }
}

// First Fit allocation strategy
int allocate_first_fit(const char *pname, int psize) {
    for (int i = 0; i < block_count; i++) {
        if (!memory[i].is_process && memory[i].size >= psize) {
            allocate_at_hole(i, pname, psize);
            return 1;
        }
    }
    return 0; // Not accommodated
}

// Best Fit allocation strategy
int allocate_best_fit(const char *pname, int psize) {
    int best_idx = -1;
    int min_size = 999999;

    for (int i = 0; i < block_count; i++) {
        if (!memory[i].is_process && memory[i].size >= psize) {
            if (memory[i].size < min_size) {
                min_size = memory[i].size;
                best_idx = i;
            }
        }
    }

    if (best_idx != -1) {
        allocate_at_hole(best_idx, pname, psize);
        return 1;
    }
    return 0;
}

// Worst Fit allocation strategy
int allocate_worst_fit(const char *pname, int psize) {
    int worst_idx = -1;
    int max_size = -1;

    for (int i = 0; i < block_count; i++) {
        if (!memory[i].is_process && memory[i].size >= psize) {
            if (memory[i].size > max_size) {
                max_size = memory[i].size;
                worst_idx = i;
            }
        }
    }

    if (worst_idx != -1) {
        allocate_at_hole(worst_idx, pname, psize);
        return 1;
    }
    return 0;
}

// Run simulation for a specific strategy and arrival sequence
void run_case(const char *case_name, const Process procs[], int num_procs, int strategy_choice) {
    const char *strat_name = (strategy_choice == 1) ? "First Fit" :
                             (strategy_choice == 2) ? "Best Fit" : "Worst Fit";

    printf("\n======================================================================\n");
    printf("   %s using %s Strategy\n", case_name, strat_name);
    printf("======================================================================\n");

    reset_memory();
    display_memory("i. After P0 to P3 are accommodated (Initial State)");

    for (int i = 0; i < num_procs; i++) {
        int success = 0;
        if (strategy_choice == 1) {
            success = allocate_first_fit(procs[i].name, procs[i].size);
        } else if (strategy_choice == 2) {
            success = allocate_best_fit(procs[i].name, procs[i].size);
        } else if (strategy_choice == 3) {
            success = allocate_worst_fit(procs[i].name, procs[i].size);
        }

        char step_title[100];
        if (success) {
            sprintf(step_title, "After %s (%d bytes) is accommodated", procs[i].name, procs[i].size);
            display_memory(step_title);
        } else {
            printf("\n--> Could not accommodate %s (%d bytes): Not enough contiguous memory!\n",
                   procs[i].name, procs[i].size);
        }
    }
}

void execute_sequence(const char *case_label, const Process procs[]) {
    for (int s = 1; s <= 3; s++) {
        run_case(case_label, procs, 3, s);
    }
}

int main() {
    Process P4 = {"P4", 350};
    Process P5 = {"P5", 500};
    Process P6 = {"P6", 150};

    Process seq_a[3] = {P4, P5, P6};
    Process seq_b[3] = {P6, P5, P4};
    Process seq_c[3] = {P5, P4, P6};
    Process seq_d[3] = {P6, P4, P5};

    int choice;
    while (1) {
        printf("\n==================================================\n");
        printf("    Memory Allocation Simulation (Question 1)   \n");
        printf("==================================================\n");
        printf("1. Run Case (a): Order P4 -> P5 -> P6\n");
        printf("2. Run Case (b): Order P6 -> P5 -> P4\n");
        printf("3. Run Case (c): Order P5 -> P4 -> P6\n");
        printf("4. Run Case (d): Order P6 -> P4 -> P5\n");
        printf("5. Run All Cases (a, b, c, d) with All Strategies\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");
        if (scanf("%d", &choice) != 1) {
            break;
        }

        switch (choice) {
            case 1:
                execute_sequence("Case (a): Arrival P4, P5, P6", seq_a);
                break;
            case 2:
                execute_sequence("Case (b): Arrival P6, P5, P4", seq_b);
                break;
            case 3:
                execute_sequence("Case (c): Arrival P5, P4, P6", seq_c);
                break;
            case 4:
                execute_sequence("Case (d): Arrival P6, P4, P5", seq_d);
                break;
            case 5:
                execute_sequence("Case (a): Arrival P4, P5, P6", seq_a);
                execute_sequence("Case (b): Arrival P6, P5, P4", seq_b);
                execute_sequence("Case (c): Arrival P5, P4, P6", seq_c);
                execute_sequence("Case (d): Arrival P6, P4, P5", seq_d);
                break;
            case 6:
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice! Please select 1-6.\n");
        }
    }

    return 0;
}

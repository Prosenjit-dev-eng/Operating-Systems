#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

typedef struct {
    int id;
    int max[MAX_RESOURCES];
    int alloc[MAX_RESOURCES];
    int need[MAX_RESOURCES];
    bool finished;
} Process;

// Function to check if the system is in a safe state
bool is_safe(Process processes[], int n, int m, int available[], int safe_seq[]) {
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES];

    for (int i = 0; i < m; i++) work[i] = available[i];
    for (int i = 0; i < n; i++) finish[i] = processes[i].finished;

    int count = 0;
    while (count < n) {
        bool found = false;
        for (int p = 0; p < n; p++) {
            if (!finish[p]) {
                int j;
                for (j = 0; j < m; j++) {
                    if (processes[p].need[j] > work[j])
                        break;
                }
                if (j == m) {
                    for (int k = 0; k < m; k++) work[k] += processes[p].alloc[k];
                    safe_seq[count++] = processes[p].id;
                    finish[p] = true;
                    found = true;
                }
            }
        }
        if (!found) return false; // Not safe
    }
    return true; // Safe
}

int main() {
    // --- HARDCODED SYSTEM SPECIFICATIONS ---
    int n = 3; // Number of processes
    int m = 4; // Number of resources
    
    // Total instances of each resource
    int total_instances[MAX_RESOURCES] = {2, 4, 5, 3}; 
    
    // Initial Available resources (calculated as Total - Initial Allocations)
    int available[MAX_RESOURCES];
    for (int i = 0; i < m; i++) available[i] = total_instances[i];

    // Hardcoded Process Data (Max needs and Initial Allocations)
    Process processes[MAX_PROCESSES] = {
        {0, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, false},
        {1, {2, 3, 1, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, false},
        {2, {2, 2, 1, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, false}
    };

    // Initialize Need = Max - Alloc
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            processes[i].need[j] = processes[i].max[j] - processes[i].alloc[j];
        }
    }

    printf("\n--- System Initialized (Hardcoded) --- \n");
    printf("Processes: %d | Resources: %d\n", n, m);

    // --- Interactive Request Simulation Loop ---
    int active_processes = n;
    while (active_processes > 0) {
        int pid;
        printf("\nEnter Process ID (0 to %d) making a request (-1 to exit): ", n - 1);
        if (scanf("%d", &pid) != 1 || pid == -1) break;

        if (pid < 0 || pid >= n || processes[pid].finished) {
            printf("Invalid Process ID or process already finished.\n");
            continue;
        }

        int request[MAX_RESOURCES];
        printf("Enter request for %d resources (separated by spaces): ", m);
        for (int i = 0; i < m; i++) {
            scanf("%d", &request[i]);
        }

        // 1. Check if request <= need and request <= available
        bool valid = true;
        for (int i = 0; i < m; i++) {
            if (request[i] > processes[pid].need[i] || request[i] > available[i]) {
                valid = false;
                break;
            }
        }

        if (!valid) {
            printf("-> Request DENIED: Exceeds process need or available resources.\n");
            continue;
        }

        // Temporarily allocate to test safety
        for (int i = 0; i < m; i++) {
            available[i] -= request[i];
            processes[pid].alloc[i] += request[i];
            processes[pid].need[i] -= request[i];
        }

        int safe_seq[MAX_PROCESSES];
        if (is_safe(processes, n, m, available, safe_seq)) {
            printf("-> Request GRANTED! System is in a SAFE state.\nSafe Sequence: ");
            for (int i = 0; i < n; i++) {
                if (!processes[safe_seq[i]].finished)
                    printf("P%d ", safe_seq[i]);
            }
            printf("\n");

            // Check if process has finished all needs (Need becomes all zeros)
            bool all_zero = true;
            for (int i = 0; i < m; i++) {
                if (processes[pid].need[i] > 0) all_zero = false;
            }
            
            if (all_zero) {
                processes[pid].finished = true;
                // Release allocated resources back to system upon completion
                for (int i = 0; i < m; i++) {
                    available[i] += processes[pid].alloc[i];
                    processes[pid].alloc[i] = 0;
                }
                active_processes--;
                printf("-> Process P%d has completed all its needs and released its resources.\n", pid);
            }
        } else {
            // Rollback allocation if unsafe
            printf("-> Request DENIED: Granting leads to an UNSAFE state. Rolling back.\n");
            for (int i = 0; i < m; i++) {
                available[i] += request[i];
                processes[pid].alloc[i] -= request[i];
                processes[pid].need[i] += request[i];
            }
        }
    }

    printf("\nSimulation ended.\n");
    return 0;
}
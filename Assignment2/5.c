#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

int main() {
    // Hardcoded parsed data from the assignment example for demonstration
    int n = 3; // Number of processes
    int m = 4; // Number of resources
    
    // Total instances: 2 4 5 3
    int available[MAX_RESOURCES] = {2, 4, 5, 3}; 
    
    // Max requirements parsed from "1 1 1 1, 2 3 1 2, 2 2 1 3"
    int max[MAX_PROCESSES][MAX_RESOURCES] = {
        {1, 1, 1, 1},
        {2, 3, 1, 2},
        {2, 2, 1, 3}
    };
    
    // Example Allocation (assuming a present situation request sequence)
    int alloc[MAX_PROCESSES][MAX_RESOURCES] = {
        {0, 1, 0, 0},
        {1, 0, 0, 1},
        {0, 0, 1, 0}
    };
    
    int need[MAX_PROCESSES][MAX_RESOURCES];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    bool finish[MAX_PROCESSES] = {false};
    int safe_sequence[MAX_PROCESSES];
    int work[MAX_RESOURCES];
    for (int i = 0; i < m; i++) work[i] = available[i];

    int count = 0;
    while (count < n) {
        bool found = false;
        for (int p = 0; p < n; p++) {
            if (!finish[p]) {
                int j;
                for (j = 0; j < m; j++)
                    if (need[p][j] > work[j])
                        break;
                
                if (j == m) {
                    for (int k = 0; k < m; k++) work[k] += alloc[p][k];
                    safe_sequence[count++] = p;
                    finish[p] = true;
                    found = true;
                }
            }
        }
        if (!found) {
            printf("System is NOT in a safe state. Request denied.\n");
            return 0;
        }
    }

    printf("System is in a SAFE state.\nSafe Sequence: ");
    for (int i = 0; i < n; i++) printf("P%d ", safe_sequence[i]);
    printf("\n");

    return 0;
}
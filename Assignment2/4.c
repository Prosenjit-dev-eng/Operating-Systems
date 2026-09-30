#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_JOBS 8
#define MAX_BURSTS 20

// Job states
#define STATE_NOT_ARRIVED 0
#define STATE_READY 1
#define STATE_RUNNING 2
#define STATE_WAITING_IO 3
#define STATE_FINISHED 4

typedef struct {
    int id;
    int priority;
    int arrival;
    int bursts[MAX_BURSTS]; // Alternating: [CPU1, IO1, CPU2, IO2, ...]
    int total_burst_elements;
    
    // Simulation tracking variables
    int state;
    int current_burst_idx; // Points to current burst in bursts[]
    int remaining_burst_time; // Remaining time for current burst (CPU or IO)
    
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Job;

void parse_jobs(const char* filename, Job original_jobs[], int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) { perror("File opening failed"); exit(1); }

    *count = 0;
    while (*count < MAX_JOBS) {
        Job* j = &original_jobs[*count];
        if (fscanf(file, "%d %d %d", &j->id, &j->priority, &j->arrival) != 3) break;
        
        j->total_burst_elements = 0;
        int val;
        while (fscanf(file, "%d", &val) == 1 && val != -1) {
            j->bursts[j->total_burst_elements++] = val;
        }
        (*count)++;
    }
    fclose(file);
}

// Reset job states before running each simulation algorithm
void reset_jobs(Job src[], Job dest[], int count) {
    for (int i = 0; i < count; i++) {
        dest[i] = src[i];
        dest[i].state = STATE_NOT_ARRIVED;
        dest[i].current_burst_idx = 0;
        dest[i].remaining_burst_time = 0;
        dest[i].completion_time = 0;
        dest[i].turnaround_time = 0;
        dest[i].waiting_time = 0;
    }
}

// --- SIMULATION ENGINE ---
void run_simulation(Job jobs[], int count, const char* algo_name, int is_rr, int quantum) {
    int current_time = 0;
    int completed_count = 0;
    int running_job = -1;
    int rr_quantum_counter = 0;

    printf("\n========================================\n");
    printf(" Simulation: %s\n", algo_name);
    printf("========================================\n");

    while (completed_count < count) {
        // 1. Check for new arrivals and IO completions
        for (int i = 0; i < count; i++) {
            if (jobs[i].state == STATE_NOT_ARRIVED && jobs[i].arrival <= current_time) {
                jobs[i].state = STATE_READY;
                jobs[i].current_burst_idx = 0;
                jobs[i].remaining_burst_time = jobs[i].bursts[0];
            } else if (jobs[i].state == STATE_WAITING_IO) {
                jobs[i].remaining_burst_time--;
                if (jobs[i].remaining_burst_time == 0) {
                    // IO finished, move to next burst (which is CPU)
                    jobs[i].current_burst_idx++;
                    jobs[i].remaining_burst_time = jobs[i].bursts[jobs[i].current_burst_idx];
                    jobs[i].state = STATE_READY;
                }
            }
        }

        // 2. If CPU is free, select next process based on algorithm
        if (running_job == -1) {
            int selected = -1;
            
            if (strcmp(algo_name, "FCFS") == 0 || is_rr) {
                // FCFS / Round Robin: Select earliest arrived ready job
                int earliest_arrival = 999999;
                for (int i = 0; i < count; i++) {
                    if (jobs[i].state == STATE_READY) {
                        if (jobs[i].arrival < earliest_arrival) {
                            earliest_arrival = jobs[i].arrival;
                            selected = i;
                        }
                    }
                }
            } else if (strcmp(algo_name, "Non-preemptive Priority") == 0) {
                // Priority: Lowest priority number has highest precedence
                int highest_priority = 999999;
                for (int i = 0; i < count; i++) {
                    if (jobs[i].state == STATE_READY) {
                        if (jobs[i].priority < highest_priority) {
                            highest_priority = jobs[i].priority;
                            selected = i;
                        }
                    }
                }
            }

            if (selected != -1) {
                running_job = selected;
                jobs[running_job].state = STATE_RUNNING;
                rr_quantum_counter = 0;
            }
        }

        // 3. Execute running job for 1 time unit
        if (running_job != -1) {
            jobs[running_job].remaining_burst_time--;
            rr_quantum_counter++;

            // Check if current CPU burst finished
            if (jobs[running_job].remaining_burst_time == 0) {
                jobs[running_job].current_burst_idx++; // Move to IO burst or end
                
                if (jobs[running_job].current_burst_idx >= jobs[running_job].total_burst_elements) {
                    // Job completely finished
                    jobs[running_job].state = STATE_FINISHED;
                    jobs[running_job].completion_time = current_time + 1;
                    jobs[running_job].turnaround_time = jobs[running_job].completion_time - jobs[running_job].arrival;
                    // Total waiting time = Turnaround - Sum of all CPU bursts
                    int total_cpu = 0;
                    for (int b = 0; b < jobs[running_job].total_burst_elements; b += 2)
                        total_cpu += jobs[running_job].bursts[b];
                    jobs[running_job].waiting_time = jobs[running_job].turnaround_time - total_cpu;
                    
                    completed_count++;
                    running_job = -1;
                } else {
                    // CPU burst done, now enter IO burst
                    jobs[running_job].state = STATE_WAITING_IO;
                    jobs[running_job].remaining_burst_time = jobs[running_job].bursts[jobs[running_job].current_burst_idx];
                    running_job = -1;
                }
            } 
            else if (is_rr && rr_quantum_counter >= quantum) {
                // Quantum expired for Round Robin, preempt job back to Ready
                jobs[running_job].state = STATE_READY;
                running_job = -1;
            }
        }

        // Increment waiting time for all ready jobs
        for (int i = 0; i < count; i++) {
            if (jobs[i].state == STATE_READY) {
                jobs[i].waiting_time++;
            }
        }

        current_time++;
    }

    // Print Results Summary
    float total_wt = 0, total_tat = 0;
    printf("Job ID | Priority | Arrival | Waiting Time | Turnaround Time\n");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("  %2d   |    %2d    |   %2d    |     %2d       |      %2d\n", 
            jobs[i].id, jobs[i].priority, jobs[i].arrival, jobs[i].waiting_time, jobs[i].turnaround_time);
        total_wt += jobs[i].waiting_time;
        total_tat += jobs[i].turnaround_time;
    }
    printf("-----------------------------------------------------------\n");
    printf("Average Waiting Time   : %.2f\n", total_wt / count);
    printf("Average Turnaround Time: %.2f\n", total_tat / count);
}

int main(int argc, char* argv[]) {
    if (argc != 2) { printf("Usage: %s <job_file>\n", argv[0]); return 1; }
    
    Job original_jobs[MAX_JOBS];
    Job sim_jobs[MAX_JOBS];
    int job_count = 0;

    parse_jobs(argv[1], original_jobs, &job_count);

    if (job_count != 8) {
        printf("Warning: Expected 8 job profiles, found %d.\n", job_count);
    }

    // 1. FCFS Scheduling
    reset_jobs(original_jobs, sim_jobs, job_count);
    run_simulation(sim_jobs, job_count, "FCFS", 0, 0);

    // 2. Non-preemptive Priority Scheduling
    reset_jobs(original_jobs, sim_jobs, job_count);
    run_simulation(sim_jobs, job_count, "Non-preemptive Priority", 0, 0);

    // 3. Round Robin Scheduling (Quantum = 16)
    reset_jobs(original_jobs, sim_jobs, job_count);
    run_simulation(sim_jobs, job_count, "Round Robin (q=16)", 1, 16);

    return 0;
}
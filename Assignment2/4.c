#include <stdio.h>
#include <stdlib.h>

#define MAX_JOBS 8
#define MAX_BURSTS 20

typedef struct {
    int id, priority, arrival;
    int cpu_bursts[MAX_BURSTS];
    int io_bursts[MAX_BURSTS];
    int burst_count;
    int total_cpu_time;
    int waiting_time, turnaround_time;
} Job;

void parse_jobs(const char* filename, Job jobs[], int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) { perror("File opening failed"); exit(1); }

    *count = 0;
    while (*count < MAX_JOBS && !feof(file)) {
        Job* j = &jobs[*count];
        if (fscanf(file, "%d %d %d", &j->id, &j->priority, &j->arrival) != 3) break;
        
        j->burst_count = 0;
        j->total_cpu_time = 0;
        int val;
        while (fscanf(file, "%d", &val) == 1 && val != -1) {
            if (j->burst_count % 2 == 0) {
                j->cpu_bursts[j->burst_count / 2] = val;
                j->total_cpu_time += val;
            } else {
                j->io_bursts[j->burst_count / 2] = val;
            }
            j->burst_count++;
        }
        (*count)++;
    }
    fclose(file);
}

void simulate_fcfs(Job jobs[], int count) {
    int current_time = 0;
    printf("\n--- FCFS Scheduling ---\n");
    for (int i = 0; i < count; i++) {
        if (current_time < jobs[i].arrival) current_time = jobs[i].arrival;
        jobs[i].waiting_time = current_time - jobs[i].arrival;
        current_time += jobs[i].total_cpu_time;
        jobs[i].turnaround_time = jobs[i].waiting_time + jobs[i].total_cpu_time;
        printf("Job %d: Wait = %d, Turnaround = %d\n", jobs[i].id, jobs[i].waiting_time, jobs[i].turnaround_time);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) { printf("Usage: %s <job_file>\n", argv[0]); return 1; }
    
    Job jobs[MAX_JOBS];
    int job_count = 0;
    parse_jobs(argv[1], jobs, &job_count);
    
    simulate_fcfs(jobs, job_count);
    // Non-preemptive Priority and RR (time slice: 16) would be simulated here
    return 0;
}
/* Create child processes: X and Y.
a. Each child process performs N iterations (N >= 10, being an input
to be provided by user). The child processes display their name/id
and the current iteration number, and sleep for some random amount
of time. Adjust the sleeping duration of the processes to have a
different output (i.e. another interleaving of processes’ traces).
b. Modify the program so that process Y is not allowed to start
iteration i before process X has terminated its own iteration i-1. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <process.h>
#else
#endif
#include <time.h>

int main() {
	int n;
	printf("Enter N (>= 10): ");
	scanf("%d", &n);
	if (n < 10) {
		printf("N must be >= 10\n");
		exit(1);
	}

	pid_t pid_x = fork();
	if (pid_x < 0) {
		perror("fork X");
		exit(1);
	} else if (pid_x == 0) {
		// ---------- Process X ----------
		srand(time(NULL) ^ getpid()); // seed differently per process
		for (int i = 1; i <= n; i++) {
			printf("Process X (pid %d): iteration %d\n", getpid(), i);
			fflush(stdout);
			sleep(rand() % 3 + 1); // sleep 1-3 seconds
		}
		exit(0);
	}

	pid_t pid_y = fork();

	if (pid_y < 0) {
		perror("fork Y");
		exit(1);
	} else if (pid_y == 0) {
		// ---------- Process Y ----------
		srand(time(NULL) ^ getpid());
		for (int i = 1; i <= n; i++) {
			printf("Process Y (pid %d): iteration %d\n", getpid(), i);
			fflush(stdout);
			sleep(rand() % 4 + 1); // sleep 1-4 seconds (different range than X)
		}
		exit(0);
	}

	// ---------- Parent ----------
	wait(NULL);
	wait(NULL);
	return 0;
}
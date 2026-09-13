/* Consider a pair of processes X and Y, which share the variables x
and y. Process X writes x and reads y while process Y writes y and
reads x. Y must not read x until X has written to it and X must not
read y until Y has written to it. Implement this using pipe/s. */

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <process.h>
#else

#endif

int main() {
    int pipe_x[2]; // X -> Y : carries x
    int pipe_y[2]; // Y -> X : carries y

    if (pipe(pipe_x) == -1) {
        perror("pipe_x");
        exit(1);
    }
    if (pipe(pipe_y) == -1) {
        perror("pipe_y");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // ---------- Child: Process Y ----------
        close(pipe_x[1]); // Y doesn't write to pipe_x
        close(pipe_y[0]); // Y doesn't read from pipe_y

        int x, y;

        read(pipe_x[0], &x, sizeof(x));
        printf("Process Y: read x = %d\n", x);

        y = 99; // Y computes the value of y
        write(pipe_y[1], &y, sizeof(y));
        printf("Process Y: wrote y = %d\n", y);

        close(pipe_x[0]);
        close(pipe_y[1]);
        exit(0);
    } else {
        // ---------- Parent: Process X ----------
        close(pipe_x[0]); // X doesn't read from pipe_x
        close(pipe_y[1]); // X doesn't write to pipe_y

        int x, y;

        x = 42; // X computes/decides the value of x
        write(pipe_x[1], &x, sizeof(x));
        printf("Process X: wrote x = %d\n", x);

        read(pipe_y[0], &y, sizeof(y));
        printf("Process X: read y = %d\n", y);

        close(pipe_x[1]);
        close(pipe_y[0]);
        wait(NULL); // wait for Y to finish
    }

    return 0;
}
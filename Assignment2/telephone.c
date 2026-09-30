#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int caller_to_receiver[2];
    int receiver_to_caller[2];

    pid_t pid;

    /*
     * Create first pipe
     */
    if (pipe(caller_to_receiver) == -1)
    {
        perror("pipe");
        return 1;
    }

    /*
     * Create second pipe
     */
    if (pipe(receiver_to_caller) == -1)
    {
        perror("pipe");
        return 1;
    }

    /*
     * Create child process
     */
    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    /*
     * ============================
     * PARENT = CALLER
     * ============================
     */
    if (pid > 0)
    {
        char message[256];
        char reply[256];

        /*
         * Caller does not read from
         * caller_to_receiver.
         */
        close(caller_to_receiver[0]);

        /*
         * Caller does not write to
         * receiver_to_caller.
         */
        close(receiver_to_caller[1]);

        printf("Caller connected.\n");

        while (1)
        {
            printf("\nCaller: ");
            fgets(message, sizeof(message), stdin);

            message[strcspn(message, "\n")] = '\0';

            /*
             * Send message to receiver
             */
            write(
                caller_to_receiver[1],
                message,
                strlen(message) + 1
            );

            if (strcmp(message, "exit") == 0)
                break;

            /*
             * Wait for receiver's reply
             */
            read(
                receiver_to_caller[0],
                reply,
                sizeof(reply)
            );

            printf("Receiver: %s\n", reply);

            if (strcmp(reply, "exit") == 0)
                break;
        }

        close(caller_to_receiver[1]);
        close(receiver_to_caller[0]);

        wait(NULL);
    }

    /*
     * ============================
     * CHILD = RECEIVER
     * ============================
     */
    else
    {
        char message[256];
        char reply[256];

        /*
         * Receiver does not write to
         * caller_to_receiver.
         */
        close(caller_to_receiver[1]);

        /*
         * Receiver does not read from
         * receiver_to_caller.
         */
        close(receiver_to_caller[0]);

        printf("Receiver connected.\n");

        while (1)
        {
            /*
             * Receive caller's message
             */
            read(
                caller_to_receiver[0],
                message,
                sizeof(message)
            );

            printf("\nCaller: %s\n", message);

            if (strcmp(message, "exit") == 0)
                break;

            printf("Receiver: ");
            fgets(reply, sizeof(reply), stdin);

            reply[strcspn(reply, "\n")] = '\0';

            /*
             * Send reply to caller
             */
            write(
                receiver_to_caller[1],
                reply,
                strlen(reply) + 1
            );

            if (strcmp(reply, "exit") == 0)
                break;
        }

        close(caller_to_receiver[0]);
        close(receiver_to_caller[1]);
    }

    return 0;
}
// to run => gcc telephone.c -o telephone
// gcc telephone.c -o telephone
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "message.h"

int main(int argc, char *argv[])
{
    key_t key;
    int msgid;
    int listener_id;

    /*
     * Check command line argument
     */
    if (argc != 2)
    {
        printf("Usage: %s <listener_id>\n", argv[0]);
        return 1;
    }

    listener_id = atoi(argv[1]);

    if (listener_id < 1 || listener_id > 3)
    {
        printf("Listener ID must be 1, 2 or 3.\n");
        return 1;
    }

    /*
     * Generate same key as broadcaster
     */
    key = ftok("message.h", 65);

    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    /*
     * Get the existing message queue
     */
    msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    printf("Listener %d started.\n", listener_id);

    while (1)
    {
        struct message msg;

        /*
         * Receive broadcast message.
         *
         * Type 1 = broadcast
         */
        if (msgrcv(
                msgid,
                &msg,
                sizeof(msg.message_text),
                1,
                0) == -1)
        {
            perror("msgrcv");
            break;
        }

        printf(
            "\nBroadcast received: %s\n",
            msg.message_text
        );

        if (strcmp(msg.message_text, "exit") == 0)
            break;

        /*
         * Prepare reply
         */
        struct message reply;

        /*
         * Listener 1 -> type 2
         * Listener 2 -> type 3
         * Listener 3 -> type 4
         */
        reply.message_type = listener_id + 1;

        printf("Listener %d reply: ", listener_id);

        fgets(
            reply.message_text,
            sizeof(reply.message_text),
            stdin
        );

        reply.message_text[
            strcspn(reply.message_text, "\n")
        ] = '\0';

        /*
         * Send reply
         */
        if (msgsnd(
                msgid,
                &reply,
                sizeof(reply.message_text),
                0) == -1)
        {
            perror("msgsnd");
            break;
        }
    }

    printf("Listener %d terminated.\n", listener_id);

    return 0;
}
// gcc mq_broadcaster.c -o mq_broadcaster
// gcc mq_listener.c -o mq_listener
// ./mq_listener 1, 2, 3
// ./mq_broadcaster
// ./mq_broadcaster
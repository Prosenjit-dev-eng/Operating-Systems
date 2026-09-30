#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "message.h"


int main(){
    key_t key;
    int msgid;

    struct message msg;

    // generate unique key
    key = fork("message.h", 65);

    if (key == -1)
    {
        perror("fork");
        return 1;
    }
    // Create a message queue
    msgid = msgget(key,0666 | IPC_CREAT);//msgget(): This is the OS function that retrieves the identifier for a message queue.
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    printf("Broadcaster started.\n");

    while (1)
    {
        printf("\nBroadcast message: ");

        fgets(
            msg.message_text,
            sizeof(msg.message_text),
            stdin
        );

        msg.message_text[
            strcspn(msg.message_text, "\n")
        ] = '\0';

        if (strcmp(msg.message_text, "exit") == 0)
            break;

        /*
         * Type 1 means broadcast
         */
        msg.message_type = 1;

        /*
         * Send message to queue
         */
        if (msgsnd(
                msgid,
                &msg,
                sizeof(msg.message_text),
                0) == -1)
        {
            perror("msgsnd");
            break;
        }

        printf("Message sent to all listeners.\n");

        /*
         * Receive replies from listeners.
         *
         * Listener 1 -> type 2
         * Listener 2 -> type 3
         * Listener 3 -> type 4
         */

        for (long type = 2; type <= 4; type++)
        {
            struct message reply;

            if (msgrcv(
                    msgid,
                    &reply,
                    sizeof(reply.message_text),
                    type,
                    0) == -1)
            {
                perror("msgrcv");
                break;
            }

            printf(
                "Received from Listener %ld: %s\n",
                type - 1,
                reply.message_text
            );
        }
    }

    /*
     * Remove message queue
     */
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
    }

    printf("Broadcaster terminated.\n");

    return 0;
    
}
// A single FIFO is a stream. If multiple listeners read from the same FIFO, they can consume different parts of the stream, rather than each receiving every broadcast.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO1 "/tmp/weather1"
#define FIFO2 "/tmp/weather2"
#define FIFO3 "/tmp/weather3"


int main(){
    int fd1, fd2, fd3;
    char weather[256];

    // create 3 named pipes(FIFOS)
    mkfifo(FIFO1,0666);
    mkfifo(FIFO2,0666);
    mkfifo(FIFO3,0666);
    /*FIFO1: This variable represents the file path and name where the named pipe will be created on the filesystem. Unlike standard pipes, named pipes exist as physical files so that unrelated processes can find and connect to them.
    0666: This sets the file permissions (read and write access for the owner, group, and others), though the final permissions are determined by the system's*/

    printf("Weather Broadcaster started.\n");
    printf("Waiting for listeners....\n");

    /*
     * Open all FIFOs for writing.
     *
     * open() will wait until a listener opens
     * the corresponding FIFO for reading.
     */
    // int fd1 (File Descriptor): In C and UNIX-like operating systems, the open() system call returns a file handle for subsequent use. This handle, known as a file descriptor, is represented simply as an integer (int).
    /*O_WRONLY (Write-Only Mode): This is a flag that stands for "Open Write-Only." It tells the operating system that your process only intends to send data into this FIFO, not read from it. Since a single FIFO acts as a one-way street for data, the sending process (like your Broadcaster or Caller) opens it with O_WRONLY, while the receiving process on the other end must open the exact same FIFO with O_RDONLY (Read-Only) to establish the connection.*/
    fd1 = open(FIFO1,O_WRONLY);
    fd2 = open(FIFO2,O_WRONLY);
    fd3 = open(FIFO3,O_WRONLY);

    if (fd1 == -1 || fd2 == -1 || fd3 == -1)
    {
        perror("Error opening FIFO");
        return 1;
    }

    while (1)
    {
        printf("\nEnter weather information: ");
        fgets(weather, sizeof(weather), stdin);
        // Remove newline
        weather[strcspn(weather,"\n")] = '\0';

        if (strcmp(weather, "exit") == 0)
        {
            break;
        }

        // Send the same message to all listeners
        write(fd1,weather,strlen(weather)+1);
        write(fd2,weather,strlen(weather)+1);
        write(fd3, weather, strlen(weather)+1);

        printf("Weather information broadcast\n");
    }

    close(fd1);
    close(fd2);
    close(fd3);

    // Remove fifos
    unlink(FIFO1);
    unlink(FIFO2);
    unlink(FIFO3);

    printf("Broadcaster terminated\n");
}




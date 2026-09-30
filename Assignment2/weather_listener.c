#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[]){
    int fd;
    char buffer[256];
    char fifo_name[100];

    if (argc != 2)
    {
        printf("Usage: %s <listener_number>\n", argv[0]);
        return 1;
    }

    /*
     * Construct FIFO name
     *
     * Listener 1 -> /tmp/weather1
     * Listener 2 -> /tmp/weather2
     * Listener 3 -> /tmp/weather3
     */

    sprintf(fifo_name, "/tmp/weather%s",argv[1]);

    fd = open(fifo_name, O_RDONLY);

    if (fd == -1)
    {
        perror("Error opening FIFO");
        return 1;
    }

    printf("Listener %s started.\n",argv[1]);

    while (1)
    {
        int bytes_read;

        bytes_read = read(fd, buffer, sizeof(buffer));

        if (bytes_read <= 0)
            break;

        printf("\n[Listener %s] Weather: %s\n",
               argv[1], buffer);

        if (strcmp(buffer, "exit") == 0)
            break;
    }

    close(fd);
    
    printf("Listener %s terminated.\n", argv[1]);

    return 0;
    
}
// gcc weather_broadcaster.c -o weather_broadcaster
// gcc weather_listener.c -o weather_listener

// ./weather_listener 1, ./weather_listener 2 .., ./weather_listener 1
// Enter Temperature: 30 C, Humidity: 70%
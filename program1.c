#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define MAX_BUF 1024

int main()
{
    int queue_fd[2], file_fd;
    char filename[MAX_BUF];
    pid_t pid;

    // create pipe for communication with queue
    if (pipe(queue_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // fork a child process for the queue
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // child process - queue
        close(queue_fd[1]); // close unused write end of queue pipe
        while (1) {
            // read from input pipe
            int n = read(queue_fd[0], filename, MAX_BUF);
            if (n == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            } else if (n == 0) {
                // input process has closed the pipe
                break;
            }
            // add file to queue
            if ((file_fd = open(filename, O_RDONLY)) == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            write(file_fd, "1", 1); // mark file as unread
            close(file_fd);
        }
        // close remaining pipe
        close(queue_fd[0]);
        exit(EXIT_SUCCESS);
    } else {
        // parent process - input
        close(queue_fd[0]); // close unused read end of queue pipe
        while (1) {
            printf("Enter filename (or 'q' to quit): ");
            fgets(filename, MAX_BUF, stdin);
            filename[strcspn(filename, "\n")] = '\0'; // remove newline character
            if (strcmp(filename, "q") == 0) {
                break;
            }
            // add file to queue
            write(queue_fd[1], filename, strlen(filename) + 1);
        }
        // close pipe to queue
        close(queue_fd[1]);
        // wait for queue to exit
        wait(NULL); // wait for child process to exit
        exit(EXIT_SUCCESS);
    }
}

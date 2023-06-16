#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define MAX_QUEUE_SIZE 10

void print_file(char *filename);
void handler(int sig);

char queue[MAX_QUEUE_SIZE][100];
int front = -1, rear = -1;

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2, pid3;
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe error");
        exit(EXIT_FAILURE);
    }
    pid1 = fork();
    if (pid1 == -1) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid1 == 0) {
        // Child process 1
        close(pipe1[0]); // Close read end of pipe 1
        close(pipe2[0]); // Close read end of pipe 2
        close(pipe2[1]); // Close write end of pipe 2
        char filename[100];
        while (1) {
            printf("Enter filename to print (or 'exit' to quit): ");
            scanf("%s", filename);
            if (strcmp(filename, "exit") == 0) {
                // Send message to process 2 to exit
                write(pipe1[1], "exit", sizeof("exit"));
                break;
            }
            else {
                // Send filename to process 2
                write(pipe1[1], filename, sizeof(filename));
            }
        }
        close(pipe1[1]); // Close write end of pipe 1
        exit(EXIT_SUCCESS);
    }
    else {
        pid2 = fork();
        if (pid2 == -1) {
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0) {
            // Child process 2
            close(pipe1[1]); // Close write end of pipe 1
            close(pipe2[0]); // Close read end of pipe 2
            while (1) {
                char filename[100];
                read(pipe1[0], filename, sizeof(filename));
                if (strcmp(filename, "exit") == 0) {
                    break;
                }
                else {
                    if (rear == MAX_QUEUE_SIZE - 1) {
                        printf("Queue full\n");
                    }
                    else {
                        if (front == -1) {
                            front = 0;
                        }
                        rear++;
                        strcpy(queue[rear], filename);
                    }
                }
            }
            close(pipe1[0]); // Close read end of pipe 1
            close(pipe2[1]); // Close write end of pipe 2
            // Send signal to process 3 to print files
            while (front != -1) {
                kill(pid3, SIGUSR1);
                pause();
                front++;
                if (front > rear) {
                    front = rear = -1;
                }
            }
            // Send signal to process 3 to exit
            kill(pid3, SIGTERM);
            exit(EXIT_SUCCESS);
        }
        else {
            pid3 = fork();
            if (pid3 == -1) {
                perror("Fork error");
                exit(EXIT_FAILURE);
            }
            else if (pid3 == 0) {
                // Child process 3
                close(pipe1[0]); // Close read end of pipe 1
                close(pipe1[1]); // Close write end of pipe 1
                close(pipe2[1]); // Close write end 
		signal(SIGUSR1, handler); // Register signal handler for SIGUSR1
		signal(SIGTERM, handler); // Register signal handler for SIGTERM
	while (1) {
	pause();
	}
	exit(EXIT_SUCCESS);
	}
	else {
// Parent process
close(pipe1[0]); // Close read end of pipe 1
close(pipe1[1]); // Close write end of pipe 1
close(pipe2[0]); // Close read end of pipe 2
while (1) {
char filename[100];
if (front != -1) {
strcpy(filename, queue[front]);
print_file(filename);
}
else {
// Wait for signal from process 2
pause();
}
}
close(pipe2[1]); // Close write end of pipe 2
// Wait for child processes to exit
wait(NULL);
wait(NULL);
wait(NULL);
exit(EXIT_SUCCESS);
}
}
}
}

void print_file(char *filename) {
FILE *fp = fopen(filename, "r");
if (fp == NULL) {
printf("File not found: %s\n", filename);
return;
}
char c;
printf("Printing file: %s\n", filename);
while ((c = fgetc(fp)) != EOF) {
putchar(c);
fflush(stdout);
usleep(50000); // Delay to slow down printing
}
fclose(fp);
}

void handler(int sig) {
if (sig == SIGUSR1) {
// Print next file in queue
printf("Printing next file...\n");
}
else if (sig == SIGTERM) {
// Exit process
printf("Exiting process 3...\n");
exit(EXIT_SUCCESS);
}
}

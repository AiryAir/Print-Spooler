#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_QUEUE_SIZE 10
#define MAX_FILE_SIZE 100

void spooler(int read_fd);
void sender(char *filename, int write_fd);

int main()
{
    int pipe1[10], pipe2[10], pipe3[10];

    // Create the pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1)
    {
        perror("Failed to create pipes");
        exit(EXIT_FAILURE);
    }

    // Fork the sender processes
    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("Failed to fork sender process");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process (sender)
            close(pipe1[0]); // Close read end of pipe1
            close(pipe2[0]); // Close read end of pipe2
            close(pipe3[0]); // Close read end of pipe3

            sender("file.txt", pipe1[1]); // Send file to spooler through pipe1
            exit(EXIT_SUCCESS);
        }
    }

    // Fork the spooler process
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Failed to fork spooler process");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process (spooler)
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2
        close(pipe3[0]); // Close read end of pipe3

        spooler(pipe1[0]); // Read from pipe1 and print to screen
        exit(EXIT_SUCCESS);
    }

    // Fork another sender process
    pid = fork();

    if (pid == -1)
    {
        perror("Failed to fork sender process");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process (sender)
        close(pipe1[0]); // Close read end of pipe1
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2
        close(pipe3[0]); // Close read end of pipe3

        sender("another_file.txt", pipe2[1]); // Send another file to spooler through pipe2
        exit(EXIT_SUCCESS);
    }

    // Fork yet another sender process
    pid = fork();

    if (pid == -1)
    {
        perror("Failed to fork sender process");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process (sender)
        close(pipe1[0]); // Close read end of pipe1
        close(pipe1[1]); // Close write end of pipe1
        close(pipe2[0]); // Close read end of pipe2
        close(pipe2[1]); // Close write end of pipe2
        close(pipe3[0]); // Close read end of pipe3

        sender("third_file.txt", pipe3[1]); // Send another file to spooler through pipe3
        exit(EXIT_SUCCESS);
    }

    // Parent process
    close(pipe1[0]); // Close read end of pipe1

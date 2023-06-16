#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_STRINGS 10
#define MAX_STRING_LEN 50

int pipe_fd[2];
char strings[MAX_STRINGS][MAX_STRING_LEN];
int strings_count = 0;

void process_one() {
    char input_string[MAX_STRING_LEN];
    printf("Enter a string: ");
    fgets(input_string, MAX_STRING_LEN, stdin);
    input_string[strcspn(input_string, "\n")] = '\0'; // Remove the newline character
    write(pipe_fd[1], input_string, MAX_STRING_LEN);
    exit(0);
}

void process_two() {
    char received_string[MAX_STRING_LEN];
    read(pipe_fd[0], received_string, MAX_STRING_LEN);
    strncpy(strings[strings_count], received_string, MAX_STRING_LEN);
    strings_count++;
    printf("Printing all strings:\n");
    printf("Array: \n");
    for (int i = 0; i < strings_count; i++) {
        printf("%s\n", strings[i]);
    }
    exit(0);
}

void process_three() {
    printf("Printing all strings:\n");
    printf("Array: \n");
    for (int i = 0; i < strings_count; i++) {
        printf("%s\n", strings[i]);
    }
    exit(0);
}

int main() {
    pid_t pid1, pid2, pid3;
    int status;

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create process one
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    }
    if (pid1 == 0) {
        process_one();
    }

    // Create process two
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(1);
    }
    if (pid2 == 0) {
        process_two();
    }

    // Create process three
    pid3 = fork();
    if (pid3 == -1) {
        perror("fork");
        exit(1);
    }
    if (pid3 == 0) {
        process_three();
    }

    // Wait for all child processes to finish
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    // We use two pipes
    // First pipe to send input string from parent
    // Second pipe to send reversed string from child

    int fd1[2]; // Used to store two ends of first pipe
    int fd2[2]; // Used to store two ends of second pipe

    char input_str[100];
    pid_t p;  // output of the fork call

    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    printf("Enter a string: ");
    scanf("%s", input_str);
    p = fork();

    if (p < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    // Parent process
    else if (p > 0) {
        char rev_str[100];

        close(fd1[0]); // Close reading end of first pipe

        // Write input string and close writing end of first
        // pipe.
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        // Wait for child to send reversed string
        wait(NULL);

        close(fd2[1]); // Close writing end of second pipe

        // Read reversed string from child, print it and close
        // reading end.
        read(fd2[0], rev_str, 100);
        printf("Reversed string: %s\n", rev_str);
        close(fd2[0]);
    }

    // child process
    else {
        close(fd1[1]); // Close writing end of first pipe

        // Read a string using first pipe
        char rev_str[100];
        read(fd1[0], rev_str, 100);

        // Reverse the string
        int len = strlen(rev_str);
        for (int i = 0; i < len / 2; i++) {
            char temp = rev_str[i];
            rev_str[i] = rev_str[len - i - 1];
            rev_str[len - i - 1] = temp;
        }

        // Close both reading ends
        close(fd1[0]);
        close(fd2[0]);

        // Write reversed string and close writing end
        write(fd2[1], rev_str, strlen(rev_str) + 1);
        close(fd2[1]);

        exit(0);
    }

    return 0;
}

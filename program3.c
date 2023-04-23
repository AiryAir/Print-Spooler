#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 100

typedef struct file_node {
    char filename[MAX_FILENAME_LENGTH];
    struct file_node* next;
} file_node_t;

void sig_handler(int signo);
void print_files();

file_node_t* head = NULL;
file_node_t* tail = NULL;

int main() {
    signal(SIGUSR1, sig_handler);

    printf("Program 3 started\n");

    while (1) {
        sleep(1); // wait for signal
    }

    return 0;
}

void sig_handler(int signo) {
    if (signo == SIGUSR1) {
        print_files();
    }
}

void print_files() {
    if (head == NULL) {
        printf("No files to print\n");
        return;
    }

    file_node_t* current = head;
    while (current != NULL) {
        printf("Printing file %s\n", current->filename);
        FILE* file = fopen(current->filename, "r");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char line[256];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
            usleep(10000); // add a small delay between lines for simulation purposes
        }

        fclose(file);

        file_node_t* temp = current;
        current = current->next;
        head = current;
        free(temp);

        printf("File %s printed successfully\n", temp->filename);
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define MAX_FILENAME_LENGTH 100

typedef struct file_node {
    char filename[MAX_FILENAME_LENGTH];
    struct file_node* next;
} file_node_t;

void sig_handler(int signo);
void add_file_to_queue(char* filename);

file_node_t* head = NULL;
file_node_t* tail = NULL;

int main() {
    signal(SIGUSR1, sig_handler);

    printf("Program 2 started\n");

    while (1) {
        sleep(1); // wait for signal
    }

    return 0;
}

void sig_handler(int signo) {
    if (signo == SIGUSR1) {
        char filename[MAX_FILENAME_LENGTH];
        read(STDIN_FILENO, filename, MAX_FILENAME_LENGTH);
        add_file_to_queue(filename);
    }
}

void add_file_to_queue(char* filename) {
    file_node_t* new_node = malloc(sizeof(file_node_t));
    if (new_node == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(new_node->filename, filename);
    new_node->next = NULL;

    if (head == NULL) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }

    printf("Added file %s to queue\n", filename);
}

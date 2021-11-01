#include<stdlib.h>
#include<stdio.h>
struct Node {
    char *word;
    struct Node *next;
};

struct Node *allocate() {
    struct Node *n = malloc(sizeof(struct Node));
    n->word = 0;
    n->next = 0;
    return n;
}

void frees(struct Node* front) {
    struct Node *ptr = front;
    struct Node *prev = NULL;
    while (ptr != 0) {
        prev = ptr;
        ptr = ptr->next;
        free(prev->word);
        free(prev);
    }
}

void printer(struct Node *front) {
    for (struct Node *ptr = front; ptr->next != 0; ptr = ptr->next) {
        printf("%s", ptr->word);
    }
}
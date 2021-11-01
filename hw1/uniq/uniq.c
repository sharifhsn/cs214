#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "linkedlist.h"

int main(int argc, char* argv[]) {
    size_t siz = 0;

    struct Node *front = allocate();
    for (struct Node *ptr = front; getline(&(ptr->word), &siz, stdin) != EOF; ptr = ptr->next) {
        ptr->next = allocate();
    }

    int count = 1;
    for (struct Node *ptr = front; ptr->next != 0; ptr = ptr->next) {
        if (strcmp(ptr->word, ptr->next->word) == 0) {
            ++count;
        } else {
            printf("%d %s", count, ptr->word);
            count = 1;
        }
    }

    frees(front);
    return 0;
}
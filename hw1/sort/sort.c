#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "linkedlist.h"


struct Node *sort_str(struct Node *front) {

    while (front->next != 0) {
        //printer(front);
        struct Node *lowest = front;
        for (struct Node *ptr = front; ptr->next != 0; ptr = ptr->next) {
            if (strcmp(lowest->word, ptr->word) > 0) {
                lowest = ptr;
            }
        }
        printf("%s", lowest->word);
        front = rm_node(front, lowest);
    }
    // struct Node *sfront = allocate();
    // char *lowest = 0;
    // strcpy(lowest, front->word);
    // for (struct Node *sptr = sfront; sptr; sptr = sptr->next) {
    //     char *firs = 0;
    //     strcpy(firs, lowest);
    //     for (struct Node *ptr = front; ptr->next != 0; ptr = ptr->next) {
    //         if (strcmp(lowest, firs) == 0) {
                
    //         }
    //     }
    //     sptr->next = allocate();
    // }
    return front;
}


int main(int argc, char *argv[]) {
    size_t n = 0;
    struct Node *front = allocate();

    int num = 0;

    for (struct Node *ptr = front; getline(&(ptr->word), &n, stdin) != EOF; ptr = ptr->next) {
        ptr->next = allocate();
    }

    front = sort_str(front);

    frees(front);
    return 0;
}
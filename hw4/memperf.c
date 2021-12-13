#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include "mymalloc.h"

typedef struct Node {
    void *p;
    struct Node *next;
} Node;

void freenode(Node *front) {
    for (Node *ptr = front; ptr != NULL;) {
        Node *prev = ptr;
        ptr = ptr->next;
        free(prev);
    }
}

void printer(Node *front) {
    for (Node *ptr = front; ptr != NULL; ptr = ptr->next) {
        printf("%p -> ", ptr->p);
    }
}

double util(double *ops, int alloc_alg) {
    clock_t beg = clock();
    int nops = 500000;

    Node *front = malloc(sizeof(Node));
    front->p = NULL;
    front->next = malloc(sizeof(Node));
    Node *ptr = front->next;
    ptr->p = NULL;
    ptr->next = NULL;
    myinit(alloc_alg);
    for (int i = 0; i < nops; i++) {
        int sz = rand() % 256 + 1;
        int r = rand() % 3;
        if (r == 0) {
            // mymalloc
            //printf("ptr: %p\n", ptr);
            void *h = mymalloc(sz);
            //printf("malloced %p\n", h);
            if (h != NULL) {
                ptr->next = malloc(sizeof(Node));
                ptr->next->next = NULL;
                ptr->next->p = h;
                ptr = ptr->next;
            }
        } else if (r == 1) {
            // myfree
            for (Node *aptr = front; aptr != NULL && aptr->next != NULL; aptr = aptr->next) {
                if (aptr->next->next == NULL && aptr->next->p != NULL) {
                    //printf("freeing %p\n", aptr->next->p);
                    myfree(aptr->next->p);
                    Node *x = aptr->next;
                    free(x);
                    aptr->next = NULL;
                    ptr = aptr;
                    break;
                }
            }
        } else {
            // myrealloc
            for (Node *ptr = front->next; ptr != NULL; ptr = ptr->next) {
                if (ptr->next == NULL) {
                    ptr->p = myrealloc(ptr->p, sz);
                }
            }
        }
        //printer(front);
    }
    freenode(front);
    // printf("utilization: %f\n", utilization());
    double u = utilization();
    mycleanup();

    clock_t end = clock();
    *ops = (nops * 1.0) / (((end - beg) * 1.0) / CLOCKS_PER_SEC);
    return u;
}

int main() {
    srand(time(NULL));
    double ops = 0.0;
    double u = 0.0;
    u = util(&ops, 0);
    printf("First fit throughput: %f ops/sec\nFirst fit utilization: %f\n", ops, u);
    u = util(&ops, 1);
    printf("Next fit throughput: %f ops/sec\nNext fit utilization: %f\n", ops, u);
    u = util(&ops, 2);
    printf("Best fit throughput: %f ops/sec\nBest fit utilization: %f\n", ops, u);
}
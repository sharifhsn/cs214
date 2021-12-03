#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"

int main() {
    myinit(0);
    int *front = mymalloc(sizeof(int));
    *front = 112;
    printf("alloc is %p\n", front);
    printf("val is %d\n", *front);
    myfree(front);
    int *nptr = mymalloc(sizeof(unsigned long));
    *nptr = 67;
    printf("for real it's %p %lu\n", nptr, *nptr);
    mycleanup();
}
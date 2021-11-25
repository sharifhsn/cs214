#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"

int main() {
    myinit(0);
    void *front = mymalloc(64);
    printf("%p\n", front);
    mycleanup();
}
#include <stdio.h>
#include <stdlib.h>

// recursively print the factors of a number
void smallest_factor(long l) {
    for (long i = 2; i <= l; i++) {
        if (l % i == 0) {
            printf("%ld ", i);
            smallest_factor(l / i);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    // argument parsing
    if (argc < 2) {
        printf("insufficient arguments!\n");
        return 0;
    }
    char *ptr;
    long l = strtol(argv[1], &ptr, 10);

    smallest_factor(l);
    printf("\n");

    return 0;
}
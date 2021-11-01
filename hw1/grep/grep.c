#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

// makes a string lowercase
void lowercase(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = tolower(s[i]);
    }
}

int main(int argc, char *argv[]) {
    // argument parsing
    int sens = 0;
    if (argc < 2) {
        printf("insufficient arguments\n");
        return 0;
    }
    if (strcmp(argv[1], "-i") == 0) {
        sens = 1;
    }

    // creating the pattern
    int ind = 1;
    if (sens) {
        ind = 2;
    }
    char *p = malloc(sizeof(argv[ind]));
    if (sens) {
        strcpy(p, argv[2]);
        lowercase(p);
        //printf("case sensitive %s\n", p);

    } else {
        strcpy(p, argv[1]);
        //printf("%s\n", p);
    }
    
    size_t siz = 0;
    char *hay = NULL;
    while (getline(&hay, &siz, stdin) != EOF) {
        int chk = 1;
        for (int i = 0; i < siz; i++) {
            chk = 1;
            if (p[0] == hay[i]) {
                //printf("%c matches at %d!\n", p[0], i);
                for (int j = 0; hay[j + i] && isalnum(p[j]); j++) {
                    //printf("index is %d\n", j);
                    //printf("comparing %c and %c\n", p[j], hay[j + i]);
                    if (p[j] != hay[j + i]) {
                        chk = 0;
                        break;
                    }
                }
                if (chk) {
                    printf("%s", hay);
                    break;
                }
            }
        }
        //printf("%s", hay);
    }


    // clean up
    free(p);
    free(hay);
    return 0;
}
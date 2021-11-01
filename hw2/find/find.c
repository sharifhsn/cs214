#include <stdio.h>
#include <string.h>
#include <fts.h>

int main(int argc, char *argv[]) {
    // argument handling
    char *search_dir = ".";
    if (argc < 2) {
        printf("insufficient arguments!\n");
        return 0;
    }
    if (argc >= 3) {
        search_dir = argv[2];
    }

    // open a fts file tree in the current directory
    char *c[] = {search_dir, 0};
    FTS *f = fts_open(c, FTS_PHYSICAL, 0);
    FTSENT *fe;

    // read through file tree until null
    while ((fe = fts_read(f)) != 0) {
        // if the pattern is a substring of the file/dir name, it is printed
        if (strstr(fe->fts_path, argv[1])) {
            printf("%s\n", fe->fts_path);
        }
    }
    fts_close(f);
    return 0;
}
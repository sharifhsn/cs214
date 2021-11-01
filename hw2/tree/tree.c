#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// case insensitive comparator for scandir
int comp_dir(const struct dirent **a, const struct dirent **b) {
    return strcasecmp((*a)->d_name, (*b)->d_name);
}

void look_dir(char *path, int depth) {
    struct dirent **de;
    struct stat s;
    int n = scandir(path, &de, 0, comp_dir);

    for (size_t i = 0 ; i < n; i++) {
        if (strcmp(de[i]->d_name, ".") != 0 && strcmp(de[i]->d_name, "..") != 0) {
            for (size_t i = 0; i < depth; ++i) {
                printf("  ");
            }
            printf("- %s\n", de[i]->d_name);
            char np[512];
            snprintf(np, sizeof(np), "%s/%s", path, de[i]->d_name);
            stat(np, &s);
            if ((s.st_mode & __S_IFDIR) == __S_IFDIR) {
                look_dir(np, depth + 1);
            }
        }
        free(de[i]);
    }
    free(de);
}

int main(int argc, char *argv[]) {
    char *search_dir = ".";
    if (argc >= 2) {
        search_dir = argv[1];
    }
    char buf[256];
    snprintf(buf, sizeof(buf), "%s", search_dir);
    printf(".\n");
    look_dir(buf, 0);
}
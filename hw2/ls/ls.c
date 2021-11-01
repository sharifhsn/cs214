#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>

// case insensitive comparator for scandir
int comp_dir(const struct dirent **a, const struct dirent **b) {
    return strcasecmp((*a)->d_name, (*b)->d_name);
}

// generate permissions string from st_mode
void perms(char s[], mode_t m) {
    s[0] = (m & S_IFDIR) == S_IFDIR ? 'd' : '-';
    s[1] = (m & S_IRUSR) == S_IRUSR ? 'r' : '-';
    s[2] = (m & S_IWUSR) == S_IWUSR ? 'w' : '-';
    s[3] = (m & S_IXUSR) == S_IXUSR ? 'x' : '-';
    s[4] = (m & S_IRGRP) == S_IRGRP ? 'r' : '-';
    s[5] = (m & S_IWGRP) == S_IWGRP ? 'w' : '-';
    s[6] = (m & S_IXGRP) == S_IXGRP ? 'x' : '-';
    s[7] = (m & S_IROTH) == S_IROTH ? 'r' : '-';
    s[8] = (m & S_IWOTH) == S_IWOTH ? 'w' : '-';
    s[9] = (m & S_IXOTH) == S_IXOTH ? 'x' : '-';
    s[10] = '\0';
}

int main(int argc, char *argv[]) {
    // command line arguments
    int l = 0;
    if (argc < 2) {
        printf("insufficient arguments!\n");
        return 0;
    }
    char *dir = argv[1];
    if (strcmp(argv[1], "-l") == 0) {
        if (argc < 3) {
            printf("insufficient arguments!\n");
            return 0;
        }
        l = 1;
        dir = argv[2];
    }

    // necessary objects/buffers
    struct dirent **de;
    struct stat s;
    char name[257];
    char time[13];
    char per[11];
    struct passwd *p;
    struct group *g;

    // populate de with entries
    int n = scandir(dir, &de, 0, comp_dir);

    for (size_t i = 0; i < n; i++) {
        snprintf(name, sizeof(name), "%s/%s", dir, de[i]->d_name);
        if (l) {
            stat(name, &s);
            strftime(time, sizeof(time), "%b %d %H:%M", localtime(&s.st_mtime));
            p = getpwuid(s.st_uid);
            g = getgrgid(s.st_gid);
            perms(per, s.st_mode);
            printf("%s %s %s %zu %s %s\n", per, p->pw_name, g->gr_name, s.st_size, time, de[i]->d_name);
        } else {
            printf("%s\n", de[i]->d_name);
        }
        free(de[i]);
    }
    free(de);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <proc/readproc.h>

#include "linkedlist.h"

pid_t pid;
Job *jfront;
Job *jptr;
int jobnum = 1;

// splits buf into args, delimited by a space or newline
// returns true if the last character is '&'
// i.e. the program will run in the background
int splt(char *args[], char *buf) {
    int i = 0;
    char *p = strtok(buf, " \n");

    while (p != NULL) {
        if (strcmp("&", p) == 0) {
            args[i] = NULL;
            return 1;
        }
        strcpy(args[i++], p);
        p = strtok(NULL, " \n");
    }
    args[i] = NULL;
    return 0;
}

void int_handler(int sig) {
    kill(pid, SIGINT);
}

void stp_handler(int sig) {
    kill(pid, SIGTSTP);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, int_handler);
    signal(SIGTSTP, stp_handler);

    pid_t pgid = getpid();
    jfront = 0;
    jptr = jfront;
    int bg = 0;
    char *buf = 0;
    size_t bufsize = 128;
    while (1) {
        printf("%d> ", jobnum);
        getline(&buf, &bufsize, stdin);
        if (strcmp(buf, "\n") == 0) {
            continue;
        }

        char **args = malloc(128 * sizeof(char *));
        for (int i = 0; i < 128; i++) {
            args[i] = malloc(128 * sizeof(char));
        }
        bg = splt(args, buf);

        // program file does not exist
        if (access(args[0], F_OK) != 0) {
            // check for built-in functions
            if (strcmp(args[0], "cd") == 0) {
                if (args[1] != NULL) {
                    chdir(args[1]);
                    setenv("PWD", args[1], 1);
                } else {
                    chdir(getenv("HOME"));
                    setenv("PWD", getenv("HOME"), 1);
                }
                continue;
            } else if (strcmp(args[0], "exit") == 0) {
                kill(0, SIGTERM);
                exit(0);
            } else if (strcmp(args[0], "jobs") == 0) {
                if (jfront != 0) {
                    printer(jfront);
                }
                continue;
            }

            // file does not exist in path given
            if (args[0][0] == '.' || args[0][0] == '~' || args[0][0] == '/') {
                printf("%s: No such file or directory\n", args[0]);
            }

            // replace with command if exists in /usr/bin
            char cat[256];
            snprintf(cat, sizeof(cat), "/usr/bin/%s", args[0]);
            if (access(cat, F_OK) == 0) {
                strcpy(args[0], cat);
            } else {
                snprintf(cat, sizeof(cat), "/bin/%s", args[0]);
                if (access(cat, F_OK) == 0) {
                    strcpy(args[0], cat);
                } else {
                    printf("%s: command not found\n", args[0]);
                    continue;
                }
            }
        }

        // fork process
        if ((pid = fork()) == 0) {
            setpgid(0, pgid);
            if (jfront == 0) {
                jfront = allocate(jobnum++, getpid(), 0, args[0], bg);
                printf("%d\n", jobnum);
            } else {
                printf("more jobs\n");
                jptr->next = allocate(jobnum++, getpid(), 0, args[0], bg);
                jptr = jptr->next;
            }
            printer(jfront);
            execve(args[0], args, NULL);
            exit(0);
        } else {
            // only wait when process in the foreground
            if (bg == 0) {
                pid_t w = 0;
                wait(&w);
            }
        }

        for (int i = 0; i < 128; i++) {
            free(args[i]);
        }
        free(args);
    }
    return 0;
}
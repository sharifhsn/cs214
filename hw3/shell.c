#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "linkedlist.h"

#define _POSIX_C_SOURCE 200809L

pid_t tpid;
pid_t pid;
Job *jfront = 0;
Job *jptr = 0;
sigset_t mask;
int jobnum = 1;
char **args;

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
        args[i] = malloc(128 * sizeof(char));
        strcpy(args[i++], p);
        p = strtok(NULL, " \n");
        
    }
    args[i] = NULL;
    return 0;
}

Job *get_job(Job *front, int get_pid) {
    for (Job *ptr = front; ptr != NULL; ptr = ptr->next) {
        if (ptr->pid == get_pid) {
            return ptr;
        }
    }
    return NULL;
}

Job *reset_ptr(Job *front) {
    if (front == 0) {
        return 0;
    }
    while (front->next != 0) {
        front = front->next;
    }
    return front;
}

void free_args(char **args) {
    for (int i = 0; args[i] != NULL && i < 128; i++) {
        free(args[i]);
    }
    free(args);
    // printf("freeing args!\n");
}

void int_handler(int sig) {
    Job *j = get_job(jfront, pid);
    if (j != NULL) {
        char j_buf[40];
        snprintf(j_buf, sizeof(j_buf), "\n[%d] %d terminated by signal %d\n", j->num, j->pid, sig);
        if (j != NULL) {
            write(STDOUT_FILENO, j_buf, strlen(j_buf));
        }
    }
    kill(pid, SIGINT);
}

void stp_handler(int sig) {
    Job *j = get_job(jfront, pid);
    if (j != NULL) {
        j->cond = 1;
    }
    kill(pid, SIGTSTP);
}

void chld_handler(int sig) {
    deleteNode(&jfront, pid);
    jptr = reset_ptr(jfront);
    // write(STDOUT_FILENO, "it could let in a child!\n", 26);
}

void term_handler(int sig) {
    for (Job *ptr = jfront; ptr != 0; ptr = ptr->next) {
        kill(ptr->pid, SIGHUP);
    }
    write(STDOUT_FILENO, "terminated!\n", 13);
    frees(jfront);
    free_args(args);
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, int_handler);
    signal(SIGTSTP, stp_handler);
    signal(SIGCHLD, chld_handler);
    signal(SIGTERM, term_handler);

    tpid = getpid();
    int bg = 0;
    char *buf = 0;
    size_t bufsize = 128;
    while (1) {
        printf("> ");
        if (getline(&buf, &bufsize, stdin) == -1) {
            break;
        }
        if (strcmp(buf, "\n") == 0) {
            continue;
        }

        args = malloc(128 * sizeof(char *));
        // for (int i = 0; i < 128; i++) {
        //     args[i] = malloc(128 * sizeof(char));
        // }
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
                for (Job *ptr = jfront; jptr != 0; jptr = jptr->next) {
                    kill(ptr->pid, SIGHUP);
                }
                kill(getpid(), SIGTERM);
            } else if (strcmp(args[0], "jobs") == 0) {
                if (jfront != 0) {
                    printer(jfront);
                }
                continue;
            } else if (strcmp(args[0], "kill") == 0) {
                if (args[1] != NULL) {
                    for (Job *ptr = jfront; jptr != 0; jptr = jptr->next) {
                        if (ptr->num == atoi(args[1])) {
                            kill(ptr->pid, SIGTERM);
                        }
                    }
                }
            }

            // file does not exist in path given
            if (args[0][0] == '.' || args[0][0] == '~' || args[0][0] == '/') {
                printf("%s: No such file or directory\n", args[0]);
                free_args(args);
                continue;
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
                    free_args(args);
                    continue;
                }
            }
        }
        // sigaddset(&mask, SIGCHLD);
        // sigaddset(&mask, SIGINT);
        // sigaddset(&mask, SIGTSTP);
        // sigprocmask(SIG_BLOCK, &mask, 0);
        // fork process
        if ((pid = fork()) == 0) {
            execve(args[0], args, NULL);
            kill(getppid(), SIGCHLD);
            exit(0);
        } else {
            // only wait when process in the foreground
            if (jptr == 0) {
                jptr = allocate(jobnum++, pid, 0, args[0], bg);
                jfront = jptr;
                // printf("%d\n", jobnum);
            } else {
                // printf("more jobs\n");
                jptr->next = allocate(jobnum++, pid, 0, args[0], bg);
                jptr = jptr->next;
            }
            // printf("pointer and front are: %p %p\n", jptr, jfront);
            printer(jfront);

            if (bg == 0) {
                pid_t w = 0;
                waitpid(pid, &w, WUNTRACED);
            }
        }
        free_args(args);
        // sigprocmask(SIG_UNBLOCK, &mask, 0);
    }
    return 0;
}
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct Job {
    int num;
    pid_t pid;
    int cond;
    char *name;
    int bg;
    struct Job *next;
} Job;

struct Job *allocate(int num, pid_t pid, int cond, char *name, int bg) {
    struct Job *job = malloc(sizeof(struct Job));
    job->num = num;
    job->pid = pid;
    job->cond = cond;
    // n->name = malloc(sizeof(name));
    // strcpy(n->name, name);
    job->name = name;
    job->bg = bg;
    job->next = 0;
    return job;
}

struct Job *rm_job(struct Job *front, struct Job *rmn) {
    if (rmn == front) {
        front = front->next;
        free(rmn->name);
        free(rmn);
    } else {
        for (struct Job *ptr = front; ptr->next != 0; ptr = ptr->next) {
            if (ptr->next == rmn) {
                ptr->next = rmn->next;
                free(rmn->name);
                free(rmn);
                break;
            }
        }
    }
    return front;
}

void frees(struct Job* front) {
    struct Job *ptr = front;
    struct Job *prev = NULL;
    while (ptr != 0) {
        prev = ptr;
        ptr = ptr->next;
        free(prev->name);
        free(prev);
    }
}
void printer(struct Job *front) {
    for (struct Job *ptr = front; ptr != 0; ptr = ptr->next) {
        char *buf;
        switch (ptr->cond) {
            case 0:
                buf = "Running";
                break;
            case 1:
                buf = "Stopped";
                break;
            case 2:
                buf = "Terminated";
                break;
            case 3:
                buf = "Killed";
                break;
            default:
                buf = "M";
                break;
        }
        char b = ' ';
        if (ptr->bg) {
            b = '&';
        }
        printf("[%d] %d %s %s %c\n", ptr->num, ptr->pid, buf, ptr->name, b);
    }
}
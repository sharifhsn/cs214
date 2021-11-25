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
    job->name = malloc(strlen(name) + 1);
    // printf("argument ptr, job ptr: %p %p\n", name, job->name);
    strcpy(job->name, name);
    job->bg = bg;
    job->next = 0;
    return job;
}

struct Job *rm_job(struct Job *front, struct Job *jptr, int rm_pid) {
    if (rm_pid == front->pid) {
        if (front->next == 0) {
            free(front->name);
            free(front);
            return 0;
        }
        struct Job *temp = front;
        if (temp == jptr) {
            jptr = front->next;
            jptr->next = front->next->next;
        }
        front = front->next;
        free(temp->name);
        free(temp);
    } else {
        for (struct Job *ptr = front; ptr->next != 0; ptr = ptr->next) {
            if (ptr->next->pid == rm_pid) {
                struct Job *temp = ptr->next;
                if (temp == jptr) {
                    jptr = ptr;
                    jptr->next = ptr->next->next;
                }
                ptr->next = ptr->next->next;
                free(temp->name);
                free(temp);
                break;
            }
        }
    }
    return front;
}


void deleteNode(struct Job **ffront, int rm_pid) {
    // Store head node
    struct Job *temp = *ffront, *prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->pid == rm_pid) {
        *ffront = temp->next; // Changed head
        free(temp->name);
        free(temp); // free old head
        return;
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->pid != rm_pid) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return;
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp->name);
    free(temp); // Free memory
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
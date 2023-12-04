#include "process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// create a new process named 'name'
process *process_create(char *name)
{
    process *p = malloc(sizeof(process));

    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->state = PROCESS_STATE_NEW;
    p->lastrun = 0;

    return p;
}

// free the process
void process_destroy(process *p)
{
    if (p)
    {
        free(p->name);
        free(p);
    }
}

// convert a process state into string for printing
char *process_state_str(process_state state)
{
    switch (state)
    {
    case PROCESS_STATE_NEW:
        return "New";
    case PROCESS_STATE_READY:
        return "Ready";
    case PROCESS_STATE_RUNNING:
        return "Running";
    case PROCESS_STATE_BLOCKED:
        return "Blocked";
    case PROCESS_STATE_EXIT:
        return "Exit";
    default:
        return "Unknown";
    }
}

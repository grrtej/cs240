#ifndef PROCESS_H
#define PROCESS_H

// the 5 states a process can be in
typedef enum process_state
{
    PROCESS_STATE_NEW,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_EXIT
} process_state;

// process struct
typedef struct process process;
struct process
{
    char *name;
    process_state state;
    int lastrun; // ticks since transitioning from running state
};

// create a new process named 'name'
process *process_create(char *name);

// free the process
void process_destroy(process *p);

// convert a process state into string for printing
char *process_state_str(process_state state);

#endif // PROCESS_H

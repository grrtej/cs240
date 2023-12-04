#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "queue.h"
#include "line.h"
#include "tok.h"

// iodevs 0, 1, 2, and 3
#define IODEVCOUNT 4

// struct that keeps track of all queues, the running process, and time
typedef struct ksim ksim;
struct ksim
{
    int ticks;
    queue *ready_queue;
    queue *exit_queue;
    queue *blocked_queues[IODEVCOUNT];
    process *running_process;
};

// init the simulator
ksim *ksim_create(void);
// free the simulator
void ksim_destory(ksim *k);
// search all queues for process matching 'name'
process *ksim_search(ksim *k, char *name);
// increment the ticks (with rollover at 1 billion)
void ksim_addticks(ksim *k, int n);

// add process named 'arg' to ready queue
void ksim_cmd_add(ksim *k, char *arg);
// dump simulator state
void ksim_cmd_query(ksim *k, char *arg);
// advance the simulation
void ksim_cmd_step(ksim *k);
// move running process to exit queue
void ksim_cmd_release(ksim *k);
// block running process on device until 1024 ticks or io-event
void ksim_cmd_wait(ksim *k, char *arg);
// send io-event for device to unblock all processes waiting for it
void ksim_cmd_ioevent(ksim *k, char *arg);

int main(void)
{
    // main object, makes it easy to pass everything around
    ksim *k = ksim_create();

    // print the prompt and get input
    for (char *cmd; (printf("ksim-%09d> ", k->ticks), cmd = line_get()); line_free(cmd))
    {
        // try to tokenize
        char **args = tok_get(cmd);
        if (!args[0])
        {
            tok_free(args);
            continue;
        }

        // branch according to given command
        if (strcmp(args[0], "exit") == 0)
        {
            exit(args[1] ? atoi(args[1]) : EXIT_SUCCESS);
        }
        else if (strcmp(args[0], "add") == 0)
        {
            ksim_cmd_add(k, args[1]);
        }
        else if (strcmp(args[0], "query") == 0)
        {
            ksim_cmd_query(k, args[1]);
        }
        else if (strcmp(args[0], "step") == 0)
        {
            ksim_cmd_step(k);
        }
        else if (strcmp(args[0], "release") == 0)
        {
            ksim_cmd_release(k);
        }
        else if (strcmp(args[0], "wait") == 0)
        {
            ksim_cmd_wait(k, args[1]);
        }
        else if (strcmp(args[0], "io-event") == 0)
        {
            ksim_cmd_ioevent(k, args[1]);
        }
        else
        {
            printf("\"%s\" is not a command.\n", args[0]);
        }

        // cleanup tokens
        tok_free(args);
    }

    // cleanup everything
    ksim_destory(k);
    return EXIT_SUCCESS;
}

// init the simulator
ksim *ksim_create(void)
{
    ksim *k = malloc(sizeof(ksim));

    k->ticks = 0;
    k->ready_queue = queue_create();
    k->exit_queue = queue_create();
    for (int i = 0; i < IODEVCOUNT; i++)
    {
        k->blocked_queues[i] = queue_create();
    }
    k->running_process = NULL;

    return k;
}

// free the simulator
void ksim_destory(ksim *k)
{
    queue_destroy(k->ready_queue);
    queue_destroy(k->exit_queue);
    for (int i = 0; i < IODEVCOUNT; i++)
    {
        queue_destroy(k->blocked_queues[i]);
    }
    process_destroy(k->running_process);
    free(k);
}

// search all queues for process matching 'name'
process *ksim_search(ksim *k, char *name)
{
    process *p = NULL;
    if (k->running_process && strcmp(k->running_process->name, name) == 0)
    {
        return k->running_process;
    }

    if ((p = queue_search(k->ready_queue, name)))
    {
        return p;
    }

    if ((p = queue_search(k->exit_queue, name)))
    {
        return p;
    }

    for (int i = 0; i < IODEVCOUNT; i++)
    {
        if ((p = queue_search(k->blocked_queues[i], name)))
        {
            return p;
        }
    }

    return p;
}

// increment the ticks (with rollover at 1 billion)
void ksim_addticks(ksim *k, int n)
{
    k->ticks = (k->ticks + n) % 1000000000;
}

// add process named 'arg' to ready queue
void ksim_cmd_add(ksim *k, char *arg)
{
    if (!arg)
    {
        printf("Opcode \"add\" requires 1 operand.\n");
        return;
    }

    if (ksim_search(k, arg))
    {
        printf("Process named \"%s\" is already being hosted.\n", arg);
        return;
    }

    enqueue(k->ready_queue, process_create(arg));
    ksim_addticks(k, 32);
}

// dump simulator state
void ksim_cmd_query(ksim *k, char *arg)
{
    if (!arg || (strcmp(arg, "all") == 0))
    {
        for (int i = 0; i < IODEVCOUNT; i++)
        {
            for (queue_item *x = k->blocked_queues[i]->head; x; x = x->next)
            {
                printf("***\n");
                printf("    id: %s\n", x->process->name);
                printf("    state: %s\n", process_state_str(x->process->state));
                printf("        waiting on device %d since tick %09d\n", i, x->process->lastrun);
                printf("***\n");
            }
        }
        for (queue_item *x = k->ready_queue->head; x; x = x->next)
        {
            printf("***\n");
            printf("    id: %s\n", x->process->name);
            printf("    state: %s\n", process_state_str(x->process->state));
            printf("***\n");
        }
        for (queue_item *x = k->exit_queue->head; x; x = x->next)
        {
            printf("***\n");
            printf("    id: %s\n", x->process->name);
            printf("    state: %s\n", process_state_str(x->process->state));
            printf("***\n");
        }
        if (k->running_process)
        {
            printf("***\n");
            printf("    id: %s\n", k->running_process->name);
            printf("    state: %s\n", process_state_str(k->running_process->state));
            printf("***\n");
        }
    }
    else
    {
        process *p = ksim_search(k, arg);
        if (!p)
        {
            printf("Process \"%s\" not found.\n", arg);
            return;
        }
        printf("***\n");
        printf("    id: %s\n", p->name);
        printf("    state: %s\n", process_state_str(p->state));
        printf("***\n");
    }
}

// advance the simulation
void ksim_cmd_step(ksim *k)
{
    int ticks = 1;

    // destroy processes in exit queue
    while (!queue_isempty(k->exit_queue))
    {
        process *p = dequeue(k->exit_queue);
        printf("Process \"%s\" is banished to the void.\n", p->name);
        process_destroy(p);
    }

    // move oldest new process to ready
    if (!queue_isempty(k->ready_queue))
    {
        for (queue_item *x = k->ready_queue->head; x; x = x->next)
        {
            if (x->process->state == PROCESS_STATE_NEW)
            {
                x->process->state = PROCESS_STATE_READY;
                printf("Process \"%s\" moved from New to Ready.\n", x->process->name);
                break;
            }
        }
    }

    // remove oldest blocked process from each blocked queue
    // if it has been waiting for more than 1024 ticks since being blocked
    for (int i = 0; i < IODEVCOUNT; i++)
    {
        if (!queue_isempty(k->blocked_queues[i]))
        {
            for (queue_item *x = k->blocked_queues[i]->head; x; x = x->next)
            {
                if (k->ticks - x->process->lastrun > 1024)
                {
                    process *p = dequeue(k->blocked_queues[i]);
                    p->state = PROCESS_STATE_READY;
                    enqueue(k->ready_queue, p);
                    printf("Process \"%s\" moved from Blocked (iodev=%d) to Ready.\n", p->name, i);
                    break;
                }
            }
        }
    }

    // send running process back to ready queue
    if (k->running_process)
    {
        k->running_process->state = PROCESS_STATE_READY;
        enqueue(k->ready_queue, k->running_process);
        printf("Process \"%s\" moved from Running to Ready.\n", k->running_process->name);
        k->running_process = NULL;
    }

    // send oldest (not the first in queue) ready process to running
    if (!queue_isempty(k->ready_queue))
    {
        process *oldest = k->ready_queue->head->process;
        for (queue_item *x = k->ready_queue->head; x; x = x->next)
        {
            if (x->process->lastrun > oldest->lastrun)
            {
                oldest = x->process;
            }
        }

        oldest->state = PROCESS_STATE_RUNNING;
        oldest->lastrun = 0;
        k->running_process = oldest;
        printf("Process \"%s\" moved from Ready to Running.\n", oldest->name);
        queue_deleteitem(k->ready_queue, oldest->name);
        ticks = 256;
    }
    ksim_addticks(k, ticks);
}

// move running process to exit queue
void ksim_cmd_release(ksim *k)
{
    if (!k->running_process)
    {
        printf("No process is currently Running.\n");
        return;
    }

    k->running_process->state = PROCESS_STATE_EXIT;
    enqueue(k->exit_queue, k->running_process);
    printf("Process \"%s\" moved from Running to Exit.\n", k->running_process->name);
    k->running_process = NULL;
    ksim_addticks(k, 32);
}

// block running process on device until 1024 ticks or io-event
void ksim_cmd_wait(ksim *k, char *arg)
{
    if (!arg)
    {
        printf("Opcode \"wait\" requires 1 operand.\n");
        return;
    }

    if (!k->running_process)
    {
        printf("No process is currently Running.\n");
        return;
    }

    int iodev = atoi(arg);
    if (iodev < 0 || iodev > IODEVCOUNT - 1)
    {
        printf("Device must be between 0 and %d (inclusive).\n", IODEVCOUNT - 1);
        return;
    }

    k->running_process->state = PROCESS_STATE_BLOCKED;
    k->running_process->lastrun = k->ticks;
    enqueue(k->blocked_queues[iodev], k->running_process);
    printf("Process \"%s\" moved from Running to Blocked.\n", k->running_process->name);
    k->running_process = NULL;
    ksim_addticks(k, 1);
}

// send io-event for device to unblock all processes waiting for it
void ksim_cmd_ioevent(ksim *k, char *arg)
{
    if (!arg)
    {
        printf("Opcode \"io-event\" requires 1 operand.\n");
        return;
    }
    int iodev = atoi(arg);
    if (iodev < 0 || iodev > IODEVCOUNT - 1)
    {
        printf("Device must be between 0 and %d (inclusive).\n", IODEVCOUNT - 1);
        return;
    }

    if (queue_isempty(k->blocked_queues[iodev]))
    {
        printf("No processes waiting on device %d\n", iodev);
        return;
    }
    else
    {
        // unblock all processes waiting for iodev
        while (!queue_isempty(k->blocked_queues[iodev]))
        {
            process *p = dequeue(k->blocked_queues[iodev]);
            p->state = PROCESS_STATE_READY;
            p->lastrun = 0;
            enqueue(k->ready_queue, p);
            printf("Process \"%s\" moved from Blocked (iodev=%d) to Ready.\n", p->name, iodev);
        }
    }
    ksim_addticks(k, 1);
}

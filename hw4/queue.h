#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "process.h"

// queue building block
typedef struct queue_item queue_item;
struct queue_item
{
    process *process;
    queue_item *next;
};

// pointers to start and end of queue
typedef struct queue queue;
struct queue
{
    queue_item *head;
    queue_item *tail;
};

// create a new, emtpy queue
queue *queue_create(void);

// free up the queue and all items in it
void queue_destroy(queue *q);

// put the process at the end of queue
void enqueue(queue *q, process *p);

// return the process at the beginning of queue
process *dequeue(queue *q);

// does the queue have any processes?
bool queue_isempty(queue *q);

// find process matching name and return it
process *queue_search(queue *q, char *name);

// delete process matching name from the queue
// other processes are not affected
void queue_deleteitem(queue *q, char *name);

#endif // QUEUE_H

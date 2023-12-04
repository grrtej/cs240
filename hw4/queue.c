#include "queue.h"

#include <stdlib.h>
#include <string.h>
#include "process.h"

// create a new, emtpy queue
queue *queue_create(void)
{
    queue *q = malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    return q;
}

// free up the queue and all items in it
void queue_destroy(queue *q)
{
    while (!queue_isempty(q))
    {
        process_destroy(dequeue(q));
    }
    free(q);
}

// put the process at the end of queue
void enqueue(queue *q, process *p)
{
    queue_item *x = malloc(sizeof(queue_item));
    x->process = p;
    x->next = NULL;

    if (q->tail)
    {
        q->tail->next = x;
        q->tail = q->tail->next;
    }
    else
    {
        q->tail = q->head = x;
    }
}

// return the process at the beginning of queue
process *dequeue(queue *q)
{
    if (!q->head)
    {
        return NULL;
    }

    queue_item *x = q->head;
    process *p = x->process;

    q->head = q->head->next;
    if (!q->head)
    {
        q->tail = q->head;
    }

    free(x);
    return p;
}

// does the queue have any processes?
bool queue_isempty(queue *q)
{
    return q->head == NULL;
}

// find process matching name and return it
process *queue_search(queue *q, char *name)
{
    for (queue_item *x = q->head; x; x = x->next)
    {
        if (strcmp(x->process->name, name) == 0)
        {
            return x->process;
        }
    }
    return NULL;
}

// delete process matching name from the queue
// other processes are not affected
void queue_deleteitem(queue *q, char *name)
{
    // could be optimized (like everything else)
    if (strcmp(q->head->process->name, name) == 0)
    {
        dequeue(q);
        return;
    }

    int index = 0;
    for (queue_item *x = q->head; (x && strcmp(x->process->name, name)); x = x->next)
    {
        index++;
    }

    queue_item *prev = q->head;
    for (int i = 0; i < index - 1; i++)
    {
        prev = prev->next;
    }
    queue_item *del = prev->next;
    prev->next = prev->next->next;

    del->next = NULL;
    free(del);
}

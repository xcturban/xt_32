
#include <common_lib/common_lib.h>
#include "ncs_queue.h"
#include <assert.h>

TYPE_QUEUE_T *QueueNew(void)
{
    TYPE_QUEUE_T *queue;

    if ((queue = malloc(sizeof(TYPE_QUEUE_T))) == NULL)
    {
        return NULL;
    }
    queue->beg = NULL;
    queue->end = NULL;
    return queue;
}

void QueueDestroy(TYPE_QUEUE_T *queue)
{
    free(queue);
}

int EnterQueue(TYPE_QUEUE_T *queue, TYPE_MSG_STRUCT elem)
{
	TYPE_QNODE *qnode_t;

	assert(queue != NULL);
	if ((qnode_t = malloc(sizeof(TYPE_QNODE))) == NULL)
	{
		return FALSE;
	}

	if (queue->end != NULL)
	{
		queue->end->prev = qnode_t;
	}

	if (queue->beg == NULL)
	{
		queue->beg = qnode_t;
	}

	qnode_t->elem = elem;
	qnode_t->prev = NULL;
	qnode_t->next = queue->end;

	queue->end = qnode_t;
	return TRUE;
}

int OutQueue(TYPE_QUEUE_T *queue, TYPE_MSG_STRUCT *pMsg)
{
    TYPE_QNODE *qnode_t;
    void *elem;

    assert(queue != NULL);
    if (queue->beg == NULL)
    {
        return FALSE;
    }

    qnode_t    = queue->beg;
    *pMsg      = qnode_t->elem;
    queue->beg = qnode_t->prev;
    if (queue->beg != NULL)
    {
        queue->beg->next = NULL;
    }
    else
    {
        queue->end = NULL;
    }
    free(qnode_t);
    return TRUE;
}

int QueueLen(TYPE_QUEUE_T *queue)
{
    int i;
    TYPE_QNODE *qnode_t;

    assert(queue != NULL);
    qnode_t = queue->beg;
    for (i = 0; qnode_t != NULL; i++)
    {
        qnode_t = qnode_t->prev;
    }
    return i;
}


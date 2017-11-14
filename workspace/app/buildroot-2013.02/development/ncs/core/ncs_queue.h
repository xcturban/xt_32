#ifndef __QUEUE_H__
#define __QUEUE_H__


typedef struct 
{
	int MsgCmd;
	int Wparam;
	int Lparam;
}TYPE_MSG_STRUCT;

typedef struct qnode
{
    struct qnode *next;
	struct qnode *prev;
    TYPE_MSG_STRUCT elem;
}TYPE_QNODE;

typedef struct
{
    TYPE_QNODE *beg;
   	TYPE_QNODE *end;
}TYPE_QUEUE_T;

typedef enum
{
	ENUM_ENTER_REV_BROADCAST,
	ENUM_INVALID,
}ENUM_TYPE;

TYPE_QUEUE_T *QueueNew(void);
void QueueDestroy(TYPE_QUEUE_T *queue);
int EnterQueue(TYPE_QUEUE_T *queue, TYPE_MSG_STRUCT elem);
int OutQueue(TYPE_QUEUE_T *queue, TYPE_MSG_STRUCT *pMsg);
int QueueLen(TYPE_QUEUE_T *queue);


#endif

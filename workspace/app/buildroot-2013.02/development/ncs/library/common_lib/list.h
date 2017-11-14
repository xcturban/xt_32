#ifndef __LIST_H__
#define __LIST_H__

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

/* ��ʼ���б�ͷ */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/* �����ͳ�ʼ���б�ͷ */
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

/* ��ʼ���б�ͷ */
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/* �б��Ƿ�Ϊ�� */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}


/* �����б� */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)


#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)


/* ����һ���ڵ㵽�������ڵĽڵ�֮�䣬����һ���ڲ��������� */
static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/* ����һ���ڵ㵽һ��ָ���Ľڵ���� */
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}


/* ����һ���ڵ㵽һ��ָ���Ľڵ�ǰ�� */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/* �ڲ��������� */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/* ɾ��һ��ָ���Ľڵ� */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

#endif

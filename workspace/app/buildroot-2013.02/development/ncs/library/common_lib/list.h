#ifndef __LIST_H__
#define __LIST_H__

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

/* 初始化列表头 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/* 声明和初始化列表头 */
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

/* 初始化列表头 */
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/* 列表是否为空 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}


/* 遍历列表 */
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


/* 插入一个节点到两个相邻的节点之间，这是一个内部操作函数 */
static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/* 插入一个节点到一个指定的节点后面 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}


/* 插入一个节点到一个指定的节点前面 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/* 内部操作函数 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/* 删除一个指定的节点 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

#endif

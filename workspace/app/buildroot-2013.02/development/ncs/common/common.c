#include "common.h"

void common_list_head_init(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

static void __common_list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void common_list_append(struct list_head *new, struct list_head *head)
{
    __common_list_add(new, head->prev, head);
}

void common_list_add(struct list_head *new, struct list_head *head)
{
	__common_list_add(new, head, head->next);
}

static void __common_list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

void common_list_del(struct list_head *entry)
{
	__common_list_del(entry->prev, entry->next);
}

int common_list_count(struct list_head *head)
{
	int count = 0;
	struct list_head *p;
	list_for_each(p, head)
		count++;
	return count;
}


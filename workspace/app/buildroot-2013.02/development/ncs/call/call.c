#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "call.h"

#define NR_CALLS (2)

static call_t *call_new(struct call_manager *call_manager, ncs_call_t type, call_dir_t dir, uint16_t target_id, uint8_t board_id, struct sockaddr *target_sockaddr)
{
    call_t *call;

    call = (call_t *)malloc(sizeof(call_t));
    if (!call)
        return NULL;

    memset(call, 0, sizeof(call_t));

    call->type = type;
    call->dir = dir;
    call->target_id = target_id;
    call->board_id = board_id;

    if (target_sockaddr)
    {
        memcpy(&call->target_sockaddr, target_sockaddr, sizeof(struct sockaddr));
    }

    ms_bufferizer_init(&call->bufferizer);
    pthread_mutex_init(&call->mutex, NULL);

    INIT_LIST_HEAD(&call->list);
    list_add_tail(&call->list, &call_manager->call_list);

    call_manager->nr_calls++;
    SPON_LOG_INFO("=====>%s nr_calls:%d\n", __func__, call_manager->nr_calls);


    return call;
}


static int call_target_sockaddr_set(struct call_manager *call_manager, call_t *call, struct sockaddr *target_sockaddr)
{
    if (!call || !target_sockaddr)
        return -1;

    memset(&call->target_sockaddr, 0, sizeof(struct sockaddr));
    memcpy(&call->target_sockaddr, target_sockaddr, sizeof(struct sockaddr));

    call->target_sockaddr_set = TRUE;

    return 0;
}


static void call_delete(struct call_manager *call_manager, call_t *call)
{
    if (!call)
        return;

    list_del(&call->list);
    call_manager->nr_calls--;

    ms_bufferizer_flush(&call->bufferizer);
    free(call);

    SPON_LOG_INFO("=====>%s nr_calls:%d\n", __func__, call_manager->nr_calls);
}


static call_t *call_find_by_sockaddr(struct call_manager *call_manager, struct sockaddr *target_sockaddr)
{
    call_t *call;
    struct list_head *pos;

    if (!target_sockaddr)
        return NULL;

    list_for_each(pos, &call_manager->call_list)
    {
        call = list_entry(pos, call_t, list);
        if (memcmp(&call->target_sockaddr, target_sockaddr, sizeof(*target_sockaddr)) == 0)
            return call;
    }

    return NULL;
}

static call_t *call_find_by_id(struct call_manager *call_manager, uint16_t target_id)
{
    call_t *call;
    struct list_head *pos;

    list_for_each(pos, &call_manager->call_list)
    {
        call = list_entry(pos, call_t, list);
        if (call->target_id == target_id)
            return call;
    }

    return NULL;
}

static call_t *current_call_get(struct call_manager *call_manager)
{
    call_t *call = NULL;
    struct list_head *head = &call_manager->call_list;
    struct list_head *p;

    list_for_each(p, head)
    call = list_entry(p, call_t, list);

    return call;
}

static call_t *monitor_call_get(struct call_manager *call_manager)
{
    call_t *call = NULL;
    struct list_head *head = &call_manager->call_list;
    struct list_head *p;

    list_for_each(p, head)
    {
        call = list_entry(p, call_t, list);
        if (call->type == MONITOR || call->type == SDK_MONITOR)
            return call;
    }

    return NULL;
}

static int calls_total_get(struct call_manager *call_manager)
{
    return call_manager->nr_calls;
}

static int nr_calls_get(struct call_manager *call_manager, call_t **call, int nr)
{
    int idx = 1;
    struct list_head *p;

    if (nr != 2 || calls_total_get(call_manager) != 2)
        return -1;

    list_for_each(p, &call_manager->call_list)
    {
        call[idx] = NULL;
        call[idx] = list_entry(p, call_t, list);
        idx--;
    }

    return 0;
}

struct call_manager *call_manager_malloc(void)
{
    struct call_manager *tmp = (struct call_manager *)malloc(sizeof(struct call_manager));
    if (!tmp)
        return NULL;

    memset(tmp, 0, sizeof(struct call_manager));

    INIT_LIST_HEAD(&tmp->call_list);

    tmp->call_new = call_new;
    tmp->call_delete = call_delete;
    tmp->call_target_sockaddr_set = call_target_sockaddr_set;
    tmp->call_find_by_sockaddr = call_find_by_sockaddr;
    tmp->call_find_by_id = call_find_by_id;
    tmp->current_call_get = current_call_get;
    tmp->monitor_call_get = monitor_call_get;
    tmp->calls_total_get = calls_total_get;
    tmp->nr_calls_get = nr_calls_get;

    return tmp;
}

void call_manager_free(struct call_manager *call_manager)
{
    free(call_manager);
}

void call_manager_set_flag_emergency(struct call_manager *call_manager, bool_t flag)
{
    call_manager->flag_call_emergency = flag;
}

bool_t call_manager_get_flag_emergency(struct call_manager *call_manager)
{
    return call_manager->flag_call_emergency;
}

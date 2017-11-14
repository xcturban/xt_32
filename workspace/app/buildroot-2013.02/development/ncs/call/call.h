#ifndef __CALL_H__
#define __CALL_H__

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include <common_lib/common_lib.h>
#include <ncs_event/ncs_event.h>

#include "mediastreamer2/msqueue.h"

typedef struct {
    ncs_call_t type;
    call_dir_t dir;

	uint16_t target_id;
	uint8_t board_id;
    struct sockaddr target_sockaddr;

    // bool_t target_sockaddr_set_by_ip_request;

    bool_t target_sockaddr_set;

	timer_t call_request_timer_id;
	bool_t call_request_timer_start;

    bool_t started;

    MSBufferizer bufferizer;
    pthread_mutex_t mutex;

	struct list_head list;
	
	int request_times; // 请求次数
} call_t;


struct call_manager {
    struct list_head call_list;
    int nr_calls;
    bool_t flag_call_emergency;

    bool_t target_sockaddr_set_by_nat;
    bool_t ip_get_ok;
    struct sockaddr target_nat_sockaddr;

    call_t *(*call_new)(struct call_manager *call_manager, ncs_call_t type, call_dir_t dir, uint16_t target_id, uint8_t board_id, struct sockaddr *target_sockaddr);
    void (*call_delete)(struct call_manager *call_manager, call_t *call);
    int (*call_target_sockaddr_set)(struct call_manager *call_manager, call_t *call, struct sockaddr *target_sockaddr);
    call_t *(*call_find_by_sockaddr)(struct call_manager *call_manager, struct sockaddr *target_sockaddr);
    call_t *(*call_find_by_id)(struct call_manager *call_manager, uint16_t target_id);
    call_t *(*current_call_get)(struct call_manager *call_manager);
    call_t *(*monitor_call_get)(struct call_manager *call_manager);
    int (*calls_total_get)(struct call_manager *call_manager);
    int (*nr_calls_get)(struct call_manager *call_manager, call_t **call, int nr);
};

struct call_manager *call_manager_malloc(void);
void call_manager_free(struct call_manager *call_manager);
void call_manager_set_flag_emergency(struct call_manager *call_manager, bool_t flag);
bool_t call_manager_get_flag_emergency(struct call_manager *call_manager);

#endif

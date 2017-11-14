#ifndef __OFFLINE_H__
#define __OFFLINE_H__

#include <stdint.h>

struct offline_call {
	uint16_t term_id; // 终端id
	char ip[32]; // 终端ip
	int port; // 端口
	struct list_head list; // 链表节点
};

struct offline_call *find_offline_call_by_id(uint16_t id);
int init_offline_call_list(void);

#endif

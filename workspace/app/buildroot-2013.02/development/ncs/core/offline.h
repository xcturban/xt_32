#ifndef __OFFLINE_H__
#define __OFFLINE_H__

#include <stdint.h>

struct offline_call {
	uint16_t term_id; // �ն�id
	char ip[32]; // �ն�ip
	int port; // �˿�
	struct list_head list; // ����ڵ�
};

struct offline_call *find_offline_call_by_id(uint16_t id);
int init_offline_call_list(void);

#endif

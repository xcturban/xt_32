#ifndef __SIP_H__
#define __SIP_H__

#include <stdint.h>
#include <common_lib/common_lib.h>

struct sip_manager {
	struct sockaddr sip_sdk_tool_sockaddr; // sip服务器浩云SDK工具网络地址
	int (*start)(void);
	int (*stop)(void);
	bool_t (*registered)(void);
	bool_t (*busy)(void);
	int (*send_dtmf)(char c);
	char *(*get_target_name)(void);
	char *(*get_target_number)(void);
	
	int (*call_request)(uint8_t *data, uint8_t data_len);
	int (*call_stop)(void);
	int (*call_start)(void);
	int (*blf_state_update)(void);
};

struct sip_manager *sip_manager_malloc(void);
void sip_manager_free(struct sip_manager *sip_manager);

#endif

#include <errno.h>
#include <common_lib/common_lib.h>
#include "lpconfig.h"
#include "offline.h"
#include "system_info.h"

// 保存脱机对讲配置的链表
static LIST_HEAD(offline_call_list);

#define MAX_SECTIONS 10 // 最多10段
#define MAX_ITEMS 16 // 每段最多16项


struct offline_call *find_offline_call_by_id(uint16_t id)
{
	struct list_head *p;
	struct offline_call *oc;
	
	list_for_each(p, &offline_call_list) {
		oc = list_entry(p, struct offline_call, list);
		if (oc->term_id == id)
			return oc;
	}

	return NULL;
}

int init_offline_call_list(void)
{
	LpConfig *lpconfig;

	char section[32];
	char item[32];
	char *value;

	int term_id;
	char ip[32];
	int port;

	int i, j;

	struct offline_call *oc;
	
	lpconfig = lp_config_new(INI_OFFLINE_CALL_FILE);
	if (lpconfig == NULL) {
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}
	
	for(i = 1; i <= MAX_SECTIONS ; i++){
		// 取段名
		sprintf(section, "alone_%d", i);
		
		// 检查段是否存在
		if(lp_config_has_section(lpconfig, section) == 0)
			continue;
		
		for(j = 0; j < MAX_ITEMS; j++){
			// 取节名
			sprintf(item, "key_%d", j);
			// 取值
			value = lp_config_get_string(lpconfig, section, item, NULL);
			if(value){

				term_id = 0;
				port = 0;
				memset(ip, 0, 32);
				
				// value格式为: 终端id_port_ip
				sscanf(value, "%d_%d_%s", &term_id, &port, ip);

				// SPON_LOG_INFO("=====>%s - term_id:%d ip:%s port:%d\n", __func__, term_id, ip, port);

				// 合法性检测
				if (term_id == 0 || port == 0 || strlen(ip) == 0)
					continue;

				oc = (struct offline_call *)malloc(sizeof(struct offline_call));
				if (oc) {
					
					SPON_LOG_INFO("=====>%s - term_id:%d ip:%s port:%d\n", __func__, term_id, ip, port);
					memset(oc, 0, sizeof(struct offline_call));
					
					INIT_LIST_HEAD(&oc->list);
					list_add(&oc->list, &offline_call_list);

					oc->term_id = term_id;
					oc->port = port;
					strcpy(oc->ip, ip);
				}
			}
		}
	}

	
	lp_config_destroy(lpconfig);

	return 0;
}



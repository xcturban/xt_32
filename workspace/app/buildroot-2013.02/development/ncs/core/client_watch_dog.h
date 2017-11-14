#ifndef __WATCHDOG_CLIENT_H__
#define __WATCHDOG_CLIENT_H__

#include "common.h"
#include "ncs_thread.h"

#define WATCHDOG_PIPE 						"watchdog_pipe"
#define NCS_MONITOR_MAX_SIZE					10
#define NCS_WATCHDOG_CLIENT_SUPPORT				1

typedef struct 
{
	int client_fd;
	int is_connected;
	ncs_thread_t client_monitor_pid;
	struct list_head monitor_thread_header;
	int 	monitor_thread_count;
	char*	data_report;
	void* 	private_data;
} ncs_watchdog_client_t;

int ncs_watchdog_thread_update(ncs_watchdog_client_t* client,ncs_thread_t* thread);
bool_t ncs_watchdog_data_check(ncs_watchdog_client_t* client);
int ncs_watchdog_monitor_add(ncs_watchdog_client_t* client, ncs_thread_t * thread);
int ncs_connect_watch_connect(ncs_watchdog_client_t* client);
ncs_watchdog_client_t* ncs_watchdog_client_create(void* arg);
int ncs_connect_watch_start(ncs_watchdog_client_t* client,void * arg);

#endif

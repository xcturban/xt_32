#ifndef __NCS_THREAD_H__
#define __NCS_THREAD_H__

#include "common.h"

typedef struct {
	int				 	state;
	pthread_mutex_t 	state_mutex;
} ncs_state_t;

#define NCS_THREAD_ERROR	(-1)
#define NCS_THREAD_FALSE	(0)
#define NCS_THREAD_TRUE		(1)

typedef enum {
	NCS_THREAD_EXIT =0,
	NCS_THREAD_CREATE,
	NCS_THREAD_START,
	NCS_THREAD_STOP,
} ncs_thread_state_t;

typedef void* (*PTHREAD_CALLBACK_FUNC)(void *);
#define NCS_THREAD_NAME_LEN				64
typedef struct {
	pthread_t 				ncs_thread_tid;
	pthread_attr_t*			attr;
	
	ncs_state_t		 		ncs_thread_state;
	pthread_mutex_t 		ncs_thread_mutex;
	PTHREAD_CALLBACK_FUNC 	ncs_thread_thread;
	 
	uint32_t				running_count;
	uint32_t				running_record;
	uint32_t 				is_alive;
	uint32_t 				noblock;	//默认阻塞模式

	time_t 					timer_sec;	//19700101到此时秒数
	char					name[NCS_THREAD_NAME_LEN];	
	struct list_head		list;
	
	void* private_data;
} ncs_thread_t;

int ncs_state_get(ncs_state_t* nst);
int ncs_state_set(ncs_state_t* nst,int status);
int ncs_state_init(ncs_state_t* nst,int status);
int ncs_thread_state_get(ncs_thread_t* thread);
int ncs_thread_state_set(ncs_thread_t* thread,int state);
int ncs_thread_param_set (ncs_thread_t* thread,char* name,void* thread_func,void* arg);
int	ncs_thread_init(ncs_thread_t* thread);
int ncs_thread_running_update(ncs_thread_t* thread);
int ncs_thread_is_running(ncs_thread_t* thread);
int ncs_thread_list_add(ncs_thread_t* thread, struct list_head*	header);
int ncs_thread_list_delete(ncs_thread_t* thread);
int ncs_thread_start(ncs_thread_t* thread,char* name,void* thread_func,void* arg);
int ncs_thread_wait_stop(ncs_thread_t* thread);
int ncs_thread_stop(ncs_thread_t* thread);
int ncs_thread_noblock_set(ncs_thread_t* thread,int noblock);
int ncs_thread_print(ncs_thread_t* thread);
int ncs_thread_timer_record(ncs_thread_t* thread);
int ncs_thread_timer_string(ncs_thread_t* thread,char* buf,int len);

#endif

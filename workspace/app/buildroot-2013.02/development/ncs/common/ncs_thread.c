#include "ncs_thread.h"

int ncs_state_get(ncs_state_t* nst)
{
#if 0
	int state = 0;
	pthread_mutex_lock(&nst->task_state_mutex);
	state = nst->task_state;
	pthread_mutex_unlock(&nst->task_state_mutex);
	return state;
#else
	return nst->state;
#endif
}

int ncs_state_set(ncs_state_t* nst,int state)
{	

	int status_last = ncs_state_get(nst);

	if( status_last != state) {
		pthread_mutex_lock(&nst->state_mutex);
		nst->state = state;
		pthread_mutex_unlock(&nst->state_mutex);
		status_last = state;
	}
	
	return status_last;
}

int ncs_state_init(ncs_state_t* nst,int state)
{	
	
	pthread_mutex_init(&nst->state_mutex,NULL);
	ncs_state_set(nst,state);
	return NCS_THREAD_TRUE;
}

int ncs_thread_state_get(ncs_thread_t* thread)
{

	return ncs_state_get(&thread->ncs_thread_state);
}


int ncs_thread_state_set(ncs_thread_t* thread,int state)
{
	return ncs_state_set(&thread->ncs_thread_state,state);
}

int ncs_thread_noblock_set(ncs_thread_t* thread,int noblock)
{
	
	pthread_mutex_lock(&thread->ncs_thread_mutex);
	thread->noblock = noblock;
	pthread_mutex_unlock(&thread->ncs_thread_mutex);

	return NCS_THREAD_TRUE;
}

int ncs_thread_param_set (ncs_thread_t* thread,char* name,void* thread_func,void* arg)
{
	
	pthread_mutex_lock(&thread->ncs_thread_mutex);
	thread->ncs_thread_thread = (PTHREAD_CALLBACK_FUNC) thread_func;
	thread->private_data = (void *)arg;
	memset(thread->name,0,NCS_THREAD_NAME_LEN);
	thread->running_count = 0;
	thread->running_record = 0;
	thread->noblock = 0;
	thread->is_alive = 0;
	snprintf(thread->name,NCS_THREAD_NAME_LEN-1,"%s",name ? name:"Default");
	pthread_mutex_unlock(&thread->ncs_thread_mutex);

	return NCS_THREAD_TRUE;
}

int	ncs_thread_init(ncs_thread_t* thread)
{
	ncs_clear(thread,ncs_thread_t);
	
	pthread_mutex_init(&thread->ncs_thread_mutex,NULL);
	ncs_state_init(&thread->ncs_thread_state,NCS_THREAD_EXIT);
	list_init(&thread->list);

	return NCS_THREAD_TRUE;
}

int ncs_thread_running_update(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_FALSE;
	
	pthread_mutex_lock(&thread->ncs_thread_mutex);
	thread->running_count++;
	pthread_mutex_unlock(&thread->ncs_thread_mutex);
	
	return NCS_THREAD_TRUE;
}

int ncs_thread_is_running(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_FALSE;

	int ret = NCS_THREAD_FALSE;
	
	pthread_mutex_lock(&thread->ncs_thread_mutex);
	if (thread->running_record != thread->running_count) {
		thread->running_record = thread->running_count;
		thread->is_alive = NCS_THREAD_TRUE;
		ret = NCS_THREAD_TRUE;
	} else {
		thread->is_alive = NCS_THREAD_FALSE;
		ret = NCS_THREAD_FALSE;
	}
	pthread_mutex_unlock(&thread->ncs_thread_mutex);
	
	return ret;
}

int ncs_thread_list_add(ncs_thread_t* thread, struct list_head*	header)
{
	if (!thread) return NCS_THREAD_ERROR;
	list_add_tail(&thread->list,header);
	return TRUE;
}

int ncs_thread_list_delete(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_ERROR;
	list_del(&thread->list);
	return TRUE;
}

int ncs_thread_start(ncs_thread_t* thread,char* name,void* thread_func,void* arg)
{
	if (!thread) return NCS_THREAD_ERROR;

	if (ncs_thread_param_set(thread,name,thread_func,arg) != NCS_THREAD_TRUE) {
		SPON_LOG_ERR("thread[%s] param error \n",thread->name);
		return NCS_THREAD_ERROR;
	}

	if(!thread_func) return NCS_THREAD_ERROR;
	
	if (ncs_thread_state_get(thread) != NCS_THREAD_EXIT) {
		SPON_LOG_ERR("thread[%s] in state:%d \n",thread->name,ncs_thread_state_get(thread));
		return NCS_THREAD_ERROR;
	}

	if (pthread_create(&thread->ncs_thread_tid, thread->attr, thread->ncs_thread_thread,thread) < 0){
		SPON_LOG_ERR("thread[%s] create error:%s\n",thread->name, strerror(errno));
		return NCS_THREAD_ERROR;
	} 

	return ncs_thread_state_set(thread,NCS_THREAD_CREATE);
}



int ncs_thread_wait_stop(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_ERROR;

	if (ncs_thread_state_get(thread) == NCS_THREAD_EXIT) {
		SPON_LOG_ERR("thread[%s] in state NCS_THREAD_EXIT \n",thread->name);
		return NCS_THREAD_ERROR;
	}
	/* 强制退出 */
	pthread_kill( thread->ncs_thread_tid, -1);
	
	/* 等待结束 */
	pthread_join(thread->ncs_thread_tid, NULL);
	
	return	ncs_thread_state_set(thread,NCS_THREAD_EXIT);
}

int ncs_thread_stop(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_ERROR;

	/* 强制退出 */
	pthread_kill( thread->ncs_thread_tid, -1);
	
	/* 等待结束 */
	pthread_join(thread->ncs_thread_tid, NULL);
	
	return	ncs_thread_state_set(thread,NCS_THREAD_EXIT);
}

const char* ncs_thread_state_name_get(ncs_thread_t* thread)
{
	static const char* state_name[] = {
		"NCS_THREAD_EXIT",
		"NCS_THREAD_CREATE",
		"NCS_THREAD_START",
		"NCS_THREAD_STOP",
	};
	int index = ncs_state_get(&thread->ncs_thread_state);
	return state_name[index];
}

int ncs_thread_print(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_ERROR;
	
	/*ncs_thread_state_name_get(thread)*/
	printf("Thread:[%-20s] \t id:[%-8d] \t running_count:[%-8d] \t running:[%-2d] \t state[%-2d] \n",
		thread->name,thread->ncs_thread_tid,thread->running_count,thread->is_alive,ncs_thread_state_get(thread));

	return NCS_THREAD_TRUE;
}

int ncs_thread_timer_record(ncs_thread_t* thread)
{
	if (!thread) return NCS_THREAD_ERROR;
	thread->timer_sec = time(NULL);
}


int ncs_thread_timer_string(ncs_thread_t* thread,char* buf,int len)
{
	if (!thread) return NCS_THREAD_ERROR;

	time_t tt;
	time(&tt);
	struct tm *t=localtime(&tt);
	if (buf && len > 0)
		snprintf(buf,len,"%4d%02d%02d-%02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	else
		printf("%4d%02d%02d-%02d:%02d:%02d\n",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

	return NCS_THREAD_TRUE;
}



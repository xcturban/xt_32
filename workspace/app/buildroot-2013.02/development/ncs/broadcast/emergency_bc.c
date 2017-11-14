#include <common_lib/common_lib.h>
#include <ncs_event/ncs_event.h>

#include "emergency_bc.h"
//#include "call_window.h"
#include "system_info.h"
//#include "window_focus.h"
//#include "local_video.h"
#include "term.h"
#include "ncs.h"
#include "sip.h"
#include "user_msg.h"


// ï¿½ï¿½ï¿½ï¿½ï¿½ã²¥ï¿½ï¿½Ê±ï¿½ï¿½
static timer_t emergency_bc_timer_id;
// ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½
static bool_t emergency_bc_timer_start_flag = FALSE;

// emergency_bc_timer_func - ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½Ú»Øµï¿½ï¿½ï¿½ï¿½ï¿½
static void emergency_bc_timer_func(union sigval v)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct broadcast *bc = ncs_manager->bc;

	emergency_bc_timer_start_flag = FALSE;

	SPON_LOG_INFO("emergency_bc_timer_func\n");
	pthread_mutex_lock(&term->mutex);

#if 0
	set_system_task(TASK_NCS_ALL_AREA_BROADCAST_OUTGOING);
	ncs_manager->ncs_call_stop_all(ncs_manager, FALSE);


	term->sip_manager->call_stop();

	if (ncs_manager->online)
		bc->ncs_global_broadcast_request(bc);
	else {
		bc->ncs_broadcast_outgoing_start(bc, BC_START_RING);
	}
#endif
	virtual_key_send(kKeySOSOut);
	
	pthread_mutex_unlock(&term->mutex);
}

/**
 * emergency_bc_timer_start - ï¿½ï¿½ï¿½ï¿½ï¿½ã²¥ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½Ê¼
 *
 * ï¿½ï¿½ï¿½ï¿½×¡'ï¿½ï¿½ï¿½ï¿½'ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ò£¬»ï¿½ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½1ï¿½ï¿½ï¿½ÓµÄ¶ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½1ï¿½ï¿½ï¿½Ó²ï¿½ï¿½É¿ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã¶ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼È«ï¿½ï¿½ï¿½ã²¥ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
 * ï¿½ï¿½ï¿½ï¿½emergency_bc_timer_stopï¿½ï¿½ï¿½ï¿½É¾ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½
 */
int emergency_bc_timer_start(void)
{
	//window_t w = get_focus_window();
	task_t task = get_system_task();

	SPON_LOG_INFO("emergency_bc_timer_start task %d flag %d state %d \n",task,emergency_bc_timer_start_flag,TASK_NCS_BROADCAST_INCOMING);
	//if (w == SETTINGS_ABOUT_WINDOW)
	//	return;

	if (task == TASK_NCS_BROADCAST_INCOMING || task == TASK_NCS_ALL_AREA_BROADCAST_OUTGOING ||
		task == TASK_NCS_AREA_BROADCAST_OUTGOING || task == TASK_NCS_TERM_BROADCAST_OUTGOING)
		return;

	if (emergency_bc_timer_start_flag == FALSE) {
		struct itimerspec its;

		emergency_bc_timer_start_flag = TRUE;

		its.it_value.tv_sec = 1;
		its.it_value.tv_nsec = 0;
		its.it_interval.tv_sec = 0;
		its.it_interval.tv_nsec = 0;

		return common_timer_create(&emergency_bc_timer_id, &its, emergency_bc_timer_func, NULL);
	}

	return 0;
}


// emergency_bc_timer_stop - ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½É¾ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
int emergency_bc_timer_stop(void)
{
	int ret;
	
	SPON_LOG_INFO("emergency_bc_timer_stop %d\n",emergency_bc_timer_start_flag);

	if (emergency_bc_timer_start_flag == FALSE)
		return 0;

	ret = common_timer_delete(emergency_bc_timer_id);
	emergency_bc_timer_start_flag = FALSE;

	return ret;
}

//ÔÀÑô¼àÓü¶¨ÖÆ°æ£¬½ô¼±°´Å¥¿ªÊ¼Ò»¼üºô½Ð
void emergency_key_up_handle(void)
{
    int ret = 0;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct call_manager *call_manager = term->call_manager;

    ret = ncs_manager->ncs_ip_request(ncs_manager, 0xFFFF, 0, TALK);
    if(ret < 0)
    {
        return;
    }
    set_system_task(TASK_NCS_CALL_OUTGOING);
    virtual_key_send(kKeyCallOut);
    call_manager_set_flag_emergency(call_manager, TRUE);
}


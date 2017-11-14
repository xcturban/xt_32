
#include <errno.h>
#include <common_lib/common_lib.h>
#include <alsa/asoundlib.h>
#include <EchoCancel.h>

#include "mediastreamer2/mediastream.h"
#include "mediastreamer2/waveheader.h"
#include "term.h"
#include "ncs.h"
#include "ncs_ring.h"
#include "missed_call.h"
#include "dialed_call.h"
#include "received_call.h"

#include "system_info.h"
/*#include "ui.h"*/
#include "sip.h"
#include "offline.h"
/*#include "keypad.h"*/
#include "user_msg.h"
#include "guipub.h"
#include "guibroadcast.h"
#include "lpconfig.h"
#include "log.h"
#include "emegcywarring.h"



static LIST_HEAD(term_name_list_head);
static LIST_HEAD(area_name_list_head);
static LIST_HEAD(boardext_name_list_head);

#define BULK_ID_QUERY_COUNT_MAX 40
#define NAME_LEN_MAX 20

struct __name
{
    uint16_t id;
    uint8_t name[NAME_LEN_MAX];
    uint16_t call_map0;
    uint16_t call_map3;
    // uint16_t call_reserve;
};

struct __name_ex
{
    uint32_t id;
    uint8_t name[NAME_LEN_MAX];
    uint16_t call_map0;
    uint16_t call_map3;
    // uint16_t call_reserve;
};

typedef struct
{
    struct __name n;
    struct list_head list;
} name_t;

typedef struct
{
    struct __name_ex n;
    struct list_head list;
} name_ex_t;

#include "ncs_video.h"


static uint8_t s_tmp_query_kfwq_flag = 0;
// ɾ���Խ�������ʱ��
static int inline call_request_timer_delete(call_t *call)
{
    int ret;

    if (!call->call_request_timer_start)
        return 0;

    ret = common_timer_delete(call->call_request_timer_id);
    call->call_request_timer_start = FALSE;

    return ret;
}

static void ncs_call_request(call_t *call);
// �Խ�������ʱ������
static void call_request_timer_func(union sigval v)
{
    call_t *call;
    ncs_event_t *evt;

    pthread_mutex_lock(&term->mutex);

    call = (call_t *)(v.sival_ptr);

    ncs_call_request(call);
    /*evt = ncs_call_request_event_build(&call->target_sockaddr, call->target_id, call->board_id, 0, 0);*/
    /*if (evt)*/
    /*{*/
    /*ncs_event_send(evt);*/
    /*}*/

    pthread_mutex_unlock(&term->mutex);
}

void ncs_kfwqName_set(void *pName)
{
	char *pData = (char*)pName;
	struct ncs_manager* ncs_manager = term->ncs_manager;

	strcpy(ncs_manager->kfwqName, pData);
	send_user_message(MSG_NCS_CHATTING_REFRESH, 1, 0);
}

void ncs_warring_get(char param1, char param2,  char *multi_ip)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
	TYPE_EMEGCY_WARRING_INFO *pInfo = g_pEmegcyWarring;

    SPON_LOG_INFO("=====>%s - multi_ip:%s \n", __func__, multi_ip);

    if (term->sip_manager->busy())
        return;

    pthread_mutex_lock(&term->mutex);


    if (!bc->bc_outgoing(bc))
    {
		if(param1 == 0x01)
		{
			SendNotifyMessage(system_info.main_hwnd, MSG_KEYUP, param2, KKEYWARINGBEGIN);
            bc->ncs_wav_broadcast_task_start(bc, multi_ip);
			pInfo->RevEmegcyWarringFromServer(pInfo, param2, ENUM_EMEGCY_WARRING_STATUE_PLAY);
		}
		else if(param1 == 0)
		{
			SendNotifyMessage(system_info.main_hwnd, MSG_KEYUP, pInfo->CurWarringIndex, KKEYWARINGOVER);
			bc->ncs_wav_broadcast_task_stop(bc);
			pInfo->RevEmegcyWarringFromServer(pInfo, param2, ENUM_EMEGCY_WARRING_STATUE_STOP);
		}
    }

    pthread_mutex_unlock(&term->mutex);
}

char* ncs_kfwqName_get(void)
{
	struct ncs_manager* ncs_manager = term->ncs_manager;

	return ncs_manager->kfwqName;
}

// �����Խ�������ʱ��
static int call_request_timer_create(call_t *call)
{
    struct itimerspec its;

    call_request_timer_delete(call);

    call->call_request_timer_start = TRUE;

    its.it_value.tv_sec     = 4;
    its.it_value.tv_nsec    = 0;
    its.it_interval.tv_sec  = 4;
    its.it_interval.tv_nsec = 0;

    return common_timer_create(&call->call_request_timer_id, &its, call_request_timer_func, (void *)call);
}

// 停止监听任务
static void ncs_monitor_stop(struct ncs_manager *ncs_manager)
{
    struct call_manager *call_manager = term->call_manager;
    call_t *call = call_manager->monitor_call_get(call_manager);

    if (call)
    {
        ncs_manager->ncs_call_stop(ncs_manager, call, FALSE);
    }
}

// 呼叫应答
static void ncs_call_answer(call_t *call)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    ncs_event_t *evt;
    evt = ncs_call_answer_event_build(&call->target_sockaddr, call->target_id, call->board_id, 0, 0);
    if (evt)
        ncs_event_send(evt);
    if(term->customer_type == CUSTOMER_QGLW && !ncs_manager->flag_qurry_name_call_in)
    {
        if(ncs_manager->ops.ncs_get_device_server_type_rcd() != -1)
        {
            ncs_manager->flag_qurry_name_call_in = TRUE;
            evt = ncs_term_name_bulk_query_event_buildEx(call->target_id, ncs_manager->ops.ncs_get_device_server_type_rcd());
            if (evt)
                ncs_event_send(evt);
        }
    }
    else if(term->customer_type == CUSTOMER_LNJY)
	{
		if(s_tmp_query_kfwq_flag == FALSE)
		{
			s_tmp_query_kfwq_flag = TRUE;
			evt = ncs_term_name_bulk_query_event_buildExx(call->target_id%1000);
			if (evt)
				ncs_event_send(evt);
		}
	}
}

static void ncs_call_incoming_ring(call_t *call)
{
    struct call_manager *call_manager = term->call_manager;
    if (call_manager->calls_total_get(call_manager) == 1)
        ncs_ring_start(INCOMING, call->target_id);
}

static void ncs_call_outgoing_ring(void)
{
    struct call_manager *call_manager = term->call_manager;
    if (call_manager->calls_total_get(call_manager) == 1)
        ncs_ring_start(OUTGOING, 0);
}

static void ncs_target_offline_ring(void)
{
    struct call_manager *call_manager = term->call_manager;
    if (call_manager->calls_total_get(call_manager) == 0)
        ncs_ring_start(TARGET_OFFLINE, 0);
}

static void ncs_target_busy_ring(void)
{
    struct call_manager *call_manager = term->call_manager;
    if (call_manager->calls_total_get(call_manager) == 0)
        ncs_ring_start(TARGET_BUSY, 0);
}

static void ncs_target_onhook_ring(void)
{
    struct call_manager *call_manager = term->call_manager;
    if (call_manager->calls_total_get(call_manager) == 0)
        ncs_ring_start(TARGET_ONHOOK, 0);
}

// 发送呼叫请求
static void ncs_call_request(call_t *call)
{
    ncs_event_t *evt;

    if (call->type == TALK || call->type == SDK_TALK)
        evt = ncs_call_request_event_build(&call->target_sockaddr, call->target_id, call->board_id, 0, 0);
    else
        evt = ncs_monitor_request_event_build(&call->target_sockaddr, call->target_id, call->board_id, 0, 0);
    if (evt)
        ncs_event_send(evt);
}

// 发送视频停止指令
static void ncs_video_stop(call_t *call)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    ncs_event_t *evt = ncs_video_request_event_build(&call->target_sockaddr, STOP_VIDEO, ncs_manager->video_request_fmt, call->board_id, ncs_manager->video_ip_route, ncs_manager->video_port_route);
    if (evt)
        ncs_event_send(evt);
}

// 发送视频忙指令
static void ncs_video_busy(call_t *call)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    ncs_event_t *evt = ncs_video_state_event_build(&call->target_sockaddr, VIDEO_BUSY, call->board_id, ncs_manager->video_ip_route, ncs_manager->video_port_route);
    if (evt)
        ncs_event_send(evt);
}

static int ncs_call_start(struct ncs_manager *ncs_manager, call_t *call, bool_t passive)
{
    ncs_event_t *evt;
    struct call_manager *call_manager = term->call_manager;
    struct audio_manager *audio_manager = term->audio_manager;

    if (call == NULL || call->started == TRUE)
        return -1;

    // sdk���п��������з���ʼ�Խ�
    if (passive == FALSE && call->dir == OUT && call->type != SDK_TALK)
        return -1;

    call->started = TRUE;	// ֹͣ����

    // ���з�ɾ���Խ�������ʱ��
    if (call->dir == OUT)
        call_request_timer_delete(call);

    ncs_ring_stop();

    if (call->type == MONITOR || call->type == SDK_MONITOR)
    {
        audio_manager->set_bc_mode(audio_manager);

        if (call->dir == IN) // 被监听
            audio_manager->stream_create(audio_manager, AUDIO_READ, 8000, NULL, NULL, &call->target_sockaddr);
        else // 监听
            audio_manager->stream_create(audio_manager, AUDIO_WRITE, 8000, &call->mutex, &call->bufferizer, NULL);
    }
    else
    {
        Turn_obj_vol_dev_src();
        audio_manager->set_talk_mode(audio_manager);

        audio_manager->conference_create(audio_manager, 8000);
        audio_manager->conference_member_add(audio_manager, &call->mutex, &call->bufferizer, &call->target_sockaddr);
    }

    audio_manager->set_talk_out_volume(audio_manager, -1);
    audio_manager->set_talk_in_volume(audio_manager, -1);

    if (call->dir == IN || call->type == SDK_TALK)
    {
        evt = ncs_call_start_event_build(&call->target_sockaddr, call->target_id, call->board_id, 0, 0);
        if (evt)
            ncs_event_send(evt);
    }

    evt = ncs_call_start_notify_server_event_build(call->target_id, call->board_id);
    if (evt)
        ncs_event_send(evt);

    if(term->customer_type == CUSTOMER_YYJY && (call->type != MONITOR))
    {
        ncs_manager->ncs_video_request(ncs_manager, call);
    }
    //视频请求对讲是在呼叫２４返回之后请求的，而监听是不会返回的，所以在这里请求视频
    if(call->dir == OUT && (call->type == SDK_MONITOR || call->type == MONITOR))
    {
        ncs_manager->ncs_video_request(ncs_manager, call);
    }

    if(term->customer_type == CUSTOMER_YYJY && call_manager_get_flag_emergency(call_manager) == TRUE)
    {
        short_out_open();
    }
    return 0;
}

static void ncs_ecpt_type_set(char type)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    ncs_manager->dev_ecpt_type = type;
}

static char ncs_ecpt_type_get(void)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    return ncs_manager->dev_ecpt_type;
}

static int ncs_call_stop(struct ncs_manager *ncs_manager, call_t *call, bool_t passive)
{
    ncs_event_t *evt;
    struct sockaddr_in *si = (struct sockaddr_in *)(&call->target_sockaddr);
    struct audio_manager *audio_manager = term->audio_manager;
    struct call_manager *call_manager = term->call_manager;
    if (term->customer_type == CUSTOMER_JXWGS || term->customer_type == CUSTOMER_YYJY)
    {
        //short_out_open();
        call_manager_set_flag_emergency(call_manager, FALSE);
    }

    if (call == NULL)
    {
        SPON_LOG_INFO("call equal null\n");
        return -1;
    }
    ncs_video_stream_stop(VideoStreamRecvOnly, &call->target_sockaddr, NULL, 0);
    ncs_video_stream_stop(VideoStreamSendOnly, &call->target_sockaddr, inet_ntoa(si->sin_addr), ncs_manager->video_port);
    ncs_video_stop(call);
	if(term->customer_type==CUSTOMER_LNJY)
	{
		if(strlen(ncs_kfwqName_get()))
		{
			s_tmp_query_kfwq_flag = FALSE;
			memset(ncs_manager->kfwqName, 0, sizeof(ncs_manager->kfwqName));
		}
	}
    /*usleep(100*1000);*/
    call_manager->ip_get_ok = FALSE;
    //挂断对讲清除加密类型
    if(term->customer_type == CUSTOMER_QGLW)
    {
        ncs_manager->flag_qurry_name_call_in = FALSE;
        ncs_ecpt_type_set(term->device_type);
        if(strlen(ncs_manager->term_qglw_name))
        {
            memset(ncs_manager->term_qglw_name, 0, 18);
        }
    }
    if (call->started == FALSE)
    {
        SPON_LOG_INFO("=====>%s calls total:%d\n", __func__, call_manager->calls_total_get(call_manager));
        if (call_manager->calls_total_get(call_manager) == 1)
            ncs_ring_stop();

        if (call->dir == OUT)
        {
            call_request_timer_delete(call);
            //dialed_call_add(call->target_id, call->board_id);
        }

        if (call->dir == IN && passive == TRUE)
            missed_call_add(call->target_id, call->board_id);
    }

    if (call->started == TRUE)
    {
        call->started = FALSE;

        if (call->type == TALK || call->type == SDK_TALK)
        {
            audio_manager->conference_member_remove(audio_manager, &call->target_sockaddr);
        }
        else
        {
            int dir;
            if (call->dir == IN)
                dir = AUDIO_READ;
            else
                dir = AUDIO_WRITE;
            audio_manager->stream_destroy(audio_manager, dir);
        }
        if (call->dir == OUT && call->type == TALK)
        {
            dialed_call_add(call->target_id, call->board_id);
        }
        if (call->dir == IN && call->type == TALK)
        {
            received_call_add(call->target_id, call->board_id);
        }
    }

    SPON_LOG_INFO("=====>%s\n",__func__);

    if (passive == FALSE)
    {
        if (call->dir == OUT)
        {
            evt = ncs_call_stop_by_caller_event_build(&call->target_sockaddr, call->target_id, call->board_id);
        }
        else
        {
            evt = ncs_call_stop_by_callee_event_build(&call->target_sockaddr, call->target_id, call->board_id);
        }
        if (evt)
        {
            ncs_event_send(evt);
        }
    }

    evt = ncs_call_stop_notify_server_event_build(call->target_id, call->board_id);
    if (evt)
    {
        ncs_event_send(evt);
    }

    call_manager->call_delete(call_manager, call);

    if(call_manager->calls_total_get(call_manager) == 0)
    {
        audio_manager->mic_set(audio_manager, FALSE);
		if(term->customer_type == CUSTOMER_XMBR)
		{
			TYPE_MSG_STRUCT Tmp;
			if(GetSysStatueCloseBroadcastFlag() == TRUE)
			{
				SetSysStatueCloseBroadcastFlag(FALSE);
				Tmp.MsgCmd = ENUM_ENTER_REV_BROADCAST;
				EnterQueue(term->queue, Tmp);
				return 1;
			}
		}
    }
    return 0;
}

// 发送视频请求
static void ncs_video_request(struct ncs_manager *ncs_manager, call_t *call)
{
    if (!ncs_video_stream_recv)
    {
        ncs_event_t *evt = ncs_video_request_event_build(&call->target_sockaddr, BOARD_VIDEO, ncs_manager->video_request_fmt, call->board_id, ncs_manager->video_ip_route, ncs_manager->video_port_route);
        if (evt)
            ncs_event_send(evt);
    }
}


static void ncs_set_device_server_type_rcd(int server_type_rec)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    ncs_manager->server_type_device_rcdno = server_type_rec;
}

static int ncs_get_device_server_type_rcd(void)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    return ncs_manager->server_type_device_rcdno;
}

static int ncs_ip_request(struct ncs_manager *ncs_manager, uint16_t target_id, uint8_t board_id, ncs_call_t type)
{
    ncs_event_t *evt;
    task_t task = get_system_task();
    call_t *call;
    struct broadcast *bc = ncs_manager->bc;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    if(term->customer_type != CUSTOMER_QGLW)
    {
        if (target_id == ncs_manager->id)
            return -1;
    }
    else
    {
        if(ncs_manager->ops.ncs_get_device_server_type_rcd() == -1)
        {
            if (target_id == ncs_manager->id)
                return -1;
        }
    }


    if (term->sip_manager->busy())
        return -1;

    ncs_monitor_stop(ncs_manager);

    if (bc->bc_outgoing(bc))
        return -1;

    if (bc->bc_incoming(bc) && ncs_manager->priority == PRIO_TALK)
        bc->ncs_wav_broadcast_task_stop(bc);

    if (!ncs_manager->conference_enable && call_manager->calls_total_get(call_manager))
        return -1;

    if (ncs_manager->conference_enable && call_manager->calls_total_get(call_manager) == ncs_manager->max_calls)
        return -1;

    call = call_manager->call_find_by_id(call_manager, target_id);
    if (call)
        return -1;

    if (ncs_manager->online == FALSE)
    {
        // �ѻ��Խ�
        struct offline_call *oc;
        struct sockaddr_in sin;

        oc = find_offline_call_by_id(target_id);
        if (oc == NULL)
        {
            SPON_LOG_INFO("=====>%s - find_offline_call_by_id not found\n", __func__);
            return -1;
        }

        SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, oc->ip, oc->port);

        sockaddr_convert(&sin, oc->ip, oc->port);

        call = call_manager->call_new(call_manager, type, OUT, target_id, board_id, (struct sockaddr *)&sin);
        if (!call)
        {
            SPON_LOG_ERR("=====>%s - call_new failed:%s\n", __func__, strerror(errno));
            return -1;
        }

        ncs_call_request(call);
        ncs_video_request(ncs_manager, call);

        call_request_timer_create(call);
    }
    else
    {
        call = call_manager->call_new(call_manager, type, OUT, target_id, board_id, NULL);
        if (!call)
        {
            SPON_LOG_ERR("=====>%s - call_new failed:%s\n", __func__, strerror(errno));
            return -1;
        }

        if (type == TALK || type == SDK_TALK)
            evt = ncs_ip_request_event_build(target_id, board_id, TALK, ncs_get_device_server_type_rcd());
        else
            evt = ncs_ip_request_event_build(target_id, board_id, MONITOR, ncs_get_device_server_type_rcd());
        if (evt)
            ncs_event_send(evt);
        if((term->customer_type == CUSTOMER_QGLW) && (ncs_get_device_server_type_rcd() != -1))
        {
            evt = ncs_term_name_bulk_query_event_buildEx(target_id, ncs_get_device_server_type_rcd());
            if (evt)
                ncs_event_send(evt);
        }
    }

    return 0;
}


static bool_t ncs_video_recv_start(struct ncs_manager *ncs_manager)
{
    if (ncs_video_stream_recv)
        return TRUE;
    else
        return FALSE;
}

static int ncs_term_name_list_size_get(struct ncs_manager *ncs_manager)
{
    int size = 0;
    struct list_head *p;

    list_for_each(p, &term_name_list_head)
    size++;

    return size;
}

static int ncs_area_name_list_size_get(struct ncs_manager *ncs_manager)
{
    int size = 0;
    struct list_head *p;

    list_for_each(p, &area_name_list_head)
    size++;
	
    return size;
}

static int ncs_boardext_name_list_size_get(struct ncs_manager *ncs_manager)
{
    int size = 0;
    struct list_head *p;

    list_for_each(p, &boardext_name_list_head)
    size++;

    return size;
}

static void ncs_term_name_modify_by_id(struct ncs_manager *ncs_manager, uint16_t id, char *pName)
{
    struct list_head *p;
    name_t *t;
    list_for_each(p, &term_name_list_head)
    {
        t = list_entry(p, name_t, list);
        if (t->n.id == id)
        {
            strcpy(t->n.name, pName);
        }
    }
}

static char *ncs_term_name_get_by_id(struct ncs_manager *ncs_manager, uint16_t id)
{
    struct list_head *p;
    name_t *t;
    list_for_each(p, &term_name_list_head)
    {
        t = list_entry(p, name_t, list);
        if (t->n.id == id)
        {
            return t->n.name;
        }
    }
    return NULL;
}

static char *ncs_area_name_get_by_id(struct ncs_manager *ncs_manager, uint16_t id)
{
    struct list_head *p;
    name_t *t;
    list_for_each(p, &area_name_list_head)
    {
        t = list_entry(p, name_t, list);
        if (t->n.id == id)
        {
            return t->n.name;
        }
    }
    return NULL;
}

static char *ncs_area_name_get_by_index(struct ncs_manager *ncs_manager, uint16_t idx)
{
	int index = 0;
    struct list_head *p;
    name_t *t;
    list_for_each(p, &area_name_list_head)
    {
        t = list_entry(p, name_t, list);
        if (index == idx)
        {
            return t->n.name;
        }
		index++;
    }
    return NULL;
}


static char *ncs_area_id_get_by_index(struct ncs_manager *ncs_manager, uint16_t idx)
{
	int index = 0;
    struct list_head *p;
    name_t *t;
    list_for_each(p, &area_name_list_head)
    {
        t = list_entry(p, name_t, list);
        if (index == idx)
        {
            return t->n.id;
        }
		index++;
    }
    return NULL;
}


static char *ncs_boardext_name_get_by_id(struct ncs_manager *ncs_manager, uint32_t id)
{
    struct list_head *p;
    name_ex_t *t;
    list_for_each(p, &boardext_name_list_head)
    {
        t = list_entry(p, name_ex_t, list);
        if (t->n.id == id)
        {
            return t->n.name;
        }
    }
    return NULL;
}

static void ncs_door_open(struct ncs_manager *ncs_manager, call_t *call)
{
    ncs_event_t *evt = ncs_door_open_event_build(&call->target_sockaddr);
    if (evt)
        ncs_event_send(evt);
}

static void ncs_term_status_ctl(struct ncs_manager *ncs_manager, call_t *call, uint8_t ctl_type, uint8_t port, uint8_t port_status, uint16_t trigger_times)
{
    ncs_event_t *evt = ncs_term_status_ctl_event_build(&call->target_sockaddr, ctl_type, port, port_status, ncs_manager->trigger_time);
    if (evt)
        ncs_event_send(evt);
}

static void ncs_ptt_status_send(struct ncs_manager *ncs_manager, call_t *call, ncs_ptt_status_t s)
{
    ncs_event_t *evt;
    evt = ncs_ptt_control_event_build(&call->target_sockaddr, s);
    if (evt)
        ncs_event_send(evt);
}

static void ncs_short_out_send(struct ncs_manager *ncs_manager, call_t *call)
{
    ncs_event_t *evt;
    evt = ncs_call_short_out_event_build(&call->target_sockaddr);
    if (evt)
        ncs_event_send(evt);
}

static void ncs_ctrl_haoyun_term_short(struct ncs_manager *ncs_manager, int keyNum)
{
    ncs_event_t *evt;
    evt = ncs_call_ctrl_haoyun_event_build(&term->sip_manager->sip_sdk_tool_sockaddr, keyNum, (uint16_t)atoi(term->sip_manager->get_target_number()));

    if (evt)
        ncs_event_send(evt);
}

static void ncs_video_switch(struct ncs_manager *ncs_manager, call_t *call, video_class_t class, uint8_t board_id)
{
//	if (term->customer_type != CUSTOMER_ZTE) {
    ncs_event_t *evt;

#if 0
    // �ȷ���Ƶֹͣ
    evt = ncs_video_request_event_build(&call->target_sockaddr, STOP_VIDEO, call->board_id, ncs_manager->video_ip_route, ncs_manager->video_port_route);
    if (evt)
        ncs_event_send(evt);
#endif
    // �ٷ���Ƶ����
    evt = ncs_video_request_event_build(&call->target_sockaddr, class, ncs_manager->video_request_fmt, board_id, ncs_manager->video_ip_route, ncs_manager->video_port_route);
    if (evt)
        ncs_event_send(evt);
//	}
}

static void ncs_call_transfer(struct ncs_manager *ncs_manager, uint16_t target_id)
{
    ncs_event_t *evt = ncs_call_transfer_event_build(target_id);
    if (evt)
    {
        ncs_event_send(evt);
    }
}



// 停止所有对讲任务
static void ncs_call_stop_all(struct ncs_manager *ncs_manager, bool_t passive)
{
    call_t *call;
    struct call_manager *call_manager = term->call_manager;
    while (call = call_manager->current_call_get(call_manager))
    {
        ncs_call_stop(ncs_manager, call, passive);
    }
}

static bool_t ncs_call_stop_all_ex_talking(struct ncs_manager *ncs_manager, bool_t passive)
{
    call_t *call_arr[8];
    call_t *call = NULL;
    int i   = 0;
    bool_t flag = FALSE;
    struct list_head *p;
    struct call_manager *call_manager = term->call_manager;
    int total = call_manager->calls_total_get(call_manager);
    list_for_each(p, &call_manager->call_list)
    {
        call = list_entry(p, call_t, list);
        if(call)
        {
            if(i < 8)
            {
                call_arr[i++] = call;
            }
        }
    }

    for(i = 0; i < total; i++)
    {
        if(call_arr[i]->started == FALSE)
        {
            ncs_call_stop(ncs_manager, call_arr[i], passive);
            usleep(10*1000);
        }
        else
        {
            flag = TRUE;
        }
    }
    return flag;
}

// ====================================================================================================================================

// 端口输出控制
static void ncs_port_out_ctl_get(int port, int status)
{
    SPON_LOG_INFO("=====>%s - port:%d status:%d\n", __func__, port, status);
    if (port == 1 && status == 0)
        short_out_close();
    else if (port == 1 && status == 1)
        short_out_open();

    //if ((term->customer_type == CUSTOMER_JXWGS || term->customer_type == CUSTOMER_YYJY) && status == 1)
    //    short_out_close();
}

static void ncs_auto_answer_ctl_get(int status)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    SPON_LOG_INFO("=====>%s - status:%d\n", __func__, status);

    ncs_manager->auto_answer = status;
}



static char ncs_get_term_custom_type(void)
{
    return term->customer_type;;
}

// ��������������
static void ncs_volume_get(uint8_t volume, void *private_data)
{
    //SPON_LOG_INFO("=====>%s - volume:%d\n", __func__, volume);

    term->audio_manager->set_bc_out_volume(term->audio_manager, volume);
    //这里暂时这么改着,对讲音量在这里其实不应该被改变
    term->audio_manager->set_talk_out_volume(term->audio_manager, volume);
    SendNotifyMessage(system_info.main_hwnd, MSG_NCS_STATUE_BAR_PAGE, 1, 0);
}

// ��������������
static void ncs_terminal_password_get(char *password, void *private_data)
{
    struct _LpConfig *ini_cfg;

    SPON_LOG_INFO("=====>%s - password:%s\n", __func__, password);

    ini_cfg = lp_config_new(INI_CONFIG_FILE);
    if (!ini_cfg)
    {
        SPON_LOG_ERR("=====>%s - lp_config_new failed\n", __func__, strerror(errno));
        return;
    }

    lp_config_set_string(ini_cfg, "terminal", "password", password);

    lp_config_sync(ini_cfg);
    lp_config_destroy(ini_cfg);

    system_config_save();
    //system("reboot");
}

// �ն���Ϣǿ��
static void ncs_terminal_info_get(uint16_t terminal_id,
                                  char *server_ip,
                                  uint16_t server_port,
                                  char *gw,
                                  char *mask,
                                  char *mac_addr,
                                  char *terminal_ip,
                                  uint16_t terminal_port,
                                  void *private_data)
{
    struct _LpConfig *ini_cfg;

    SPON_LOG_INFO("=====>%s - terminal_id:%d server_ip:%s server_port:%d gw:%s mask:%s mac_addr:%s terminal_ip:%s terminal_port:%d\n",
                  __func__, terminal_id, server_ip, server_port, gw, mask, mac_addr, terminal_ip, terminal_port);

    ini_cfg = lp_config_new(INI_CONFIG_FILE);
    if (!ini_cfg)
    {
        SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
        return;
    }

    lp_config_set_int(ini_cfg, "terminal", "id", terminal_id);
    lp_config_set_string(ini_cfg, "terminal", "ip", terminal_ip);
    lp_config_set_int(ini_cfg, "terminal", "port", terminal_port);
    lp_config_set_string(ini_cfg, "terminal", "gate", gw);
    lp_config_set_string(ini_cfg, "terminal", "mask", mask);
    if(strlen(mac_addr) > 0)
    {
        lp_config_set_string(ini_cfg, "terminal", "mac_addr", mac_addr);
    }
    lp_config_set_string(ini_cfg, "server_cfg", "ip_server1", server_ip);
    lp_config_set_int(ini_cfg, "server_cfg", "port_server1", server_port);

    lp_config_sync(ini_cfg);
    lp_config_destroy(ini_cfg);

    if(strlen(mac_addr) >= 17)
    {
        ini_cfg = lp_config_new(INI_CONFIG_BAK_FILE);
        lp_config_set_string(ini_cfg, "terminal", "mac_addr", mac_addr);
        lp_config_sync(ini_cfg);
        lp_config_destroy(ini_cfg);
    }

    // SPON_LOG_INFO("=====>%s - reboot...\n", __func__);

    // ֹͣ��ͨ�Խ�
    // ncs_call_stop_all(FALSE);
    // ֹͣ���ܹ㲥
    // ncs_wav_broadcast_task_stop();
    // TODO:ֹͣ���͹㲥

    system_config_save();

	WEB_SYS_LOG(USRLOG_REBOOT);
    system("reboot");

    // TODO:����������ϵͳ
}

// ��ʼ������¼��
static void ncs_third_party_record_start_get(struct sockaddr *third_party_record_addr,
        void *private_data)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    SPON_LOG_INFO("=====>%s\n", __func__);

    memcpy(&ncs_manager->third_party_record_addr, third_party_record_addr, sizeof(struct sockaddr));
    ncs_manager->third_party_record = TRUE;
}

// ֹͣ������¼��
static void ncs_third_party_record_stop_get(void *private_data)
{
    //SPON_LOG_INFO("=====>%s\n", __func__);

    term->ncs_manager->third_party_record = FALSE;
}


static void ncs_sdk_call_stop_get(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data)
{
    call_t *call;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    pthread_mutex_lock(&term->mutex);

    call = call_manager->current_call_get(call_manager);
    if (call)
    {
        ncs_call_stop(term->ncs_manager, call, FALSE);
        virtual_key_send(kKeyCallStop);
    }

    pthread_mutex_unlock(&term->mutex);
}

static void ncs_sdk_call_request_get(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data)
{
    call_t *call;
    ncs_event_t *evt;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);
    /*printf("sdk manager->nameLen : %d\n", strlen(ncs_manager->term_qglw_name));*/


    pthread_mutex_lock(&term->mutex);


    // 正在发送广播
    if (bc->bc_outgoing(bc))
        goto BUSY;

    if (bc->bc_incoming(bc))
    {
        // 对讲优先的情况下停止接收广播
        if (ncs_manager->priority == PRIO_TALK)
            bc->ncs_wav_broadcast_task_stop(bc);
        else
            goto BUSY;
    }


    if (ncs_ip_request(ncs_manager, target_id, board_id, SDK_TALK) < 0)
        goto BUSY;



    set_system_task(TASK_NCS_CALL_OUTGOING);
    virtual_key_send(kKeyCallOut);

    pthread_mutex_unlock(&term->mutex);
    return;

BUSY:
    evt = ncs_call_busy_event_build(target_sockaddr, target_id, board_id);
    if (evt)
        ncs_event_send(evt);
    pthread_mutex_unlock(&term->mutex);
}

static void ncs_sdk_monitor_request_get(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data)
{
    ncs_event_t *evt;
    call_t *call;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
    struct audio_manager *audio_manager = term->audio_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    pthread_mutex_lock(&term->mutex);

    // 正在发送广播
    if (bc->bc_outgoing(bc))
        goto BUSY;

    if (bc->bc_incoming(bc))
    {
        // 对讲优先的情况下停止接收广播
        if (ncs_manager->priority == PRIO_TALK)
        {
            bc->ncs_wav_broadcast_task_stop(bc);
        }
        else
        {
            goto BUSY;
        }
    }

    if (ncs_ip_request(ncs_manager, target_id, board_id, SDK_MONITOR) < 0)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    ncs_ring_stop();

    set_system_task(TASK_NCS_MONITOR_OUTGOING);
    virtual_key_send(kKeyCallOut);

    pthread_mutex_unlock(&term->mutex);
    return;

BUSY:
    evt = ncs_call_busy_event_build(target_sockaddr, target_id, board_id);
    if (evt)
        ncs_event_send(evt);
    pthread_mutex_unlock(&term->mutex);
}

static void ncs_sdk_call_start_get(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data)
{
    call_t *call;
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);


    pthread_mutex_lock(&term->mutex);

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
    call = call_manager->current_call_get(call_manager);
    if (call)
    {
        int ret = ncs_call_start(call_manager, call, TRUE);
        if (ret == 0)
            virtual_key_send(kKeyCallStart);
    }


    pthread_mutex_unlock(&term->mutex);
}

static void ncs_call_reject_get(struct sockaddr *target_sockaddr,
                                uint16_t target_id,
                                uint8_t board_id,
                                void *private_data)
{
    call_t *call;
    bool_t started;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    pthread_mutex_lock(&term->mutex);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (call == NULL)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    started = call->started;

    ncs_call_stop(term->ncs_manager, call, TRUE);

    if (started == FALSE)
        ncs_target_busy_ring();
    else
        ncs_target_onhook_ring();


    virtual_key_send(kKeyCallReject);

    pthread_mutex_unlock(&term->mutex);
}

static void ncs_call_busy_get(struct sockaddr *target_sockaddr,
                              uint16_t target_id,
                              uint8_t board_id,
                              void *private_data)
{
    call_t *call;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    pthread_mutex_lock(&term->mutex);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (call == NULL || call->started)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    ncs_call_stop(term->ncs_manager, call, TRUE);

    ncs_target_busy_ring();

    virtual_key_send(kKeyCallBusy);

    pthread_mutex_unlock(&term->mutex);
}


static void ncs_call_stop_get(struct sockaddr *target_sockaddr,
                              uint16_t target_id,
                              uint8_t board_id,
                              void *private_data)
{
    call_t *call;
    bool_t server_stop_call = FALSE; // �����ǲ��Ƿ������Ҷ϶Խ�
    bool_t monitor_incoming = FALSE; // �����ǲ��Ǳ�����
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct audio_manager* audio_manager = term->audio_manager;
	uint8_t ret = 0;
    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    pthread_mutex_lock(&term->mutex);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (call == NULL)
    {
        if (memcmp(&ncs_manager->server_sockaddr, target_sockaddr, sizeof(ncs_manager->server_sockaddr)) != 0)
        {
            pthread_mutex_unlock(&term->mutex);
            return;
        }
        else
            server_stop_call = TRUE;

        SPON_LOG_INFO("=====>%s - call stop from server\n", __func__);
    }

    if (call && call->type == MONITOR && call->dir == IN)
        monitor_incoming = TRUE;

    if (server_stop_call)
        ncs_call_stop_all(ncs_manager, TRUE);
    else
       ret = ncs_call_stop(ncs_manager, call, TRUE);
    SPON_LOG_INFO("=====>%s monitor_incoming %d - call stop after %d\n", __func__, monitor_incoming, call_manager->calls_total_get(call_manager));

    if (monitor_incoming)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    if (server_stop_call == FALSE)
	{
		if(ret != 1)
		{
			ncs_target_onhook_ring();
		}
	}


    virtual_key_send(kKeyCallStop);


    pthread_mutex_unlock(&term->mutex);
}

// �յ���������
static void ncs_monitor_request_get(struct sockaddr *target_sockaddr,
                                    uint16_t target_id,
                                    uint8_t board_id,
                                    int samplerate,
                                    ncs_audio_encode_t encode,
                                    void *private_data)
{
    call_t *call;
    ncs_event_t *evt;
    struct broadcast *bc = term->ncs_manager->bc;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);

    pthread_mutex_lock(&term->mutex);

    if (term->sip_manager->busy())
        goto BUSY;


    if (bc->bc_incoming(bc) || bc->bc_outgoing(bc))
        goto BUSY;


    // �������������ȼ�����
    if (call_manager->calls_total_get(call_manager))
        goto BUSY;

    call = call_manager->call_new(call_manager, MONITOR, IN, target_id, board_id, target_sockaddr);
    if (!call)
    {
        SPON_LOG_ERR("=====>%s - call_new failed:%s\n", __func__, strerror(errno));
        pthread_mutex_unlock(&term->mutex);
        return;
    }

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
    // ������ʼ
    ncs_call_start(term->ncs_manager, call, FALSE);

    pthread_mutex_unlock(&term->mutex);
    return;

BUSY:
    pthread_mutex_unlock(&term->mutex);
    evt = ncs_call_busy_event_build(target_sockaddr, target_id, board_id);
    if (evt)
        ncs_event_send(evt);
}


static void ncs_realtime_wave_data_get(uint8_t *wave_data, int wave_data_len, void *private_data)
{
    mblk_t *m;
    /*int avail = 0;*/
    struct broadcast *bc = term->ncs_manager->bc;
    if (!bc->bc_incoming(bc))
        return;
    m = allocb(NCS_PCM_LEN, 0);
    if (m)
    {
        memcpy(m->b_wptr, wave_data, NCS_PCM_LEN);
        m->b_wptr += NCS_PCM_LEN;

			/*{*/
				/*static int newMs1 = 0;*/
				/*static int oldMs1 = 0;*/
				/*struct timeval tv1;*/
				/*struct timezone tz;*/
				/*gettimeofday(&tv1, &tz);*/
				/*newMs1 = tv1.tv_usec;*/
				/*printf("ms1= %d\n", newMs1-oldMs1);*/
				/*oldMs1 = newMs1;*/
			/*}*/
        pthread_mutex_lock(&bc->mutex);
        ms_bufferizer_put(&bc->bufferizer, m);
		/*avail = ms_bufferizer_get_avail(&bc->bufferizer);*/
		/*printf("avail 1: %d\n", avail);*/
        pthread_mutex_unlock(&bc->mutex);
    }
    /*#endif*/
}



// �յ������л�
static void ncs_task_switch_get(char *multi_ip, ncs_task_t task, void *private_data)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("=====>%s - multi_ip:%s task:%d\n", __func__, multi_ip, task);

    if (term->sip_manager->busy())
        return;

    pthread_mutex_lock(&term->mutex);


    ncs_monitor_stop(ncs_manager);

    if (call_manager->calls_total_get(call_manager) > 0 &&
            ncs_manager->priority == PRIO_TALK)
        goto EXIT;

    ncs_call_stop_all(ncs_manager, FALSE);


    if (!bc->bc_outgoing(bc))
    {
        if (task == WAVE_BROADCAST_TASK)
        {
            ncs_ring_stop();
            set_system_task(TASK_NCS_BROADCAST_INCOMING);
            virtual_key_send(kKeyTaskSwitch);
            bc->ncs_wav_broadcast_task_start(bc, multi_ip);
        }
    }

EXIT:
    pthread_mutex_unlock(&term->mutex);
}


static void ncs_task_idle_get(void *private_data)
{
    struct broadcast *bc = term->ncs_manager->bc;

    SPON_LOG_INFO("=====>%s\n", __func__);

    pthread_mutex_lock(&term->mutex);

    if (bc->bc_incoming(bc))
    {
        bc->ncs_wav_broadcast_task_stop(bc);
        /*virtual_key_send(kKeyTaskIdle);*/
        set_system_task(TASK_NONE);
        EnterWin(system_info.main_hwnd, WelComeWinProc);
    }

    pthread_mutex_unlock(&term->mutex);
}

static uint16_t area_bulk_query_id;
static void ncs_area_name_bulk_get(void *bulk, void *private_data)
{
    int j, empty_bulk = 0;
    struct __name *p = (struct __name *)bulk;
    ncs_event_t *evt;

    pthread_mutex_lock(&term->mutex);

    for (j = 0; j < BULK_ID_QUERY_COUNT_MAX; j++)
    {
        name_t *t = NULL;

        if (p->id == 0)
        {
            //pthread_mutex_unlock(&term->mutex);
            empty_bulk++;
			if(empty_bulk >= 10)
			{
				pthread_mutex_unlock(&term->mutex);
				return;
			}
			else
			{
				p++;
            	continue;
			}
        }
		else
		{
	        t = malloc(sizeof(name_t));
	        if (!t)
	        {
				pthread_mutex_unlock(&term->mutex);
	            return;
	        }

			empty_bulk = 0;
			
	        memset(t, 0, sizeof(*t));
	        memcpy(t, p, sizeof(*p));
			//SPON_LOG_INFO("area_name name %s id %d\n",p->name,p->id);
	        list_add_tail(&t->list, &area_name_list_head);
		}
        p++;		
    }
    area_bulk_query_id += BULK_ID_QUERY_COUNT_MAX;
    evt = ncs_area_name_bulk_query_event_build(area_bulk_query_id, BULK_ID_QUERY_COUNT_MAX);
    if (evt)
        ncs_event_send(evt);

    //SPON_LOG_INFO("=====>%s\n", __func__);

    pthread_mutex_unlock(&term->mutex);
}

// 呼叫请求
static void ncs_call_request_get(struct sockaddr *target_sockaddr,
                                 uint16_t target_id,
                                 uint8_t board_id,
                                 int samplerate,
                                 ncs_audio_encode_t encode,
                                 void *private_data)
{
    call_t *call;
    ncs_event_t *evt;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
    struct call_manager *call_manager = term->call_manager;

    struct sockaddr_in *sin = (struct sockaddr_in *)target_sockaddr;

    pthread_mutex_lock(&term->mutex);

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);
    SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
    if(term->customer_type != CUSTOMER_QGLW)
    {
        if (target_id == ncs_manager->id)
        {
            pthread_mutex_unlock(&term->mutex);
            return;
        }
    }
    else
    {
        if(ncs_manager->ops.ncs_get_device_server_type_rcd() == -1)
        {
            if (target_id == ncs_manager->id)
            {
                pthread_mutex_unlock(&term->mutex);
                return;
            }
        }
    }

    // 正在处理sip任务
    if (term->sip_manager->busy())
        goto BUSY;

    // 正在发送广播
    if (bc->bc_outgoing(bc))
        goto BUSY;

    if (bc->bc_incoming(bc))
    {
        // 对讲优先的情况下停止接收广播
        if (ncs_manager->priority == PRIO_TALK)
            bc->ncs_wav_broadcast_task_stop(bc);
		/*else*/
		/*{*/
			/*goto BUSY;*/
		/*}*/
		if(term->customer_type == CUSTOMER_XMBR)
		{
			guibroadcast_handup(system_info.main_hwnd);
			SetSysStatueCloseBroadcastFlag(TRUE);
		}
		else
		{
			goto BUSY;
		}
    }

    // 停止监听任务
    ncs_monitor_stop(ncs_manager);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (!call)   // 首次呼叫请求
    {

        // 对讲超限
        if (call_manager->calls_total_get(call_manager) == ncs_manager->max_calls)
            goto BUSY;

        // 分配一个call
        call = call_manager->call_new(call_manager, TALK, IN, target_id, board_id, target_sockaddr);
        if (!call)
        {
            SPON_LOG_ERR("=====>%s - call_new failed:%s\n", __func__, strerror(errno));
            pthread_mutex_unlock(&term->mutex);
            return;
        }

        if(term->customer_type != CUSTOMER_YYJY)
        {
            ncs_video_request(ncs_manager, call);
        }

        // 开始振铃
        if (call_manager->calls_total_get(call_manager) == 1)
        {
            ncs_call_incoming_ring(call);
        }

        set_system_task(TASK_NCS_CALL_INCOMING);
        virtual_key_send(kKeyCallIn);
    }

    if (call)
    {
        if (!call->started)
            ncs_call_answer(call); // 呼叫应答
        else
            goto EXIT;
    }

    // 自动接听
    call->request_times++;
    if (call->request_times > 1)
    {
        if (ncs_manager->auto_answer)
        {
            virtual_key_send(kKeyCallStart);
            ncs_manager->ncs_call_start(ncs_manager, call, FALSE);
        }
    }


EXIT:
    pthread_mutex_unlock(&term->mutex);
    return;

BUSY:
    missed_call_add(target_id, board_id); // 保存未接来电
    pthread_mutex_unlock(&term->mutex);
    evt = ncs_call_busy_event_build(target_sockaddr, target_id, board_id);
    if (evt)
        ncs_event_send(evt);
    return;
}

// 收到设置系统时间指令
static void ncs_system_time_get(int year, int month, int day, int hour, int minute, int second, int weekday, void *private_data)
{
    char date[128];
    static int flag = 1;

    if(flag == 1)
    {
        flag = 0;

        sprintf(date, "date -s '%d-%d-%d %d:%d:%d'", year, month, day, hour, minute, second);

        pthread_mutex_lock(&term->mutex);

        // 写入rtc
        system(date);
        system("hwclock -w");

        // 同时保存时间
        date_set(year, month, day, hour, minute, second, weekday);

        pthread_mutex_unlock(&term->mutex);
    }
}

// 收到广播发送停止指令
// sdk也会调用到此接口
static void ncs_broadcast_stop_get(void *private_data)
{
    struct broadcast *bc = term->ncs_manager->bc;

    SPON_LOG_INFO("=====>%s\n", __func__);

    pthread_mutex_lock(&term->mutex);

    bc->ncs_broadcast_outgoing_stop(bc, BC_STOP_RING);


    pthread_mutex_unlock(&term->mutex);
}

// 收到广播发送开始指令
// sdk也会调用到此接口
static void ncs_broadcast_start_get(bool_t ring, void *private_data)
{
    struct broadcast *bc = term->ncs_manager->bc;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    task_t task = get_system_task();

    SPON_LOG_INFO("=====>%s - ring:%d\n", __func__, ring);

    pthread_mutex_lock(&term->mutex);

    ncs_manager->ncs_call_stop_all(ncs_manager, FALSE);

    if (bc->bc_incoming(bc))
        bc->ncs_wav_broadcast_task_stop(bc);


    if (!bc->bc_outgoing(bc))
    {
        bc->ring = ring;
        if (task == TASK_NONE)
        {
            set_system_task(TASK_NCS_TERM_BROADCAST_OUTGOING);
            virtual_key_send(kKeyBcOut);
        }
        ncs_ring_stop();
        bc->ncs_broadcast_outgoing_start(bc, BC_START_RING);
    }


    pthread_mutex_unlock(&term->mutex);
}

// 收到视频状态指令
static void ncs_video_state_get(struct sockaddr *target_sockaddr,
                                video_state_t state,
                                uint8_t board_id,
                                char *ip,
                                uint16_t port,
                                void *private_data)
{
    SPON_LOG_INFO("=====>%s - ip:%s port:%d board_id:%d state:%d\n", __func__, ip, port, board_id, state);

    call_t *call;
    struct sockaddr_in *sin = (struct sockaddr_in *)target_sockaddr;
    struct call_manager *call_manager = term->call_manager;

    if (state == STOP_VIDEO || state == VIDEO_BUSY)
    {
        return;
    }

    pthread_mutex_lock(&term->mutex);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (call)
    {
        // 开始接收视频流
        ncs_video_stream_start(VideoStreamRecvOnly, target_sockaddr, inet_ntoa(sin->sin_addr), 0);
        // ui显示视频
        /*display_video();*/
    }

    pthread_mutex_unlock(&term->mutex);
}

void ncs_short_in_request(int gpi, int state)
{
    SPON_LOG_INFO("=====>%s - gpi:%d state:%d\n", __func__, gpi, state);

    ncs_event_t *evt = ncs_short_in_event_build(gpi, state);
    if (evt)
        ncs_event_send(evt);
}

// 解除警铃
void close_emergency_alarm(uint16_t id1, uint16_t id2)
{
    ncs_event_t *evt = close_emergency_alarm_event_build(id1, id2);
    if (evt)
        ncs_event_send(evt);
}

// 收到视频请求指令
static void ncs_video_request_get(struct sockaddr *target_sockaddr, // ���ͷ�
                                  video_class_t class, // ��Ƶ��������
                                  video_fmt_t fmt,	//��Ƶ������ʽ
                                  uint8_t board_id, // ������
                                  char *ip, // �Է���ip
                                  uint16_t port, // �Է��Ķ˿�
                                  void *private_data)
{
    ncs_event_t *evt;

    SPON_LOG_INFO("=====>%s - ip:%s port:%d board_id:%d class:%d fmt:%d nvs_send_total:%d\n", __func__, ip, port, board_id, class, fmt, nvs_send_total);

    if(term->video_type == FALSE)
    {
        SPON_LOG_INFO("video not support, return\n");
        return;
    }

    pthread_mutex_lock(&term->mutex);

    if (class == STOP_VIDEO)   // 视频停止
    {
        // 停止视频发送
        ncs_video_stream_stop(VideoStreamSendOnly, target_sockaddr, ip, port);
        // 回复视频状态
        evt = ncs_video_state_event_build(target_sockaddr, class, board_id, ip, port);
        if (evt)
            ncs_event_send(evt);
    }
    else
    {
        //VGA有问题,先改成默认为CIF格式
        /*if(fmt == VIDEO_1080P_FMT || fmt == VIDEO_720P_FMT || fmt == VIDEO_D1_FMT)*/
        fmt = VIDEO_CIF_FMT;

        // 设置视频格式
        if (nvs_send_total == 0)
            video_fmt_set(fmt);

        // 开始发送视频
        int ret = ncs_video_stream_start(VideoStreamSendOnly, target_sockaddr, ip, port);
        if (ret < 0)
        {
            evt = ncs_video_state_event_build(target_sockaddr, VIDEO_BUSY, board_id, ip, port);
            if (evt)
                ncs_event_send(evt);
            pthread_mutex_unlock(&term->mutex);
            return;
        }
        evt = ncs_video_state_event_build(target_sockaddr, class, board_id, ip, port);
        if (evt) ncs_event_send(evt);

    }

    pthread_mutex_unlock(&term->mutex);
}

static uint16_t term_bulk_query_id;
static void ncs_term_name_bulk_get(void *bulk, void *private_data)
{
    int j;
    struct __name *p = (struct __name *)bulk;
    ncs_event_t *evt;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    pthread_mutex_lock(&term->mutex);

    for (j = 0; j < BULK_ID_QUERY_COUNT_MAX; j++)
    {
        name_t *t = NULL;

        if (p->id == 0)
        {
            SPON_LOG_INFO("=====>%s\n", __func__);
            phonebook_info_update();
            ncs_manager->name = ncs_term_name_get_by_id(ncs_manager, ncs_manager->id);
            SendNotifyMessage(system_info.main_hwnd, MSG_NCS_STATUE_BAR_PAGE, 3, 1);
            //virtual_key_send(kKeyLoginOk);
            pthread_mutex_unlock(&term->mutex);
            return;
        }

        t = malloc(sizeof(name_t));
        if (!t)
            return;

        memset(t, 0, sizeof(*t));
        memcpy(t, p, sizeof(*p));

        list_add_tail(&t->list, &term_name_list_head);

        p++;
    }

    term_bulk_query_id += BULK_ID_QUERY_COUNT_MAX;
    evt = ncs_term_name_bulk_query_event_build(term_bulk_query_id, BULK_ID_QUERY_COUNT_MAX);
    if (evt)
    {
        ncs_event_send(evt);
    }

    pthread_mutex_unlock(&term->mutex);
}

static void ncs_term_name_qglw_get(void *bulk, void *private_data)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    pthread_mutex_lock(&term->mutex);

    strcpy(ncs_manager->term_qglw_name, &bulk[4]);
    //virtual_key_send(kKeyCallInfoUpdate);

    pthread_mutex_unlock(&term->mutex);
}

static void ncs_term_boardext_name_qglw_get(void *bulk, char lineNum, void *private_data)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct __name_ex *p = (struct __name_ex *)bulk;
    int i = 0;

    pthread_mutex_lock(&term->mutex);
    for(i = 0; i < lineNum; i++)
    {
        name_ex_t *t = NULL;
        t = malloc(sizeof(name_ex_t));
        if (!t)
            return;

        memset(t, 0, sizeof(*t));
        memcpy(t, p, sizeof(*p));

        list_add_tail(&t->list, &boardext_name_list_head);
        p++;
    }
    pthread_mutex_unlock(&term->mutex);
}


static void ncs_talk_wave_data_get(struct sockaddr *target_sockaddr,
                                   uint8_t *wave_data,
                                   int wave_data_len,
                                   void *private_data)
{
    call_t *call;
    mblk_t *m;
    struct call_manager *call_manager = term->call_manager;

    pthread_mutex_lock(&term->mutex);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);

    if (!call || !call->started)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    pthread_mutex_lock(&call->mutex);
    m = allocb(NCS_PCM_LEN, 0);
    if (m)
    {
        memcpy(m->b_wptr, wave_data, NCS_PCM_LEN);
        m->b_wptr += NCS_PCM_LEN;

        ms_bufferizer_put(&call->bufferizer, m);
    }
    pthread_mutex_unlock(&call->mutex);

    pthread_mutex_unlock(&term->mutex);
}

// 收到对讲开始指令
static void ncs_call_start_get(struct sockaddr *target_sockaddr,
                               uint16_t target_id,
                               uint8_t board_id,
                               int samplerate,
                               ncs_audio_encode_t encode,
                               void *private_data)
{
    ncs_event_t *evt;
    call_t *call;
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    struct sockaddr_in *sin = (struct sockaddr_in *)target_sockaddr;



    pthread_mutex_lock(&term->mutex);
    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);
    SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));


    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (!call || call->started)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    if (target_id != call->target_id || board_id != call->board_id)
    {
        call->target_id = target_id;
        call->board_id = board_id;
    }

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}

    virtual_key_send(kKeyCallStart);

    ncs_manager->ncs_call_start(ncs_manager, call, TRUE);

    pthread_mutex_unlock(&term->mutex);
}

// 收到对讲回复指令
static void ncs_call_answer_get(struct sockaddr *target_sockaddr,
                                uint16_t target_id,
                                uint8_t board_id,
                                int samplerate,
                                ncs_audio_encode_t encode,
                                void *private_data)
{
    call_t *call;
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    struct sockaddr_in *sin = (struct sockaddr_in *)target_sockaddr;

    SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, target_id, board_id);
    SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));


    pthread_mutex_lock(&term->mutex);

    call = call_manager->call_find_by_sockaddr(call_manager, target_sockaddr);
    if (!call || call->started)
    {
        pthread_mutex_unlock(&term->mutex);
        return;
    }

    if (target_id != call->target_id || board_id != call->board_id)
    {
        call->target_id = target_id;
        call->board_id = board_id;
        //virtual_key_send(kKeyCallInfoUpdate);
    }

    ncs_video_request(ncs_manager, call);

    pthread_mutex_unlock(&term->mutex);

    return;
}

// ip请求失败
static void ncs_ip_request_failed_get(void *private_data)
{
    call_t *call;
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    SPON_LOG_INFO("=====>%s\n", __func__);

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}

    pthread_mutex_lock(&term->mutex);

    call = call_manager->current_call_get(call_manager);

    if (call)
    {
        ncs_call_stop(ncs_manager, call, FALSE);
    }

    SPON_LOG_INFO("=====>%s calls total:%d\n", __func__, call_manager->calls_total_get(call_manager));


    ncs_target_offline_ring();

    virtual_key_send(kKeyIpRequestFailed);

    pthread_mutex_unlock(&term->mutex);
}

// ip请求成功
static void ncs_ip_request_ok_get(char *ip, uint16_t port, void *private_data)
{
    struct sockaddr_in target_sockaddr;
    call_t *call = NULL;
    struct call_manager *call_manager = term->call_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
    ncs_call_t type;
    call = call_manager->current_call_get(call_manager);
    SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, ip, port);
    if (port == 0 || strcmp(ip, "0.0.0.0") == 0)
        return;

    if (strcmp(ip, ncs_manager->local_ip) == 0)
        return;

    if (bc->bc_incoming(bc))
	{
		return;
	}

    if (bc->bc_outgoing(bc))
        return;

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
    call_manager->ip_get_ok = TRUE;
    if(!call_manager->target_sockaddr_set_by_nat)
    {
        memset(&target_sockaddr, 0, sizeof(target_sockaddr));
        target_sockaddr.sin_family = AF_INET;
        if (inet_pton(AF_INET, ip, &(target_sockaddr.sin_addr)) <= 0)
        {
            SPON_LOG_ERR("=====>%s - inet_pton failed:%s\n", __func__, strerror(errno));
            return;
        }
        target_sockaddr.sin_port = htons(port);
    }
    else
    {
        memset(&target_sockaddr, 0, sizeof(struct sockaddr_in));
        memcpy(&target_sockaddr, (struct sockaddr_in*)&call_manager->target_nat_sockaddr, sizeof(struct sockaddr_in));
    }
    pthread_mutex_lock(&term->mutex);

    if (call)
    {
        if (call->target_sockaddr_set == TRUE)
        {
            // 无响应转移
            if(!call_manager->target_sockaddr_set_by_nat)
            {
                type = call->type;
                ncs_manager->ncs_call_stop(ncs_manager, call, FALSE);
                call = call_manager->call_new(call_manager, type, OUT, 0, 0, (struct sockaddr *)&target_sockaddr);
                if (!call)
                {
                    SPON_LOG_ERR("=====>%s - call_new failed:%s\n", __func__, strerror(errno));
                    pthread_mutex_unlock(&term->mutex);
                    return;
                }
            }
        }
        else
        {
            call_manager->call_target_sockaddr_set(call_manager, call, (struct sockaddr *)&target_sockaddr);
        }
    }
    else
    {
        // 手动转移
        type = TALK;
        call = call_manager->call_new(call_manager, type, OUT, 0, 0, (struct sockaddr *)&target_sockaddr);
        if (!call)
        {
            SPON_LOG_ERR("=====>%s - call_new failed:%s\n", __func__, strerror(errno));
            pthread_mutex_unlock(&term->mutex);
            return;
        }

        set_system_task(TASK_NCS_CALL_OUTGOING);
        virtual_key_send(kKeyCallOut);
    }

    if (call_manager->calls_total_get(call_manager) == 1 && call->type != MONITOR && call->type != SDK_MONITOR)
    {
        ncs_call_outgoing_ring();
    }

    if(term->customer_type == CUSTOMER_YYJY && call_manager_get_flag_emergency(call_manager) == TRUE)
    {
        ncs_manager->ncs_short_out(ncs_manager, call);
    }

    ncs_call_request(call);
    if(call_manager->target_sockaddr_set_by_nat)
    {
        call_manager->target_sockaddr_set_by_nat = FALSE;
    }

    call_request_timer_create(call);

    pthread_mutex_unlock(&term->mutex);
}


static void ncs_type_ecpt_get(char *pdata, void *private_data)
{
    char Tmp = ~pdata[1];
    struct ncs_manager *ncs_manager = term->ncs_manager;

    if(pdata[0] == Tmp)
    {
        ncs_manager->ops.ncs_ecpt_type_set(pdata[0]);
        return;
    }
    ncs_manager->ops.ncs_ecpt_type_set(term->device_type);
}

// 退出服务器登陆
static void ncs_login_exit_get(void *private_data)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    pthread_mutex_lock(&term->mutex);

    ncs_manager->online = FALSE;
    SendNotifyMessage(system_info.main_hwnd, MSG_NCS_STATUE_BAR_PAGE, 0, 1);
    SendNotifyMessage(system_info.main_hwnd, MSG_NCS_STATUE_BAR_PAGE, 3, 0);
    ncs_manager->bc->set_samplerate(ncs_manager->bc, 22050); //离线要置成２２ｋ，防止离线广播不能采到数据

    SPON_LOG_INFO("=====>%s\n", __func__);

    //virtual_key_send(kKeyLoginExit);
    SPON_LOG_INFO("=====>%s\n", __func__);

    pthread_mutex_unlock(&term->mutex);
}

static void ncs_clean_area_name_list(void)
{
    struct list_head *head = &area_name_list_head;
    struct list_head *p;
    name_t *t;

    while (!list_empty(head))
    {
        p = head->next;
        t = list_entry(p, name_t, list);
        list_del(p);
        free(t);
    }
}

static void ncs_clean_term_name_list(void)
{
    struct list_head *head = &term_name_list_head;
    struct list_head *p;
    name_t *t;

    while (!list_empty(head))
    {
        p = head->next;
        t = list_entry(p, name_t, list);
        list_del(p);
        free(t);
    }
}

static void ncs_clean_boardext_name_list(void)
{
    struct list_head *head = &boardext_name_list_head;
    struct list_head *p;
    name_ex_t *t;

    while (!list_empty(head))
    {
        p = head->next;
        t = list_entry(p, name_ex_t, list);
        list_del(p);
        free(t);
    }
}


// ��½����ͨ������
static void ncs_login_ok_get(
    int bc_samplerate,   // �㲥������
    int talk_samplerate, // �Խ�������
    bool_t have_task,    // �Ƿ��е�¼����
    ncs_task_t task,     // ��¼��������
    char *multi_ip,      // ���չ㲥���鲥��ַ
    void *private_data)
{
    ncs_event_t *evt;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = term->ncs_manager->bc;
    uint32_t *p = NULL;
    char count = 0;

    pthread_mutex_lock(&term->mutex);

    SPON_LOG_INFO("=====>%s\n", __func__);

    //virtual_key_send(kKeyLoginOk);

    ncs_manager->online = TRUE;
    SendNotifyMessage(system_info.main_hwnd, MSG_NCS_STATUE_BAR_PAGE, 0, 1);

    netlink_led_open();

    ncs_manager->bc->set_samplerate(ncs_manager->bc, bc_samplerate);

    ncs_clean_term_name_list();
    ncs_clean_area_name_list();
    ncs_manager->name = NULL; //目的是当连接８５０１时，显示的终端名称会乱码（８５０１没有回复终端名称查询）
    term_bulk_query_id = 1;
    evt = ncs_term_name_bulk_query_event_build(term_bulk_query_id, BULK_ID_QUERY_COUNT_MAX);
    if (evt)
        ncs_event_send(evt);

    area_bulk_query_id = 1;
    evt = ncs_area_name_bulk_query_event_build(area_bulk_query_id, BULK_ID_QUERY_COUNT_MAX);
    if (evt)
        ncs_event_send(evt);

    //查ｗｅｂ页
    if(term->customer_type == CUSTOMER_QGLW)
    {
        /*        ncs_clean_boardext_name_list();*/
        /*p = GetBoardext_info(&count);*/
        /*evt = ncs_boardext_name_bulk_query_event_build(p, count);*/
        /*if (evt)*/
        /*ncs_event_send(evt);*/
        /*free(p);*/
        /*ncs_ecpt_type_set(term->device_type);*/
    }

    if (have_task)
    {
        if (task == WAVE_BROADCAST_TASK)
        {
            set_system_task(TASK_NCS_BROADCAST_INCOMING);
            virtual_key_send(kKeyTaskSwitch);
            bc->ncs_wav_broadcast_task_start(bc, multi_ip);
        }
    }

    pthread_mutex_unlock(&term->mutex);
}


static void ncs_nat_transfer_get(char *ip, uint16_t port, void *private_data)
{
    call_t *call = NULL;
    struct sockaddr_in target_sockaddr;
    struct call_manager *call_manager = term->call_manager;

    pthread_mutex_lock(&term->mutex);


    memset(&target_sockaddr, 0, sizeof(target_sockaddr));
    target_sockaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip, &(target_sockaddr.sin_addr)) <= 0)
    {
        call_t *call = call_manager->current_call_get(call_manager);
        SPON_LOG_ERR("=====>%s - inet_pton failed:%s\n", __func__, strerror(errno));
        pthread_mutex_unlock(&term->mutex);
        return;
    }
    target_sockaddr.sin_port = htons(port);

    if(call_manager->ip_get_ok)
    {
        call_manager->ip_get_ok = FALSE;
    }
    else
    {
        call_manager->target_sockaddr_set_by_nat = TRUE;
    }
    call = call_manager->current_call_get(call_manager);
    if(call)
    {
        call_manager->call_target_sockaddr_set(call_manager, call, (struct sockaddr *)&target_sockaddr);
    }
    else
    {
        memset(&call_manager->target_nat_sockaddr, 0, sizeof(struct sockaddr));
        memcpy(&call_manager->target_nat_sockaddr, (struct sockaddr*)&target_sockaddr, sizeof(struct sockaddr));
    }
    pthread_mutex_unlock(&term->mutex);
}

static int ncs_start(struct ncs_manager *ncs_manager)
{
    int ret;

    // 保存服务软件网络地址
    sockaddr_convert((struct sockaddr_in *)&ncs_manager->server_sockaddr, ncs_manager->server1_ip, ncs_manager->server1_port);

    // 设置内通回调函数
    ncs_manager->ops.ncs_login_ok_get                 = ncs_login_ok_get;
    ncs_manager->ops.ncs_login_exit_get               = ncs_login_exit_get;
    ncs_manager->ops.ncs_ip_request_ok_get            = ncs_ip_request_ok_get;
    ncs_manager->ops.ncs_type_ecpt_get                = ncs_type_ecpt_get;
    ncs_manager->ops.ncs_ip_request_failed_get        = ncs_ip_request_failed_get;
    ncs_manager->ops.ncs_call_answer_get              = ncs_call_answer_get;
    ncs_manager->ops.ncs_call_start_get               = ncs_call_start_get;
    ncs_manager->ops.ncs_talk_wave_data_get           = ncs_talk_wave_data_get;
    ncs_manager->ops.ncs_term_name_bulk_get           = ncs_term_name_bulk_get;
    ncs_manager->ops.ncs_term_name_qglw_get           = ncs_term_name_qglw_get;
    ncs_manager->ops.ncs_term_boardext_name_qglw_get  = ncs_term_boardext_name_qglw_get;
    ncs_manager->ops.ncs_video_request_get            = ncs_video_request_get;
    ncs_manager->ops.ncs_video_state_get              = ncs_video_state_get;
    ncs_manager->ops.ncs_broadcast_start_get          = ncs_broadcast_start_get;
    ncs_manager->ops.ncs_broadcast_stop_get           = ncs_broadcast_stop_get;
    ncs_manager->ops.ncs_system_time_get              = ncs_system_time_get;
    ncs_manager->ops.ncs_call_request_get             = ncs_call_request_get;
    ncs_manager->ops.ncs_area_name_bulk_get           = ncs_area_name_bulk_get;
    ncs_manager->ops.ncs_task_switch_get              = ncs_task_switch_get;
    ncs_manager->ops.ncs_task_idle_get                = ncs_task_idle_get;
    ncs_manager->ops.ncs_realtime_wave_data_get       = ncs_realtime_wave_data_get;
    ncs_manager->ops.ncs_monitor_request_get          = ncs_monitor_request_get;
    ncs_manager->ops.ncs_call_stop_get                = ncs_call_stop_get;
    ncs_manager->ops.ncs_call_busy_get                = ncs_call_busy_get;
    ncs_manager->ops.ncs_call_reject_get              = ncs_call_reject_get;
    ncs_manager->ops.ncs_sdk_call_stop_get            = ncs_sdk_call_stop_get;
    ncs_manager->ops.ncs_sdk_call_request_get         = ncs_sdk_call_request_get;
    ncs_manager->ops.ncs_sdk_monitor_request_get      = ncs_sdk_monitor_request_get;
    ncs_manager->ops.ncs_sdk_call_start_get           = ncs_sdk_call_start_get;
    ncs_manager->ops.ncs_terminal_password_get        = ncs_terminal_password_get;
    ncs_manager->ops.ncs_terminal_info_get            = ncs_terminal_info_get;
    ncs_manager->ops.ncs_third_party_record_start_get = ncs_third_party_record_start_get;
    ncs_manager->ops.ncs_third_party_record_stop_get  = ncs_third_party_record_stop_get;
    ncs_manager->ops.ncs_volume_get                   = ncs_volume_get;
    ncs_manager->ops.ncs_nat_transfer_get             = ncs_nat_transfer_get;
    ncs_manager->ops.ncs_port_out_ctl_get             = ncs_port_out_ctl_get;
    ncs_manager->ops.ncs_auto_answer_ctl_get          = ncs_auto_answer_ctl_get;
    ncs_manager->ops.ncs_ecpt_type_get                = ncs_ecpt_type_get;
    ncs_manager->ops.ncs_ecpt_type_set                = ncs_ecpt_type_set;
    ncs_manager->ops.ncs_get_term_custom_type         = ncs_get_term_custom_type;
    ncs_manager->ops.ncs_set_device_server_type_rcd   = ncs_set_device_server_type_rcd;
    ncs_manager->ops.ncs_get_device_server_type_rcd   = ncs_get_device_server_type_rcd;
    ncs_manager->ops.ncs_kfwqName_set                 = ncs_kfwqName_set;
    ncs_manager->ops.ncs_kfwqName_get                 = ncs_kfwqName_get;
    ncs_manager->ops.ncs_warring_get                 = ncs_warring_get;



    ret = ncs_init(ncs_manager->server1_ip, ncs_manager->server1_port,
                   ncs_manager->server2_ip, ncs_manager->server2_port,
                   ncs_manager->id, ncs_manager->local_port,
                   &ncs_manager->ops, NULL);
    if (ret < 0)
    {
        SPON_LOG_ERR("=====>%s - ncs_init failed:%d\n", __func__, ncs_get_error_code());
        return ret;
    }

    ncs_login_period_set(3);
    ncs_handshake_period_set(30);
    ncs_handshake_timeout_set(150);

    ncs_talk_audio_encode_set(ncs_manager->talk_audio_encode);
    ncs_bc_audio_encode_set(ncs_manager->bc_audio_encode);

    //ncs_login(0);

    return 0;
}

static int ncs_network_cfg(struct ncs_manager *ncs_manager)
{
    char command[128];
    //char mac_addr[18];
    uint16_t id = ncs_manager->id;
    char result[30] = {0};
    char *p[4] = {0};

    sprintf(command, "echo nameserver %s > /etc/resolv.conf", ncs_manager->dns1);
    system(command);
    sprintf(command, "echo nameserver %s >> /etc/resolv.conf", ncs_manager->dns2);
    system(command);

    strcpy(result, ncs_manager->server1_ip);
    //p[0] = strtok(result, ".");
    //p[1] = strtok(NULL, ".");
    //p[2] = strtok(NULL, ".");
    //p[3] = strtok(NULL, ".");
    //sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", 0x00, 0xE0, 0x4C, atoi(p[3]), (id >> 8) & 0xff, id & 0xff);

    sprintf(command, "ifconfig eth0 hw ether %s", ncs_manager->mac_addr);
    system(command);

    sprintf(command, "ifconfig eth0 %s netmask %s up", ncs_manager->local_ip, ncs_manager->netmask);
    system(command);

    sprintf(command, "route add default gw %s", ncs_manager->gw);
    system(command);

    return 0;
}


static int ncs_manager_init(struct ncs_manager *ncs_manager)
{
    LpConfig *lpconfig;
    char *ip;
    struct hostent *host;
    char *hostname;
    struct broadcast *bc = ncs_manager->bc;
    char mac_address[32] = {0};

    lpconfig = lp_config_new(INI_CONFIG_FILE);

    // 获取主服务器IP和端口
    hostname = (char *)lp_config_get_string(lpconfig, "server_cfg", "ip_server1", "192.168.1.13");
    if ( (host = gethostbyname(hostname)) == NULL)
        ip = hostname;
    else
        ip = inet_ntoa(*((struct in_addr *)host->h_addr));
    ncs_manager->server1_ip = strdup(ip);
    ncs_manager->server1_port = lp_config_get_int(lpconfig, "server_cfg", "port_server1", 2048);

    // 获取备用服务器IP和端口
    hostname = (char *)lp_config_get_string(lpconfig, "server_cfg", "ip_server2", "192.168.1.14");
    if (hostname)
    {
        if ( (host = gethostbyname(hostname)) == NULL)
            ip = hostname;
        else
            ip = inet_ntoa(*((struct in_addr *)host->h_addr));
        ncs_manager->server2_ip = strdup(ip);
        ncs_manager->server2_port = lp_config_get_int(lpconfig, "server_cfg", "port_server2", 2048);
    }

    ncs_manager->id = lp_config_get_int(lpconfig, "terminal", "id", 3);
    // 本地网络设置
    ip = (char *)lp_config_get_string(lpconfig, "terminal", "ip", "192.168.1.101");
    ncs_manager->local_ip = strdup(ip);
    ncs_manager->local_port = lp_config_get_int(lpconfig, "terminal", "port", 2046);
    ip = (char *)lp_config_get_string(lpconfig, "terminal", "mask", "255.255.255.0");
    ncs_manager->netmask = strdup(ip);

    ip = (char *)lp_config_get_string(lpconfig, "terminal", "mac_addr", "00:E0:4C:EA:00:AA");
    ncs_manager->mac_addr = strdup(ip);

    ip = (char *)lp_config_get_string(lpconfig, "terminal", "gate", "192.168.1.1");
    ncs_manager->gw = strdup(ip);
    ip = (char *)lp_config_get_string(lpconfig, "terminal", "dns1", "192.168.1.1");
    ncs_manager->dns1 = strdup(ip);
    ip = (char *)lp_config_get_string(lpconfig, "terminal", "dns2", "192.168.1.1");
    ncs_manager->dns2 = strdup(ip);

    // 视频端口配置
    ncs_manager->video_port = lp_config_get_int(lpconfig, "terminal", "port_video", 2058);
    ip = lp_config_get_string(lpconfig, "terminal", "ip_route", NULL);
    if (ip == NULL)
        ncs_manager->video_ip_route = ncs_manager->local_ip;
    else
        ncs_manager->video_ip_route = strdup(ip);
    ncs_manager->video_port_route = lp_config_get_int(lpconfig, "terminal", "port_route", 0);
    if (ncs_manager->video_port_route == 0)
        ncs_manager->video_port_route = ncs_manager->video_port;

    // 音频数据编码方式
    ncs_manager->talk_audio_encode = lp_config_get_int(lpconfig, "talk_cfg", "is_pcm", 1);
    ncs_manager->bc_audio_encode = lp_config_get_int(lpconfig, "broadcast_cfg", "is_pcm", 1);

    // 对讲广播优先级
    ncs_manager->priority = lp_config_get_int(lpconfig, "priority_cfg", "call_priority", TRUE);

    // ptt触发方式
    ncs_manager->ptt_trigger_type = lp_config_get_int(lpconfig, "talk_cfg", "ptt_ctrl_method", PRESS_NONE);

    // 会议模式使能
    ncs_manager->conference_enable = lp_config_get_int(lpconfig, "talk_cfg", "conference_talk", FALSE);
    // 视频请求格式
    ncs_manager->video_request_fmt = lp_config_get_int(lpconfig, "terminal", "video_request_fmt", VIDEO_VGA_FMT);

    // 排队呼叫参数
    ncs_manager->queueing_enable = lp_config_get_int(lpconfig, "queue_cfg", "queue_enable", TRUE);
    ncs_manager->queueing_number = lp_config_get_int(lpconfig, "queue_cfg", "queue_number", 7);
    //定时重启
    ncs_manager->dtreboot_enable = lp_config_get_int(lpconfig, "dtreboot_cfg", "dtreboot_enable", TRUE);
    ncs_manager->dtreboot_days_number = lp_config_get_int(lpconfig, "dtreboot_cfg", "days_number", 0);
    ncs_manager->dtreboot_time = lp_config_get_int(lpconfig, "dtreboot_cfg", "time", 0);
    ncs_manager->curdtreboot_Hour = 0;
    ncs_manager->curdtreboot_days = 0;

    ncs_manager->short_out_value = lp_config_get_int(lpconfig, "short_cfg", "shortout_type", 0);

    bc->offline_ring = lp_config_get_int(lpconfig, "broadcast_cfg", "alert_audio", 1);
    bc->offline_ring = bc->offline_ring ? 0 : 1;
    ncs_manager->trigger_time = lp_config_get_int(lpconfig, "short_cfg", "trigger_time", 5);

    ncs_manager->auto_answer = 0;//lp_config_get_int(lpconfig, "terminal", "auto_answer", 0);


    lp_config_destroy(lpconfig);

    if (ncs_manager->queueing_enable)
    {
        ncs_manager->conference_enable = FALSE;
        /*ncs_manager->max_calls = ncs_manager->queueing_number + 1;*/
        ncs_manager->max_calls = ncs_manager->queueing_number;
    }
    else
    {
        if (ncs_manager->conference_enable)
            ncs_manager->max_calls = 2;
        else
            ncs_manager->max_calls = 1;
    }

    return 0;

}

struct ncs_manager *ncs_manager_malloc(void)
{
    struct ncs_manager *tmp = (struct ncs_manager *)malloc(sizeof(struct ncs_manager));

    if (!tmp)
        return NULL;

    memset(tmp, 0, sizeof(struct ncs_manager));

    tmp->bc = broadcast_malloc();
    ncs_manager_init(tmp);

    tmp->ncs_network_cfg              = ncs_network_cfg;
    tmp->ncs_start                    = ncs_start;
    tmp->ncs_call_start               = ncs_call_start;
    tmp->ncs_call_stop                = ncs_call_stop;
    tmp->ncs_ip_request               = ncs_ip_request;
    tmp->ncs_video_recv_start         = ncs_video_recv_start;
    tmp->ncs_term_name_get_by_id      = ncs_term_name_get_by_id;
    tmp->ncs_term_name_modify_by_id   = ncs_term_name_modify_by_id;
    tmp->ncs_area_name_get_by_id      = ncs_area_name_get_by_id;
    tmp->ncs_area_name_get_by_index   = ncs_area_name_get_by_index;	
    tmp->ncs_area_id_get_by_index     = ncs_area_id_get_by_index;
    tmp->ncs_boardext_name_get_by_id  = ncs_boardext_name_get_by_id;
    tmp->ncs_term_name_list_size_get  = ncs_term_name_list_size_get;
    tmp->ncs_area_name_list_size_get  = ncs_area_name_list_size_get;
    tmp->ncs_door_open                = ncs_door_open;
    tmp->ncs_term_status_ctl          = ncs_term_status_ctl;
    tmp->ncs_ptt_status_send          = ncs_ptt_status_send;
    tmp->ncs_call_transfer            = ncs_call_transfer;
    tmp->ncs_video_switch             = ncs_video_switch;
    tmp->ncs_video_request            = ncs_video_request;
    tmp->ncs_call_stop_all            = ncs_call_stop_all;
    tmp->ncs_call_stop_all_ex_talking = ncs_call_stop_all_ex_talking;
    tmp->ncs_monitor_stop             = ncs_monitor_stop;
    tmp->ncs_short_out                = ncs_short_out_send;
    tmp->ncs_ctrl_haoyun_term_short   = ncs_ctrl_haoyun_term_short;



    return tmp;
}

void ncs_manager_free(struct ncs_manager *ncs_manager)
{
    free(ncs_manager);
}


// ===================================================================================================================================================

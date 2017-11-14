#include "broadcast.h"
#include "term.h"
#include "system_info.h"
#include "user_msg.h"

static void set_samplerate(struct broadcast *bc, int samplerate)
{
    bc->samplerate = samplerate;
}

static int get_samplerate(struct broadcast *bc)
{
    return bc->samplerate;
}

static bool_t bc_outgoing(struct broadcast *bc)
{
    //SPON_LOG_INFO("=====>%s %d\n", __func__,bc->outgoing_bc_start);
    return bc->outgoing_bc_start;
}

static bool_t bc_incoming(struct broadcast *bc)
{
    return bc->incoming_bc_start;
}

static void bc_outgoing_start_flag_set(struct broadcast *bc, bool_t flag)
{
    //SPON_LOG_INFO("=====>%s flag %d\n", __func__,flag);
    bc->outgoing_bc_start = flag;
}

static void bc_incoming_start_flag_set(struct broadcast *bc, bool_t flag)
{
    bc->incoming_bc_start = flag;
}

static void ncs_broadcast_set_ring(struct audio_manager *audio_manager)
{
	//audio_channel_mode_set(AUDIO_CHANNEL_BROADCAST_RING);
}

static void ncs_broadcast_set_wav_task(struct audio_manager *audio_manager)
{
	//audio_channel_mode_set(AUDIO_CHANNEL_BROADCAST_IN);
}

// 开始接收wav广播
static void ncs_wav_broadcast_task_start(struct broadcast *bc, char *multi_ip)
{
	struct audio_manager *audio_manager = term->audio_manager;

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
	if (strcmp(bc->prev_multi_ip, multi_ip) == 0)
		return;

	audio_manager->ring_stop(audio_manager);
	audio_manager->bc_ring_stop(audio_manager);

	if (strlen(bc->prev_multi_ip))
		ncs_multicast_drop(bc->prev_multi_ip);

	memset(bc->prev_multi_ip, 0, sizeof(bc->prev_multi_ip));
	strcpy(bc->prev_multi_ip, multi_ip);
	strcpy(g_SysStatue.mutil_ip, multi_ip);

	ncs_multicast_join(multi_ip);

	audio_manager->set_bc_mode(audio_manager);
	audio_manager->set_bc_out_volume(audio_manager, -1);
	audio_manager->stream_create(audio_manager, AUDIO_WRITE, bc->get_samplerate(bc), &bc->mutex, &bc->bufferizer, NULL);

	bc->bc_incoming_start_flag_set(bc, TRUE);
}

// 结束接收wav广播
static void ncs_wav_broadcast_task_stop(struct broadcast *bc)
{
	struct audio_manager *audio_manager = term->audio_manager;

	// 退出组播组
	ncs_multicast_drop(bc->prev_multi_ip);
	memset(bc->prev_multi_ip, 0, sizeof(bc->prev_multi_ip));
	// 清标记
	bc->bc_incoming_start_flag_set(bc, FALSE);
	// 销毁音频流
	audio_manager->stream_destroy(audio_manager, AUDIO_WRITE);

	// 刷缓存
	ms_bufferizer_flush(&bc->bufferizer);
}

// 停止实时广播
static void ncs_realtime_broadcast_stop(struct broadcast *bc)
{
	struct audio_manager *audio_manager = term->audio_manager;
	/*audio_manager->mic_set(audio_manager, FALSE);*/
	Turn_obj_vol_dev_src();
	audio_manager->stream_destroy(audio_manager, AUDIO_READ);
}

// 开始实时广播
static void ncs_realtime_broadcast_start(struct broadcast *bc)
{
	struct audio_manager *audio_manager = term->audio_manager;

	Turn_obj_vol_dev_src();
	/*if(headset_mic_status_get() == 1)*/
	/*{*/
		/*audio_manager->mic_set(audio_manager, FALSE);*/
	/*}*/
	/*else*/
	/*{*/
		/*audio_manager->mic_set(audio_manager, TRUE);*/
	/*}*/
	audio_manager->stream_create(audio_manager, AUDIO_READ, bc->samplerate, NULL, NULL, NULL);
	// 设置广播输入音量
	audio_manager->set_bc_in_volume(audio_manager, -1);
}

// 广播提示音播放完了的回调函数
static void ncs_bc_ring_play_complete(void *userdata, struct _MSFilter *f, unsigned int id, void *arg)
{
    struct audio_manager *audio_manager = term->audio_manager;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct broadcast *bc = ncs_manager->bc;
	bool_t flag = 0;

    bc->ncs_file_broadcast_stop(bc);

    if(bc->bc_outgoing(bc))
    {
		audio_manager->set_bc_mode(audio_manager);
		ncs_realtime_broadcast_start(bc);
    }
    else
    {
		task_t task = get_system_task();

		if (task == TASK_NCS_TERM_BROADCAST_OUTGOING)
		{
			bc->ncs_term_broadcast_stop(bc);
		}
		else if (task == TASK_NCS_AREA_BROADCAST_OUTGOING)
		{
			bc->ncs_area_broadcast_stop(bc);
		}
		else
		{
			if (ncs_manager->online)
			{
				bc->ncs_global_broadcast_stop(bc);
			}
			else
			{
				bc->ncs_offline_task_switch_send(bc, IDLE_TASK_OLD);
				bc->ncs_offline_task_switch_send(bc, IDLE_TASK);
			}
		}
        virtual_key_send(kKeyBcStop);
    }
}

// 脱机广播
static void ncs_offline_task_switch_send(struct broadcast *bc, ncs_task_t t)
{
    ncs_event_t *evt;
    evt = ncs_offline_task_switch_event_build(t);
    if (evt)
        ncs_event_send(evt);
}


// 开始发送广播
static int ncs_broadcast_outgoing_start(struct broadcast *bc, char *file)
{
    struct audio_manager *audio_manager = term->audio_manager;
    bool_t online = term->ncs_manager->online;

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
    if (bc_outgoing(bc))
	{
        return 0;
	}

	audio_manager->set_bc_mode(audio_manager);

    bc->bc_outgoing_start_flag_set(bc, TRUE);

    // 脱机广播
    if (online == FALSE)
        ncs_offline_task_switch_send(bc, WAVE_BROADCAST_TASK);


	if ( (online && bc->ring) || (!online && bc->offline_ring))
	{
		// 播放广播开始提示音
		//ncs_broadcast_set_ring(audio_manager);
		audio_manager->bc_ring_start(audio_manager, file, ncs_bc_ring_play_complete, NULL);
	}		
	else
	{
		// 实时广播
		ncs_realtime_broadcast_start(bc);
	}
    // 设置广播输入音量
    audio_manager->set_bc_in_volume(audio_manager, -1);

    return 0;
}

// 停止发送广播
static int ncs_broadcast_outgoing_stop(struct broadcast *bc, char *file)
{
    struct audio_manager *audio_manager = term->audio_manager;
    bool_t online = term->ncs_manager->online;

    if (!bc_outgoing(bc))
	{
    	return 0;
	}

    // 清除广播开始标记
    bc->bc_outgoing_start_flag_set(bc, FALSE);

    // 停止实时广播
    ncs_realtime_broadcast_stop(bc);

	/*audio_manager->mic_set(audio_manager, FALSE);*/
	Turn_obj_vol_dev_src();

	if ( (online && bc->ring) || (!online && bc->offline_ring)) {
		// 预防广播开始提示音还没播完就停止广播
		audio_manager->bc_ring_stop(audio_manager);
		// 播放广播结束提示音
		//ncs_broadcast_set_ring(audio_manager);
		audio_manager->bc_ring_start(audio_manager, file, ncs_bc_ring_play_complete, audio_manager);
	} 
	else
	{
        ncs_bc_ring_play_complete(NULL, NULL, 0, NULL);
	}
    return 0;
}

// 终端广播结束ֹ
static void ncs_term_broadcast_stop(struct broadcast *bc)
{
    ncs_event_t *evt;

    evt = ncs_term_broadcast_stop_event_build();
    if (evt)
        ncs_event_send(evt);
}

// 全区广播结束ֹ
static void ncs_global_broadcast_stop(struct broadcast *bc)
{
    ncs_event_t *evt;

    evt = ncs_all_area_broadcast_stop_event_build();
    if (evt)
        ncs_event_send(evt);
}

// 分区广播结束ֹ
static void ncs_area_broadcast_stop(struct broadcast *bc)
{
    ncs_event_t *evt;

    evt = ncs_area_broadcast_stop_event_build();
    if (evt)
        ncs_event_send(evt);
}

static bool_t ncs_file_broadcasting(struct broadcast *bc)
{
    struct audio_manager *audio_manager = term->audio_manager;
    if (audio_manager->bc_ring)
        return TRUE;
    else
        return FALSE;
}

// 开始文件广播
static void ncs_file_broadcast_start(struct broadcast *bc, char *file)
{
    struct audio_manager *audio_manager = term->audio_manager;

	if(term->customer_type == CUSTOMER_XMBR)
	{
		//厦门BRT项目,定制对讲输出固定为本地输出, 广播还是按照原来的输出
		AudioOutDevSetReg();
	}
	audio_manager->bc_ring_start(audio_manager, file, ncs_bc_ring_play_complete, audio_manager);
}

// 结束文件广播
static void ncs_file_broadcast_stop(struct broadcast *bc)
{
    struct audio_manager *audio_manager = term->audio_manager;
    audio_manager->bc_ring_stop(audio_manager);
}

// 终端广播发送请求
static void ncs_term_broadcast_request(struct broadcast *bc, int *term_list, int term_list_len)
{
    ncs_event_t *evt;

    evt = ncs_term_broadcast_request_event_build(term_list, term_list_len);
    if (evt)
        ncs_event_send(evt);
}

// 全区广播发送请求
static void ncs_global_broadcast_request(struct broadcast *bc)
{
    ncs_event_t *evt;

    evt = ncs_all_area_broadcast_request_event_build();
    if (evt)
        ncs_event_send(evt);
}

// 分区广播发送请求
static void ncs_area_broadcast_request(struct broadcast *bc, int *area_list, int area_list_len)
{
    ncs_event_t *evt;

    evt = ncs_area_broadcast_request_event_build(area_list, area_list_len);
    if (evt)
        ncs_event_send(evt);
}


struct broadcast *broadcast_malloc(void)
{
    struct broadcast *tmp = (struct broadcast *)malloc(sizeof(struct broadcast));
    if (!tmp)
        return NULL;

    memset(tmp, 0, sizeof(*tmp));

    pthread_mutex_init(&tmp->mutex, NULL);
    ms_bufferizer_init(&tmp->bufferizer);

    tmp->set_samplerate = set_samplerate;
    tmp->get_samplerate = get_samplerate;
    tmp->bc_outgoing = bc_outgoing;
    tmp->bc_incoming = bc_incoming;
    tmp->bc_outgoing_start_flag_set = bc_outgoing_start_flag_set;
    tmp->bc_incoming_start_flag_set = bc_incoming_start_flag_set;
    tmp->ncs_wav_broadcast_task_start = ncs_wav_broadcast_task_start;
    tmp->ncs_wav_broadcast_task_stop = ncs_wav_broadcast_task_stop;
    tmp->ncs_broadcast_outgoing_start = ncs_broadcast_outgoing_start;
    tmp->ncs_broadcast_outgoing_stop = ncs_broadcast_outgoing_stop;
    tmp->ncs_term_broadcast_stop = ncs_term_broadcast_stop;
    tmp->ncs_global_broadcast_stop = ncs_global_broadcast_stop;
    tmp->ncs_area_broadcast_stop = ncs_area_broadcast_stop;
    tmp->ncs_file_broadcast_start = ncs_file_broadcast_start;
    tmp->ncs_file_broadcast_stop = ncs_file_broadcast_stop;
    tmp->ncs_realtime_broadcast_start = ncs_realtime_broadcast_start;
    tmp->ncs_realtime_broadcast_stop = ncs_realtime_broadcast_stop;
    tmp->ncs_term_broadcast_request = ncs_term_broadcast_request;
    tmp->ncs_global_broadcast_request = ncs_global_broadcast_request;
    tmp->ncs_area_broadcast_request = ncs_area_broadcast_request;
    tmp->ncs_offline_task_switch_send = ncs_offline_task_switch_send;
    tmp->ncs_file_broadcasting = ncs_file_broadcasting;

    return tmp;
}

void broadcast_free(struct broadcast *bc)
{
    free(bc);
}

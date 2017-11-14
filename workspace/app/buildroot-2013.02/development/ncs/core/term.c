#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include "guipub.h"

#include <common_lib/common_lib.h>

#include "term.h"
#include "lpconfig.h"
#include "system_info.h"
#include "offline.h"
#include "emegcywarring.h"

TYPE_SYS_STATUE g_SysStatue;

void SetSysStatueCloseBroadcastFlag(bool_t flag)
{
	g_SysStatue.fCloseBroadcast = flag;
}

bool_t GetSysStatueCloseBroadcastFlag(void)
{
	return g_SysStatue.fCloseBroadcast;
}

static void sigusr_opt(int arg)
{
	int a = 0;
	do
	{
		a = 0;
	}while(0);
}

void SetBitAudioOutDev(TYPE_SYS_STATUE *pAudioOutDev, int DevBit, bool_t flag)
{
	if(flag)
	{
		pAudioOutDev->AudioOutDev |= (1<<DevBit); 
	}
	else
	{
		pAudioOutDev->AudioOutDev &= (~(1<<DevBit)); 
	}
}

void AudioOutDevSetReg(void)
{
	struct audio_manager *audio_manager = term->audio_manager;

	if(IsCurTaskBroadcast())
	{
		printf("========#################cur task is broadcast!\n");
		if(M_GET_AUDIO_OUT_DEV(ENUM_DEV_OUT_LINEOUT) || lineout_get_status() == 1)
		{
			audio_manager->set_rt3261regEx(audio_manager, 01, 15, 1);
			audio_manager->set_rt3261regEx(audio_manager, 02, 15, 0);
			return ;
		}
		else
		{
			audio_manager->set_rt3261regEx(audio_manager, 02, 15, 1);
		}

		if(M_GET_AUDIO_OUT_DEV(ENUM_DEV_OUT_HEARPHONE))
		{
			audio_manager->set_rt3261regEx(audio_manager, 01, 15, 1);
			audio_manager->set_rt3261regEx(audio_manager, 03, 15, 0);
			return;
		}
		else
		{
			audio_manager->set_rt3261regEx(audio_manager, 03, 15, 1);
		}
		audio_manager->set_rt3261regEx(audio_manager, 01, 15, 0);
	}
	else
	{
		printf("========#################cur task is talk!\n");
		audio_manager->set_rt3261regEx(audio_manager, 01, 15, 0);
		audio_manager->set_rt3261regEx(audio_manager, 02, 15, 1);
		audio_manager->set_rt3261regEx(audio_manager, 03, 15, 1);
	}
}

bool_t GetBitAudioOutDev(TYPE_SYS_STATUE *pAudioOutDev, int DevBit)
{
	if(pAudioOutDev->AudioOutDev & (1 << DevBit))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void SetBitAudioInDev(TYPE_SYS_STATUE *pAudioOutDev, int DevBit, bool_t flag)
{
	if(flag)
	{
		pAudioOutDev->AudioInDev |= (1<<DevBit); 
	}
	else
	{
		pAudioOutDev->AudioInDev &= (~(1<<DevBit)); 
	}
}

bool_t GetBitAudioInDev(TYPE_SYS_STATUE *pAudioOutDev, int DevBit)
{
	if(pAudioOutDev->AudioInDev & (1<<DevBit))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool_t IsCurTaskBroadcast(void)
{
	task_t task = get_system_task();  

	if(task == TASK_NCS_BROADCAST_INCOMING || 
			task == TASK_NCS_TERM_BROADCAST_OUTGOING || 
			task == TASK_NCS_AREA_BROADCAST_OUTGOING || 
			task == TASK_NCS_ALL_AREA_BROADCAST_OUTGOING
			)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static int terminal_init(struct terminal *terminal)
{
	LpConfig *lpconfig;
	char *customer_type = getenv("NCS_CUSTOMER_TYPE");
	char *video_type = getenv("VIDEO_TYPE");
	char Buf[128];

	SPON_LOG_INIT("8532", SPON_LOGTYPE_BOTH, SPON_LOGLVL_INFO);


	ortp_init();
	/*ortp_set_log_level_mask(ORTP_MESSAGE);*/
	ms_init();
	pthread_mutex_init(&terminal->mutex, NULL);

	pthread_mutex_init(&terminal->gui_mutex, NULL);

	lpconfig = lp_config_new(INI_CONFIG_FILE);

	terminal->language = lp_config_get_int(lpconfig, "language", "language", 0);
	terminal->dial_rapid = lp_config_get_int(lpconfig, "talk_cfg", "quick_callType", 0);
	strcpy(term->version_info[5], (char *)lp_config_get_string(lpconfig, "sip", "sip_username", "999"));
	lp_config_destroy(lpconfig);

	lpconfig = lp_config_new(INI_VERSION_FILE);
	if (lpconfig == NULL) {
		SPON_LOG_ERR("lp_config_new failed:%s\n", strerror(errno));
		return -1;
	}
	strcpy(Buf, (char *)lp_config_get_string(lpconfig, "version_info", "verInfo", "nomarl")) ;
	utf8_to_gb2312(Buf, term->version_info[0], strlen(Buf));

	strcpy(term->version_info[1], (char *)lp_config_get_string(lpconfig, "version_info", "verMajor", "1"));
	strcpy(term->version_info[2], (char *)lp_config_get_string(lpconfig, "version_info", "verMinor", "0"));
	strcpy(term->version_info[3], (char *)lp_config_get_string(lpconfig, "version_info", "verInner", "0"));
	strcpy(term->version_info[4], (char *)lp_config_get_string(lpconfig, "version_info", "verBuild", "1"));
	lp_config_destroy(lpconfig);

	// 获取定制版型号
	if (customer_type)
		terminal->customer_type = strtol(customer_type, NULL, 16);

	if(term->customer_type == CUSTOMER_XMBR)
	{
		system("echo 1 > /proc/vol_ver_ctrl");
	}
	else
	{
		system("echo 0 > /proc/vol_ver_ctrl");
	}
	
	// 获取视频定制类型
	if ((video_type != NULL) && (strcmp(video_type, "ENABLE") == 0))
	{
		terminal->video_type = TRUE;
	}
	else
	{
		terminal->video_type = FALSE;
	}
	// terminal->log_manager = log_manager_malloc();
	terminal->ncs_manager = ncs_manager_malloc();
	terminal->call_manager = call_manager_malloc();
	terminal->audio_manager = audio_manager_malloc();
	terminal->sip_manager = sip_manager_malloc();
	g_pEmegcyWarring = EmegcyWarringMalloc();
	terminal->queue = QueueNew();
	SetSysStatueCloseBroadcastFlag(FALSE);

	terminal->ncs_manager->ncs_network_cfg(terminal->ncs_manager);

	terminal->audio_manager->init(terminal->audio_manager);

	terminal->ao = (struct audio_output *)alsa_audio_output_malloc();
	terminal->ai = (struct audio_output *)alsa_audio_output_malloc();

	init_offline_call_list();

	terminal->ncs_manager->ncs_start(terminal->ncs_manager);

	terminal->sip_manager->start();

#ifdef __NCS_SUPPORT_ONVIF__
	ncs_rtsp_init();
#endif

	obj_vol_dev_src_init();
	gpio_monitor_init(term);

	phonebook_info_init();

	user_log_init(term->language);

    terminal->watchdog_client = ncs_watchdog_client_create(terminal);
	ncs_connect_watch_start(terminal->watchdog_client, terminal->watchdog_client);
	ncs_watchdog_monitor_add(terminal->watchdog_client, &terminal->watchdog_client->client_monitor_pid);

	return (int)terminal;
}

void terminal_audio_packnum_clear(struct terminal *terminal)
{
	terminal->audio_data_packnum = 0;
}

void terminal_audio_packnum_handle(struct terminal *terminal)
{
    struct broadcast *bc = terminal->ncs_manager->bc;
    struct call_manager *call_manager = term->call_manager;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	call_t *call = call_manager->current_call_get(call_manager);
	task_t task = get_system_task();

	if((task == TASK_NCS_CALL_INCOMING) || (task == TASK_NCS_BROADCAST_INCOMING))
	{
		if(terminal->audio_data_packnum++ > 60)
		{
			SPON_LOG_INFO("back last win\n");
			terminal_audio_packnum_clear(terminal);
			if(get_system_task() == TASK_NCS_BROADCAST_INCOMING)
			{
				WEB_SYS_LOG(USRLOG_BCIN_END);
				bc->ncs_wav_broadcast_task_stop(bc);
			}
			else if(get_system_task() == TASK_NCS_CALL_INCOMING)
			{
				WEB_SYS_LOG(USRLOG_TALKEND);
				ncs_manager->ncs_call_stop(ncs_manager, call, FALSE);
			}
			set_system_task(TASK_NONE);
			EnterWin(system_info.main_hwnd, g_StauteFunOld);
		}
	}
}

struct terminal *terminal_malloc(void)
{
	struct terminal *tmp = (struct terminal *)malloc(sizeof(struct terminal));
	if (!tmp)
		return NULL;

	memset(tmp, 0, sizeof(struct terminal));

	term = tmp;

	terminal_init(tmp);
	return tmp;
}

void terminal_free(struct terminal *terminal)
{
	free(terminal);
}

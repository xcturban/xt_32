#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <common_lib/common_lib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


#include "sip.h"
#include "system_info.h"
/*#include "window_focus.h"*/
#include "lpconfig.h"
#include "ortp/ortp.h"
#include "term.h"
/*#include "keypad.h"*/
/*#include "ui.h"*/


typedef struct
{
	uint8_t cmd;
	uint8_t param1;
	uint8_t param2;
	uint8_t param3;
	uint8_t param4;
	uint8_t data[128];
} __attribute__ ((packed)) sip_event_data_t;

typedef enum
{
	SIP_CALL = 0,
	SIP_CAMERA_ON,
	SIP_CAMERA_OFF,
	SIP_REGISTER_STATE,
	SIP_DTMF,
	SIP_DISPLAY_VIDEO,
	SIP_BLF_STATE_SET,
	SIP_BLF_STATE_GET,
	SIP_BLF_STATE_UPDATE,
} sip_msg_t;

typedef enum
{
	CALL_REQUEST = 0,
	CALL_IN,
	CALL_START,
	CALL_BUSY,
	CALL_ANSWER,
	CALL_STOP,
} call_param_t;

typedef enum
{
	UNREGISTERED = 0,
	REGISTERED,
} register_param_t;


#define LINPHONEC_CTRL_PIPE "linphonec"


struct sip_internal_data
{
	bool_t registered;
	bool_t busy;

	ortp_pipe_t server_pipe;
	ortp_pipe_t client_pipe;

	pthread_t evt_get_id;
	pthread_mutex_t evt_get_mutex;

	char target_name[128];
	char target_Number[128];
};

static struct sip_internal_data *sid;



static int sip_event_send(uint8_t cmd,
		uint8_t param1, uint8_t param2,
		uint8_t param3, uint8_t param4,
		uint8_t *data, uint8_t data_len)
{
	int ret;
	sip_event_data_t __evt_data;

	if (sid->client_pipe == ORTP_PIPE_INVALID)
		return -1;

	memset(&__evt_data, 0, sizeof(__evt_data));
	__evt_data.cmd = cmd;
	__evt_data.param1 = param1;
	__evt_data.param2 = param2;
	__evt_data.param3 = param3;
	__evt_data.param4 = param4;

	if (data != NULL)
		memcpy(__evt_data.data, data, data_len);

	ret = ortp_pipe_write(sid->client_pipe, (unsigned char *)&__evt_data, sizeof(__evt_data));
	if (ret == sizeof(__evt_data))
		return 0;
	else
		return -1;
}


static int sip_call(call_param_t param1, uint8_t *data, uint8_t data_len)
{
	return sip_event_send(SIP_CALL, param1, 0, 0, 0, data, data_len);
}


static void sip_ctrl_event_recv(sip_event_data_t *__evt_data)
{
	task_t task = get_system_task(); // ��ȡ��ǰ����
	struct audio_manager *audio_manager = term->audio_manager;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	char Buf[133];

	if (__evt_data->cmd == SIP_CALL)
	{
		switch (__evt_data->param1)
		{
			case CALL_REQUEST: // �յ�sip����
				SPON_LOG_INFO("=====>%s - sip call request recv from %s\n", __func__, __evt_data->data);

				if (task == TASK_NONE)
				{

					ncs_manager->ncs_monitor_stop(ncs_manager);

					term->audio_manager->set_talk_in_volume(audio_manager, -1);
					term->audio_manager->set_talk_out_volume(audio_manager, -1);

					sid->busy = TRUE;

					// ����sip�û���
					if(term->customer_type == CUSTOMER_GDZH)
					{
						if(strstr(__evt_data->data, "##"))
						{
							sscanf(__evt_data->data, "%[^#]##%s", sid->target_name, sid->target_Number);
						}
						else
						{
							sscanf(__evt_data->data, "%s", sid->target_name);
							sscanf(__evt_data->data, "%s", sid->target_Number);
						}
						utf8_to_gb2312(sid->target_name, Buf, strlen(sid->target_name));
						strcpy(sid->target_name, Buf);
					}
					else
					{
						strcpy(sid->target_name, __evt_data->data);
					}

					/*set_system_task(TASK_SIP_CALL_INCOMING);*/
					////virtual_key_send(kKeyCallIn);

				}
				else
					sip_call(CALL_STOP, NULL, 0);

				break;
			case CALL_IN:
				break;
			case CALL_START:
				SPON_LOG_INFO("=====>%s - sip call start recv\n", __func__);

				////virtual_key_send(kKeyCallStart);

				break;
			case CALL_BUSY:
				break;
			case CALL_ANSWER:
				break;
			case CALL_STOP:
				sid->busy = FALSE;

				////virtual_key_send(kKeyCallStop);
				memset(sid->target_name, 0, 128);
				memset(sid->target_Number, 0, 128);
				break;
			default:
				break;
		}
	}
	else if (__evt_data->cmd == SIP_REGISTER_STATE)
	{
		sid->registered = __evt_data->param1;
		// notify_ui__sip_online_state_changed();
		////virtual_key_send(kKeyLoginOk);
	}
	else if (__evt_data->cmd == SIP_DISPLAY_VIDEO)
	{
		/*display_video();*/
	}
	else if (__evt_data->cmd == SIP_BLF_STATE_SET)
	{
		/*board_msg_t *msg = NULL;*/
		/*unsigned char params[5];*/

		/*// printf("param1=%d, param2=%d, param3=%d id=%s\n", __evt_data->param1, __evt_data->param2, __evt_data->param3, __evt_data->data);*/

		/*memset(params, 0, 5);*/
		/*params[0] = 0x02;*/
		/*params[1] = __evt_data->param3; // λ����չ����һ��*/
		/*params[2] = __evt_data->param1;     // sip�ն�״̬*/
		/*params[3] = term->language;     // ��ȡϵͳ���ԣ�����չ������Ӣ���л�*/

		/*msg =  msg_send_new(__evt_data->param2, MASTER_CONTROL, params, NULL);*/
		/*if (msg)*/
		/*{*/
			/*msg->send_len = 8;*/
			/*board_msg_send(msg);*/
		/*}*/


	}
}


static int sip_fd_set(fd_set *rset)
{
	int i;
	ortp_pipe_t pipe;
	int max_fd = 0;

	pipe = sid->server_pipe;
	if (pipe != ORTP_PIPE_INVALID)
	{
		FD_SET(pipe, rset);
		max_fd = max_fd > pipe ? max_fd : pipe;
	}

	pipe = sid->client_pipe;
	if (pipe != ORTP_PIPE_INVALID)
	{
		FD_SET(pipe, rset);
		max_fd = max_fd > pipe ? max_fd : pipe;
	}

	return max_fd;
}

static void *sip_event_get_thread(void *arg)
{
	while (1)
	{
		fd_set rset;
		int ret;
		int max_fd;
		ortp_pipe_t pipe;

		FD_ZERO(&rset);
		max_fd = sip_fd_set(&rset);
		ret = select(max_fd + 1, &rset, NULL, NULL, NULL);
		if (ret > 0)
		{
			pipe = sid->server_pipe;
			if (pipe != ORTP_PIPE_INVALID)
			{
				if (FD_ISSET(pipe, &rset))
				{
					SPON_LOG_INFO("=====>%s - client linphonec connect\n", __func__);
					sid->client_pipe = ortp_server_pipe_accept_client(pipe);
					if (sid->client_pipe == ORTP_PIPE_INVALID)
						SPON_LOG_ERR("=====%s - client linphonec connect failed:%s\n", __func__, strerror(errno));
				}
			}

			pipe = sid->client_pipe;
			if (pipe != ORTP_PIPE_INVALID)
			{
				if (FD_ISSET(pipe, &rset))
				{
					sip_event_data_t evt_data;
					memset(&evt_data, 0, sizeof(evt_data));
					ret = ortp_pipe_read(pipe, (unsigned char *)(&evt_data), sizeof(evt_data));
#if 0
					if (ret == 0)
					{
						// linphonec�����쳣�˳�
						SPON_LOG_ERR("=====>%s - linphonec exit abort\n", __func__);
						sid->client_pipe = ORTP_PIPE_INVALID;
						// ����linphonec����
						system("killall -9 linphonec");
						system("linphonec -C -D -c /root/linphonerc &");
					}
					else if (ret == sizeof(evt_data))
						sip_ctrl_event_recv(&evt_data);
#else
					if (ret == sizeof(evt_data))
						sip_ctrl_event_recv(&evt_data);
#endif
				}
			}
		}
		usleep(100*1000);
	}

	ortp_thread_exit(NULL);
	return NULL;
}


static int linphone_start(void)
{
	/*signal(SIGPIPE, SIG_IGN);*/

	sid->server_pipe = ortp_server_pipe_create(LINPHONEC_CTRL_PIPE);
	if (sid->server_pipe == ORTP_PIPE_INVALID)
	{
		SPON_LOG_ERR("=====>%s - server_pipe create failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	if (ortp_thread_create(&sid->evt_get_id, NULL, sip_event_get_thread, NULL) < 0)
	{
		SPON_LOG_ERR("sip_event_get_thread create failed:%s\n", strerror(errno));
		return -1;
	}

	pthread_mutex_init(&sid->evt_get_mutex, NULL);

	system("linphonec -C -D -c /root/linphonerc &");

	return 0;
}


static int linphone_cfg_file_update(void)
{
	LpConfig *ini_config, *linphone_config;
	char *name, *passwd;
	char tmp[128] = {0};
	char *ip;
	int port;
	char *p = NULL;

	ini_config = lp_config_new(INI_CONFIG_FILE);
	if (ini_config == NULL)
	{
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	linphone_config = lp_config_new(LINPHONE_CONFIG_FILE);
	if (linphone_config == NULL)
	{
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	// ����sip �û���
	name = lp_config_get_string(ini_config, "sip", "sip_username", "admin");
	p = strchr(name, '@');
	if (p)
	{
		memcpy(tmp, name, p - name);
		lp_config_set_string(linphone_config, "auth_info_0", "username", tmp);
	}
	else
		lp_config_set_string(linphone_config, "auth_info_0", "username", name);


	// ����sip ����
	passwd = lp_config_get_string(ini_config, "sip", "sip_passwd", "admin");
	lp_config_set_string(linphone_config, "auth_info_0", "passwd", passwd);

	// ����realm
	if (p)
	{
		int realm_len = strlen(name) - (p - name) - 1;
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, p + 1, realm_len);
		lp_config_set_string(linphone_config, "auth_info_0", "realm", tmp);
	}

	// ����sip proxy
	ip = lp_config_get_string(ini_config, "sip", "ip", "192.168.1.101");
	port = lp_config_get_int(ini_config, "sip", "port", 0);
	sprintf(tmp, "sip:%s:%d", ip, port);
	lp_config_set_string(linphone_config, "proxy_0", "reg_proxy", tmp);

	sockaddr_convert((struct sockaddr_in *)&term->sip_manager->sip_sdk_tool_sockaddr, ip, 8530);
	// ����sip identity
	if (!p)
	{
		sprintf(tmp, "sip:%s@%s:%d", name, ip, port);
		lp_config_set_string(linphone_config, "proxy_0", "reg_identity", tmp);
	}
	else
	{
		sprintf(tmp, "sip:%s", name);
		lp_config_set_string(linphone_config, "proxy_0", "reg_identity", tmp);
	}

	// ������Ƶ���ն˿�
	port = lp_config_get_int(ini_config, "sip", "port_audio", 7080);
	lp_config_set_int(linphone_config, "rtp", "audio_rtp_port", port);

	// ������Ƶ���ն˿�
	port = lp_config_get_int(ini_config, "sip", "port_vedio", 9080);
	lp_config_set_int(linphone_config, "rtp", "video_rtp_port", port);

	lp_config_destroy(ini_config);

	lp_config_sync(linphone_config);
	lp_config_destroy(linphone_config);

	return 0;
}

static int sip_call_request(uint8_t *data, uint8_t data_len)
{
	struct audio_manager *audio_manager = term->audio_manager;
	term->audio_manager->set_talk_in_volume(audio_manager, -1);
	term->audio_manager->set_talk_out_volume(audio_manager, -1);

	strcpy(sid->target_name, data);
	strcpy(sid->target_Number, data);
	sid->busy = TRUE;
	return sip_call(CALL_REQUEST, data, data_len);
}

static int sip_call_stop(void)
{
	sid->busy = FALSE;
	return sip_call(CALL_STOP, NULL, 0);
}

static int sip_call_start(void)
{
	return sip_call(CALL_START, NULL, 0);
}

static bool_t sip_registered(void)
{
	return sid->registered;
}

static char *sip_target_name_get(void)
{
	return sid->target_name;
}

static char *sip_target_number_get(void)
{
	return sid->target_Number;
}

static int sip_send_dtmf(char c)
{
	return sip_event_send(SIP_DTMF, c, 0, 0, 0, NULL, 0);
}

bool_t sip_busy(void)
{
	return sid->busy;
}

static int sip_stop(void)
{
	return 0;
}

static int sip_start(void)
{
	int ret;

	sid = malloc(sizeof(struct sip_internal_data));
	if (sid == NULL)
	{
		SPON_LOG_ERR("=====>%s - malloc failed\n", __func__);
		return -1;
	}

	memset(sid, 0, sizeof(struct sip_internal_data));

	ret = linphone_cfg_file_update();
	if (ret < 0)
		return ret;

	return linphone_start();
}

static int sip_blf_state_update(void)
{
	return sip_event_send(SIP_BLF_STATE_UPDATE, 0, 0, 0, 0, NULL, 0);
}

struct sip_manager *sip_manager_malloc(void)
{
	struct sip_manager *sip_manager = malloc(sizeof(struct sip_manager));
	if (sip_manager == NULL)
	{
		SPON_LOG_ERR("=====>%s - malloc failed\n", __func__);
		return NULL;
	}

	sip_manager->start             = sip_start;
	sip_manager->stop              = sip_stop;
	sip_manager->registered        = sip_registered;
	sip_manager->busy              = sip_busy;
	sip_manager->send_dtmf         = sip_send_dtmf;
	sip_manager->get_target_name   = sip_target_name_get;
	sip_manager->get_target_number = sip_target_number_get;
	sip_manager->call_request      = sip_call_request;
	sip_manager->call_stop         = sip_call_stop;
	sip_manager->call_start        = sip_call_start;
	sip_manager->blf_state_update  = sip_blf_state_update;

	return sip_manager;
}

void sip_manager_free(struct sip_manager *sip_manager)
{
	free(sip_manager);
}

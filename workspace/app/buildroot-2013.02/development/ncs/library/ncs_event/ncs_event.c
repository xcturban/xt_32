#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "ncs_event.h"
/*#include "msg_printf.h"*/
#include "private.h"

#include "adpcm.h"

ncs_t *ncs;
int error_code;
#if 0
static int ncs_login_timer_delete(int server_type)
{
	if(server_type == MASTER_SERVER)
	{
		if (ncs->login_timer_start)
		{
			ncs->login_timer_start = FALSE;
			return common_timer_delete(ncs->login_timer_id);
		}
	}
	else if(ncs->back_server_enable == TRUE)
	{
		if (ncs->login_back_timer_start)
		{
			ncs->login_back_timer_start = FALSE;
			return common_timer_delete(ncs->login_back_timer_id);
		}
	}
	return 0;
}

static int ncs_handshake_timer_delete(int server_type)
{
	if(server_type == MASTER_SERVER)
	{
		if (ncs->handshake_timer_start)
		{
			ncs->handshake_timer_start = FALSE;
			return common_timer_delete(ncs->handshake_timer_id);
		}
	}
	else
	{
		if (ncs->handshake_back_timer_start)
		{
			ncs->handshake_back_timer_start = FALSE;
			return common_timer_delete(ncs->handshake_back_timer_id);
		}
	}
	return 0;
}
#else
static int ncs_login_timer_delete(void)
{
	if (ncs->login_timer_start)
	{
		ncs->login_timer_start = FALSE;
		return common_timer_delete(ncs->login_timer_id);
	}

	return 0;
}

static int ncs_handshake_timer_delete(void)
{
	if (ncs->handshake_timer_start)
	{
		ncs->handshake_timer_start = FALSE;
		return common_timer_delete(ncs->handshake_timer_id);
	}

	return 0;
}
#endif

#if 0
void hostname_parse(char *hostname, int server_port)
{
	char *ip;
	struct hostent *host;
	char tmp_ip[20];

	if((host = gethostbyname(hostname)) == NULL)
	{
		memset(tmp_ip, 0, 20);
		strcpy(tmp_ip, "192.168.1.13");
		ip = tmp_ip;
	}
	else
	{
		ip = inet_ntoa(*((struct in_addr *)host->h_addr));
	}

	if(strcmp(ip, ncs->hostname_ip) == 0)
	{
		printf("===>server ip no change\n");
		return;
	}

	strcpy(ncs->hostname_ip, ip);

	if (sockaddr_convert((struct sockaddr_in *)&ncs->server_sockaddr, ip, server_port) < 0)
	{
		free(ncs);
		error_code = NCS_ERROR_SOCKADDR_CONVERT_FAILED;
		return;
	}
}
#endif

static void ncs_login_timer_func(union sigval v)
{
	//	int server_type;
	//	server_type = (int *)(v.sival_ptr);

	//	ncs_event_t *evt = ncs_login_request_event_build(server_type);
	ncs_event_t *evt = ncs_login_request_event_buildEx(&ncs->server_master_sockaddr);
	if(evt)
		ncs_event_send(evt);

	evt = ncs_login_request_event_buildEx(&ncs->server_back_sockaddr);
	if(evt)
		ncs_event_send(evt);
}

// static int ncs_login_timer_create(int server_type);
static int ncs_login_timer_create(void);
static void ncs_handshake_timer_func(union sigval v)
{
	//	int server_type;
	//	server_type = (int *)(v.sival_ptr);

	ncs_event_t *evt;

	if (ncs->handshake_timeout_count == 0)
	{
		// ??g???????k??????k
		ncs->online = FALSE;
		/*
		   if(server_type == MASTER_SERVER)
		   ncs->master_server_login_success = FALSE;
		   else if(server_type == BACKUP_SERVER)
		   ncs->backup_server_login_success = FALSE;
		   */

		if (ncs->ops->ncs_login_exit_get)
			ncs->ops->ncs_login_exit_get(ncs->private_data);

		// ncs_login_timer_create(MASTER_SERVER);
		// ncs_login_timer_create(BACKUP_SERVER);
		ncs_login_timer_create();
	}
	else
	{
		// evt = ncs_handshake_event_build(server_type);
		evt = ncs_handshake_event_build();
		if(evt)
		{
			ncs_event_send(evt);
			ncs->handshake_timeout_count--;
		}
	}
}

// static int ncs_login_timer_create(int server_type)
static int ncs_login_timer_create(void)
{
	struct itimerspec its;
	int ret;

#if 1
	if (!ncs->login_timer_start)
	{
		ncs_handshake_timer_delete();

		its.it_value.tv_sec = ncs->login_period;
		its.it_value.tv_nsec = 0;
		its.it_interval.tv_sec = its.it_value.tv_sec;
		its.it_interval.tv_nsec = its.it_value.tv_nsec;

		ret = common_timer_create(&ncs->login_timer_id, &its,
				ncs_login_timer_func, NULL);
		if (ret < 0)
			return ret;

		ncs->login_timer_start = TRUE;
	}
#else

	if(server_type == MASTER_SERVER)
	{
		if (!ncs->login_timer_start)
		{
			ncs_handshake_timer_delete(server_type);

			its.it_value.tv_sec = ncs->login_period;
			its.it_value.tv_nsec = 0;
			its.it_interval.tv_sec = its.it_value.tv_sec;
			its.it_interval.tv_nsec = its.it_value.tv_nsec;

			ret = common_timer_create(&ncs->login_timer_id, &its,
					ncs_login_timer_func, (void *)server_type);
			if (ret < 0)
				return ret;

			ncs->login_timer_start = TRUE;
		}
	}
	else if(ncs->back_server_enable == TRUE)
	{
		if (!ncs->login_back_timer_start)
		{
			ncs_handshake_timer_delete(server_type);

			its.it_value.tv_sec = ncs->login_period + 1;
			its.it_value.tv_nsec = 0;
			its.it_interval.tv_sec = its.it_value.tv_sec;
			its.it_interval.tv_nsec = its.it_value.tv_nsec;

			ret = common_timer_create(&ncs->login_back_timer_id, &its,
					ncs_login_timer_func, (void *)server_type);
			if (ret < 0)
				return ret;

			ncs->login_back_timer_start = TRUE;
		}
	}
#endif

	return 0;
}

// static int ncs_handshake_timer_create(int server_type)
static int ncs_handshake_timer_create(void)
{
	struct itimerspec its;
	int ret;

#if 1
	if (!ncs->handshake_timer_start)
	{
		ncs_login_timer_delete();

		its.it_value.tv_sec = ncs->handshake_period;
		its.it_value.tv_nsec = 0;
		its.it_interval.tv_sec = its.it_value.tv_sec;
		its.it_interval.tv_nsec = its.it_value.tv_nsec;

		ret = common_timer_create(&ncs->handshake_timer_id, &its,
				ncs_handshake_timer_func, NULL);
		if (ret < 0)
			return ret;

		ncs->handshake_timer_start = TRUE;

		ncs->handshake_timeout_count =
			ncs->handshake_timeout / ncs->handshake_period;
	}
#else
	if(server_type == MASTER_SERVER)
	{
		if (!ncs->handshake_timer_start)
		{
			ncs_login_timer_delete(server_type);

			its.it_value.tv_sec = ncs->handshake_period;
			its.it_value.tv_nsec = 0;
			its.it_interval.tv_sec = its.it_value.tv_sec;
			its.it_interval.tv_nsec = its.it_value.tv_nsec;

			ret = common_timer_create(&ncs->handshake_timer_id, &its,
					ncs_handshake_timer_func, (void *)server_type);
			if (ret < 0)
				return ret;

			ncs->handshake_timer_start = TRUE;

			ncs->handshake_timeout_count =
				ncs->handshake_timeout / ncs->handshake_period;
		}
	}
	else
	{
		if (!ncs->handshake_back_timer_start)
		{
			ncs_login_timer_delete(server_type);

			its.it_value.tv_sec = ncs->handshake_period;
			its.it_value.tv_nsec = 0;
			its.it_interval.tv_sec = its.it_value.tv_sec;
			its.it_interval.tv_nsec = its.it_value.tv_nsec;

			ret = common_timer_create(&ncs->handshake_back_timer_id, &its,
					ncs_handshake_timer_func, (void *)server_type);
			if (ret < 0)
				return ret;

			ncs->handshake_back_timer_start = TRUE;

			ncs->handshake_timeout_count =
				ncs->handshake_timeout / ncs->handshake_period;
		}
	}
#endif

	return 0;
}

static void ncs_broadcast_state_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;

	if (evt_data->param1 == 1 || evt_data->param1 == 3)
	{
		char multi_ip[20] = {0};
		int port = 2046;
		uint8_t multi_ip_4 = evt_data->param2;
		struct sockaddr_in *si;

		sprintf(multi_ip, "%d.%d.%d.%d", ncs->multi_ip_1, ncs->multi_ip_2, ncs->multi_ip_3, multi_ip_4);
		memset(&ncs->data_multi_sockaddr, 0, sizeof(ncs->data_multi_sockaddr));
		si = (struct sockaddr_in *)(&ncs->data_multi_sockaddr);
		si->sin_family = AF_INET;
		si->sin_port = htons(port);

		inet_pton(AF_INET, multi_ip, &(si->sin_addr));

		if (evt_data->param1 == 1)
			ncs->bc_type = MULTICAST;
		else
			ncs->bc_type = UNICAST;

		if (ncs->ops->ncs_broadcast_start_get)
			ncs->ops->ncs_broadcast_start_get(evt_data->param3, ncs->private_data);
	}
	else if (evt_data->param1 == 0)
	{
		if (ncs->ops->ncs_broadcast_stop_get)
			ncs->ops->ncs_broadcast_stop_get(ncs->private_data);
	}
}

static void ncs_sdk_control_parse(ncs_event_t *evt)
{
	uint16_t target_id;
	uint8_t board_id;
	uint32_t Tmp = 0;
	ncs_event_data_t *evt_data = evt->evt_data;

	if(ncs->ops->ncs_get_term_custom_type() == 0x18)
	{
		if(evt_data->param2 == 0 && evt_data->param3 == 0)
		{
			Tmp = (*((uint32_t *)(evt_data->extra_data)));
			target_id = Tmp % 100;
			board_id = evt_data->param4;
			if((Tmp/1000))
			{
				ncs->ops->ncs_set_device_server_type_rcd(Tmp/1000);
			}
			else
			{
				ncs->ops->ncs_set_device_server_type_rcd(-1);
			}
		}
	}
	else
	{
		target_id = *((uint16_t *)(&evt_data->param2));
		board_id = evt_data->param4;
		ncs->ops->ncs_set_device_server_type_rcd(-1);
	}

	switch (evt_data->param1)
	{
		case 0: /* ???V??k???V */
			if (ncs->ops->ncs_sdk_call_stop_get)
				ncs->ops->ncs_sdk_call_stop_get(evt->target_sockaddr, target_id, board_id, ncs->private_data);
			break;
		case 1: /* ???V???V???V???V */
			if (ncs->ops->ncs_sdk_call_request_get)
				ncs->ops->ncs_sdk_call_request_get(evt->target_sockaddr, target_id, board_id, ncs->private_data);
			break;
		case 2: /* ???V???V???V???V */
			if (ncs->ops->ncs_sdk_monitor_request_get)
				ncs->ops->ncs_sdk_monitor_request_get(evt->target_sockaddr, target_id, board_id, ncs->private_data);
			break;
		case 3: /* ???V???V???V???V */
			if (ncs->ops->ncs_sdk_call_start_get)
				ncs->ops->ncs_sdk_call_start_get(evt->target_sockaddr, target_id, board_id, ncs->private_data);
			break;
		default:
			break;
	}
}

static void ncs_name_query_ret_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;

	if (evt_data->param1 == 0x04)
	{
		if (ncs->ops->ncs_area_name_bulk_get)
			ncs->ops->ncs_area_name_bulk_get((void *)evt_data->extra_data, ncs->private_data);
	}
	else if (evt_data->param1 == 0x01)
	{
		if(ncs->ops->ncs_get_term_custom_type() == 0x19)  // 辽宁监狱版
		{
			if (ncs->ops->ncs_kfwqName_set)
				ncs->ops->ncs_kfwqName_set((void *)evt_data->extra_data);

		}
	}
	else if (evt_data->param1 == 0x02)
	{
		if (ncs->ops->ncs_term_name_bulk_get)
		{
			ncs->ops->ncs_term_name_bulk_get((void *)evt_data->extra_data, ncs->private_data);
		}
	}
	else if (evt_data->param1 == 0x05)
	{
		if (ncs->ops->ncs_term_name_qglw_get)
		{
			ncs->ops->ncs_term_name_qglw_get((void *)evt_data->extra_data, ncs->private_data);
		}
	}
	else if(evt_data->param1 == 0x06)
	{
		if (ncs->ops->ncs_term_boardext_name_qglw_get)
		{
			ncs->ops->ncs_term_boardext_name_qglw_get((void *)evt_data->extra_data, evt_data->param4, ncs->private_data);
		}
	}
}

static void ncs_third_party_record_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t *p = evt_data->extra_data;

	if (evt_data->param1 == 1)
	{
		struct sockaddr_in sin;
		char ip[20];
		uint16_t port = *((uint16_t *)(&p[0]));
		sprintf(ip, "%d.%d.%d.%d", evt_data->param2, evt_data->param3, evt_data->param4, evt_data->param5);

		if (sockaddr_convert(&sin, ip, port) < 0)
			return;

		if (ncs->ops->ncs_third_party_record_start_get)
			ncs->ops->ncs_third_party_record_start_get((struct sockaddr *)&sin, ncs->private_data);

	}
	else if (evt_data->param1 == 0)
	{
		if (ncs->ops->ncs_third_party_record_stop_get)
			ncs->ops->ncs_third_party_record_stop_get(ncs->private_data);
	}
}

static void ncs_terminal_info_get_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t *p = evt_data->extra_data;
	int checksum = evt_data->param2 + evt_data->param3 + evt_data->param4 + evt_data->param5;

	if (evt->evt_data_len >= 20 + NCS_EVENT_DATA_HEADER_LEN)//(checksum == evt_data->param1)
	{
		uint16_t terminal_id;
		char server_ip[20] = {0};
		char gw[20] = {0};
		char terminal_ip[20] = {0};
		char mask[20] = {0};
		char mac_addr[18];
		
		uint16_t server_port;
		uint16_t terminal_port;
		
		terminal_id = *((uint16_t *)(&evt_data->param2));

		sprintf(server_ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
		sprintf(gw, "%d.%d.%d.%d", p[4], p[5], p[6], p[7]);
		sprintf(terminal_ip, "%d.%d.%d.%d", p[8], p[9], p[10], p[11]);
		/*sprintf(gw, "%d.%d.%d.%d", 192, 168, 1, 1);*/
		/*sprintf(terminal_ip, "%d.%d.%d.%d", 192, 168, 1, 101);*/
		
		sprintf(mask, "%d.%d.%d.%d", p[12], p[13], p[14], p[15]);
		server_port = *((uint16_t *)(&p[16]));
		terminal_port = *((uint16_t *)(&p[18]));
		
		if(p[20] == 0x11 && p[21] == 0x11 && p[28] == 0x22 && p[29] == 0x22)
		{
			sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", p[22], p[23], p[24], p[25], p[26], p[27]);
		}

		if (ncs->ops->ncs_terminal_info_get)
			ncs->ops->ncs_terminal_info_get(terminal_id,
					server_ip,
					server_port,
					gw,
					mask,
					mac_addr,
					terminal_ip,
					terminal_port,
					ncs->private_data);
	}
}

static void ncs_terminal_password_get_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t *p = evt_data->extra_data;
	int i;
	char password[20] = {0};

	for(i=0; i<6; i++)
		password[i]=p[i];

	if (ncs->ops->ncs_terminal_password_get)
		ncs->ops->ncs_terminal_password_get(password, ncs->private_data);
}


static void ncs_video_request_get_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t *p = evt_data->extra_data;
	char ip[20] = {0};
	uint16_t port = *((uint16_t *)(&evt_data->param4));
	video_class_t class = evt_data->param1;
	uint8_t board_id = evt_data->param2 + 1;
	video_fmt_t fmt = evt_data->param3;
	printf("fmt : %d\n", fmt);

	sprintf(ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

	if (ncs->ops->ncs_video_request_get)
		ncs->ops->ncs_video_request_get(evt->target_sockaddr,
				class,
				fmt,
				board_id,
				ip,
				port,
				ncs->private_data);
}

static void ncs_video_state_get_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t *p = evt_data->extra_data;
	char ip[20] = {0};
	uint16_t port = *((uint16_t *)(&evt_data->param4));
	video_state_t state = evt_data->param1;
	uint8_t board_id = evt_data->param2 + 1;

	sprintf(ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);

	if (ncs->ops->ncs_video_state_get)
		ncs->ops->ncs_video_state_get(evt->target_sockaddr,
				state,
				board_id,
				ip,
				port,
				ncs->private_data);
}

static void ncs_talk_wav_data_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	int wave_data_len = evt->evt_data_len - NCS_EVENT_DATA_HEADER_LEN;
	uint16_t checksum = uint16_checksum_calculate(
			(uint16_t *)(evt_data->extra_data),
			wave_data_len / 2);
	char *device_type = getenv("NCS_DEVICE_TYPE");
	uint16_t type = 0x0;
	uint8_t *p;
	uint8_t decode_data[NCS_PCM_LEN] = {0};

	if (device_type)
		type = strtol(device_type, NULL, 16);

	if (ncs->talk_audio_encode == AUDIO_ENCODE_NONE)
	{
		if (checksum != *((uint16_t *)(&evt_data->param2)))
			return;
	}

	if(ncs->ops->ncs_get_term_custom_type() == 0x18 )
	{
		if((ncs->ops->ncs_ecpt_type_get() == 0x60 || ncs->ops->ncs_ecpt_type_get() == 0x41))
		{

			uint16_t *p = (uint16_t *)(evt_data->extra_data);
			uint16_t key;
			int i;
			type = ncs->ops->ncs_ecpt_type_get();
			p[wave_data_len/2-1] ^= ((uint16_t)type << 8) | (~type & 0xFF);//1???V???V.???V??V???V???V???V???V???V???V???V1???V???V???V???V(???V???V?????V???V???V???h???V???V???V???V)
			key = ((uint16_t)p[wave_data_len/2-1] << 5) |   //2???V???V.???V???V???V???V???V???V1???V???V???V???V???V???V?????V???V???V???V??
				((uint16_t)p[wave_data_len/2-1] >> 11);

			for (i = 1; i<wave_data_len/2-1; i++)
				p[i] ^= p[i - 1] ^ key;				  //3???V???V.???V???V???V???V???V???V1???V???V???V???V???V???V???V???V???V???V??
		}
	}
	else
	{
		if (type == 0x60 || type == 0x41 || type == 0x40 || type == 0x30)
		{
			uint16_t *p = (uint16_t *)(evt_data->extra_data);
			uint16_t key;
			int i;
			p[wave_data_len/2-1] ^= ((uint16_t)type << 8) | (~type & 0xFF);//1???V???V.???V??V???V???V???V???V???V???V???V1???V???V???V???V(???V???V?????V???V???V???h???V???V???V???V)
			key = ((uint16_t)p[wave_data_len/2-1] << 5) |   //2???V???V.???V???V???V???V???V???V1???V???V???V???V???V???V?????V???V???V???V??
				((uint16_t)p[wave_data_len/2-1] >> 11);

			for (i = 1; i<wave_data_len/2-1; i++)
				p[i] ^= p[i - 1] ^ key;				  //3???V???V.???V???V???V???V???V???V1???V???V???V???V???V???V???V???V???V???V??
		}
	}

	if (wave_data_len > 0 && wave_data_len <= NCS_PCM_LEN)
	{
		if (wave_data_len == NCS_PCM_LEN)
		{
			p = evt_data->extra_data;
		}
		else
		{
			int len = FADPCM_Decode(evt_data->extra_data, (short *)decode_data,  NCS_ADPCM_LEN);
			p = decode_data;
			wave_data_len = len;
		}
	}

	if (ncs->ops->ncs_talk_wave_data_get)
		ncs->ops->ncs_talk_wave_data_get(evt->target_sockaddr,
				p,
				wave_data_len,
				ncs->private_data);
}

static void ncs_realtime_mp3_data_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	int mp3_data_len = evt->evt_data_len - NCS_EVENT_DATA_HEADER_LEN;

	if (ncs->ops->ncs_realtime_mp3_data_get)
		ncs->ops->ncs_realtime_mp3_data_get(evt_data->extra_data,
				mp3_data_len,
				ncs->private_data);
}

static void ncs_realtime_wav_data_parse(ncs_event_t *evt)
{
	uint8_t *p;
	uint8_t decode_data[NCS_PCM_LEN] = {0};
	ncs_event_data_t *evt_data = evt->evt_data;
	int wave_data_len = evt->evt_data_len - NCS_EVENT_DATA_HEADER_LEN;

	if (ncs->bc_audio_encode == AUDIO_ENCODE_NONE)
	{
		uint16_t checksum = uint16_checksum_calculate((uint16_t *)(evt_data->extra_data), wave_data_len / 2);
		if (checksum != *((uint16_t *)(&evt_data->param2)))
			return;
	}

	if (wave_data_len > 0 && wave_data_len <= NCS_PCM_LEN)
	{
		if (wave_data_len == NCS_PCM_LEN)
			p = evt_data->extra_data;
		else
		{
			int len = FADPCM_Decode(evt_data->extra_data, (short *)decode_data, NCS_ADPCM_LEN);
			p = decode_data;
			wave_data_len = len;
		}
	}

	if (ncs->ops->ncs_realtime_wave_data_get)
		ncs->ops->ncs_realtime_wave_data_get(p, wave_data_len, ncs->private_data);
}

static void ncs_task_switch_get_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	ncs_task_t task;
	char multi_ip[20] = {0};

	if (evt_data->param1 == 0x0 || evt_data->param1 == 0xff)
	{
		if (ncs->ops->ncs_task_idle_get)
			ncs->ops->ncs_task_idle_get(ncs->private_data);
		return;
	}
	else if (evt_data->param1 == 0x1)
	{
		task = MP3_BROADCAST_TASK;
	}
	else if (evt_data->param1 == 0x11)
	{
		task = WAVE_BROADCAST_TASK;
	}

	sprintf(multi_ip, "%d.%d.%d.%d", evt_data->extra_data[14],
			evt_data->extra_data[15],
			evt_data->extra_data[16],
			evt_data->extra_data[17]);

	if (ncs->ops->ncs_task_switch_get != NULL)
		ncs->ops->ncs_task_switch_get(multi_ip, task, ncs->private_data);
}

static void ncs_ecpt_flag_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	/*if (evt_data->param1 == 0x60 || evt_data->param1 == 0x41)*/
	{
			if(ncs->ops->ncs_ecpt_type_set)
				ncs->ops->ncs_ecpt_type_set(evt_data->param1);
	}
}

static void ncs_ip_request_ret_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	if (evt_data->param1 == 0x11)
	{
		char ip[20] = {0};
		uint16_t port;
		sprintf(ip, "%d.%d.%d.%d", evt_data->param2, evt_data->param3, evt_data->param4, evt_data->param5);
		port = *((uint16_t *)(&evt_data->extra_data[4]));
		//2017-01-09新增附加数据９２字节的跨服务器名称，邹工
		if(evt->evt_data_len > 92) //如果长度没有９２的话，说明不是跨服务器，所以就不需要获取名称显示
		{
			if(evt_data->extra_data[92])//这里邹工要求判断.
			{
				if(ncs->ops->ncs_kfwqName_set)
					ncs->ops->ncs_kfwqName_set(&evt_data->extra_data[92]);
			}
		}
		if (ncs->ops->ncs_ip_request_ok_get)
			ncs->ops->ncs_ip_request_ok_get(ip, port, ncs->private_data);
		if(evt->evt_data_len > 8+6)
		{
			if(ncs->ops->ncs_type_ecpt_get)
				ncs->ops->ncs_type_ecpt_get(&evt_data->extra_data[6], ncs->private_data);
		}
	}
	else
	{
		if (ncs->ops->ncs_ip_request_failed_get)
			ncs->ops->ncs_ip_request_failed_get(ncs->private_data);
	}
}

static void ncs_call_state_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t tmp;
	uint16_t target_id;
	uint8_t board_id;
	int samplerate;
	ncs_audio_encode_t encode;

	target_id = *((uint16_t *)(&evt_data->param4));

	board_id = 0;
	tmp = evt_data->extra_data[1];
	if (((tmp & 0x0F) ^ 0x05) == ((tmp & 0xF0) >> 4))
	{
#if 0
		board_id = tmp & 0x0F;
		board_id += 1;
#else
		board_id = (((tmp >> 2) & 0x03) | ((tmp << 2) & 0x0C));
		board_id += 1;
#endif
	}

	samplerate = 0;
	encode = AUDIO_ENCODE_INVALID;

#ifdef SUPPORT_CUSTOMIZABLE_TALK_AUDIO_FORMAT
	if (evt_data->extra_data[4] == 0x00)
		samplerate = 8000;
	else if (evt_data->extra_data[4] == 0x01)
		samplerate = 16000;
	encode = evt_data->extra_data[5];
#endif

	switch (evt_data->param1)
	{
		case 0:
			if (ncs->ops->ncs_call_stop_get)
				ncs->ops->ncs_call_stop_get(evt->target_sockaddr,
						target_id,
						board_id,
						ncs->private_data);
			break;
		case 2:
			if (ncs->ops->ncs_call_answer_get)
				ncs->ops->ncs_call_answer_get(evt->target_sockaddr,
						target_id,
						board_id,
						samplerate,
						encode,
						ncs->private_data);
			break;
		case 3:
			if (ncs->ops->ncs_call_start_get)
				ncs->ops->ncs_call_start_get(evt->target_sockaddr,
						target_id,
						board_id,
						samplerate,
						encode,
						ncs->private_data);
			break;
		case 4:
			if (ncs->ops->ncs_call_reject_get)
				ncs->ops->ncs_call_reject_get(evt->target_sockaddr,
						target_id,
						board_id,
						ncs->private_data);
			break;
		case 5:
			if (ncs->ops->ncs_call_busy_get)
				ncs->ops->ncs_call_busy_get(evt->target_sockaddr,
						target_id,
						board_id,
						ncs->private_data);
			break;
		default:
			break;
	}
}

static void ncs_call_request_get_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint8_t tmp;
	uint16_t target_id;
	uint8_t board_id;
	uint16_t sever_num = 0;
	int samplerate;
	ncs_audio_encode_t encode;

	target_id = *((uint16_t *)(&evt_data->param4));

	board_id = 0;
	tmp = evt_data->extra_data[1];
	if (((tmp & 0x0F) ^ 0x05) == ((tmp & 0xF0) >> 4))
	{
#if 0
		board_id = tmp & 0x0F;
		board_id += 1;
#else
		board_id = (((tmp >> 2) & 0x03) | ((tmp << 2) & 0x0C));
		board_id += 1;
#endif
	}

	samplerate = 0;
	encode = AUDIO_ENCODE_INVALID;

#ifdef SUPPORT_CUSTOMIZABLE_TALK_AUDIO_FORMAT
	if (evt_data->extra_data[4] == 0x00)
		samplerate = 8000;
	else if (evt_data->extra_data[4] == 0x01)
		samplerate = 16000;
	encode = evt_data->extra_data[5];
#endif
	if(ncs->ops->ncs_get_term_custom_type() == 0x18)
	{
		if(evt_data->param1 == 1)
		{
			if(evt->evt_data_len > (NCS_EVENT_DATA_HEADER_LEN+66))
			{
				
				if(evt_data->extra_data[63] == 0x78 && evt_data->extra_data[64] == 0x56)
				{
					sever_num = evt_data->extra_data[66];
					sever_num <<= 8;
					sever_num |= evt_data->extra_data[65];
					ncs->ops->ncs_set_device_server_type_rcd(sever_num);
				}
				else
				{
					ncs->ops->ncs_set_device_server_type_rcd(-1);
				}
			}
			else
			{
				ncs->ops->ncs_set_device_server_type_rcd(-1);
			}
		}
	}
	switch (evt_data->param1)
	{
		case 0:
			if (ncs->ops->ncs_call_stop_get)
				ncs->ops->ncs_call_stop_get(evt->target_sockaddr,
						target_id,
						board_id,
						ncs->private_data);
			break;
		case 1:
			if (ncs->ops->ncs_call_request_get)
				ncs->ops->ncs_call_request_get(evt->target_sockaddr,
						target_id,
						board_id,
						samplerate,
						encode,
						ncs->private_data);
			break;
		case 8:
			SPON_LOG_INFO("call not be monitored\n");
			/*if (ncs->ops->ncs_monitor_request_get)*/
				/*ncs->ops->ncs_monitor_request_get(evt->target_sockaddr,*/
						/*target_id,*/
						/*board_id,*/
						/*samplerate,*/
						/*encode,*/
						/*ncs->private_data);*/
			/*增加目标拒绝给对方*/
			/*evt = ncs_call_start_event_build(evt->target_sockaddr, target_id, board_id, 0, 0);*/
			evt = ncs_call_reject_event_build(evt->target_sockaddr, target_id, board_id);
			
			if (evt)
			{
				ncs_event_send(evt);
			}
			break;
		default:
			break;
	}
}

static void ncs_nat_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	if (evt_data->param1)
	{
		ncs_event_t *tmp;
		char ip[20];
		uint8_t *p = evt_data->extra_data;
		uint16_t port = *((uint16_t *)(&p[4]));
		struct sockaddr_in sin;

		sprintf(ip, "%d.%d.%d.%d", evt_data->param2, evt_data->param3, evt_data->param4, evt_data->param5);

		// SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, ip, port);

		if (sockaddr_convert(&sin, ip, port) < 0)
			return;

		// ???V???V???V???V
		tmp = ncs_nat_event_build((struct sockaddr *)&sin, ip, port);
		if (tmp != NULL)
			ncs_event_send(tmp);
	}
	else
	{
		struct sockaddr_in *sin = (struct sockaddr_in *)evt->target_sockaddr;
		char ip[20];
		inet_ntop(AF_INET, &sin->sin_addr, ip, 16);

		// SPON_LOG_INFO("=====>%s - ip:%s port:%d\n", __func__, ip, ntohs(sin->sin_port));

		if (ncs->ops->ncs_nat_transfer_get)
			ncs->ops->ncs_nat_transfer_get(ip, ntohs(sin->sin_port), ncs->private_data);
	}
}

#if 0
static int ncs_get_server_type_by_parse_sockaddr_ip(struct sockaddr *target_sockaddr)
{
	struct sockaddr_in *sin_evt 	  	= (struct sockaddr_in *)target_sockaddr;
	struct sockaddr_in *sin_master_ip 	= (struct sockaddr_in *)&ncs->server_master_sockaddr;
	struct sockaddr_in *sin_back_ip 	= (struct sockaddr_in *)&ncs->server_back_sockaddr;

	char *ip_evt;
	char ip_master[20];
	char ip_back[20];

	ip_evt = inet_ntoa(sin_evt->sin_addr);
	inet_ntop(AF_INET, (void *)&sin_master_ip->sin_addr, ip_master, 16);
	inet_ntop(AF_INET, (void *)&sin_back_ip->sin_addr, ip_back, 16);

	if(strcmp(ip_evt, ip_master) == 0)
	{
		return MASTER_SERVER;
	}
	else if(strcmp(ip_evt, ip_back) == 0)
	{
		return BACKUP_SERVER;
	}

	return NOTHING_SERVER;
}
#endif

static void ncs_login_event_parse(ncs_event_t *evt)
{
	//	int server_type;
	ncs_event_data_t *evt_data = evt->evt_data;

#if 0
	if(ncs_get_server_type_by_parse_sockaddr_ip(evt->target_sockaddr) == MASTER_SERVER)
		server_type = MASTER_SERVER;
	else if(ncs_get_server_type_by_parse_sockaddr_ip(evt->target_sockaddr) == BACKUP_SERVER)
		server_type = BACKUP_SERVER;
	else
	{
		server_type = NOTHING_SERVER;
		printf("login event parse ip isn't master or backup ip\n");
		return;
	}
#endif

	if (evt_data->param1 == 0)
	{
		// ???Z?????????c????????]
		if (!ncs->online)
			return;

		ncs->online = FALSE;
		WEB_SYS_LOG(USRLOG_OFFLINE);
#if 0
		if(server_type == MASTER_SERVER)
			ncs->master_server_login_success = FALSE;
		else if(server_type == BACKUP_SERVER)
			ncs->backup_server_login_success = FALSE;

		if (ncs->ops->ncs_login_exit_get)
			ncs->ops->ncs_login_exit_get(ncs->private_data);

		ncs_login_timer_delete(MASTER_SERVER);
		ncs_login_timer_delete(BACKUP_SERVER);

		ncs_login_timer_create(MASTER_SERVER);
		ncs_login_timer_create(BACKUP_SERVER);
#else
		if (ncs->ops->ncs_login_exit_get)
			ncs->ops->ncs_login_exit_get(ncs->private_data);

		ncs_handshake_timer_delete();
		ncs_login_timer_create();
#endif
	}
	else
	{
		uint16_t *p;
		int bc_samplerate, talk_samplerate;
		ncs_audio_encode_t bc_encode, talk_encode;
		bool_t have_task = FALSE;
		ncs_task_t task;
		char multi_ip[20] = {0};

		if (ncs->online)
			return;

		if (evt->evt_data_len < NCS_EVENT_DATA_HEADER_LEN + 6 + 8 + 8)
			return;

		WEB_SYS_LOG(USRLOG_LOGIN);
		
		p = (uint16_t *)(&(evt_data->extra_data[6+8]));
		if (p[0] == 0x1234 && p[3] == 0x4321)
		{
			if ( (p[2] & 0xff) == 0)
				bc_samplerate = 8000;
			else
				bc_samplerate = 22050;

			if ( (p[2] >> 8) == 0)
				bc_encode = AUDIO_ENCODE_NONE;
			else
				bc_encode = AUDIO_ENCODE_ADPCM_SPON;

			ncs->online = TRUE;
			memcpy(&ncs->server_sockaddr, evt->target_sockaddr, sizeof(struct sockaddr));

#if 0
			if(server_type == MASTER_SERVER)
			{
				ncs_handshake_timer_delete(BACKUP_SERVER);
				ncs_login_timer_delete(BACKUP_SERVER);
				ncs->server_sockaddr = ncs->server_master_sockaddr;
			}
			else if(server_type == BACKUP_SERVER)
				ncs->server_sockaddr = ncs->server_back_sockaddr;

			ncs_handshake_timer_create(server_type);
#else
			if(evt_data->param1 == 0)
			{
				ncs->server_type = MASTER_SERVER;
			}
			else if(evt_data->param2 == 1)
			{
				ncs->server_type = BACKUP_SERVER;
			}
			ncs_login_timer_delete();
			ncs_handshake_timer_create();
#endif
			//QGLW
			if(evt->evt_data_len >= NCS_EVENT_DATA_HEADER_LEN + 6 + 8 + 8+10)
			{
				ncs->server_type_host_rcdno = *((uint16_t *)(&(evt_data->extra_data[30]))); //保存服务器的类型
			}
			ncs->multi_ip_1 = evt_data->param3;
			if(ncs->server_type == BACKUP_SERVER)
			{
				ncs->multi_ip_1 = 234;//由于备用服务器8501组播地址发的是０，这里强制改为２３４
			}
			ncs->multi_ip_2 = evt_data->param4;
			ncs->multi_ip_3 = evt_data->param5;

			if (evt_data->param2 == 0x11 || evt_data->param2 == 0x1)
			{
				have_task = TRUE;

				if (evt_data->param2 == 0x11)
					task = WAVE_BROADCAST_TASK;
				else
					task = MP3_BROADCAST_TASK;
				sprintf(multi_ip,
						"%d.%d.%d.%d",
						evt_data->param3,
						evt_data->param4,
						evt_data->param5,
						evt_data->extra_data[5]);
			}
		}

		talk_encode = bc_encode;
		talk_samplerate = 8000;

#ifdef SUPPORT_CUSTOMIZABLE_TALK_AUDIO_FORMAT
		if (evt->evt_data_len >= NCS_EVENT_DATA_HEADER_LEN + 6 + 8 + 8 + 2)
		{

			p = (uint8_t *)(&(evt_data->extra_data[22]));
			if (p[0] == 0x01)
				talk_samplerate = 16000;

			talk_encode = p[1];
		}
#endif

		ncs->talk_audio_encode = talk_encode;
		ncs->bc_audio_encode = bc_encode;

		if (ncs->ops->ncs_login_ok_get)
			ncs->ops->ncs_login_ok_get(bc_samplerate, talk_samplerate, have_task, task, multi_ip, ncs->private_data);

		/*
		   if(server_type == MASTER_SERVER)
		   ncs->master_server_login_success = TRUE;
		   else
		   ncs->backup_server_login_success = TRUE;
		   */
	}
}

static void ncs_system_time_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	uint16_t *p = (uint16_t *)evt_data->extra_data;

	if (ncs->ops->ncs_system_time_get)
		ncs->ops->ncs_system_time_get(p[0], evt_data->param2, evt_data->param3, evt_data->param4, evt_data->param5, p[1], evt_data->param1, ncs->private_data);
}

static void	ncs_warring_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	char multi_ip[20] = {0};

	sprintf(multi_ip, "%d.%d.%d.%d", evt_data->extra_data[0],
			evt_data->extra_data[1],
			evt_data->extra_data[2],
			evt_data->extra_data[3]);

	if (ncs->ops->ncs_warring_get)
		ncs->ops->ncs_warring_get(evt_data->param1, evt_data->param2, multi_ip);
}

static void ncs_port_out_ctl_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	if (ncs->ops->ncs_port_out_ctl_get)
		// ??g????_?1: ??m???g????g?l
		// ??g????_?2: ????k????? 0-????d 1-??\??
		ncs->ops->ncs_port_out_ctl_get(evt_data->param1, evt_data->param2);
}

static void ncs_auto_answer_ctl_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	if (ncs->ops->ncs_auto_answer_ctl_get)
		ncs->ops->ncs_auto_answer_ctl_get(evt_data->param1);
}

static void ncs_event_parse(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	/*my_msg_eve_pares(evt_data->msg, evt->target_sockaddr, evt->evt_data, evt->evt_data_len);*/
	switch (evt_data->msg)
	{
		case 0x10:
			ncs->handshake_timeout_count++;
			break;
		case 0x11:
			ncs_system_time_parse(evt);
			break;
		case 0x12:
			if (ncs->ops->ncs_volume_get)
				ncs->ops->ncs_volume_get(evt_data->param1, ncs->private_data);
			break;
		case 0x1A:
			ncs_login_event_parse(evt);
			break;
		case 0x21:
			ncs_nat_parse(evt);
			break;
		case 0x24:
			ncs_call_request_get_parse(evt);
			break;
		case 0x25:
			ncs_call_state_parse(evt);
			break;
		case 0x27:
			ncs_ip_request_ret_parse(evt);
			break;
		case 0x1B:
			ncs_ecpt_flag_parse(evt);
			break;
		case 0x2F:
			ncs_task_switch_get_parse(evt);
			break;
		case 0x41:
			ncs_realtime_mp3_data_parse(evt);
			break;
		case 0x45:
			ncs_realtime_wav_data_parse(evt);
			break;
		case 0x4E:
			ncs_talk_wav_data_parse(evt);
			break;
		case 0x5B:
			ncs_video_request_get_parse(evt);
			break;
		case 0x5C:
			ncs_video_state_get_parse(evt);
			break;
		case 0x81:
			ncs_terminal_info_get_parse(evt);
			break;
		case 0x85:
			ncs_terminal_password_get_parse(evt);
			break;
		case 0xb7:
			ncs_third_party_record_parse(evt);
			break;
		case 0x33:
			ncs_port_out_ctl_parse(evt);
			break;
		case 0x36:
			ncs_warring_parse(evt);
			break;
		case 0x75:
			break;
		case 0x23:
			ncs_broadcast_state_parse(evt);
			break;
		case 0x66:
			ncs_name_query_ret_parse(evt);
			break;
		case 0x91:
			ncs_sdk_control_parse(evt);
			break;
		case 0xB5:
			ncs_auto_answer_ctl_parse(evt);
			break;
		default:
			break;
	}
}

static void ncs_event_free(ncs_event_t *evt)
{
	if (evt)
	{
		if (evt->evt_data)
			free(evt->evt_data);
		free(evt);
	}
}

static void *ncs_event_get_thread(void *arg)
{
	fd_set rfds;
	int ret;
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	// pthread_mutex_t *mutex = (pthread_mutex_t *)ncs->private_data;

	evt_data = (ncs_event_data_t *)malloc(UDP_MAX_SIZE);
	if (evt_data == NULL)
		return NULL;

	evt = (ncs_event_t *)malloc(sizeof(*evt));
	if (evt == NULL)
	{
		free(evt_data);
		return NULL;
	}

	while (1)
	{
		fd_set rfds;
		int ret;

		FD_ZERO(&rfds);
		FD_SET(ncs->sfd, &rfds);
		ret = select(ncs->sfd + 1, &rfds, NULL, NULL, NULL);
		if (ret <= 0)
			continue;

		if (FD_ISSET(ncs->sfd, &rfds))
		{
			struct sockaddr s;
			socklen_t addrlen = sizeof(struct sockaddr);

			memset(evt_data, 0, UDP_MAX_SIZE);
			ret = recvfrom(ncs->sfd, (char *)evt_data, UDP_MAX_SIZE, 0, &s, &addrlen);
			if (ret < NCS_EVENT_DATA_HEADER_LEN)
				continue;

			memset(evt, 0, sizeof(*evt));
			evt->evt_data = evt_data;
			evt->evt_data_len = ret;
			evt->target_sockaddr = &s;

			ncs_event_parse(evt);
		}
	}

	ncs_event_free(evt);
	pthread_exit(NULL);
	return NULL;
}

int ncs_get_error_code(void)
{
	return error_code;
}

int ncs_init(char *main_server_ip, int main_server_port, char *bak_server_ip, int bak_server_port, uint16_t id, int local_port, ncs_event_ops_t *ops, void *private_data)
{
	int ret;
	int bak_server_status = 0;

	if (main_server_ip == NULL || main_server_port <= 0 || id == 0 || local_port <= 0)
	{
		error_code = NCS_ERROR_INVALID_ARGUMENT;
		return -1;
	}

	ncs = (ncs_t *)malloc(sizeof(ncs_t));
	if (!ncs)
	{
		error_code = NCS_ERROR_MALLOC_FAILED;
		return -1;
	}

	memset(ncs, 0, sizeof(*ncs));

	ncs->id = id;

	pthread_mutex_init(&ncs->event_send_mutex, NULL);

	ncs->sfd = local_udp_socket_bind(local_port);
	if (ncs->sfd < 0)
	{
		free(ncs);
		error_code = NCS_ERROR_LOCAL_SOCKET_BIND_FAILED;
		return -1;
	}

	if(sockaddr_convert((struct sockaddr_in *)&ncs->server_sockaddr, main_server_ip, main_server_port) < 0)
	{
		free(ncs);
		error_code = NCS_ERROR_SOCKADDR_CONVERT_FAILED;
		return -1;
	}
	if(sockaddr_convert((struct sockaddr_in *)&ncs->server_master_sockaddr, main_server_ip, main_server_port) < 0)
	{
		free(ncs);
		error_code = NCS_ERROR_SOCKADDR_CONVERT_FAILED;
		return -1;
	}
	if(sockaddr_convert((struct sockaddr_in *)&ncs->server_back_sockaddr, bak_server_ip, bak_server_port) < 0)
	{
		free(ncs);
		error_code = NCS_ERROR_SOCKADDR_CONVERT_FAILED;
		return -1;
	}

	ret = pthread_create(&ncs->event_get_tid, NULL, ncs_event_get_thread, NULL);
	if (ret < 0)
	{
		free(ncs);
		error_code = NCS_ERROR_PTHREAD_CREATE_FAILED;
		return -1;
	}

	sockaddr_convert((struct sockaddr_in *)&ncs->msg_multi_sockaddr, "234.0.0.254", 2046);

	ret = multicast_add_membership(ncs->sfd, "234.0.0.254");
	if (ret < 0)
	{
		error_code = NCS_ERROR_MULTICAST_JOIN_FAILED;
		return -1;
	}

	ncs->private_data = private_data;
	ncs->ops = ops;

	//设置主ＩＰ和备份ＩＰ
	/*set_main_msg_ip(main_server_ip);*/
	/*set_backup_main_msg_ip(bak_server_ip);*/
	// ncs->master_server_login_success = FALSE;
	// ncs->backup_server_login_success = FALSE;

	return 0;
}

/*int ncs_event_send(ncs_event_t *evt)*/
/*{*/
/*pthread_mutex_lock(&ncs->event_send_mutex);*/
/*sendto(ncs->sfd,*/
/*(char *)(evt->evt_data),*/
/*evt->evt_data_len,*/
/*0,*/
/*evt->target_sockaddr,*/
/*sizeof(struct sockaddr));*/
/*pthread_mutex_unlock(&ncs->event_send_mutex);*/

/*ncs_event_free(evt);*/

/*return 0;*/
/*}*/

int ncs_event_send(ncs_event_t *evt)
{
	ncs_event_data_t *evt_data = evt->evt_data;
	pthread_mutex_lock(&ncs->event_send_mutex);
	sendto(ncs->sfd,
			(char *)(evt->evt_data),
			evt->evt_data_len,
			0,
			evt->target_sockaddr,
			sizeof(struct sockaddr));
	/*my_msg_eve_send(evt_data->msg, evt->target_sockaddr, evt->evt_data, evt->evt_data_len);*/
	pthread_mutex_unlock(&ncs->event_send_mutex);
	ncs_event_free(evt);

	return 0;
}

int ncs_handshake_period_set(int seconds)
{
	if (ncs == NULL)
	{
		error_code = NCS_ERROR_CORE_NOT_INITED;
		return -1;
	}

	ncs->handshake_period = seconds;
	return 0;
}

int ncs_handshake_timeout_set(int seconds)
{
	if (ncs == NULL)
	{
		error_code = NCS_ERROR_CORE_NOT_INITED;
		return -1;
	}

	ncs->handshake_timeout = seconds;
	return 0;
}

int ncs_login_period_set(int seconds)
{
	if (ncs == NULL)
	{
		error_code = NCS_ERROR_CORE_NOT_INITED;
		return -1;
	}

	ncs->login_period = seconds;
	return 0;
}

// int ncs_login(uint16_t boards_status, int back_server_enable)
int ncs_login(uint16_t boards_status)
{
	int ret;

	if (ncs == NULL)
	{
		error_code = NCS_ERROR_CORE_NOT_INITED;
		return -1;
	}

	if (ncs->online)
		return 0;

	ncs->boards_status = boards_status;
	//	if(ncs->back_server_enable == TRUE)
	//		ncs->back_server_enable = back_server_enable;

	//	ret = ncs_login_timer_create(MASTER_SERVER);
	ret = ncs_login_timer_create();
	if (ret < 0)
	{
		error_code = NCS_ERROR_TIMER_CREATE_FAILED;
		return -1;
	}
#if 0
	ret = ncs_login_timer_create(BACKUP_SERVER);
	if (ret < 0)
	{
		error_code = NCS_ERROR_TIMER_CREATE_FAILED;
		return -1;
	}
#endif
	return 0;
}

int ncs_multicast_join(char *multi_ip)
{
	if (ncs == NULL)
	{
		error_code = NCS_ERROR_CORE_NOT_INITED;
		return -1;
	}

	return multicast_add_membership(ncs->sfd, multi_ip);
}

int ncs_multicast_drop(char *multi_ip)
{
	if (ncs == NULL)
	{
		error_code = NCS_ERROR_CORE_NOT_INITED;
		return -1;
	}

	return multicast_drop_membership(ncs->sfd, multi_ip);
}

void ncs_talk_audio_encode_set(ncs_audio_encode_t encode)
{
	ncs->talk_audio_encode = encode;
}

void ncs_bc_audio_encode_set(ncs_audio_encode_t encode)
{
	ncs->bc_audio_encode = encode;
}


/***************************************************************************/

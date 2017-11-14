#include "config.h"

static void terminal_audio_param_config(struct audio_manager *audio_manager, LpConfig *lpconfig)
{
	int in_sel;
	int out_sel;

	audio_manager->talk_in_volume	= lp_config_get_int(lpconfig, "talk_cfg", "volume_in", 8);
	audio_manager->talk_out_volume	= lp_config_get_int(lpconfig, "talk_cfg", "volume_out", 8);
	audio_manager->set_talk_out_volume(audio_manager, audio_manager->talk_out_volume);
	audio_manager->bc_in_volume 	= lp_config_get_int(lpconfig, "broadcast_cfg", "volume_in", 8);
	audio_manager->bc_out_volume	= lp_config_get_int(lpconfig, "broadcast_cfg", "volume_out", 8);
	audio_manager->ring_volume		= lp_config_get_int(lpconfig, "terminal", "volume_ring", 8);
	audio_manager->system_volume	= audio_manager->ring_volume;

	in_sel = lp_config_get_int(lpconfig, "terminal", "input_channel_sel", 0);
	/*//mic*/
	/*if (in_sel == 0) */
	/*{*/
		/*if (audio_manager->codec == WM8960)*/
		/*{*/
			/*audio_manager->orig_input_channel = 1;*/
			/*audio_manager->input_channel = 1;*/
			/*system("echo 1 > /proc/audio_input_switch");*/
		/*}*/
		/*else*/
		/*{*/
			/*audio_manager->orig_input_channel = 3;*/
			/*audio_manager->input_channel = 3;*/
			/*system("echo 3 > /proc/adc_sel");*/
		/*}*/

		/*audio_manager->orig_in_chn = MIC_INPUT;*/
		/*audio_manager->in_chn = MIC_INPUT;*/
	/*}*/
	/*else*/
	/*{*/
		/*Line in*/
		/*if (audio_manager->codec == WM8960)*/
		/*{*/
			/*audio_manager->orig_input_channel = 3;*/
			/*audio_manager->input_channel = 3;*/
			/*system("echo 3 > /proc/audio_input_switch");*/
		/*}*/
		/*else*/
		/*{*/
			/*audio_manager->orig_input_channel = 9;*/
			/*audio_manager->input_channel = 9;*/
			/*system("echo 9 > /proc/adc_sel");*/
		/*}*/

		/*audio_manager->orig_in_chn = LINE_INPUT;*/
		/*audio_manager->in_chn = LINE_INPUT;*/
	/*}*/


	/*//Speaker*/
	/*out_sel = lp_config_get_int(lpconfig, "terminal", "output_channel_sel", 0);*/
	/*if (out_sel == 0)*/
	/*{*/
		/*if (audio_manager->codec == WM8960)*/
		/*{*/
			/*audio_manager->orig_output_channel = 1;*/
			/*audio_manager->output_channel = 1;*/
			/*system("echo 1 > /proc/audio_output_switch");*/
		/*}*/
		/*else*/
		/*{*/
			/*audio_manager->orig_output_channel = 3;*/
			/*audio_manager->output_channel = 3;*/
			/*system("echo 3 > /proc/dac_sel");*/
		/*}*/

		/*audio_manager->orig_out_chn = SPK_OUTPUT;*/
		/*audio_manager->out_chn = SPK_OUTPUT;*/
	/*}*/
	/*else*/
	/*{*/
		/*//Line out*/
		/*if (audio_manager->codec == WM8960)*/
		/*{*/
			/*audio_manager->orig_output_channel = 0;*/
			/*audio_manager->output_channel = 0;*/
			/*system("echo 0 > /proc/audio_output_switch");*/
		/*}*/
		/*else*/
		/*{*/
			/*audio_manager->orig_output_channel = 2;*/
			/*audio_manager->output_channel = 2;*/
			/*system("echo 2 > /proc/dac_sel");*/
		/*}*/

		/*audio_manager->orig_out_chn = LINE_OUTPUT;*/
		/*audio_manager->out_chn = LINE_OUTPUT;*/
	/*}	*/
}
#if 0//def __NAS8532_CONFIG_H__


void system_config_save(void)
{
//	NAND_WP_CLOSE();

	system("cp /var/www/ini/sys_cfg.txt /mnt/nand1-3/ini/sys_cfg.txt");
	system("cp /var/www/ini/board_cfg.txt /mnt/nand1-3/ini/board_cfg.txt");
	system("cp /var/www/ini/alone_cfg.txt /mnt/nand1-3/ini/alone_cfg.txt");
	system("cp /var/www/ini/missed_call.txt /mnt/nand1-3/ini/missed_call.txt");

//	NAND_WP_OPEN();
}


void terminal_config_set_password(char *password, void *private_data)
{
	struct _LpConfig *ini_cfg;

	SPON_LOG_INFO("=====>%s - password:%s\n", __func__, password);

	ini_cfg = lp_config_new(INI_CONFIG_FILE);
	if (!ini_cfg) {
		SPON_LOG_ERR("=====>%s - lp_config_new failed\n", __func__, strerror(errno));
		return;
	}

	lp_config_set_string(ini_cfg, "terminal", "password", password);

	lp_config_sync(ini_cfg);
	lp_config_destroy(ini_cfg);

	system_config_save();
	system("reboot");
}


char * terminal_config_get_password( LpConfig *lpconfig)
{
	struct _LpConfig *ini_cfg;
	char *password;
	
	ini_cfg = lp_config_new(INI_CONFIG_FILE);
	password = (char *)lp_config_get_string(lpconfig, "terminal", "password", "123456");
	lp_config_destroy(ini_cfg);
	
	return password;
}

static void terminal_ncs_manager_config(struct ncs_manager *ncs_manager, LpConfig *lpconfig)
{
	char *ip;
	struct hostent *host;
	char *hostname;
	struct broadcast *bc = ncs_manager->bc;

	hostname = (char *)lp_config_get_string(lpconfig, "server_cfg", "ip_server1", "192.168.1.13");
	if ( (host = gethostbyname(hostname)) == NULL)
		ip = hostname;
	else
		ip = inet_ntoa(*((struct in_addr *)host->h_addr));
	
	ncs_manager->server1_ip = strdup(ip);
	ncs_manager->server1_port = lp_config_get_int(lpconfig, "server_cfg", "port_server1", 2048);

	hostname = (char *)lp_config_get_string(lpconfig, "server_cfg", "ip_server2", NULL);
	if (hostname)
	{
		if ((host = gethostbyname(hostname)) == NULL)
			ip = hostname;
		else
			ip = inet_ntoa(*((struct in_addr *)host->h_addr));
		
		ncs_manager->server2_ip = strdup(ip);
		ncs_manager->server2_port = lp_config_get_int(lpconfig, "server_cfg", "port_server2", 2048);
	}

	ncs_manager->id = lp_config_get_int(lpconfig, "terminal", "id", 3);

	ip = (char *)lp_config_get_string(lpconfig, "terminal", "ip", "192.168.1.101");
	ncs_manager->local_ip = strdup(ip);
	
	ncs_manager->local_port = lp_config_get_int(lpconfig, "terminal", "port", 2046);
	ip = (char *)lp_config_get_string(lpconfig, "terminal", "mask", "255.255.255.0");
	ncs_manager->netmask = strdup(ip);
	ip = (char *)lp_config_get_string(lpconfig, "terminal", "gate", "192.168.1.1");
	ncs_manager->gw = strdup(ip);
	ip = (char *)lp_config_get_string(lpconfig, "terminal", "dns1", "192.168.1.1");
	ncs_manager->dns1 = strdup(ip);
	ip = (char *)lp_config_get_string(lpconfig, "terminal", "dns2", "192.168.1.1");
	ncs_manager->dns2 = strdup(ip);

	ncs_manager->video_port = lp_config_get_int(lpconfig, "terminal", "port_video", 2058);
	ip = lp_config_get_string(lpconfig, "terminal", "ip_route", NULL);
	if (ip == NULL)
		ncs_manager->video_ip_route = ncs_manager->local_ip;
	else
		ncs_manager->video_ip_route = strdup(ip);
	
	ncs_manager->video_port_route = lp_config_get_int(lpconfig, "terminal", "port_route", 0);
	if (ncs_manager->video_port_route == 0)
		ncs_manager->video_port_route = ncs_manager->video_port;

	ncs_manager->talk_audio_encode = lp_config_get_int(lpconfig, "talk_cfg", "is_pcm", 1);
	ncs_manager->bc_audio_encode = lp_config_get_int(lpconfig, "broadcast_cfg", "is_pcm", 1);
	ncs_manager->priority = lp_config_get_int(lpconfig, "priority_cfg", "call_priority", TRUE);
	ncs_manager->ptt_trigger_type = lp_config_get_int(lpconfig, "talk_cfg", "ptt_ctrl_method", PRESS_NONE);
	ncs_manager->conference_enable = lp_config_get_int(lpconfig, "talk_cfg", "conference_talk", FALSE);
	ncs_manager->video_request_fmt = lp_config_get_int(lpconfig, "terminal", "video_request_fmt", VIDEO_VGA_FMT);
	ncs_manager->queueing_enable = lp_config_get_int(lpconfig, "queue_cfg", "queue_enable", TRUE);
	ncs_manager->queueing_number = lp_config_get_int(lpconfig, "queue_cfg", "queue_number", 7);
	ncs_manager->short_out_value = lp_config_get_int(lpconfig, "short_cfg", "shortout_type", 0);

	bc->offline_ring = lp_config_get_int(lpconfig, "broadcast_cfg", "alert_audio", 1);
	bc->offline_ring = bc->offline_ring ? 0 : 1;

	ncs_manager->trigger_time = lp_config_get_int(lpconfig, "short_cfg", "trigger_time", 5);

	if (ncs_manager->queueing_enable)
	{
		ncs_manager->conference_enable = FALSE;
		ncs_manager->max_calls = ncs_manager->queueing_number;
	}
	else
	{
		if (ncs_manager->conference_enable)
			ncs_manager->max_calls = 2;
		else
			ncs_manager->max_calls = 1;
	}
}


static void terminal_talk_config(struct terminal *term, LpConfig *lpconfig)
{
	term->dial_rapid = lp_config_get_int(lpconfig, "talk_cfg", "quick_callType", 0);
}


static void terminal_board_config(struct audio_manager *audio_manager, LpConfig *lpconfig)
{

}


static void terminal_language_config(struct terminal *term, LpConfig *lpconfig)
{
	term->language = lp_config_get_int(lpconfig, "language", "language", 0);
}


static void terminal_web_config_set_time(void)
{
	char date[128];
	LpConfig *lpconfig;
	struct web_time web_time;
	lpconfig = lp_config_new(INI_CONFIG_FILE);
	if (lpconfig == NULL)
	{
		SPON_LOG_ERR("=====>%s - web time set fail\n", __func__ );
		return;
	}

	web_time.year   = lp_config_get_int(lpconfig, "time_cfg", "year",   2015);
	web_time.month  = lp_config_get_int(lpconfig, "time_cfg", "month",  8);
	web_time.day    = lp_config_get_int(lpconfig, "time_cfg", "day",    1);
	web_time.hour   = lp_config_get_int(lpconfig, "time_cfg", "hour",   12);
	web_time.minute = lp_config_get_int(lpconfig, "time_cfg", "minute", 0);
	web_time.second = lp_config_get_int(lpconfig, "time_cfg", "second", 0);

	sprintf(date, "date -s '%d-%d-%d %d:%d:%d'", web_time.year, web_time.month, web_time.day, web_time.hour, web_time.minute, web_time.second);

	system(date);
	system("hwclock -w");

	lp_config_destroy(lpconfig);
}


void terminal_web_config_read(struct terminal *term)
{
	LpConfig *lpconfig = lp_config_new(INI_CONFIG_FILE);
	
	if (!lpconfig)
		SPON_LOG_FATAL("ini config file error\n");

	terminal_audio_param_config(term->ncs_manager, lpconfig);
	terminal_ncs_manager_config(term->audio_manager, lpconfig);

	terminal_talk_config(term, lpconfig);
	terminal_language_config(term, lpconfig);
	
	lp_config_sync(lpconfig);
	lp_config_destroy(lpconfig);
	system_config_save();
}

void* terminal_web_config_modify_monitor(void *arg)
{
	char buf[128];
	ortp_pipe_t client = ORTP_PIPE_INVALID;
	ortp_pipe_t pipe = ortp_server_pipe_create(WEB_CONFIG_PIPE);

	if (pipe == ORTP_PIPE_INVALID)
	{
		SPON_LOG_ERR("=====>%s - ortp_server_pipe_create failed:%s\n", __func__, strerror(errno));
		return NULL;
	}

	while (1)
	{
		fd_set rset;
		int ret;
		int maxfd = pipe;

		FD_ZERO(&rset);
		FD_SET(pipe, &rset);

		if (client != ORTP_PIPE_INVALID)
		{
			FD_SET(client, &rset);
			maxfd = maxfd > client ? maxfd : client;
		}
		ret = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (ret > 0)
		{
			if (FD_ISSET(pipe, &rset))
			{
				client = ortp_server_pipe_accept_client(pipe);
				if (client == ORTP_PIPE_INVALID)
				{
					SPON_LOG_ERR("=====>%s - ortp_server_pipe_accept_client failed:%s\n", __func__, strerror(errno));
					continue;
				}
			}

			if (client != ORTP_PIPE_INVALID)
			{
				if (FD_ISSET(client, &rset))
				{
					ret = ortp_pipe_read(client, (uint8_t *)buf, 128);
                    /*printf("ret :%d\n", ret);*/
                    /*printf("buf[0] :%d\n", buf[0]);*/
					if (ret == 0)
					{
						client = ORTP_PIPE_INVALID;
						SPON_LOG_INFO("=====>%s - web config client closed\n", __func__);
						continue;
					}

					if (ret == 1 && buf[0] == 4)
					{
						system_config_save();
						board_reboot();
						system("reboot");
					}
					else if(ret == 1 && buf[0] == 3)
					{
						terminal_web_config_set_time();
					}
					else if (ret == 1 && buf[0] == 5)
					{
						NAND_WP_CLOSE();
						system("touch /mnt/nand1-3/recovery_file");
						NAND_WP_OPEN();
						board_reboot();
						system("reboot");
					}

				}
			}
		}
	}
	return NULL;
}


#endif

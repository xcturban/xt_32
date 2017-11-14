#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <common_lib/common_lib.h>


#include "ortp/ortp.h"
#include "date.h"
#include "system_info.h"
#include "language.h"
#include "term.h"

#include "mediastreamer2/mseventqueue.h"

#include "config.h"
#include "guiwelcomwin.h"


static bool_t current_net_link = FALSE;
static bool_t current_usb_link = FALSE;
static bool_t current_ftp      = FALSE;
static HWND hMainWnd;
extern struct time_st g_RtcTime;

pthread_t tid_main_window_DispatchMessage;


bool_t net_link(void);
bool_t usb_link(void);

RECT LEFT_BUTTON = {0,0,LCD_WIDTH/2,LCD_HEIGH};
RECT RIGHT_BUTTON = {LCD_WIDTH/2,0,LCD_WIDTH,LCD_HEIGH};

static void *crond_task_thread(void *arg)
{
	static int led_count_flash = 0;
	usleep(1000 * 1000);

    INIT_LIST_HEAD(&s_file_list_obj.file_player_list);
    INIT_LIST_HEAD(&s_file_list_obj.file_broadcast_list);
	while (1)
	{
		struct audio_manager *audio_manager = term->audio_manager;
		static int codec_reinit_count = 0; // 音频芯片复位计数器
		struct ncs_manager *ncs_manager = term->ncs_manager;

		pthread_mutex_lock(&term->mutex);

		bool_t tmp = net_link();
		bool_t tmp_usb = usb_link();
		if(current_usb_link != tmp_usb)
		{
			current_usb_link = tmp_usb;
			if(current_usb_link)
			{
				current_ftp = TRUE;
				file_list_obj_set_play_path(&s_file_list_obj, FILE_USB_DIR);
				file_list_dst();
				file_list_init();
				SendNotifyMessage(system_info.main_hwnd, MSG_NCS_BROADCAST_FILE_LIST_UPDATE, 0, 0);
			}
			else
			{
				current_ftp = FALSE;
			}
		}
		else
		{
			if(!current_ftp)
			{
				current_ftp = TRUE;
				file_list_obj_set_play_path(&s_file_list_obj, FILE_FTP_PATH);
				file_list_dst();
				file_list_init();
			}
		}

		if (current_net_link != tmp)
		{
			current_net_link = tmp;
			if(tmp == 1)
			{
				led_count_flash = 0;
				netlink_led_open();
			}
			else
			{
				netlink_led_close();
			}
			SendNotifyMessage(system_info.main_hwnd, MSG_NCS_STATUE_BAR_PAGE, 0, 0);
		}
		else
		{
			if(term->image_load == 1)
			{
				if(ncs_manager->online == 0 && tmp == 1)
				{
					led_count_flash++;
					if(led_count_flash == 2)
					{
						netlink_led_open();
					}
					else if(led_count_flash == 4) {
						led_count_flash = 0;
						netlink_led_close();
					}
				}
			}
		}

		terminal_audio_packnum_handle(term);
		audio_manager->audio_switch(audio_manager, FALSE);

		if (audio_manager->evt_queue)
		{
			ms_event_queue_pump(audio_manager->evt_queue);
		}

		pthread_mutex_unlock(&term->mutex);
	 	ReadRtcTimeEx(&g_RtcTime);
		usleep(500 * 1000);
		if(system_info.flagCreate || term->image_load == 2)
		{
			//刷新主界面时钟
			/*printf("%s, %d\n", __FILE__, __LINE__);*/
			SendNotifyMessage(system_info.main_hwnd, MSG_NCS_WELCOM_PAGE, 1, 0);
		}
	}
}

static void* web_config_modify_monitor(void *arg)
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
					printf("ret :%d\n", ret);
					printf("buf[0] :%d\n", buf[0]);
					if (ret == 0)
					{
						client = ORTP_PIPE_INVALID;
						SPON_LOG_INFO("=====>%s - web config client closed\n", __func__);
						continue;
					}

					if (ret == 1 && buf[0] == 4)
					{
						system_config_save();
						/*board_reboot();*/
						WEB_SYS_LOG(USRLOG_REBOOT);
						system("reboot");
					}
					else if(ret == 1 && buf[0] == 3)
					{
						/*ncs_web_set_time();*/
					}
					else if (ret == 1 && buf[0] == 5)
					{
						NAND_WP_CLOSE();
						system("touch /mnt/nand1-2/recovery_file");
						NAND_WP_OPEN();
						/*board_reboot();*/
						WEB_SYS_LOG(USRLOG_REBOOT);
						system("reboot");
					}

				}
			}
		}
		usleep(100*1000);
	}
	return NULL;
}

static int clear_fb()
{
#define _USE_FBIOPAN_
#define IOCTL_LCD_GET_DMA_BASE      	_IOR('v', 32, unsigned int *)


	struct fb_var_screeninfo g_fb_var;
	__u32 g_u32VpostWidth, g_u32VpostHeight, fb_bpp;
	unsigned int FB_PAddress;
	void* FB_VAddress;
	unsigned int g_u32VpostBufMapSize = 0;

	int fb_fd = open("/dev/fb0", O_RDWR);
	if (fb_fd == -1)
	{
		perror("open fbdevice fail");
		return -1;
	}
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &g_fb_var) < 0)
	{
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb_fd);
		fb_fd = -1;
		return -1;
	}
	g_u32VpostWidth = g_fb_var.xres;
	g_u32VpostHeight = g_fb_var.yres;
	fb_bpp = g_fb_var.bits_per_pixel/8;
	printf("FB width =%d, height = %d, bpp=%d\n",g_u32VpostWidth,g_u32VpostHeight,fb_bpp);
	// Get FB Virtual Buffer address
	if (ioctl(fb_fd, IOCTL_LCD_GET_DMA_BASE, &FB_PAddress) < 0)
	{
		perror("ioctl IOCTL_LCD_GET_DMA_BASE ");
		close(fb_fd);
		return -1;
	}

#ifdef _USE_FBIOPAN_
	g_u32VpostBufMapSize = g_u32VpostWidth*g_u32VpostHeight*fb_bpp*2;	/* MAP 2 buffer */
#else
	g_u32VpostBufMapSize = g_u32VpostWidth*g_u32VpostHeight*fb_bpp*1;	/* MAP 1 buffer */
#endif
	FB_VAddress = mmap(NULL, 	g_u32VpostBufMapSize,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			fb_fd,
			0);

	if ( FB_VAddress == MAP_FAILED)
	{
		printf("LCD Video Map failed!\n");
		close(fb_fd);
		return -1;
	}

	memset (FB_VAddress, 0x0, g_u32VpostBufMapSize );

	munmap(FB_VAddress, g_u32VpostBufMapSize);

	close(fb_fd);

	return 0;
}

terminal_t *term = NULL;
/*PLOGFONT lfangting16;*/
PLOGFONT logfont_ttf_verdana;

static void *image_load_thread(void *arg)
{
	struct broadcast *bc = term->ncs_manager->bc;
	terminal_t *terminal = (terminal_t *)term;
	
	HDC hdc = GetClientDC(hMainWnd);

	
	terminal->image_load = 0;
	/*system_bmp_res_loading(hdc, &system_info.bmp_res_list);*/
	/*system_bmp_res_loading_ex(hdc, &system_info.bmp_res_list);*/
	/*system_bmp_res_init(hdc, &system_info.bmp_res_list);*/
	system_bmp_res_loading_ex(hdc);
	terminal->image_load = 1;
	ReleaseDC(hdc);

	ncs_login(0);
	
	//guiloading_newscreen(hMainWnd);
	/*SetStatueFunc(WelComeWinProc);*/
	/*if(bc->bc_outgoing(bc))*/
	{
		/*guibroadcastwin_newscreen(hMainWnd, get_system_task());*/
	}
	/*else*/
	{
		EnterWin(hMainWnd, WelComeWinProc);
	}

    pthread_exit(NULL);
    return NULL;
}

static void init_res_list_load(void)
{
	HDC hdc = GetClientDC(hMainWnd);
	system_bmp_res_init(&system_info.bmp_res_list);
	ReleaseDC(hdc);
}

int main_window_image_load_thread_create(void *arg)
{
	if (pthread_create(&tid_main_window_DispatchMessage, NULL, image_load_thread, arg) < 0)
	{
		SPON_LOG_ERR("create thread failed:%s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int MiniGUIMain(int argc, const char *argv[])
{

	/*signal(SIGPIPE, SIG_IGN);*/
	MSG Msg;
    // HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    pthread_t tid1, tid2;
	HDC sec_dc;
	char Buf[100];

	logfont_ttf_verdana = CreateLogFont ("ttf", "SIMYOU", "GB2312",
			FONT_WEIGHT_SUBPIXEL, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,  
			FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
			32, 0);

	/*sprintf (Buf, "中文ttf字体%s-%s-%s-size%d-regular: ",*/
			/*logfont_ttf_verdana->type, logfont_ttf_verdana->family,*/
			/*logfont_ttf_verdana->charset, logfont_ttf_verdana->size);*/
	/*printf("Buf: %s\n", Buf);*/

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, "main_window", 0, 0);
#endif


	terminal_malloc();
	system_info_init();

	WEB_SYS_LOG(USRLOG_STRAT);
	pthread_create(&tid1, NULL, web_config_modify_monitor, NULL); // 监测web参数修改
	pthread_create(&tid2, NULL, crond_task_thread, NULL);
	init_png_res_sl();

	CreateInfo.dwStyle        = WS_VISIBLE;
	CreateInfo.dwExStyle      = WS_EX_AUTOSECONDARYDC;
	CreateInfo.spCaption      = "Hello";
	CreateInfo.hMenu          = 0;
	CreateInfo.hIcon          = 0;
	CreateInfo.MainWindowProc = MainWinProc;
	CreateInfo.lx             = 0;
	CreateInfo.ty             = 0;
	CreateInfo.rx             = g_rcScr.right;
	CreateInfo.by             = g_rcScr.bottom;
	CreateInfo.iBkColor       = system_info.window_bg_color;
	CreateInfo.dwAddData      = 0;
	CreateInfo.hHosting       = HWND_DESKTOP;
	hMainWnd = CreateMainWindow(&CreateInfo);
	system_info.main_hwnd = hMainWnd;




	if (hMainWnd == HWND_INVALID)
	{
		return -1;
	}


	clear_fb();
	sec_dc = GetSecondaryDC(hMainWnd);
	SetSecondaryDC(hMainWnd, sec_dc, ON_UPDSECDC_DEFAULT);
	guiloading_newscreen(hMainWnd);
	
	ShowWindow(hMainWnd, SW_SHOWNORMAL);
	while (GetMessage(&Msg, hMainWnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	MainWindowThreadCleanup(hMainWnd);
	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

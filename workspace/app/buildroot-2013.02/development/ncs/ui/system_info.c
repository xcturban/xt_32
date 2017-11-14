#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <common_lib/common_lib.h>

#include "system_bmp_res.h"
#include "system_info.h"
#include "status_bar.h"
#include "term.h"

// ȫ��ϵͳ��Ϣ
system_info_t system_info;
pStatueFunc g_StauteFun      = NULL;
pStatueFunc g_StauteFunOld   = NULL;
bool_t g_vol_change_win_flag = 0;
bool_t g_firstEnter          = FALSE;
void *g_pWinPrivatePrt;
void *g_pWinPrivatePrtOld;

// 初始化系统信息
int system_info_init(void)
{
	memset(&system_info, 0, sizeof(system_info));

	system_info.window_bg_color     = RGB2Pixel(HDC_SCREEN, 0, 0, 9);
	system_info.status_bar_bg_color = RGB2Pixel(HDC_SCREEN, 46, 49, 54);
	system_info.button_bg_color     = RGB2Pixel(HDC_SCREEN, 247, 247, 247);
	system_info.button_border_color = RGB2Pixel(HDC_SCREEN, 215, 215, 215);
	system_info.title_bar_bg_color  = RGB2Pixel(HDC_SCREEN, 112, 112, 112);

	system_info.status_bar_height = 25;
	system_info.title_bar_height  = 30;
	system_info.margin            = 5;
	system_info.spacing           = 5;
    system_info.step              = 60;
	system_info.round_size        = 8;

	system_info.task      = TASK_NONE;
	system_info.prev_task = TASK_NONE;

	return 0;
}

// 获取系统任务
task_t get_system_task(void)
{
	return system_info.task;
}

// 设置系统任务
void set_system_task(task_t task)
{
	system_info.prev_task = system_info.task;
	system_info.task      = task;
}

// 获取上次系统任务
task_t get_system_prev_task(void)
{
	return system_info.prev_task;
}

// 音量加
void system_volume_inc(void)
{
	struct audio_manager *audio_manager = term->audio_manager;
	int value = term->audio_manager->get_talk_out_volume(audio_manager);

	value++;
	if (value > 15)
		value = 15;

	term->audio_manager->set_talk_out_volume(audio_manager, value);
}

// 音量减
void system_volume_dec(void)
{
	struct audio_manager *audio_manager = term->audio_manager;
	int value = term->audio_manager->get_talk_out_volume(audio_manager);

	value--;
	if (value < 0)
		value = 0;
	term->audio_manager->set_talk_out_volume(audio_manager, value);
}


// 保存系统配置信息
void system_config_save(void)
{
	NAND_WP_CLOSE();
	system("cp /var/www/ini/sys_cfg.txt /mnt/nand1-2/ini/sys_cfg.txt");
	system("cp /var/www/ini/warring_cfg.txt /mnt/nand1-2/ini/warring_cfg.txt");
	system("cp /var/www/ini/board_cfg.txt /mnt/nand1-2/ini/board_cfg.txt");
	system("cp /var/www/ini/alone_cfg.txt /mnt/nand1-2/ini/alone_cfg.txt");
	system("cp /var/www/ini/missed_call.txt /mnt/nand1-2/ini/missed_call.txt");
	NAND_WP_OPEN();
}


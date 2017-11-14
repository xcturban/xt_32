#ifndef GUIBROADCASTWIN_H
#define GUIBROADCASTWIN_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <linux/rtc.h>
#include "term.h"
#include "system_bmp_res.h"
#include "guipub.h"
#include "user_msg.h"
#include "file.h"
#include "system_info.h"
#include "guicominputdigtal.h"

typedef enum{
	GUIBROADCAST_FILE = 0,
	GUIBROADCAST_TERN = 0,
	GUIBROADCAST_AREA = 0,
	GUIBROADCAST_ALL  = 0,	
}GUIBROADCAST;


#define GUIBROADCAST_BTN_W			114
#define GUIBROADCAST_BTN_H			93
#define GUIBROADCAST_BTN_X			((LCD_WIDTH-GUIBROADCAST_BTN_W*GUIBROADCAST_BTN_NUM)>>1)
#define GUIBROADCAST_BTN_Y			((LCD_HEIGH-GUIBROADCAST_BTN_H)>>1)
#define BROADCAST_INPUT_NUM			16
#define BROADCAST_TERM_WIDGETS_NUM	3
#define BROADCAST_FILE_WIDGETS_NUM	3
#define GUIBROADCAST_INPUT_NUM 		12
#define DIGITAL_BTN_W				
#define DIGITAL_BTN_H
#define TIMERID_GUIBROADCAST_TIME	103
#define FILE_NUMBER_PER_PAGE		6
#define BROADCAST_AREA_MAX_PAGE		21

typedef enum
{
	BC_TERMINAL     = 0,
	BC_AREA         = 1,
	BC_AREA_SELECT  = 2,
	BC_ALL_TERMINAL = 3,
	BC_FILE         = 4,
	BC_NONE         = 0xffff,
}broadcast_type;

typedef enum
{
	BC_STOP	= 0,
	BC_START,
}broadcast_status;

typedef struct
{
	unsigned char	tab;
	broadcast_type	type;
	broadcast_status status;
	char input[BROADCAST_INPUT_NUM];
	unsigned int 	time;
	unsigned int 	input_position;
	//gui_res_id_t	res[RES_BC_TOTAL];

	unsigned char file_name[FILE_NUMBER_PER_PAGE][128];
	unsigned char file_select[FILE_NUMBER_PER_PAGE];
	struct list_head* file_current_list;
	unsigned int select[FILE_NUMBER_PER_PAGE];
	unsigned int file_current_page;
	unsigned int file_all_pages;
	int file_current_id;
	int old_sel_index;
	int old_sel_page_num;
	unsigned int file_all_count;

	unsigned char area_name[FILE_NUMBER_PER_PAGE][128];
	int area_id[BROADCAST_AREA_MAX_PAGE][FILE_NUMBER_PER_PAGE];
	unsigned char area_select[BROADCAST_AREA_MAX_PAGE][FILE_NUMBER_PER_PAGE];
	//unsigned int area_list[128];

	//struct list_head* area_current_list;
	unsigned int area_current_page;
	unsigned int area_all_pages;
	int area_current_id;
	int old_area_sel_index;
	int old_area_sel_page_num;
	broadcast_status file_play_statue;	
}broadcast_data_struct;

void guibroadcastwin_newscreen(HWND hWnd, int scrn_id);
static void get_next_page(void);
static void get_previous_page(void);
#endif

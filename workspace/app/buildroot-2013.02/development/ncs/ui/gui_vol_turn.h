
#ifndef GUI_VOL_TURN
#define GUI_VOL_TURN

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
#include "system_info.h"
#include "date.h"
#include "guicominputdigtal.h"
#include "guisettingpager.h"
#include "guilocalvideo.h"

typedef struct
{
	TYPE_GUI_AREA_HANDLE *b_p_vol_win_ptr;
	int vol_value[2];
	int vol_slider_down_left;
	bool_t f_first_down;
	RECT rc[2];
}TYPE_CLTL_VOL_TURN;


#endif


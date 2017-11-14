#ifndef GUIMISSEDCALL_H
#define GUIMISSEDCALL_H

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
#include "language.h"
#include "missed_call.h"



#define GUIMISSEDCALL_BTN_X			LCD_WIDTH-BTN_W
#define GUIMISSEDCALL_BTN_Y			LCD_HEIGH-BTN_H

#define GUIMISSEDCALL_INFO_L		((LCD_WIDTH - 585)/2)
#define GUIMISSEDCALL_INFO_T		((LCD_HEIGH - 346)/2)
#define GUIMISSEDCALL_INFO_R		((LCD_WIDTH + 585)/2)
#define GUIMISSEDCALL_INFO_B		((LCD_HEIGH + 346)/2)
#define MISSED_CALL_INFO_OFFSET		56
#define MISSED_CALL_LINE_OFFSET		45
#define MISSED_CALL_UNVALID_ID		0XFFFF
typedef struct
{
	int 			select_idx;
	int 			totalNum;
	int 			timeNum;
}GUIMISSEDCALL;

void guimissedcall_newscreen(HWND hWnd);
#endif

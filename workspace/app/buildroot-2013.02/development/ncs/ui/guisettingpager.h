
#ifndef GUISETTINGPAGER_H
#define GUISETTINGPAGER_H

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

typedef struct
{
	int cur_sel_setting_foucs;
	int cur_sel_setting_foucs_item_index;
	int cur_sel_setting_foucs_Sec_index;
	int cur_sel_setting_foucs_item_indexBegin;
	int cur_sel_setting_foucs_item_indexEnd;
	int cur_press_bar_id;
	int old_press_bar_id;
	bool_t flagDisp;
	char EditBuf[5][16];
}TYPE_CTRL_SETTING_DLG;

typedef struct
{
	int x;
	int y;
	int Pos;
	char Buf[20];
	void(*Paint)(int index, int curPos);
}TYPE_EDIT_DISP_CTLR;

int SettingWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
#endif

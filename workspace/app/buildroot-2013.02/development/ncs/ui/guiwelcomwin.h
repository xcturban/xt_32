
#ifndef GUIWELCOMEWIN_H
#define GUIWELCOMEWIN_H

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
//#include "guimissedcall.h"
#include "guicalllog.h"

#include "guibroadcastwin.h"
#include "gui_call_chatting.h"
#include "guiloginwin.h"

#define POS_BROADCAST_X     165
#define POS_TALK_X     		(POS_BROADCAST_X + 150)
#define POS_JT_X 			(POS_TALK_X + 150)
#define POS_SETTING_X   	(POS_JT_X + 150)


int MainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
#endif


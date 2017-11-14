#ifndef GUIBROADCAST_H
#define GUIBROADCAST_H

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


#define GUIBROADCAST_BTN_W			114
#define GUIBROADCAST_BTN_H			93
#define GUIBROADCAST_BTN_X			167
#define GUIBROADCAST_BTN_Y			387



typedef struct
{
	task_t 	Type;
	int 			timeNum;
}GUIBROADCAST;

void guibroadcast_newscreen(HWND hWnd,task_t type);
void guibroadcast_handup(HWND hWnd);
#endif

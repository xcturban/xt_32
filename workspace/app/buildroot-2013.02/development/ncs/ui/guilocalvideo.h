#ifndef GUILOCALVIDEO_H
#define GUILOCALVIDEO_H

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


#define GUILOCALVIDEO_BTN_W			114
#define GUILOCALVIDEO_BTN_H			93
#define GUILOCALVIDEO_BTN_X			167
#define GUILOCALVIDEO_BTN_Y			387



typedef struct
{
	task_t 	Type;
	int 			timeNum;
}GUILOCALVIDEO;

void guilocalvideo_newscreen(HWND hWnd);
#endif

#ifndef GUILOADING_H
#define GUILOADING_H

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


#define GUILOADING_BTN_W			800
#define GUILOADING_BTN_H			480
#define GUILOADING_BTN_X			0
#define GUILOADING_BTN_Y			0
#define TIMERID_GUILOADING_TIME		104



typedef struct
{
    task_t 	Type;
    int 			timeNum;
} GUILOADING;

void guiloading_newscreen(HWND hWnd);
#endif

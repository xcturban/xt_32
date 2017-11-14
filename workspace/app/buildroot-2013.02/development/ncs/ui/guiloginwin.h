#ifndef GUILOGINWIN_H
#define GUILOGINWIN_H

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
#include "guicominputdigtal.h"


#define LOGINWIN_INPUT_NUM			6
#define LOGINWIN_TERM_WIDGETS_NUM	3
#define LOGINWIN_FILE_WIDGETS_NUM	3
#define GUILOGINWIN_INPUT_NUM 		12


typedef struct
{
	char input[LOGINWIN_INPUT_NUM];
	unsigned int 	input_position;
}loginwin_data_struct;

void guiloginwin_newscreen(HWND hWnd);
#endif

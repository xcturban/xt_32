#ifndef GUICOMINPUTDIGTAL_H
#define GUICOMINPUTDIGTAL_H

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
	int Type;               // 0锁屏,1广播,2对讲,3监听
	int EditCurPos;         // 编辑框当前的位置
	char EditTextBuf[128];  //编辑框内容
	char TitleTextBuf[128]; // 标题栏
	char TipTextBuf[128];   // 提示框
}TYPE_CTRL_INPUT_DIGTAL;
extern TYPE_CTRL_INPUT_DIGTAL g_input_ctrl_type;

//void Init(TYPE_CTRL_INPUT_DIGTAL* pCtrl, int type);
void InputDigtal_NewScreen(HWND hWnd);
#endif

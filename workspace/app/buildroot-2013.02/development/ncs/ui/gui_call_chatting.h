
#ifndef GUI_CALL_CHATTING_H
#define GUI_CALL_CHATTING_H

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
#include "language.h"

static void guicall_videostreamer_display(PRECT pRc, void* pData);
static void guicall_info_display(PRECT pRc, void* pData);
static void guicall_button_display(PRECT pRc, void* pData);
static void guicall_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guicall_handup(HWND hWnd);
int guicall_chatting_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

#endif

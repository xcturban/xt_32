

#ifndef GUI_CALL_OUTCOMMING_H
#define GUI_CALL_OUTCOMMING_H

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

static void guicall_button_display(PRECT pRc, void* pData);
static void guicall_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guicall_videostreamer_display(PRECT pRc, void* pData);
int guicall_out_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
#endif

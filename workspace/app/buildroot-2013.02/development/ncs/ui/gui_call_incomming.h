
#ifndef GUI_CALL_INCOMMING_H
#define GUI_CALL_INCOMMING_H

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
int guicall_incomming_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
#endif

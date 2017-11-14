#ifndef GUICALLLOG_H
#define GUICALLLOG_H

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
#include "language.h"
#include "missed_call.h"
#include "dialed_call.h"
#include "received_call.h"




#define CALLLOG_INFO_OFFSET		10
#define CALLLOG_LINE_OFFSET		35
#define CALLLOG_UNVALID_ID		0XFFFF

#define CALLLOG_TAB_W	128
#define CALLLOG_TAB_H	41

#define CALLLOG_TAB_L	145
#define CALLLOG_TAB_T	74
#define CALLLOG_TAB_B	(CALLLOG_TAB_T+CALLLOG_TAB_H)

#define CALLLOG_INFO_L		CALLLOG_TAB_L
#define CALLLOG_INFO_T		(CALLLOG_TAB_T+CALLLOG_TAB_H)
#define CALLLOG_INFO_R		(CALLLOG_INFO_L+549)
#define CALLLOG_INFO_B		(CALLLOG_INFO_T+308)

typedef enum
{
	CALLLOG_MISSED,
	CALLLOG_DIALED,
	CALLLOG_RECEIVED,
}calllog_item;

typedef struct
{
	calllog_item item;
	int			select_idx;
	int			totalNum;
	int			(*del_callback)(int idx);
	int			(*get_id_callback)(int idx, int *target_id, int *board_id);
	int			(*get_count_callback)(void);
}GUICALLLOG;

void guicalllog_newscreen(HWND hWnd);
#endif

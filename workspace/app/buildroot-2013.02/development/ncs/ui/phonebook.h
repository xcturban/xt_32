#ifndef GUIPHONEBOOK_H
#define GUIPHONEBOOK_H

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
#include "gui_calloutcomming.h"
#include "gui_call_chatting.h"
#include "gui_conference_window.h"
typedef enum {
    INFO_TERM_TALK = 0,
    INFO_SIP = 2,
}info_type_t;

typedef struct {
	char type;
	char id[7];
	char name[24];
}info_t;

#define PHONEBOOK_PAGE_NUM 			10
#define PHONEBOOK_UNVALID_ID		0XFFFF


typedef struct {
	int id;
	info_t info[PHONEBOOK_PAGE_NUM];
	struct list_head list;
}phonebook_t;


typedef struct
{
	char bar_cout; //根据服务器下发的数量来决定大小
	int cur_press_bar_id;
	int old_press_bar_id;
	int select_id;
    struct list_head phonebook_list;
	phonebook_t *phonebook;
}TYPE_PHONE_BOOK;

static void PaintButton(PRECT pRc, void* pData);
static void ButtonProc(HWND hWnd, int message, int wParam, int lParam);
int PhoneBookWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
int phonebook_info_update(void);
int phonebook_info_init(void);

#endif


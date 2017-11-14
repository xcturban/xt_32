#ifndef __SYSTEM_RES_H__
#define __SYSTEM_RES_H__

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "common.h"


typedef struct{
	int id;
	BITMAP bitmap;
	char pic_name[128];
	struct list_head list;
}bmp_res_t;
bool_t system_bmp_res_unload(struct list_head * head );
bool_t system_bmp_res_load(HDC hdc,struct list_head *head);
bmp_res_t *bmp_find_by_name(struct list_head *head,char* name);
bool_t system_bmp_res_loading(HDC hdc,struct list_head *head);

#endif

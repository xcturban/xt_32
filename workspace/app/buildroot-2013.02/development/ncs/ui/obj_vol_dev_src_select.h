#ifndef OBJ_VOL_DEV_SRC_SELECT_H
#define OBJ_VOL_DEV_SRC_SELECT_H

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
#include "date.h"
#include "system_info.h"

typedef  enum
{
	VOICE_IN_HTG,
	VOICE_IN_LINE_IN,
	VOICE_IN_HEAD_IN,
}VOICE_IN_GPIO_CLT;

typedef struct
{
	VOICE_IN_GPIO_CLT dev_src;
	bool_t flag;
}TYPE_VOL_DEV_SRC_CTRL;

extern TYPE_VOL_DEV_SRC_CTRL obj_vol_dev_src_array[3];

#define SIZE_VOL_DEV_SRC 3



void obj_vol_dev_src_init(void);
void obj_vol_dev_src_set_line_in(bool_t flag);
bool_t obj_vol_dev_src_get_head_in(void);
void obj_vol_dev_src_set_head_in(bool_t flag);
bool_t obj_vol_dev_src_get_head_in(void);
bool_t is_obj_vol_dev_src_plugin_line_in(void);
bool_t is_obj_vol_dev_src_plugin_head_in(void);

#endif

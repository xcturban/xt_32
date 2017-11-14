#ifndef __NAS8532_DATE_H__
#define __NAS8532_DATE_H__
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define DATE_LEFT    70
#define DATE_TOP     80
#define DATE_RIGHT   (100+230)
#define DATE_BOTTOOM (80+100)

struct time_st{
	int year;
	int month;
	int day;
	int weekday;
	int hour;
	int minute;
	int second;
};

void draw_date(HDC hdc);
void ReadRtcTimeEx(struct time_st* pRtc);


#endif /* __NAS8532_DATE_H__ */

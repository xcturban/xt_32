#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <signal.h>

#include "system_info.h"
#include "language.h"
#include "date.h"
#include "term.h"



struct time_st g_time;

void date_set(int year, int month, int day, int hour, int minute, int second, int weekday)
{
	g_time.year = year;
	g_time.month = month;
	g_time.day = day;
	g_time.hour = hour;
	g_time.minute = minute;
	g_time.second = second;
	g_time.weekday = weekday;
}

void draw_date(HDC hdc)
{
	char text_buf[128];
	char buf[128] = {0};
	int text_height = GetFontHeight(hdc);
	RECT text_rect;
	BITMAP num;
	static BOOL b_dot = TRUE;
	int num_height = 44;
	int num_width = 28;
	int dot_width = 14;
	int i;
	char tmp[8] = {0};
	static int dot_left = 0;
	int time_left = (g_rcScr.right - num_width*4 - dot_width) / 2;
	struct rtc_time rtc_tm;
	int retval;
	char str1[128];
	char str2[128];
	int fd;
	struct ncs_manager *ncs_manager = term->ncs_manager;

	fd = open ("/dev/rtc0", O_RDONLY);
	if (fd >= 0) {
		retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (retval >= 0) {
			g_time.second=rtc_tm.tm_sec;
			g_time.year=rtc_tm.tm_year + 1900;
			g_time.month=rtc_tm.tm_mon + 1;
			g_time.day=rtc_tm.tm_mday;
			if(ncs_manager->online)
				g_time.weekday=g_time.weekday;
			else
				g_time.weekday=rtc_tm.tm_wday;
			g_time.hour=rtc_tm.tm_hour;
			g_time.minute=rtc_tm.tm_min;
		}
		close(fd);
	}


	time_left=(g_time.hour/10==1?dot_width:num_width)
		+(g_time.hour%10==1?dot_width:num_width)
		+(g_time.minute/10==1?dot_width:num_width)
		+(g_time.minute%10==1?dot_width:num_width)
		+dot_width;
	time_left=(g_rcScr.right - time_left) / 2;

	i=g_time.hour/10;
	sprintf(buf, "%s/%d.bmp", SYSTEM_RES_PATH, i);
	LoadBitmap(hdc, &num, buf);
	FillBoxWithBitmap(hdc,
					time_left,
					(300 - num.bmHeight)/2+120-20,
					num.bmWidth,
					num.bmHeight,
					&num);
	UnloadBitmap(&num);
	if(i==1){
		time_left+=dot_width;
	}else{
		time_left+=num_width;
	}

	i=g_time.hour%10;
	sprintf(buf, "%s/%d.bmp", SYSTEM_RES_PATH, i);
	LoadBitmap(hdc, &num, buf);
	FillBoxWithBitmap(hdc,
					time_left,
					(300 - num.bmHeight)/2+120-20,
					num.bmWidth,
					num.bmHeight,
					&num);
	UnloadBitmap(&num);
	if(i==1){
		time_left+=dot_width;
	}else{
		time_left+=num_width;
	}

	dot_left=time_left;
	time_left+=dot_width;

	i=g_time.minute/10;
	sprintf(buf, "%s/%d.bmp", SYSTEM_RES_PATH, i);
	LoadBitmap(hdc, &num, buf);
	FillBoxWithBitmap(hdc,
					time_left,
					(300 - num.bmHeight)/2+120-20,
					num.bmWidth,
					num.bmHeight,
					&num);
	UnloadBitmap(&num);
	if(i==1){
		time_left+=dot_width;
	}else{
		time_left+=num_width;
	}
	i=g_time.minute%10;
	sprintf(buf, "%s/%d.bmp", SYSTEM_RES_PATH, i);
	LoadBitmap(hdc, &num, buf);
	FillBoxWithBitmap(hdc,
					time_left,
					(300 - num.bmHeight)/2+120-20,
					num.bmWidth,
					num.bmHeight,
					&num);
	UnloadBitmap(&num);

	SetTextColor(hdc, COLOR_lightwhite);
	SetBkMode(hdc, BM_TRANSPARENT);
	text_rect.left = 0;
	text_rect.top = (300 - text_height)/2+120+5+num_height-20;
	text_rect.right = g_rcScr.right;
	text_rect.bottom = text_rect.top + text_height;
	switch(g_time.weekday){
		case 1:
			sprintf(tmp, "%s", get_str_from_id(MONDAY_ID,text_buf));
			break;
		case 2:
			sprintf(tmp, "%s", get_str_from_id(TUESDAY_ID,text_buf));
			break;
		case 3:
			sprintf(tmp, "%s", get_str_from_id(WEDNESDAY_ID,text_buf));
			break;
		case 4:
			sprintf(tmp, "%s", get_str_from_id(THURSDAY_ID,text_buf));
			break;
		case 5:
			sprintf(tmp, "%s", get_str_from_id(FRIDAY_ID,text_buf));
			break;
		case 6:
			sprintf(tmp, "%s", get_str_from_id(SATURDAY_ID,text_buf));
			break;
		case 0:
		default:
			sprintf(tmp, "%s", get_str_from_id(SUNDAY_ID,text_buf));
			break;
	}
	sprintf(str1, "%s", get_str_from_id(MONTH_ID,text_buf));
	sprintf(str2, "%s", get_str_from_id(DAY_ID,text_buf));
	sprintf(buf, "%d%s%d%s %s", g_time.month,str1,g_time.day,str2, tmp);
	DrawText(hdc, buf, -1, &text_rect, DT_CENTER);

	sprintf(buf, "%s/%s", SYSTEM_RES_PATH, "dot.bmp");
	LoadBitmap(hdc, &num, buf);
	b_dot=!b_dot;
	if(b_dot){
		FillBoxWithBitmap(hdc,
						dot_left,
						(300 - num.bmHeight)/2+120-20,
						num.bmWidth,
						num.bmHeight,
						&num);
	}

	UnloadBitmap(&num);
}
void ReadRtcTimeEx(struct time_st* pRtc)
{
	int fd;
	int retval;
	struct rtc_time rtc_tm;

	fd = open ("/dev/rtc0", O_RDONLY);
	if (fd >= 0)
	{
		retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (retval >= 0)
		{
			pRtc->year   = rtc_tm.tm_year + 1900;
			pRtc->month  = rtc_tm.tm_mon + 1;
			pRtc->day    = rtc_tm.tm_mday;
			pRtc->weekday = rtc_tm.tm_wday;
			pRtc->hour    = rtc_tm.tm_hour;
			pRtc->minute  = rtc_tm.tm_min;
			pRtc->second = rtc_tm.tm_sec;
		}
		close(fd);
	}
}



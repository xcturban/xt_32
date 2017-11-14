#include <stdio.h>
#include <errno.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <common_lib/common_lib.h>

#include "system_info.h"
#include "lpconfig.h"
#include "missed_call.h"

#include "date.h"

extern struct time_st g_time;

#include "log.h"


#define LOG_FILE "/mnt/nand1-3/ini/log.txt"

static void log2file(char *log)
{
	FILE *fp;
	struct rtc_time tm;
	char date[128];
	int fd;
	char line[256] = {0};
	int ret;

	fp = fopen(LOG_FILE, "a");
	if (!fp)
		return;

	fd = open ("/dev/rtc0", O_RDONLY);
	if (fd < 0) {
		// SPON_LOG_ERR("=====>%s - open /dev/rtc0 faild:%s\n", __func__, strerror(errno));
		sprintf(date, "%04d/%02d/%02d - %02d:%02d", g_time.year, g_time.month, g_time.day, g_time.hour, g_time.minute);
	} else {
		ret = ioctl(fd, RTC_RD_TIME, &tm);
		if (ret < 0) {
			// SPON_LOG_ERR("=====>%s - ioctl RTC_RD_TIME faild:%s\n", __func__, strerror(errno));
			close(fd);
			// return -1;
			sprintf(date, "%04d/%02d/%02d - %02d:%02d", g_time.year, g_time.month, g_time.day, g_time.hour, g_time.minute);
		} else {
			sprintf(date, "%04d/%02d/%02d - %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
			close(fd);
		}
	}

	sprintf(line, "%s  %s", date, log);
	fputs(line, fp);

	fclose(fp);

}

void log_manager_init(struct log_manager *log_manager)
{
	pthread_mutex_init(&log_manager->mutex, NULL);
	log_manager->log2file = log2file;
}



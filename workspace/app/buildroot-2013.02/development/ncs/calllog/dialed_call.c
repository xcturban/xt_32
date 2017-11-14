#include "dialed_call.h"

#ifdef __NAS8532_DIALED_CALL_H__

extern struct time_st g_time;
int dialed_call_add(int target_id, int board_id)
{
	LpConfig *cfg;
	int total;
	char section[128];
	int fd;
	int ret;
	struct rtc_time tm;
	char date[128];
	int i;

	cfg = lp_config_new(INI_DIALED_CALL_FILE);
	if (cfg == NULL) {
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	total = lp_config_get_int(cfg, "dialed_call_total", "total", 0);
	if (total < 0) {
		lp_config_set_int(cfg, "dialed_call_total", "total", 0);
		total = 0;
	}

	if (total >= MAX_DIALED_CALL) {
		lp_config_destroy(cfg);
		dialed_call_del(1);
		dialed_call_add(target_id, board_id);
		return 0;
	}

	fd = open ("/dev/rtc0", O_RDONLY);
	if (fd < 0) {
		SPON_LOG_ERR("=====>%s - open /dev/rtc0 faild:%s\n", __func__, strerror(errno));
		sprintf(date, "%04d/%02d/%02d - %02d:%02d", g_time.year, g_time.month, g_time.day, g_time.hour, g_time.minute);
	} else {
		ret = ioctl(fd, RTC_RD_TIME, &tm);
		if (ret < 0) {
			SPON_LOG_ERR("=====>%s - ioctl RTC_RD_TIME faild:%s\n", __func__, strerror(errno));
			close(fd);
			return -1;
		}
		sprintf(date, "%04d/%02d/%02d - %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
		close(fd);
	}

	sprintf(section, "dialed_call_%d", total + 1);

	lp_config_set_int(cfg, "dialed_call_total", "total", total + 1);

	lp_config_set_int(cfg, section, "target_id", target_id);
	lp_config_set_int(cfg, section, "board_id", board_id);
	lp_config_set_string(cfg, section, "date", date);

EXIT:
	lp_config_sync(cfg);
	lp_config_destroy(cfg);

	return 0;
}

int dialed_call_del(int idx)
{
	LpConfig *cfg;
	int total;
	int i;
	char section[128];

	if (idx < 0)
		return 0;

	cfg = lp_config_new(INI_DIALED_CALL_FILE);
	if (cfg == NULL) {
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	total = lp_config_get_int(cfg, "dialed_call_total", "total", 0);

	if (total == 0)
		goto EXIT;

	sprintf(section, "dialed_call_%d", idx);
	lp_config_clean_section(cfg, section);

	for (i = idx + 1; i <= total; i++) {
		char *tmp1, *tmp2;
		int target_id, board_id;

		sprintf(section, "dialed_call_%d", i);
		target_id = lp_config_get_int(cfg, section, "target_id", 0);
		board_id = lp_config_get_int(cfg, section, "board_id", 0);
		tmp1 = tmp2 = NULL;
		tmp1 = lp_config_get_string(cfg, section, "date", NULL);
		if (tmp1 != NULL)
			tmp2 = strdup(tmp1);

		lp_config_clean_section(cfg, section);

		sprintf(section, "dialed_call_%d", i - 1);
		lp_config_set_int(cfg, section, "target_id", target_id);
		lp_config_set_int(cfg, section, "board_id", board_id);
		lp_config_set_string(cfg, section, "date", tmp2);

		if (tmp2)
			free(tmp2);
	}

	lp_config_set_int(cfg, "dialed_call_total", "total", total - 1);

	lp_config_sync(cfg);

EXIT:
	lp_config_destroy(cfg);

	return 0;
}

int dialed_call_count_get(void)
{
	LpConfig *cfg;
	int total;

	cfg = lp_config_new(INI_DIALED_CALL_FILE);
	if (cfg == NULL) {
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	total = lp_config_get_int(cfg, "dialed_call_total", "total", 0);

	lp_config_destroy(cfg);

	return total;
}

int dialed_call_clear(void)
{
	char cl[128] = {0};
	sprintf(cl, "echo > %s", INI_DIALED_CALL_FILE);
	system(cl);

	return 0;
}

int dialed_call_id_get(int idx, int *target_id, int *board_id)
{
	LpConfig *cfg;
	char section[128];
	int total;

	if (idx < 0) {
		*target_id = 0;
		*board_id = 0;
		return 0;
	}

	cfg = lp_config_new(INI_DIALED_CALL_FILE);
	if (cfg == NULL) {
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	total = lp_config_get_int(cfg, "dialed_call_total", "total", 0);
	if (idx > total) {
		*target_id = 0;
		*board_id = 0;
	} else {
		sprintf(section, "dialed_call_%d", idx);
		*target_id = lp_config_get_int(cfg, section, "target_id", 0);
		*board_id = lp_config_get_int(cfg, section, "board_id", 0);
	}

	SPON_LOG_INFO("=====>%s - target_id:%d board_id:%d\n", __func__, *target_id, *board_id);

	lp_config_destroy(cfg);

	return 0;
}
#endif

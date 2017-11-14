#include <signal.h>
#include <time.h>
#include <string.h>

#include "timer.h"

/* 定时器时间采用实时时间 */
#define CLOCKID CLOCK_REALTIME

/**
 * common_timer_create - 创建一个 POSIX per-process 定时器
 *
 * @id: 创建成功后返回的定时器ID
 * @its: 设置定时器的初始值和间隔值
 *		struct itimerspec {
 *			struct timespec it_value;
 *			struct timespec it_interval;
 *		}
 *		it_value - 定时器超时时间
 *		it_interval - 定时器超时后，如果设置了it_interval，会重新加载it_interval到it_value
 * @routine: 定时器超时后执行的回调函数
 * @arg: 传给回调函数的参数
 *
 * @RET: 成功返回0 失败返回-1 
 */
int common_timer_create(timer_t *id, struct itimerspec *its, void (*routine)(union sigval), void *arg)
{
	struct sigevent sev;
	int ret;

	memset(&sev, 0, sizeof(struct sigevent));

	sev.sigev_notify = SIGEV_THREAD; /* 定时器超时后的通知方式采用 SIGEV_THREAD */
	sev.sigev_notify_function = routine; /* 通知函数 */
	sev.sigev_value.sival_ptr = arg; /* 参数 */

	/* 创建定时器 */
	ret = timer_create(CLOCKID, &sev, id);
	if (ret < 0)
		return ret;

	/* 设置定时器的初始值和间隔值，并启动定时器 */
	return timer_settime(*id, 0, its, NULL);
}

/**
 * common_timer_delete - 删除定时器
 *
 * @id: 定时器ID
 *
 * @RET: 成功返回0 失败返回-1
 */
int common_timer_delete(timer_t id)
{
	return timer_delete(id);
}

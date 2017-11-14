#include <signal.h>
#include <time.h>
#include <string.h>

#include "timer.h"

/* ��ʱ��ʱ�����ʵʱʱ�� */
#define CLOCKID CLOCK_REALTIME

/**
 * common_timer_create - ����һ�� POSIX per-process ��ʱ��
 *
 * @id: �����ɹ��󷵻صĶ�ʱ��ID
 * @its: ���ö�ʱ���ĳ�ʼֵ�ͼ��ֵ
 *		struct itimerspec {
 *			struct timespec it_value;
 *			struct timespec it_interval;
 *		}
 *		it_value - ��ʱ����ʱʱ��
 *		it_interval - ��ʱ����ʱ�����������it_interval�������¼���it_interval��it_value
 * @routine: ��ʱ����ʱ��ִ�еĻص�����
 * @arg: �����ص������Ĳ���
 *
 * @RET: �ɹ�����0 ʧ�ܷ���-1 
 */
int common_timer_create(timer_t *id, struct itimerspec *its, void (*routine)(union sigval), void *arg)
{
	struct sigevent sev;
	int ret;

	memset(&sev, 0, sizeof(struct sigevent));

	sev.sigev_notify = SIGEV_THREAD; /* ��ʱ����ʱ���֪ͨ��ʽ���� SIGEV_THREAD */
	sev.sigev_notify_function = routine; /* ֪ͨ���� */
	sev.sigev_value.sival_ptr = arg; /* ���� */

	/* ������ʱ�� */
	ret = timer_create(CLOCKID, &sev, id);
	if (ret < 0)
		return ret;

	/* ���ö�ʱ���ĳ�ʼֵ�ͼ��ֵ����������ʱ�� */
	return timer_settime(*id, 0, its, NULL);
}

/**
 * common_timer_delete - ɾ����ʱ��
 *
 * @id: ��ʱ��ID
 *
 * @RET: �ɹ�����0 ʧ�ܷ���-1
 */
int common_timer_delete(timer_t id)
{
	return timer_delete(id);
}

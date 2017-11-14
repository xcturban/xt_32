#ifndef __TIMER_H__
#define __TIMER_H__

#include <signal.h>
#include <time.h>


int common_timer_create(timer_t *id, struct itimerspec *its, void (*routine)(union sigval), void *arg);
int common_timer_delete(timer_t id);

	
#endif

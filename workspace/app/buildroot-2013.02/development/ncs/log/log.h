#ifndef __LOG_H__
#define __LOG_H__

#include <pthread.h>

struct log_manager {
	pthread_mutex_t mutex;
	void (*log2file)(struct log_manager *log_manager, char *log);
};

void log_manager_init(struct log_manager *log_manager);

#endif

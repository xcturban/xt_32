#ifndef __BROADCAST_H__
#define __BROADCAST_H__

#include <pthread.h>
#include <common_lib/common_lib.h>
#include <ncs_event/ncs_event.h>

#include "mediastreamer2/msqueue.h"


struct broadcast {
    int samplerate;

    pthread_mutex_t mutex;
    MSBufferizer bufferizer;

    bool_t incoming_bc_start;
    bool_t outgoing_bc_start;

    char prev_multi_ip[20];

	bool_t ring; // 播放广播提示音标记
    bool_t offline_ring; // 脱机广播提示音

    void (*set_samplerate)(struct broadcast *bc, int samplerate);
    int (*get_samplerate)(struct broadcast *bc);
    bool_t (*bc_outgoing)(struct broadcast *bc);
    bool_t (*bc_incoming)(struct broadcast *bc);
    void (*bc_outgoing_start_flag_set)(struct broadcast *bc, bool_t flag);
    void (*bc_incoming_start_flag_set)(struct broadcast *bc, bool_t flag);
	void (*ncs_wav_broadcast_task_start)(struct broadcast *bc, char *multi_ip);
	void (*ncs_wav_broadcast_task_stop)(struct broadcast *bc);
	void (*ncs_realtime_broadcast_stop)(struct broadcast *bc);
	void (*ncs_realtime_broadcast_start)(struct broadcast *bc);
	int (*ncs_broadcast_outgoing_start)(struct broadcast *bc, char *file);
	int (*ncs_broadcast_outgoing_stop)(struct broadcast *bc, char *file);
	void (*ncs_term_broadcast_stop)(struct broadcast *bc);
	void (*ncs_global_broadcast_stop)(struct broadcast *bc);
	void (*ncs_area_broadcast_stop)(struct broadcast *bc);
	void (*ncs_file_broadcast_start)(struct broadcast *bc, char *file);
	void (*ncs_file_broadcast_stop)(struct broadcast *bc);
	void (*ncs_term_broadcast_request)(struct broadcast *bc, int *term_list, int term_list_len);
	void (*ncs_global_broadcast_request)(struct broadcast *bc);
	void (*ncs_area_broadcast_request)(struct broadcast *bc, int *area_list, int area_list_len);
	void (*ncs_offline_task_switch_send)(struct broadcast *bc, ncs_task_t t);
	bool_t (*ncs_file_broadcasting)(struct broadcast *bc);

};

struct broadcast *broadcast_malloc(void);
void broadcast_free(struct broadcast *bc);

#endif

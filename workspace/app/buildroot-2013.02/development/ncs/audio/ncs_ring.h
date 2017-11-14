#ifndef __NCS_RING_H__
#define __NCS_RING_H__

#include <stdint.h>

typedef enum {
	OUTGOING,
	INCOMING,
	TARGET_BUSY,
	TARGET_OFFLINE,
	TARGET_ONHOOK,
} ring_t;


void ncs_ring_start(ring_t t, uint16_t target_id);
void ncs_ring_stop(void);

#endif

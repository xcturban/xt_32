#include <stdio.h>
#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_video_state_event_data_pack(
		video_state_t state,
		uint8_t board_id,
		char *ip,
		uint16_t port,
		int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 4;
	uint8_t *p;
	int ip1, ip2, ip3, ip4;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;
	memset(evt_data, 0, len);
	p = evt_data->extra_data;

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x5c;
	evt_data->param1 = state;
	evt_data->param2 = board_id - 1;
	sscanf(ip, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	p[0] = ip1;
	p[1] = ip2;
	p[2] = ip3;
	p[3] = ip4;
	*((uint16_t *)(&evt_data->param4)) = port;

	return evt_data;
}

ncs_event_t *ncs_video_state_event_build(struct sockaddr *target_sockaddr, video_state_t state, uint8_t board_id, char *ip, uint16_t port)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_video_state_event_data_pack(state, board_id, ip, port, &evt_data_len);
	if (evt_data == NULL) {
		error_code = NCS_ERROR_MALLOC_FAILED;
		return NULL;
	}
	
	evt = (ncs_event_t *)malloc(sizeof(*evt));
	if (evt == NULL) {
		free(evt_data);
		error_code = NCS_ERROR_MALLOC_FAILED;
		return NULL;
	}

	memset(evt, 0, sizeof(*evt));

	evt->evt_data = evt_data;
	evt->evt_data_len = evt_data_len;
	evt->target_sockaddr = target_sockaddr;

	return evt;
}


#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_close_emergency_alarm_event_data_pack(int *evt_data_len, uint16_t id1, uint16_t id2)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 4;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;

	memset(evt_data, 0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x90;

	evt_data->param1 = 0xFF;
	evt_data->param2 = 5;
	
	*((uint16_t *)(&evt_data->extra_data[0])) = id1;
	*((uint16_t *)(&evt_data->extra_data[2])) = id2;

	return evt_data;
}

ncs_event_t *close_emergency_alarm_event_build(uint16_t id1, uint16_t id2)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_close_emergency_alarm_event_data_pack(&evt_data_len, id1, id2);
	if (evt_data == NULL) {
		error_code = NCS_ERROR_MALLOC_FAILED;
		return NULL;
	}

	evt = (ncs_event_t *)malloc(sizeof(*evt));
	if (evt == NULL)
	{
		free(evt_data);
		error_code = NCS_ERROR_MALLOC_FAILED;
		return NULL;
	}

	memset(evt, 0, sizeof(*evt));

	evt->evt_data = evt_data;
	evt->evt_data_len = evt_data_len;
	evt->target_sockaddr = &ncs->server_sockaddr;

	return evt;
}


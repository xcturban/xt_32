#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_call_transfer_event_data_pack(
		uint16_t target_id,
		int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 60;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;

	memset(evt_data, 0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x28;

	if (target_id == 0)
		evt_data->param1 = 0;
	else {
		evt_data->param1 = 1;
		*((uint16_t *)(&evt_data->param2)) = target_id;
	}

	return evt_data;
}

ncs_event_t *ncs_call_transfer_event_build(uint16_t target_id)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_call_transfer_event_data_pack(target_id, &evt_data_len);
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
	evt->target_sockaddr = &ncs->server_sockaddr;

	return evt;
}

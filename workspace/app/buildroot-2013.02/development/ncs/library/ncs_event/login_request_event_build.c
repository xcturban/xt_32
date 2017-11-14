#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_login_request_event_data_pack(int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 4;
	char *device_type = getenv("NCS_DEVICE_TYPE");

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;

	memset(evt_data, 0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x17;
	evt_data->param1 = device_type ? strtol(device_type, NULL, 16) : 0x0;
	evt_data->param2 = 0x28;

	if (ncs->boards_status > 0) {
		*((uint16_t *)(&evt_data->extra_data[0])) = ncs->boards_status;
		*((uint16_t *)(&evt_data->extra_data[2])) = ~ncs->boards_status;
	}

	return evt_data;
}

ncs_event_t *ncs_login_request_event_build(void)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_login_request_event_data_pack(&evt_data_len);
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
	/*
	if(server_type == MASTER_SERVER)
		evt->target_sockaddr = &ncs->server_master_sockaddr;
	else
		evt->target_sockaddr = &ncs->server_back_sockaddr;
	*/
	evt->target_sockaddr = &ncs->server_sockaddr;

	return evt;
}

ncs_event_t *ncs_login_request_event_buildEx(struct sockaddr *sockaddr)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_login_request_event_data_pack(&evt_data_len);
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
	/*
	if(server_type == MASTER_SERVER)
		evt->target_sockaddr = &ncs->server_master_sockaddr;
	else
		evt->target_sockaddr = &ncs->server_back_sockaddr;
	*/
	evt->target_sockaddr = sockaddr;

	return evt;
}

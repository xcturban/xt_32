#include <stdio.h>
#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_nat_event_data_pack(char *ip, 
		uint16_t port, 
		int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 6;
	uint8_t *p;
	int ip1, ip2, ip3, ip4;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;
	memset(evt_data, 0, len);
	p = evt_data->extra_data;

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x21;
	evt_data->param1 = 0;

	sscanf(ip, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	evt_data->param2 = ip1;
	evt_data->param3 = ip2;
	evt_data->param4 = ip3;
	evt_data->param5 = ip4;

	*((uint16_t *)(&p[4])) = port;

	return evt_data;
}

ncs_event_t *ncs_nat_event_build(struct sockaddr *target_sockaddr, char *ip, uint16_t port)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_nat_event_data_pack(ip, port, &evt_data_len);
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

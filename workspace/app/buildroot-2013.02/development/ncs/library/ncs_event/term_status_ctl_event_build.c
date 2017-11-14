#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_term_status_ctl_event_data_pack(int *evt_data_len, uint8_t ctl_type, uint8_t port, uint8_t port_status, uint16_t trigger_times)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;

	memset(evt_data, 0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x72;
	evt_data->param1 = ctl_type; // 控制类型
	evt_data->param2 = port; // 端口号，从0开始
	evt_data->param3 = port_status; // 端口状态
	*((uint16_t *)(&evt_data->param4)) = trigger_times; // 触发时间

	return evt_data;
}

ncs_event_t *ncs_term_status_ctl_event_build(struct sockaddr *target_sockaddr, uint8_t ctl_type, uint8_t port, uint8_t port_status, uint16_t trigger_times)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_term_status_ctl_event_data_pack(&evt_data_len, ctl_type, port, port_status, trigger_times);
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

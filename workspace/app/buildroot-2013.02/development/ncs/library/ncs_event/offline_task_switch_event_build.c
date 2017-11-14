#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_offline_task_switch_event_data_pack(int *evt_data_len, ncs_task_t t)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 18;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;
	memset(evt_data, 0x0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x2f;
	evt_data->param1 = t;
	evt_data->param2 = 0x00;
	evt_data->param3 = 234;
	evt_data->param4 = 0;
	evt_data->param5 = 0;

	SPON_LOG_INFO("\n____start boardcast___\n");
	
	evt_data->extra_data[0] = 0x00;
	evt_data->extra_data[1] = 0x00;
	evt_data->extra_data[2] = 0x00;
	evt_data->extra_data[3] = 0x00;
	evt_data->extra_data[4] = 0x00;
	evt_data->extra_data[5] = 0x10;
	evt_data->extra_data[6] = 0x00;
	evt_data->extra_data[7] = 0x10;
	evt_data->extra_data[8] = 0x00;
	evt_data->extra_data[9] = 0x80;
	evt_data->extra_data[10] = 0x00;
	evt_data->extra_data[11] = 0x00;
	evt_data->extra_data[12] = 0x00;
	evt_data->extra_data[13] = 0x00;
	
	evt_data->extra_data[14] = 234;
	evt_data->extra_data[15] = 0;
	evt_data->extra_data[16] = 0;
	evt_data->extra_data[17] = 16;
	
	ncs->bc_type = MULTICAST;
	
	return evt_data;
}

ncs_event_t *ncs_offline_task_switch_event_build(ncs_task_t t)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_offline_task_switch_event_data_pack(&evt_data_len, t);
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

	sockaddr_convert((struct sockaddr_in *)&ncs->data_multi_sockaddr, "234.0.0.16", 2046);
	sockaddr_convert((struct sockaddr_in *)&ncs->msg_multi_sockaddr, "234.0.0.254", 2046);

	if (t == IDLE_TASK_OLD)		//如果是0xff停止，则是把组播地址设置为234.0.0.16
    	sockaddr_convert((struct sockaddr_in *)&ncs->msg_multi_sockaddr, "234.0.0.16", 2046);
	
	evt->evt_data = evt_data;
	evt->evt_data_len = evt_data_len;
	evt->target_sockaddr = &ncs->msg_multi_sockaddr;

	return evt;
}

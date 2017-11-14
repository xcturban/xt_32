#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_term_broadcast_request_event_data_pack(int *term_list, int term_list_len, int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + 121; /* 目标扩展终端附带121字节 */
	uint8_t *p;
	int j;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	p = &evt_data->param2;

	*evt_data_len = len;
	memset(evt_data, 0, len);

	for (j = 0; j < term_list_len; j++) {
		if (term_list[j] > 0 && term_list[j] <= 999) {
			int step = term_list[j] / 8;
			int remain = term_list[j] % 8;

			if (remain == 0) {
				step--;
				p[step] |= (1 << 7);
			} else
				p[step] |= (1 << (remain-1));
		}
	}
	
	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x22;
	evt_data->param1 = 9; /* 目标终端扩展开始 */
	
	return evt_data;
}

ncs_event_t *ncs_term_broadcast_request_event_build(int *term_list, int term_list_len)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_term_broadcast_request_event_data_pack(term_list, term_list_len, &evt_data_len);
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

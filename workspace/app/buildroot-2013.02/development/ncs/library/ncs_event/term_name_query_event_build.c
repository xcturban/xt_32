#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"

static ncs_event_data_t *ncs_term_name_bulk_query_event_data_pack(
		uint16_t start,
		int count,
		int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + count * 2;
	uint16_t *p;
	int j;

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;

	memset(evt_data, 0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x65;
	evt_data->param1 = 0x02;
	evt_data->param4 = count;

	p = (uint16_t *)evt_data->extra_data;

	for (j = 0; j < count; j++)
		*p++ = start++;

	return evt_data;
}

ncs_event_t *ncs_term_name_bulk_query_event_build(uint16_t start, int count)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_term_name_bulk_query_event_data_pack(start, count, &evt_data_len);
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

static ncs_event_data_t *ncs_term_name_bulk_query_event_data_packExx(
    uint16_t start,
    int *evt_data_len)
{
    ncs_event_data_t *evt_data;
    int len = NCS_EVENT_DATA_HEADER_LEN + 4;
    uint16_t *p;
    int j;

    evt_data = (ncs_event_data_t *)malloc(len);
    if (evt_data == NULL)
        return NULL;

    *evt_data_len = len;

    memset(evt_data, 0, len);

    evt_data->id = ncs->id - 1;
    evt_data->msg = 0x65;
    evt_data->param1 = 0x01;
    *((uint16_t*)&evt_data->param2) = start;
    evt_data->param4 = 0;
    evt_data->param5 = 0;
    /**((uint32_t*)evt_data->extra_data) = 1000*servertype+start;*/

    return evt_data;
}

ncs_event_t *ncs_term_name_bulk_query_event_buildExx(uint16_t start)
{
    ncs_event_t *evt;
    ncs_event_data_t *evt_data;
    int evt_data_len;

    evt_data = ncs_term_name_bulk_query_event_data_packExx(start, &evt_data_len);
    if (evt_data == NULL)
    {
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
static ncs_event_data_t *ncs_term_name_bulk_query_event_data_packEx(
    uint16_t start,
    int servertype,
    int *evt_data_len)
{
    ncs_event_data_t *evt_data;
    int len = NCS_EVENT_DATA_HEADER_LEN + 4;
    uint16_t *p;
    int j;

    evt_data = (ncs_event_data_t *)malloc(len);
    if (evt_data == NULL)
        return NULL;

    *evt_data_len = len;

    memset(evt_data, 0, len);

    evt_data->id = ncs->id - 1;
    evt_data->msg = 0x65;
    evt_data->param1 = 0x05;
    *((uint16_t*)&evt_data->param2) = start;
    evt_data->param4 = 0;
    evt_data->param5 = 0;
    *((uint32_t*)evt_data->extra_data) = 1000*servertype+start;

    return evt_data;
}

ncs_event_t *ncs_term_name_bulk_query_event_buildEx(uint16_t start, int servertype)
{
    ncs_event_t *evt;
    ncs_event_data_t *evt_data;
    int evt_data_len;

    evt_data = ncs_term_name_bulk_query_event_data_packEx(start, servertype, &evt_data_len);
    if (evt_data == NULL)
    {
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

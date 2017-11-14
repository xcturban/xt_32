#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"
#include <common_lib/common_lib.h>

static ncs_event_data_t *ncs_realtime_wave_data_event_data_pack(
		uint8_t *wave_data,
		int wave_data_len,
		int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len;
	uint8_t adpcm[NCS_ADPCM_LEN];
	uint8_t *p_wave_data;

	if (ncs->bc_audio_encode == AUDIO_ENCODE_ADPCM_SPON) {
		int adpcmlen = FADPCM_Encode((short *)wave_data, adpcm, wave_data_len / 2);
		len = NCS_EVENT_DATA_HEADER_LEN + adpcmlen;
		wave_data_len = adpcmlen;
		p_wave_data = adpcm;
	} else {
		len = NCS_EVENT_DATA_HEADER_LEN + wave_data_len;
		p_wave_data = wave_data;
	}

	evt_data = (ncs_event_data_t *)malloc(len);
	if (evt_data == NULL)
		return NULL;

	*evt_data_len = len;

	memset(evt_data, 0, len);

	evt_data->id = ncs->id - 1;
	evt_data->msg = 0x45;
	*((uint16_t *)(&evt_data->param2)) = uint16_checksum_calculate((uint16_t *)p_wave_data, wave_data_len / 2);
	memcpy(evt_data->extra_data, p_wave_data, wave_data_len);

	return evt_data;
}

ncs_event_t *ncs_realtime_wave_data_event_build(uint8_t *wave_data, int wave_data_len)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_realtime_wave_data_event_data_pack(wave_data, wave_data_len, &evt_data_len);
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

	if (ncs->bc_type == MULTICAST)
		evt->target_sockaddr = &ncs->data_multi_sockaddr;
	else
		evt->target_sockaddr = &ncs->server_sockaddr;

	return evt;
}

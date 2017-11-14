#include <stdlib.h>

#include "private.h"
#include "ncs_event.h"
#include <common_lib/common_lib.h>

static ncs_event_data_t *ncs_call_wave_data_event_data_pack(
		uint8_t *wave_data,
		int wave_data_len,
		int *evt_data_len)
{
	ncs_event_data_t *evt_data;
	int len = NCS_EVENT_DATA_HEADER_LEN + wave_data_len;
	char *device_type = getenv("NCS_DEVICE_TYPE");
	uint16_t type = 0x0;
	uint8_t adpcm[NCS_ADPCM_LEN];
	uint8_t *p_wave_data;

	if (device_type)
		type = strtol(device_type, NULL, 16);

	if (ncs->talk_audio_encode == AUDIO_ENCODE_ADPCM_SPON) {
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
	evt_data->msg = 0x4e;

	if (type == 0x60 || type == 0x41 || type == 0x40 || type == 0x30) {
		int i;
		uint16_t *p = (uint16_t *)p_wave_data;
		uint16_t key = ((uint16_t)p[wave_data_len/2-1] << 5) |  //1��.������1������λ����Կ
                             ((uint16_t)p[wave_data_len/2-1] >> 11);

        for (i=wave_data_len/2-2; i>0; i--)
        	p[i] ^= p[i - 1] ^ key;    //2��.������1����������Կ

       	p[wave_data_len/2-1] ^= ((uint16_t)type << 8) | (~type & 0xFF);     //3��.��������1����(��Կ���豸����)
	}

	*((uint16_t *)(&evt_data->param2)) = uint16_checksum_calculate((uint16_t *)p_wave_data, wave_data_len / 2);
	memcpy(evt_data->extra_data, p_wave_data, wave_data_len);

	return evt_data;
}

ncs_event_t *ncs_call_wave_data_event_build(struct sockaddr *target_sockaddr, uint8_t *wave_data, int wave_data_len)
{
	ncs_event_t *evt;
	ncs_event_data_t *evt_data;
	int evt_data_len;

	evt_data = ncs_call_wave_data_event_data_pack(wave_data, wave_data_len, &evt_data_len);
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

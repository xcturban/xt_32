
#include <stdlib.h>
#include <stdio.h>

#include "warring_event_build.h"

ncs_event_t *PackProcWithBuf(char *pBufArray)
{
	int Len                    = 0;
	int i                      = 0;
	ncs_event_t *evt           = NULL;
	ncs_event_data_t *evt_data = NULL;

	evt_data = (ncs_event_data_t *)malloc(100*sizeof(char));
	M_ADD_WORD_PACK_BUF((char*)&evt_data->id, Len, ncs->id-1);
	for(i = 0; ; i++)
	{
		if(pBufArray[i] == 0xff)
		{
			break;
		}
		M_ADD_BYTE_PACK_BUF((char*)&evt_data->id, Len, pBufArray[i]);
	}

	evt = (ncs_event_t*)malloc(sizeof(ncs_event_t));
	evt->evt_data = evt_data;
	evt->evt_data_len = Len;
	evt->target_sockaddr = &ncs->server_master_sockaddr;
	return evt;
}



#ifndef _WARRING_EVENT_BUILD_H
#define _WARRING_EVENT_BUILD_H

#include "private.h"
#include "ncs_event.h"


#define M_ADD_BYTE_PACK_BUF(pData, Len, data)	*(pData+Len) = data; Len++
#define M_ADD_WORD_PACK_BUF(pData, Len, data)	M_ADD_BYTE_PACK_BUF(pData, Len, data&0xff); M_ADD_BYTE_PACK_BUF(pData, Len, data>>8)
#define M_ADD_DWORD_PACK_BUF(pData, Len, data)	M_ADD_WORD_PACK_BUF(pData, Len, data&0xffff); M_ADD_WORD_PACK_BUF(pData, Len, data>>16)

ncs_event_t *PackProcWithBuf(char *pBufArray);

#endif


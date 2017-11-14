#ifndef __NAS8532_DIALED_CALL_H__
#define __NAS8532_DIALED_CALL_H__

#include "common.h"

int dialed_call_add(int target_id, int board_id);
int dialed_call_del(int idx);
int dialed_call_count_get(void);
int dialed_call_clear(void);
int dialed_call_id_get(int idx, int *target_id, int *board_id);

#endif//__NAS8532_DIALED_CALL_H__

#ifndef __NAS8532_RECEIVED_CALL_H__
#define __NAS8532_RECEIVED_CALL_H__

#include "common.h"

int received_call_add(int target_id, int board_id);
int received_call_del(int idx);
int received_call_count_get(void);
int received_call_clear(void);
int received_call_id_get(int idx, int *target_id, int *board_id);

#endif//__NAS8532_RECEIVED_CALL_H__

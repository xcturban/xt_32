#ifndef __NAS8532_MISSED_CALL_H__
#define __NAS8532_MISSED_CALL_H__

#include "common.h"

int missed_call_add(int target_id, int board_id);
int missed_call_del(int idx);
int missed_call_count_get(void);
int missed_call_clear(void);
int missed_call_id_get(int idx, int *target_id, int *board_id);

#endif//__NAS8532_MISSED_CALL_H__

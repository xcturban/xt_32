#ifndef __NAS8532_CONFIG_H__
#define __NAS8532_CONFIG_H__

#include "lpconfig.h"
#include "audio.h"
#include "ncs.h"
#include "call.h"
#include "term.h"
#include "NAS8532.h"

char * terminal_config_get_password( LpConfig *lpconfig);
void terminal_config_set_password(char *password, void *private_data);
void* terminal_web_config_modify_monitor(void *arg);

#endif

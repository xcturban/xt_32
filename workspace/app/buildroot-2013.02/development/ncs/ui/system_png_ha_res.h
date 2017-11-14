
#ifndef _SYS_PNG_HA_RES_H
#define _SYS_PNG_HA_RES_H 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

#include "system_info.h"
#include "system_bmp_res.h"
#define PNG_HA_RES_SIZE 300
typedef struct
{
	bool_t flag;
	BITMAP bitmap;
	char pic_name[30];
}TYPE_PNG_RES_SL;

#endif

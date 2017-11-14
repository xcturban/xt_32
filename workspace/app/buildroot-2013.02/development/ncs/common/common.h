#include <stdio.h>
#include <errno.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "system_info.h"
#include "lpconfig.h"
#include "date.h"
#include "list.h"

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <common_lib/common_lib.h>
#include <ncs_event/ncs_event.h>




#define ncs_new(type)			(type*)malloc(sizeof(type))
#define ncs_clear(p,type)		do{memset(p,0,sizeof(type));}while(0)
#define NCS_PTR_CONVERT(type,arg)    (type *)(arg)

#define NAND_WP_CLOSE() do { \
    system("echo 0 > /proc/nand_wp"); \
} while (0)

#define NAND_WP_OPEN() do { \
    system("sync"); \
    system("echo 1 > /proc/nand_wp"); \
} while (0)


void common_list_head_init(struct list_head *head);
void common_list_append(struct list_head *new, struct list_head *head);
void common_list_add(struct list_head *new, struct list_head *head);
void common_list_del(struct list_head *entry);
int common_list_count(struct list_head *head);



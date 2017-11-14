#ifndef __FILE_H__
#define __FILE_H__

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <pthread.h>

#include <common_lib/common_lib.h>
#include "mediastreamer2/msqueue.h"
#include "mediastreamer2/msticker.h"
#include "ortp/str_utils.h"
#include "mediastreamer2/mstee.h"
#include "mediastreamer2/msfileplayer.h"
#include "wave_list.h"

typedef enum {
	FILE_STOP,
	FILE_PLAY,
	FILE_PAUSE,
} file_status_t;

typedef enum 
{
	NOT_FILE_SEL,
	FILE_SEL,
} file_sel_t;

typedef enum {
    WAVE,
    MP3,
    FILE_OTHER,
} file_type_t;

typedef enum {
    FILE_DIR_PLAY,
    FILE_DIR_BROADCAST,
    FILE_DIR_RECORD,
} file_dir_t;


typedef struct {

	char path[512];
	char name[128];
	int fsize;	//file size
	int time_sec;
	int hsize;	// wave header size
	int rate; // Sample rate (binary, in Hz)
	int nchannels; // Channel numbers (0x01 = mono, 0x02 = stereo)
	int bps ;	// Average Bytes Per Second
	int id;	// file id

	file_dir_t dir;
	file_type_t type;
	file_status_t status;
	file_sel_t file_sel_status;
    struct list_head list; /* 用于链接到文件链表中 */
} file_t;

typedef struct
{
	struct list_head file_player_list;
	struct list_head file_broadcast_list;
	bool_t flag_init_complete;
	file_status_t statue;
	char path[512]; //ｕｓｂ或ｆｔｐ路径
}TYPE_BROADCAST_FILE_PLAY;

#define SIZE_RCDSELIDBUF 100
typedef struct
{
	struct list_head file_rec_sel_list;
	int b_cur_cnt;
	int b_cur_play_id;
}TYPE_RCD_PLAY_CTRL;

void file_list_obj_set_play_path(TYPE_BROADCAST_FILE_PLAY *pobj, char *pDecPath);
char *file_list_obj_get_play_path(TYPE_BROADCAST_FILE_PLAY *pobj);
int file_list_count(struct list_head *head);
file_t *file_find_by_id(struct list_head *head,int id);
void file_list_update_sel_statue(struct list_head *head, int id, file_sel_t statue);
void file_list_destroy(struct list_head * head );
int file_wave_list_create(struct list_head * head);
void file_list_init(void);
void file_list_dst(void);
extern TYPE_BROADCAST_FILE_PLAY s_file_list_obj;

#endif

#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__
 

#include "NAS8532.h"
#include "common.h"

typedef enum {
	TASK_NONE = 0,
	TASK_SIP_CALL_INCOMING,               // sip��������
	TASK_SIP_CALL_OUTGOING,               // sipȥ������
	TASK_NCS_CALL_INCOMING,               // ��ͨ��������
	TASK_NCS_CALL_OUTGOING,               // ��ͨȥ������
	TASK_NCS_CALL_CHATTING,               // ��ͨȥ������
	TASK_NCS_FILE_PLAYING,                // �ļ���������
	TASK_NCS_FILE_BROADCAST_OUTGOING,     // �ļ��㲥����
	TASK_NCS_MONITOR_INCOMING,            // ����������
	TASK_NCS_MONITOR_OUTGOING,            // ��������
	TASK_NCS_BROADCAST_INCOMING,          // ���ܹ㲥����
	TASK_NCS_TERM_BROADCAST_OUTGOING,     // �ն˹㲥��������
	TASK_NCS_AREA_BROADCAST_OUTGOING,     // �����㲥��������
	TASK_NCS_ALL_AREA_BROADCAST_OUTGOING, // ȫ���㲥��������
	TASK_NCS_CALL_TRANSFER,               // ��ͨ����ת������
	TASK_NCS_CONFERENCE,                  // ��ͨ�����Խ�����
} task_t;

typedef enum{
	NCS_INCOMMING_CALL = 0 , 
	NCS_OUTGOING_CALL      , 
	NCS_CHATTING           , 
}call_status_t;

typedef struct {
	Uint32 window_bg_color;
	Uint32 status_bar_bg_color;
	Uint32 button_bg_color;
	Uint32 button_border_color;
	Uint32 title_bar_bg_color;

	int status_bar_height;
	int title_bar_height;

	int margin;
	int spacing;
	int step;
	int round_size;
	bool_t flagCreate;

	HWND main_hwnd; // �����ھ���

	task_t task; // ��ǰ����
	task_t prev_task; // �ϴ�����
	RECT 	frame;
	//add	
	struct list_head  bmp_res_list;
    //usb_file_list
	struct list_head  file_media_list;
	HDC hdc;

} system_info_t;

extern system_info_t system_info;
extern bool_t g_firstEnter ;
extern void *g_pWinPrivatePrt;
extern void *g_pWinPrivatePrtOld;
extern bool_t g_vol_change_win_flag;

typedef int(*pStatueFunc)(HWND, int, WPARAM, LPARAM);
extern pStatueFunc g_StauteFun;
extern pStatueFunc g_StauteFunOld;

int system_info_init(void);

task_t get_system_task(void);
void set_system_task(task_t task);
task_t get_system_prev_task(void);
//-----------------------------------------------------------------
void system_volume_inc(void);
void system_volume_dec(void);

void system_config_save(void);




#endif

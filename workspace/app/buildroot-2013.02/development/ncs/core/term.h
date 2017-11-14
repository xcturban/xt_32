#ifndef __TERM_H__
#define __TERM_H__

#include <stdint.h>
#include <sys/socket.h>
#include <pthread.h>
#include <common_lib/common_lib.h>
#include "client_watch_dog.h"

#include "ncs.h"
#include "call.h"
#include "audio.h"
#include "sip.h"
#include "broadcast.h"
#include "ncs_queue.h"

typedef enum
{
    ENGLISH = 0,
    CHINESE,
} language_t;


enum
{
    DIAL_NCS = 0,
    DIAL_SIP,
};

enum
{
    TERMINAL_BC = 0,
    AREA_BC,
    ALL_AREA_BC
};


enum
{
    CUSTOMER_CSLQ    = 0x11,
    CUSTOMER_JXWGS   = 0x12,
    CUSTOMER_ZTE     = 0x13,
    CUSTOMER_RUSSIAN = 0x14,
    CUSTOMER_GXGS    = 0x15,
    CUSTOMER_YYJY    = 0x16,
    CUSTOMER_GDZH    = 0x17,
    CUSTOMER_QGLW    = 0x18,
    CUSTOMER_LNJY    = 0x19,
    CUSTOMER_XMBR    = 0x1A,     //厦门BRT
};


struct web_time
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

typedef struct terminal
{

    language_t language;

    uint16_t customer_type; // 定制版类型
    uint16_t device_type; // 设备类型
	bool_t calllog_flag;
    bool_t video_type; // 视频类型(disable 0,enable 1)

	bool_t image_load;
    int dial_rapid; // 快捷拨号
	int bc_type;
    struct log_manager *log_manager;
    struct ncs_manager *ncs_manager;
    struct call_manager *call_manager;
    struct audio_manager *audio_manager;
    struct sip_manager *sip_manager;
	pthread_mutex_t gui_mutex; //界面锁
    pthread_mutex_t mutex; // 同步锁
	char version_info[6][20]; //版本信息
	int audio_data_packnum;	//接受音频数据包数


	ncs_watchdog_client_t* watchdog_client;

    int (*init)(struct terminal    *terminal);
    void (*uninit)(struct terminal *terminal);
	struct audio_output *ao;
	struct audio_output *ai;
	queue_t *queue;
} terminal_t;

enum
{
	ENUM_DEV_OUT_SPK,
	ENUM_DEV_OUT_HEARPHONE,
	ENUM_DEV_OUT_LINEOUT,
	ENUM_DEV_IN_LINEIN,
	ENUM_DEV_IN_MIC,
	ENUM_DEV_IN_MIHEAD,
};

typedef struct
{
	int AudioOutDev;
	int AudioInDev;
	uint8_t mutil_ip[30];
	bool_t fCloseBroadcast;
}TYPE_SYS_STATUE;

#define M_SET_AUDIO_OUT_DEV(bit, flag) SetBitAudioOutDev(&g_SysStatue, bit, flag)
#define M_SET_AUDIO_IN_DEV(bit,  flag) SetBitAudioInDev(&g_SysStatue, bit, flag)
#define M_GET_AUDIO_OUT_DEV(bit) GetBitAudioOutDev(&g_SysStatue, bit)
#define M_GET_AUDIO_IN_DEV(bit)  GetBitAudioInDev(&g_SysStatue, bit)

extern TYPE_SYS_STATUE g_SysStatue;
bool_t IsCurTaskBroadcast(void);
struct terminal *terminal_malloc(void);
bool_t GetSysStatueCloseBroadcastFlag(void);
void SetSysStatueCloseBroadcastFlag(bool_t flag);
bool_t GetBitAudioOutDev(TYPE_SYS_STATUE *pAudioOutDev, int DevBit);
void AudioOutDevSetReg(void);
void terminal_audio_packnum_clear(struct terminal *terminal);
void terminal_free(struct terminal *terminal);
void terminal_audio_packnum_handle(struct terminal *terminal);



extern terminal_t *term;


#endif

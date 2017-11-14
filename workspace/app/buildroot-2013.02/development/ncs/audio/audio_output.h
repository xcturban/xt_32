#ifndef __AUDIO_OUTPUT_H__
#define __AUDIO_OUTPUT_H__

#include <stdlib.h>
#include "common.h"

#define MAX_VOLUME_SIZE 15
#define MIN_VOLUME_SIZE 0
#define PCM_DATA_LEN	440
typedef enum {
	PCM_PLAYBACK,
	PCM_CAPTURE,
}audio_mode;

typedef enum{
	AUDIO_IN_SET,
	AUDIO_OUT_SET,
}audio_dir;

#define AUDIO_FUNCTION_BROADCAST			0
#define AUDIO_FUNCTION_TALK					1

#define AUDIO_IN_FROM_NONE			0 	//无
#define AUDIO_IN_FROM_CPU_OUT		1  	//远端cpu音频
#define AUDIO_IN_FROM_LOCAL_MIC		2	//本地mic // REMOVE
#define AUDIO_IN_FROM_EXT_MIC		4	//话筒杆mic
#define AUDIO_IN_FROM_BOARD_MIC		8	//面板mic //REMOVE
#define AUDIO_IN_FROM_LINE_MIC		16	//扩展线路
#define AUDIO_IN_FROM_HEADSET_MIC	32	//耳机mic


#define AUDIO_OUT_TO_NONE			0	//无
#define AUDIO_OUT_TO_LOCAL_SPK		1	//本地面板喇叭
#define AUDIO_OUT_TO_EXT_SPK		2  	//外部功放//LINE OUT
#define AUDIO_OUT_TO_BOARD_SPK		4  	//面板喇叭//REMOVE
#define AUDIO_OUT_TO_CPU_IN			8	//输出
#define AUDIO_OUT_TO_HEADSET_RECEIVER	16	//输出

typedef enum{
	AUDIO_CHANNEL_MONITOR_IN	= 0,
	AUDIO_CHANNEL_MONITOR_OUT	= 1,		
	AUDIO_CHANNEL_CALL_RING		= 2,
	AUDIO_CHANNEL_CALL_START	= 3,
	AUDIO_CHANNEL_BROADCAST_RING= 4,
	AUDIO_CHANNEL_BROADCAST_IN	= 5,	
	AUDIO_CHANNEL_BROADCAST_OUT	= 6,
	AUDIO_CHANNEL_FILE_PLAY		= 7,
	AUDIO_CHANNEL_IDLE			= 8,
	AUDIO_CHANNEL_DEFAULT 		= 0xffff,
}AUDIO_NCS_CHANNEL;


typedef enum {
	AUDIO_MONO_OUT_ON ,//		0
	AUDIO_MONO_OUT_OFF ,//		1
	AUDIO_HPO_OUT_ON ,//			2
	AUDIO_HPO_OUT_OFF ,//		3
	AUDIO_LINE_OUT_ON ,//		4
	AUDIO_LINE_OUT_OFF ,//		5
	AUDIO_SPK_OUT_ON ,//			7
	AUDIO_SPK_OUT_OFF ,//		8
}audio_out_type;

typedef enum{
	AUDIO_DSP_PATH_SET,
	AUDIO_DSP_REC_DISTANCE_SET,
	AUDIO_DSP_EC_RATE_SET,
}audio_dsp;


struct audio_output {
    char *device;
    int channels;
    int samplerate;
    int frame_len;
    audio_mode mode;
    int is_init;
    void *handle; 
    
    int (*open)(struct audio_output *);
	int (*close)(struct audio_output *);
	int (*process)(struct audio_output *, unsigned char *, int);
	int (*init)(struct audio_output *, audio_mode, int ,int , int );
};

typedef struct {
	char data[4] ;	/* "data" (ASCII characters) */
	int  len ;	/* length of data */
} __attribute__ ((packed)) chunk_t;

int audio_open(struct audio_output *ao, audio_mode mode, int channels,
		int samplerate, int frame_len);
int audio_close(struct audio_output *ao);
int audio_process(struct audio_output *ao, unsigned char *buf, int len);
int audio_streo_write(struct audio_output *ao,unsigned char *left,unsigned char *right,int len);

#endif

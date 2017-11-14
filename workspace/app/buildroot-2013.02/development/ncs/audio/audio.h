#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include <ncs_event/ncs_event.h>


#include "mediastreamer2/msqueue.h"
#include "mediastreamer2/mediastream.h"
#include "mediastreamer2/mssndcard.h"
#include "mediastreamer2/msaudiomixer.h"
#include "mediastreamer2/mseventqueue.h"

#include "ncs_audio_filter.h"

#include "audio_output.h"

#define NR_MEMBERS (16)


#define NCS_MAX_VOL_VALUE		15
#define NCS_MIN_VOL_VALUE		0
#define AUDIO_PLAY_ONCE			1
#define AUDIO_PLAY_LOOP			0xffff

#define BIT(x)        (1UL<<((x) & (BITS_PER_LONG - 1)))
#define BITS_PER_LONG (sizeof(long) * 8)
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

struct conference_member
{
    MSFilter *receiver;
    MSFilter *sender;
    struct sockaddr *target_sockaddr;
    int mixer_pin;
};

struct audio_stream
{
    MSTicker *ticker;
    MSFilter *soundread;
    MSFilter *soundwrite;
    MSFilter *ec;
    MSFilter *mixer;
    MSFilter *receiver;
    MSFilter *sender;
    struct conference_member *member[NR_MEMBERS];
};

typedef enum
{
    RECEIVER_INPUT,
    MIC_INPUT,
    LINE_INPUT,
} audio_input_channel_t;

typedef enum
{
    RECEIVER_OUTPUT,
    SPK_OUTPUT,
    LINE_OUTPUT,
} audio_output_channel_t;

typedef enum
{
    WM8960, // 旧板（两层板）
    RT3261, // 新板（四层板）
} codec_type_t;

typedef struct
{
	uint8_t reg;
	uint16_t val;
}rt3261_init_reg;

typedef enum
{
    AUDIO_READ,
    AUDIO_WRITE,
    AUDIO_BOTH,
} audio_dir_t;


#ifdef __NCS_AUDIO_THREAD_UPDATE__
typedef enum
{
    TS_IDLE = 0,
    TS_LOCAL_BROADCAST,
    TS_BROADCAST,
    TS_TALK,
    TS_MONITOR,
    TS_LOCAL_MONITOR,
    TS_RING,
    TS_PLAYIP,
    TS_ALARM,
    TS_BUSY,
    TS_FILE_PLAY,
    TS_OFFLINE_RING_PLAY,
    TS_ONHOOK_RING_PLAY,
    TS_MAX,
} audio_mode_enum_t;


typedef enum
{
    RECORD_IDLE = 0,
    RECORD_MONITOR,
    RECORD_TALK,
    RECORD_LOCAL_BROADCAST,
} audio_record_enum_t;

#endif


struct audio_manager
{
    codec_type_t codec;

    int talk_in_volume;
    int talk_out_volume;
    int bc_in_volume;
    int bc_out_volume;
    int ring_volume;
    int system_volume;

    int orig_input_channel;
    int orig_output_channel;

    int input_channel; // 音频输入通道
    int output_channel; // 音频输出通道

    audio_input_channel_t orig_in_chn;
    audio_output_channel_t orig_out_chn;

    audio_input_channel_t in_chn;
    audio_output_channel_t out_chn;

    struct audio_stream *stream;
    RingStream *ring; // 循环振铃音
    RingStream *bc_ring; // 单次广播提示音
    RingStream *file_ring; // 单次文件播放

    MSEventQueue *evt_queue; // 用于文件播放完了后回调

    int (*init)(struct audio_manager *audio_manager);
    int (*reinit)(struct audio_manager *audio_manager);
    int (*set_ring_volume)(struct audio_manager *audio_manager, int volume);
    int (*set_system_volume)(struct audio_manager *audio_manager, int volume);
    int (*set_talk_in_volume)(struct audio_manager *audio_manager, int volume);
    int (*get_talk_in_volume)(struct audio_manager *audio_manager);
    int (*set_talk_out_volume)(struct audio_manager *audio_manager, int volume);
    int (*set_bc_in_volume)(struct audio_manager *audio_manager, int volume);
    int (*set_bc_out_volume)(struct audio_manager *audio_manager, int volume);
    int (*get_talk_out_volume)(struct audio_manager *audio_manager);
    int (*set_talk_mode)(struct audio_manager *audio_manager);
    int (*set_bc_mode)(struct audio_manager *audio_manager);
    void (*set_rt3261reg)(struct audio_manager *audio_manager, uint8_t reg, uint16_t val);
    void (*set_rt3261regEx)(struct audio_manager *audio_manager, uint8_t reg, uint8_t bit, bool_t val);
    int (*get_codec)(struct audio_manager *audio_manager);
    int (*stream_create)(struct audio_manager *audio_manager, audio_dir_t dir, int samplerate, pthread_mutex_t *mutex, MSBufferizer *bufferizer, struct sockaddr *target_sockaddr);
    int (*stream_destroy)(struct audio_manager *audio_manager, audio_dir_t dir);
    int (*conference_create)(struct audio_manager *audio_manager, int samplerate);
    int (*conference_destroy)(struct audio_manager *audio_manager);
    int (*conference_member_add)(struct audio_manager *audio_manager, pthread_mutex_t *mutex, MSBufferizer *bufferizer, struct sockaddr *target_sockaddr);
    int (*conference_member_remove)(struct audio_manager *audio_manager, struct sockaddr *target_sockaddr);
    int (*ring_start)(struct audio_manager *audio_manager, char *file, int interval);
    int (*ring_stop)(struct audio_manager *audio_manager);
    int (*bc_ring_start)(struct audio_manager *audio_manager, char *file, MSFilterNotifyFunc func, void * user_data);
    int (*bc_ring_stop)(struct audio_manager *audio_manager);
    void (*audio_switch)(struct audio_manager *audio_manager, bool_t force_switch);
    audio_input_channel_t (*audio_input_channel_get)(struct audio_manager *audio_manager);
    audio_output_channel_t (*audio_output_channel_get)(struct audio_manager *audio_manager);
    void (*audio_input_mute)(struct audio_manager *audio_manager);
    void (*audio_input_unmute)(struct audio_manager *audio_manager);
    int (*file_play_start)(struct audio_manager *audio_manager, char *file);
    int (*file_play_stop)(struct audio_manager *audio_manager);
    bool_t (*file_playing)(struct audio_manager *audio_manager);
    void (*mic_set)(struct audio_manager *audio_manager, bool_t flag);
};

struct audio_manager *audio_manager_malloc(void);
void audio_free(struct audio_manager *audio_manager);

#endif

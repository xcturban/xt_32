#ifndef __NCS_AUDIO_FILTER_H__
#define __NCS_AUDIO_FILTER_H__

#include "mediastreamer2/msfilter.h"

#define MS_FILTER_SET_RECEIVER_MUTEX            MS_FILTER_BASE_METHOD(202, int)
#define MS_FILTER_SET_RECEIVER_BUFFERIZER       MS_FILTER_BASE_METHOD(203, int)
#define MS_FILTER_SET_SENDER_ENCODE 		    MS_FILTER_BASE_METHOD(204, int)
#define MS_FILTER_SET_SENDER_TARGET_SOCKADDR	MS_FILTER_BASE_METHOD(205, int)

extern MSFilterDesc ncs_audio_receiver_desc;
extern MSFilterDesc ncs_audio_sender_desc;

#endif

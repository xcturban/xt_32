
#ifndef _AUDIO_LOCAL_H 
#define _AUDIO_LOCAL_H 

#include "alsa/asoundlib.h" 
#include "audio_output.h"

#define NCS_MAX_VOL_VALUE		15
#define NCS_MIN_VOL_VALUE		0

//#define PT2258_MAX_VOL_VALUE	79
//#define PT2258_MIN_VOL_VALUE	34
//#define VOL_STEP_INC	((PT2258_MAX_VOL_VALUE-PT2258_MIN_VOL_VALUE)/NCS_MAX_VOL_VALUE)


#define RT3261_DSP_DISTANCE_NEAR 		0
#define RT3261_DSP_DISTANCE_MID 		1
#define RT3261_DSP_DISTANCE_FAR 		2

//int ncs_vol_to_pt2258_vol(int vol);
//int pt2258_vol_to_ncs_vol(int vol);

int audio_path_get(int dir);
int audio_volume_get(int dir);
void audio_path_set(int dir,int param);
void audio_volume_set(int dir,int param);
void audio_dsp_set(int dir,int param);
struct audio_output * alsa_audio_output_malloc(void);
void alsa_audio_output_free(struct audio_output * ao);
void audio_channel_mode_set(int mode);
#endif


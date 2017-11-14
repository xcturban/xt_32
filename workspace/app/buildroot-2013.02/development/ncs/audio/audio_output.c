#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "gpio.h"
#include "audio_output.h"


int audio_open(struct audio_output *ao, audio_mode mode, int channels,
		int samplerate, int frame_len)
{
	SPON_LOG_INFO("mode = %d, channels=%d, samplerate=%d, frame_len=%d\n", mode, channels, samplerate,frame_len);
	ao->init(ao, mode, channels,samplerate, frame_len);
	return ao->open(ao);
}

int audio_close(struct audio_output *ao)
{
	return ao->close(ao);
}


int audio_process(struct audio_output *ao, unsigned char *buf, int len)
{
	return ao->process(ao, buf, len);
}

void mono_to_stereo(unsigned char *left,unsigned char *right,
unsigned char *stereo,int len){

	unsigned short * l = (unsigned short * )left;
	unsigned short * r = (unsigned short * )right;
	unsigned short * o = (unsigned short * )stereo;
	
	int i = 0;
	for(i=0;i<len/2;i++){
		*(o + i*2) = *(l + i);
		*(o + i*2 + 1) = *(r + i);
	}
}

int audio_streo_write(struct audio_output *ao,
		unsigned char *left,unsigned char *right,
		int len)
{
	
	if (ao->channels == 1)
		return ao->process(ao, left, len/2);

	unsigned char * stereo = malloc(len * 2);
	
	memset(stereo,0,len * 2);
	mono_to_stereo(left,right,stereo,len);

	ao->process(ao, stereo, len);
	free(stereo);

	return 0;
}


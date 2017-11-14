
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "audio_output.h"
#include "audio_local.h"
//#include "gpio.h"
#include "common.h"


//CHOOSE 1  ALSA_METHOD_FROM_MEDIASTREAMER
//CHOOSE 0  ALSA_METHOD_FROM_NCS3091

//#define DEBUG_ENTER()  do{printf("=======>Enter %s-%d\n",__func__,__LINE__);}while(0)
#define DEBUG_ENTER()

/*in case of troubles with a particular driver, try incrementing ALSA_PERIOD_SIZE
to 512, 1024, 2048, 4096...
then try incrementing the number of periods*/

#define ALSA_PERIODS 		4
#define ALSA_PERIOD_SIZE 	256 //512

/*uncomment the following line if you have problems with an alsa driver
having sound quality trouble:*/
/*#define EPIPE_BUGFIX 1*/
static void alsa_resume(snd_pcm_t *handle)
{
    DEBUG_ENTER();
    int err;
    snd_pcm_status_t *status=NULL;

    snd_pcm_status_alloca(&status);

    if ((err=snd_pcm_status(handle,status))!=0)
    {
        SPON_LOG_ERR("snd_pcm_status() failed: %s\n",snd_strerror(err));
        return;
    }

	if (snd_pcm_status_get_state(status)==SND_PCM_STATE_SUSPENDED){
		SPON_LOG_INFO("Maybe suspended, trying resume\n");
		if ((err=snd_pcm_resume(handle))!=0){
			if (err!=EWOULDBLOCK) SPON_LOG_ERR("snd_pcm_resume() failed: %s\n",snd_strerror(err));
		}
	}
}

static int alsa_set_params(snd_pcm_t *pcm_handle, int rw, int bits, int stereo, int rate)
{
    DEBUG_ENTER();
    snd_pcm_hw_params_t *hwparams=NULL;
    snd_pcm_sw_params_t *swparams=NULL;
    int dir;
    uint exact_uvalue;
    unsigned long exact_ulvalue;
    int channels;
    int periods	= ALSA_PERIODS;
    int periodsize = ALSA_PERIOD_SIZE;
    snd_pcm_uframes_t buffersize;
    int err;
    int format;

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Cannot configure this PCM device.\n");
        return -1;
    }

    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting access.\n");
        return -1;
    }
    /* Set sample format */
    format=SND_PCM_FORMAT_S16;
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, format) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting format.\n");
        return -1;
    }
    /* Set number of channels */
    if (stereo) channels=2;
    else channels=1;
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting channels.\n");
        return -1;
    }
    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    exact_uvalue=rate;
    dir=0;
    if ((err=snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_uvalue, &dir))<0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting rate to %i:%s\n",rate,snd_strerror(err));
        return -1;
    }
    if (dir != 0)
    {
        SPON_LOG_ERR("alsa_set_params: The rate %d Hz is not supported by your hardware.\n "
                     "==> Using %d Hz instead.\n", rate, exact_uvalue);
    }
    /* choose greater period size when rate is high */
    /*periodsize=periodsize*(rate/8000);*/

    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
    /* set period size */
    exact_ulvalue=periodsize;
    dir=0;
    if (snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, &exact_ulvalue, &dir) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting period size.\n");
        return -1;
    }

    if (dir != 0)
    {
        /*SPON_LOG_INFO("alsa_set_params: The period size %d is not supported by your hardware.\n "
        "==> Using %d instead.", periodsize, (int)exact_ulvalue);*/
    }
    periodsize=exact_ulvalue;
    /* Set number of periods. Periods used to be called fragments. */
    exact_uvalue=periods;
    dir=0;
    if (snd_pcm_hw_params_set_periods_near(pcm_handle, hwparams, &exact_uvalue, &dir) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting periods.\n");
        return -1;
    }
    if (dir != 0)
    {
        SPON_LOG_INFO("alsa_set_params: The number of periods %d is not supported by your hardware.\n "
                      "==> Using %d instead.\n", periods, exact_uvalue);
    }
    periods=exact_ulvalue;

    if (snd_pcm_hw_params_get_buffer_size(hwparams, &buffersize)<0)
    {
        buffersize=0;
        SPON_LOG_INFO("alsa_set_params: could not obtain hw buffer size.\n");
    }
    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if ((err=snd_pcm_hw_params(pcm_handle, hwparams)) < 0)
    {
        SPON_LOG_ERR("alsa_set_params: Error setting HW params:%s\n",snd_strerror(err));
        return -1;
    }
    if (snd_pcm_hw_params_get_buffer_size(hwparams, &buffersize)<0)
    {
        buffersize=0;
        SPON_LOG_INFO("alsa_set_params: could not obtain hw buffer size.\n");
    }
    /*prepare sw params */
    if (rw)
    {
        snd_pcm_sw_params_alloca(&swparams);
        snd_pcm_sw_params_current(pcm_handle, swparams);

        if ((err=snd_pcm_sw_params_set_start_threshold(pcm_handle, swparams,periodsize*2))<0)
        {
            SPON_LOG_ERR("alsa_set_params: Error setting start threshold:%s\n",snd_strerror(err));
        }
        if ((err=snd_pcm_sw_params_set_stop_threshold(pcm_handle, swparams,periodsize*periods))<0)
        {
            SPON_LOG_ERR("alsa_set_params: Error setting stop threshold:%s\n",snd_strerror(err));
        }
        if ((err=snd_pcm_sw_params(pcm_handle, swparams))<0)
        {
            SPON_LOG_ERR("alsa_set_params: Error setting SW params:%s\n",snd_strerror(err));
            return -1;
        }
    }
    return 0;
}

#ifdef EPIPE_BUGFIX
static void alsa_fill_w (snd_pcm_t *pcm_handle)
{
    snd_pcm_hw_params_t *hwparams=NULL;
    int channels;
    snd_pcm_uframes_t buffer_size;
    int buffer_size_bytes;
    void *buffer;

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_hw_params_current(pcm_handle, hwparams);

    /* get channels */
    snd_pcm_hw_params_get_channels (hwparams, &channels);

    /* get buffer size */
    snd_pcm_hw_params_get_buffer_size (hwparams, &buffer_size);

    /* fill half */
    buffer_size /= 2;

    /* allocate buffer assuming 2 bytes per sample */
    buffer_size_bytes = buffer_size * channels * 2;
    buffer = alloca (buffer_size_bytes);
    memset (buffer, 0, buffer_size_bytes);

    /* write data */
    snd_pcm_writei(pcm_handle, buffer, buffer_size);
}
#endif


static void alsa_close_pcm(snd_pcm_t * handle)
{
    DEBUG_ENTER();
    if (handle!=NULL) snd_pcm_close(handle);
}

static snd_pcm_t* alsa_open_r(const char *pcmdev,int bits,int stereo,int rate)
{
    DEBUG_ENTER();
    snd_pcm_t *pcm_handle;
    int err;

    SPON_LOG_INFO("alsa_open_r: opening %s at %iHz, bits=%i, stereo=%i\n",pcmdev,rate,bits,stereo);


    /* want blocking mode for threaded version */
    if (snd_pcm_open(&pcm_handle, pcmdev,SND_PCM_STREAM_CAPTURE,0) < 0)
    {
        SPON_LOG_ERR("alsa_open_r: Error opening PCM device %s\n",pcmdev );
        return NULL;
    }
    {
        struct timeval tv1;
        struct timeval tv2;
        struct timezone tz;
        int diff = 0;
        err = gettimeofday(&tv1, &tz);
        while (1)
        {
            if (!(alsa_set_params(pcm_handle,0,bits,stereo,rate)<0))
            {
                SPON_LOG_ERR("alsa_open_r: Audio params set\n");
                break;
            }
            if (!gettimeofday(&tv2, &tz) && !err)
            {
                diff = ((tv2.tv_sec - tv1.tv_sec) * 1000000) + (tv2.tv_usec - tv1.tv_usec);
            }
            else
            {
                diff = -1;
            }
            if ((diff < 0) || (diff > 3000000))   /* 3 secondes */
            {
                SPON_LOG_ERR("alsa_open_r: Error setting params for more than 3 seconds\n");
                snd_pcm_close(pcm_handle);
                return NULL;
            }
            SPON_LOG_INFO("alsa_open_r: Error setting params (for %d micros)\n", diff);
            usleep(200000);
        }
    }

    err=snd_pcm_start(pcm_handle);
    if (err<0)
    {
        SPON_LOG_INFO("snd_pcm_start() failed: %s\n", snd_strerror(err));
    }
    return pcm_handle;
}

static snd_pcm_t * alsa_open_w(const char *pcmdev,int bits,int stereo,int rate)
{

    snd_pcm_t *pcm_handle;
//	SPON_LOG_INFO("alsa_open_w: opening %s at %iHz, bits=%i, stereo=%i",pcmdev,rate,bits,stereo);
    if (snd_pcm_open(&pcm_handle, pcmdev,SND_PCM_STREAM_PLAYBACK,0) < 0)
    {
        SPON_LOG_ERR("alsa_open_w: Error opening PCM device %s\n",pcmdev );
        return NULL;
    }
    alsa_resume(pcm_handle);
    {
        struct timeval tv1;
        struct timeval tv2;
        struct timezone tz;
        int diff = 0;
        int err;
        err = gettimeofday(&tv1, &tz);
        while (1)
        {
            if (!(alsa_set_params(pcm_handle,1,bits,stereo,rate)<0))
            {
//				SPON_LOG_INFO("alsa_open_w: Audio params set");
                break;
            }
            if (!gettimeofday(&tv2, &tz) && !err)
            {
                diff = ((tv2.tv_sec - tv1.tv_sec) * 1000000) + (tv2.tv_usec - tv1.tv_usec);
            }
            else
            {
                diff = -1;
            }
            if ((diff < 0) || (diff > 3000000))   /* 3 secondes */
            {
                SPON_LOG_ERR("alsa_open_w: Error setting params for more than 3 seconds\n");
                snd_pcm_close(pcm_handle);
                return NULL;
            }
            SPON_LOG_INFO("alsa_open_w: Error setting params (for %d micros)\n", diff);
            usleep(200000);
        }
    }

    return pcm_handle;
}

static int alsa_read(snd_pcm_t *handle,unsigned char *buf,int nsamples)
{
    int err;
    err=snd_pcm_readi(handle,buf,nsamples);
    if (err<0)
    {
        //SPON_LOG_INFO("alsa_read: snd_pcm_readi() returned %i",err);
        if (err==-EPIPE)
        {
            snd_pcm_prepare(handle);
            err=snd_pcm_readi(handle,buf,nsamples);
            if (err<0) SPON_LOG_ERR("alsa_read: snd_pcm_readi() failed:%s.\n",snd_strerror(err));
        }
        else if (err==-ESTRPIPE)
        {
            alsa_resume(handle);
        }
        else if (err!=-EWOULDBLOCK)
        {
            SPON_LOG_ERR("alsa_read: snd_pcm_readi() failed:%s.\n",snd_strerror(err));
        }
    }
    else if (err==0)
    {
        SPON_LOG_ERR("alsa_read: snd_pcm_readi() returned 0\n");
    }
    return err;
}


static void alsa_fill_zero_w (snd_pcm_t *pcm_handle)
{
    snd_pcm_hw_params_t *hwparams=NULL;
    int channels;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_uframes_t period_size;
    snd_pcm_uframes_t fill_size;
    int buffer_size_bytes;
    void *buffer;
    int err;

    snd_pcm_status_t *status;
    snd_pcm_status_alloca(&status);

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_hw_params_current(pcm_handle, hwparams);

    /* get channels */
    snd_pcm_hw_params_get_channels (hwparams, &channels);

    /* get buffer size */
    snd_pcm_hw_params_get_buffer_size (hwparams, &buffer_size);

    snd_pcm_hw_params_get_period_size (hwparams, &period_size, NULL);

    if ((err = snd_pcm_status(pcm_handle, status))<0)
    {
        SPON_LOG_ERR("snd_pcm_status() failed:%s.\n",snd_strerror(err));
        return;
    }

    fill_size = buffer_size - snd_pcm_status_get_avail(status);
    if (fill_size < period_size)
    {
        SPON_LOG_ERR("fill_size=%lu period_size=%lu, buffer_size=%lu\n",fill_size, period_size, buffer_size);
        buffer = alloca (period_size * channels * 2);
        memset (buffer, 0, period_size * channels * 2);

        /* write data */
        snd_pcm_writei(pcm_handle, buffer, period_size * channels * 2);
    }
}


static int alsa_write(snd_pcm_t *handle, unsigned char *buf, int nsamples)
{
    int err;
#if 0
    snd_pcm_status_t *status;
    snd_pcm_status_alloca(&status);
    if ((err = snd_pcm_status(handle, status))<0)
        printf("snd_pcm_status() failed:%s.",snd_strerror(err));
    printf("snd_pcm_status_get_avail:%lu\n", snd_pcm_status_get_avail(status));
#endif
//	printf("### alsa_write sample:%d \n",nsamples);
//	alsa_fill_zero_w(handle);
    if ((err=snd_pcm_writei(handle,buf,nsamples))<0)
    {
        if (err==-EPIPE)
        {
            snd_pcm_prepare(handle);
#ifdef EPIPE_BUGFIX
            alsa_fill_w (handle);
#endif
            err=snd_pcm_writei(handle,buf,nsamples);
            if (err<0) SPON_LOG_ERR("alsa_card_write: Error writing sound buffer (nsamples=%i):%s\n",nsamples,snd_strerror(err));
        }
        else if (err==-ESTRPIPE)
        {
            SPON_LOG_ERR("ESTRPIPE\n");
            alsa_resume(handle);
        }
        else if (err!=-EWOULDBLOCK)
        {
            SPON_LOG_ERR("alsa_card_write: snd_pcm_writei() failed:%s.\n",snd_strerror(err));
        }
    }
    else if (err!=nsamples)
    {
        //SPON_LOG_ERR("Only %i samples written instead of %i\n",err,nsamples);
    }
    return err;
}


static int alsa_audio_process(struct audio_output *ao,
                              unsigned char *buf,int len)
{
    if(!ao->is_init)
    {
        SPON_LOG_ERR("ao is not inited \n");
        return -1;
    }
    switch(ao->mode)
    {
    case PCM_PLAYBACK:
        if(ao->handle)
            return alsa_write(ao->handle,buf,len/(2*ao->channels));
        break;
    case PCM_CAPTURE:
        if(ao->handle)
            return alsa_read(ao->handle,buf,len/(2*ao->channels));
        break;
    default:
        return -1;
        break;
    }
    return -1;
}

static int alsa_audio_close(struct audio_output *ao)
{
    if(ao->is_init)
    {
        alsa_close_pcm(ao->handle);
    }
    ao->handle = NULL;
    ao->is_init = 0;
    return 0;
}

static int alsa_audio_init(struct audio_output *ao, audio_mode mode, int channels,
                           int samplerate, int frame_len)
{

    ao->channels = channels;
    ao->samplerate = samplerate;
    ao->frame_len = frame_len;
    ao->mode = mode;
    ao->handle = NULL;
    ao->is_init = 0;
}

static int alsa_audio_open(struct audio_output *ao)
{
    //每次打开前关闭当前音频
    alsa_audio_close(ao);
    switch (ao->mode)
    {
		case PCM_PLAYBACK:
			ao->handle = alsa_open_w("plughw:0,0",16,ao->channels==2,ao->samplerate);
			break;
		case PCM_CAPTURE:
			ao->handle = alsa_open_r("plughw:1,0",16,ao->channels==2,ao->samplerate);
			break;
    }
    if (!ao->handle)
    {
        ao->is_init = 0;
        return -1;
    }
    ao->is_init = 1;
    return 0;
}



void rt3261_audio_dsp_ctl(int ctl)
{
    static int audio_dsp_ctl = -1;
    if(audio_dsp_ctl == ctl)
        return;
    else
        audio_dsp_ctl = ctl;
    SPON_LOG_INFO("=====>%s ctl:%d\n", __func__,ctl);
    char cmd[10];
    switch(ctl)
    {
    case RT3261_DSP_DISTANCE_NEAR :
    case RT3261_DSP_DISTANCE_MID :
    case RT3261_DSP_DISTANCE_FAR :
        memset(cmd,0,10);
        sprintf(cmd,"%d ",ctl);
        system_echo(cmd,"/proc/dsp_ctl");
        break;
    default:
        break;
    }
}

void rt3261_audio_dsp_ec_rate(int rate)
{
    static int audio_dsp_ec_rate = -1;
    if(audio_dsp_ec_rate == rate)
        return;
    else
        audio_dsp_ec_rate = rate;

    char cmd[10];
    memset(cmd,0,10);
    sprintf(cmd,"%d ",rate);
    SPON_LOG_INFO("=====>%s rate:%d\n", __func__,rate);
    system_echo(cmd,"/proc/dsp_ec_rate");
}

static int audio_dsp_sel = -1;
void rt3261_audio_dsp_sel(int sel)
{
    //SPON_LOG_INFO("=====>%s audio_dsp_sel %d sel:%d\n", __func__,audio_dsp_sel,sel);

    if(audio_dsp_sel == sel)
        return;
    else
        audio_dsp_sel = sel;
    if(sel)
        system_echo("1 ","/proc/dsp_sel");
    else
        system_echo("0 ","/proc/dsp_sel");
}

int rt3261_audio_dsp_get(void)
{
    return audio_dsp_sel;
}


static int audio_in_sel = -1;
static int audio_in_pre_sel = -1;
void rt3261_audio_in_sel(int sel)
{
    if(audio_in_sel == sel)
        return;
    else
        audio_in_sel = sel;

    char cmd[10];
    memset(cmd,0,10);
    sprintf(cmd, "%d ", sel);
    system_echo(cmd,"/proc/adc_sel");
}

int rt3261_audio_in_sel_get(void)
{
    return audio_in_sel;
}


static int audio_out_sel = -1;
void rt3261_audio_out_sel(int sel)
{
    if(audio_out_sel == sel)
        return;
    else
        audio_out_sel = sel;


    char cmd[10];
    memset(cmd,0,10);
    sprintf(cmd, "%d ", sel);
    system_echo(cmd,"/proc/dac_sel");

}

int rt3261_audio_out_sel_get(void)
{
    return audio_out_sel;
}


static int audio_in_vol = -1;
static int audio_in_pre_vol = -1;

void rt3261_audio_in_vol(int vol)
{
    if(audio_in_vol == vol)
        return;
    else
        audio_in_vol = vol;

    char cmd[10] = {0};
    sprintf(cmd, "%d ", vol);
    system_echo(cmd,"/proc/adc_vol");
}

int rt3261_audio_in_vol_get(void)
{
    return audio_in_vol;
}

static int audio_out_vol = -1;
void rt3261_audio_out_vol(int vol)
{
    if(audio_out_vol == vol)
        return;
    else
        audio_out_vol = vol;

    char cmd[10] = {0};
    sprintf(cmd, "%d ", vol);
    system_echo(cmd,"/proc/dac_vol");
}

int rt3261_audio_out_vol_get(void)
{
    return audio_out_vol;
}

int audio_path_get(int dir)
{
    int path = 0;
    switch(dir)
    {
    case AUDIO_IN_SET:
        path =  rt3261_audio_in_sel_get();
        break;
    case AUDIO_OUT_SET:
        path =  rt3261_audio_out_sel_get();
        break;
    }

    //SPON_LOG_INFO("=====>%s path:%d\n", __func__,path);
    return path;
}

int audio_volume_get(int dir)
{
    int vol = 0;

    switch(dir)
    {
    case AUDIO_IN_SET:
        vol =  rt3261_audio_in_vol_get();
        break;
    case AUDIO_OUT_SET:
        vol =  rt3261_audio_out_vol_get();
        break;
    }

    SPON_LOG_INFO("=====>%s vol:%d\n", __func__,vol);
    return vol;
}

void audio_path_set(int dir,int param)
{
/*    DEBUG_ENTER();*/
    /*//SPON_LOG_INFO("=====>%s dir:%d param:%d\n", __func__,dir,param);*/
    /*switch(dir)*/
    /*{*/
    /*case AUDIO_IN_SET:*/
        /*rt3261_audio_in_sel(param);*/
        /*break;*/
    /*case AUDIO_OUT_SET:*/
        /*rt3261_audio_out_sel(param);*/
        /*break;*/
    /*}*/
}

void audio_volume_set(int dir,int param)
{
	DEBUG_ENTER();
	//SPON_LOG_INFO("=====>%s dir:%d param:%d\n", __func__,dir,param);
	switch(dir)
	{
		case AUDIO_IN_SET:
			rt3261_audio_in_vol(param);
			break;
		case AUDIO_OUT_SET:
			rt3261_audio_out_vol(param);
			break;
	}
}

void audio_dsp_set(int dir,int param)
{
    //SPON_LOG_INFO("=====>%s dir:%d param:%d\n", __func__,dir,param);
    switch(dir)
    {
		case AUDIO_DSP_PATH_SET:
			rt3261_audio_dsp_sel(param);
			break;
		case AUDIO_DSP_REC_DISTANCE_SET:
			rt3261_audio_dsp_ctl(param);
			break;
		case AUDIO_DSP_EC_RATE_SET:
			rt3261_audio_dsp_ec_rate(param);
			break;
    }
}


static int audio_channel_mode = AUDIO_CHANNEL_IDLE;
void audio_channel_mode_set(int mode)
{
   /* int dir_channel = AUDIO_FUNCTION_TALK;*/
	/*int in_channel = AUDIO_IN_FROM_NONE;*/
	/*int out_channel = AUDIO_OUT_TO_NONE;*/

	/*int headset_receiver_detect = headset_receiver_get_status();*/
	/*int lineout = lineout_status_get();*/
	/*int headset_mic_insert = headset_mic_status_get();*/
	/*int linein = linein_get_status();*/

	/*if(headset_mic_insert == 1)*/
	/*{*/
		/*in_channel |= AUDIO_IN_FROM_HEADSET_MIC;*/
	/*}*/
	/*else if(linein == 1)*/
	/*{*/
		/*in_channel |= AUDIO_IN_FROM_LINE_MIC;*/
	/*}*/
	/*else*/
	/*{*/
		/*in_channel |= AUDIO_IN_FROM_EXT_MIC;*/
	/*}*/

	/*if(headset_receiver_detect == 1)*/
	/*{*/
		/*out_channel |= AUDIO_OUT_TO_HEADSET_RECEIVER;*/
	/*}*/
	/*else if(lineout == 1)*/
	/*{*/
		/*out_channel |= AUDIO_OUT_TO_EXT_SPK;*/
	/*}*/
	/*else*/
	/*{*/
		/*out_channel |= AUDIO_OUT_TO_LOCAL_SPK;*/
	/*}*/

	/*if(mode != AUDIO_CHANNEL_DEFAULT)*/
	/*{*/
		/*audio_channel_mode = mode;*/
	/*}*/

	/*switch(audio_channel_mode)*/
	/*{*/
		/*case AUDIO_CHANNEL_MONITOR_IN:*/
			/*out_channel = AUDIO_OUT_TO_CPU_IN;*/
			/*break;*/
		/*case AUDIO_CHANNEL_MONITOR_OUT:*/
			/*in_channel = AUDIO_IN_FROM_CPU_OUT;*/
			/*break;*/
		/*case AUDIO_CHANNEL_CALL_RING:*/
			/*in_channel = AUDIO_IN_FROM_CPU_OUT;*/
			/*break;*/
		/*case AUDIO_CHANNEL_CALL_START:*/
			/*in_channel |= AUDIO_IN_FROM_CPU_OUT;*/
			/*out_channel |= AUDIO_OUT_TO_CPU_IN;*/
			/*break;*/
		/*case AUDIO_CHANNEL_BROADCAST_RING:*/
			/*in_channel = AUDIO_IN_FROM_CPU_OUT;*/
			/*break;*/
		/*case AUDIO_CHANNEL_BROADCAST_IN:*/
			/*dir_channel = AUDIO_FUNCTION_TALK;*/
			/*in_channel = AUDIO_IN_FROM_CPU_OUT;*/
			/*break;*/
		/*case AUDIO_CHANNEL_BROADCAST_OUT:*/
			/*dir_channel = AUDIO_FUNCTION_TALK;*/
			/*out_channel = AUDIO_OUT_TO_CPU_IN;*/
			/*break;*/
		/*case AUDIO_CHANNEL_FILE_PLAY:*/
			/*in_channel = AUDIO_IN_FROM_CPU_OUT;*/
			/*break;*/
		/*case AUDIO_CHANNEL_IDLE:*/
			/*in_channel = AUDIO_IN_FROM_NONE;*/
			/*out_channel = AUDIO_OUT_TO_NONE;*/
			/*break;*/
		/*default:*/
			/*in_channel = AUDIO_IN_FROM_NONE;*/
			/*out_channel = AUDIO_OUT_TO_NONE;*/
			/*break;*/
	/*}*/

	/*SPON_LOG_INFO("%s mode:%d param %d dir:%d in:%d out:%d\n", __func__,audio_channel_mode,mode,dir_channel,in_channel,out_channel);*/

	/*audio_dsp_set(AUDIO_DSP_REC_DISTANCE_SET,dir_channel);*/
	/*audio_path_set(AUDIO_IN_SET,in_channel);*/
	/*audio_path_set(AUDIO_OUT_SET,out_channel);*/
}


struct audio_output * alsa_audio_output_malloc(void)
{
    struct audio_output *ao = (struct audio_output *)malloc(sizeof(*ao));

    memset(ao, 0, sizeof(*ao));
    ao->device = "plughw:0,0";
    ao->open = alsa_audio_open;
    ao->close = alsa_audio_close;
    ao->process = alsa_audio_process;
    ao->init = alsa_audio_init;
    return ao;
}
void alsa_audio_output_free(struct audio_output * ao)
{
    if(ao) free(ao);
}


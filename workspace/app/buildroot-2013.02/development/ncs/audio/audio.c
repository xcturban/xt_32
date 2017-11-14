#include "audio.h"
#include "lpconfig.h"
#include "system_info.h"
#include "term.h"
#include "user_msg.h"

#include "mediastreamer2/msfileplayer.h"

// 接了线路输出时，io口为高电平
#define LINE_OUT_TRIGGER_IO_LEVEL (1)
// 接了线路输入时，io口为低电平
#define LINE_IN_TRIGGER_IO_LEVEL (0)
// 摘机时，io口为高电平
#define RECEIVER_TRIGGER_IO_LEVEL (1)


// line_out_trigger_io_init - 初始化线路输出检测口
static void line_out_trigger_io_init(void)
{
//	system("echo 169 > /sys/class/gpio/export");
//	system("echo in > /sys/class/gpio/gpio169/direction");
}


// line_out_trigger_io_value_get - 获取线路输出检测口电平值
static int line_out_trigger_io_value_get()
{
    FILE *fp;
    char buf[10] = {0};

    fp = fopen("/sys/class/gpio/gpio169/value", "r");
    if (!fp)
    {
        SPON_LOG_ERR("=====>%s - fopen /sys/class/gpio/gpio169/value failed:%s\n", __func__, strerror(errno));
        return -1;
    }
    fread(buf, sizeof(buf), 1, fp);
    fclose(fp);

    return atoi(buf);
}


// line_in_trigger_io_init - 初始化线路输入检测口
static void line_in_trigger_io_init(void)
{
//	system("echo 167 > /sys/class/gpio/export");
//	system("echo in > /sys/class/gpio/gpio167/direction");
}


// line_in_trigger_io_value_get - 获取线路输入检测口电平值
static int line_in_trigger_io_value_get()
{
    FILE *fp;
    char buf[10] = {0};

    fp = fopen("/sys/class/gpio/gpio167/value", "r");
    if (!fp)
    {
        SPON_LOG_ERR("=====>%s - fopen /sys/class/gpio/gpio167/value failed:%s\n", __func__, strerror(errno));
        return -1;
    }
    fread(buf, sizeof(buf), 1, fp);
    fclose(fp);

    return atoi(buf);
}


// receiver_trigger_io_init - 听筒摘机检测口
static void receiver_trigger_io_init(void)
{
//	system("echo 111 > /sys/class/gpio/export");
//	system("echo in > /sys/class/gpio/gpio111/direction");
}


// receiver_trigger_io_value_get - 获取听筒摘机检测口电平值
static int receiver_trigger_io_value_get()
{
    FILE *fp;
    char buf[10] = {0};

    fp = fopen("/sys/class/gpio/gpio111/value", "r");
    if (!fp)
    {
        SPON_LOG_ERR("=====>%s - fopen /sys/class/gpio/gpio111/value failed:%s\n", __func__, strerror(errno));
        return -1;
    }
    fread(buf, sizeof(buf), 1, fp);
    fclose(fp);

    return atoi(buf);
}

/**
 * audio_switch - 音频切换函数，io口电平变化的时候自动切换音频
 * @force_switch: 强制切换标记
 */
static void audio_switch(struct audio_manager *audio_manager, bool_t force_switch)
{
//	static int line_out_io_value_cur, line_out_io_value_prev;
//	static int line_in_io_value_cur, line_in_io_value_prev;
    //static int receiver_io_value_cur, receiver_io_value_prev;
    //bool_t need_switch = FALSE;

	return;
}

static audio_input_channel_t audio_input_channel_get(struct audio_manager *audio_manager)
{
    return audio_manager->in_chn;
}

static audio_output_channel_t audio_output_channel_get(struct audio_manager *audio_manager)
{
    return audio_manager->out_chn;
}

static void audio_input_mute(struct audio_manager *audio_manager)
{
	audio_path_set(AUDIO_IN_SET,AUDIO_IN_FROM_CPU_OUT);
}

static void audio_input_unmute(struct audio_manager *audio_manager)
{
	audio_channel_mode_set(AUDIO_CHANNEL_DEFAULT);
}

static int audio_stream_create(struct audio_manager *audio_manager, audio_dir_t dir, int samplerate, pthread_mutex_t *mutex, MSBufferizer *bufferizer, struct sockaddr *target_sockaddr)
{
    struct audio_stream *stream;
    MSSndCard *sndcard_playback;
    MSSndCard *sndcard_capture;
    MSConnectionHelper h;
    int tmp = 1;

    if (audio_manager->stream)
        return 0;

    sndcard_capture = ms_snd_card_manager_get_default_capture_card(ms_snd_card_manager_get());
    sndcard_playback = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());

    audio_manager->stream = (struct audio_stream *)malloc(sizeof(struct audio_stream));
    if (!audio_manager->stream)
        return -1;

    stream = audio_manager->stream;
    memset(stream, 0, sizeof(struct audio_stream));

    if (dir == AUDIO_BOTH)
    {
        stream->mixer = ms_filter_new(MS_AUDIO_MIXER_ID);
        ms_filter_call_method(stream->mixer, MS_AUDIO_MIXER_ENABLE_CONFERENCE_MODE, &tmp);
        ms_filter_call_method(stream->mixer, MS_FILTER_SET_SAMPLE_RATE, &samplerate);
    }

    if (dir == AUDIO_READ || dir == AUDIO_BOTH)
    {
        if (!sndcard_capture)
            return -1;

        stream->soundread = ms_snd_card_create_reader(sndcard_capture);
        ms_filter_call_method(stream->soundread, MS_FILTER_SET_SAMPLE_RATE, &samplerate);
    }

    if (dir == AUDIO_WRITE || dir == AUDIO_BOTH)
    {
        if (!sndcard_playback)
            return -1;

        stream->soundwrite = ms_snd_card_create_writer(sndcard_playback);
        ms_filter_call_method(stream->soundwrite, MS_FILTER_SET_SAMPLE_RATE, &samplerate);
    }

    if (dir == AUDIO_BOTH)
    {
        ms_connection_helper_start(&h);
        ms_connection_helper_link(&h, stream->soundread, -1, 0);
        if (stream->ec)
            ms_connection_helper_link(&h, stream->ec, 1, 1);
        ms_connection_helper_link(&h, stream->mixer, 0, 0);
        if (stream->ec)
            ms_connection_helper_link(&h, stream->ec, 0, 0);
        ms_connection_helper_link(&h, stream->soundwrite, 0, -1);
    }

    if (dir == AUDIO_READ)
    {
        stream->sender = ms_filter_new_from_desc(&ncs_audio_sender_desc);
        ms_filter_call_method(stream->sender, MS_FILTER_SET_SENDER_TARGET_SOCKADDR, target_sockaddr);

        ms_connection_helper_start(&h);
        ms_connection_helper_link(&h, stream->soundread, -1, 0);
        ms_connection_helper_link(&h, stream->sender, 0, -1);
    }

    if (dir == AUDIO_WRITE)
    {
        stream->receiver = ms_filter_new_from_desc(&ncs_audio_receiver_desc);
        ms_filter_call_method(stream->receiver, MS_FILTER_SET_RECEIVER_MUTEX, mutex);
        ms_filter_call_method(stream->receiver, MS_FILTER_SET_RECEIVER_BUFFERIZER, bufferizer);

        ms_connection_helper_start(&h);
        ms_connection_helper_link(&h, stream->receiver, -1, 0);
		ms_connection_helper_link(&h, stream->soundwrite, 0, -1);
    }

    stream->ticker = ms_ticker_new();
    ms_ticker_set_name(stream->ticker, "Audio MSTicker");
	/*ms_ticker_set_priority(stream->ticker, __ms_get_default_prio(FALSE));*/

	if (stream->soundread)
		ms_ticker_attach(stream->ticker, stream->soundread);
	if (stream->receiver)
		ms_ticker_attach(stream->ticker, stream->receiver);

    return 0;
}

static int audio_stream_destroy(struct audio_manager *audio_manager, audio_dir_t dir)
{
    struct audio_stream *stream = audio_manager->stream;
    MSConnectionHelper h;
	

    if (!stream)
        return -1;

    if (stream->soundread)
        ms_ticker_detach(stream->ticker, stream->soundread);
    if (stream->receiver)
        ms_ticker_detach(stream->ticker, stream->receiver);

    if (dir == AUDIO_BOTH)
    {

        ms_connection_helper_start(&h);
        ms_connection_helper_unlink(&h, stream->soundread, -1, 0);

        if (stream->ec)
            ms_connection_helper_unlink(&h, stream->ec, 1, 1);
        ms_connection_helper_unlink(&h, stream->mixer, 0, 0);

        if (stream->ec)
            ms_connection_helper_unlink(&h, stream->ec, 0, 0);
        ms_connection_helper_unlink(&h, stream->soundwrite, 0, -1);

    }

    if (dir == AUDIO_READ)
    {
        ms_connection_helper_start(&h);
        ms_connection_helper_unlink(&h, stream->soundread, -1, 0);
        ms_connection_helper_unlink(&h, stream->sender, 0, -1);
    }

    if (dir == AUDIO_WRITE)
    {
        ms_connection_helper_start(&h);
        ms_connection_helper_unlink(&h, stream->receiver, -1, 0);
        ms_connection_helper_unlink(&h, stream->soundwrite, 0, -1);
    }

    if (stream->soundread)
        ms_filter_destroy(stream->soundread);
    if (stream->soundwrite)
        ms_filter_destroy(stream->soundwrite);
    if (stream->ec)
        ms_filter_destroy(stream->ec);
    if (stream->mixer)
        ms_filter_destroy(stream->mixer);
    if (stream->receiver)
        ms_filter_destroy(stream->receiver);
    if (stream->sender)
        ms_filter_destroy(stream->sender);
    if (stream->ticker)
        ms_ticker_destroy(stream->ticker);

    free(stream);
    audio_manager->stream = NULL;

    return 0;
}

static int audio_conference_create(struct audio_manager *audio_manager, int samplerate)
{
    return audio_stream_create(audio_manager, AUDIO_BOTH, samplerate, NULL, NULL, NULL);
}

static int audio_conference_destroy(struct audio_manager *audio_manager)
{
    return audio_stream_destroy(audio_manager, AUDIO_BOTH);
}

static int find_free_mixer_pin(struct audio_manager *audio_manager)
{
    int i;
    struct audio_stream *stream = audio_manager->stream;
    MSFilter *mixer = stream->mixer;
    for (i = 0; i < mixer->desc->ninputs; ++i)
    {
        if (mixer->inputs[i] == NULL)
        {
            return i;
        }
    }

    return -1;
}

static struct conference_member *conference_member_malloc(struct audio_manager *audio_manager)
{
    int i;
    struct audio_stream *stream = audio_manager->stream;

    for (i = 0; i < NR_MEMBERS; ++i)
    {
        if (stream->member[i] == NULL)
        {
            struct conference_member *tmp = (struct conference_member *)malloc(sizeof(struct conference_member));
            stream->member[i] = tmp;
            return tmp;
        }
    }

    return NULL;
}

static int conference_member_index_find(struct audio_manager *audio_manager, struct sockaddr *target_sockaddr)
{
    int i;
    struct audio_stream *stream = audio_manager->stream;

    for (i = 0; i < NR_MEMBERS; ++i)
    {
        if (stream->member[i])
        {
            struct conference_member *tmp = stream->member[i];
            if (memcmp(tmp->target_sockaddr, target_sockaddr, sizeof(*target_sockaddr)) == 0)
                return i;
        }
    }

    return -1;
}

static bool_t has_conference_member(struct audio_manager *audio_manager)
{
    int i;
    struct audio_stream *stream = audio_manager->stream;

    for (i = 0; i < NR_MEMBERS; ++i)
    {
        if (stream->member[i])
        {
            return TRUE;
        }
    }

    return FALSE;
}

static int audio_conference_member_add(struct audio_manager *audio_manager, pthread_mutex_t *mutex, MSBufferizer *bufferizer, struct sockaddr *target_sockaddr)
{
    struct conference_member *member = conference_member_malloc(audio_manager);
    int mixer_pin = find_free_mixer_pin(audio_manager);
    struct audio_stream *stream = audio_manager->stream;
    MSTicker *ticker = stream->ticker;
    MSList *tmp = NULL;

    if (!member || mixer_pin < 0)
        return -1;
    member->receiver = ms_filter_new_from_desc(&ncs_audio_receiver_desc);
    member->sender = ms_filter_new_from_desc(&ncs_audio_sender_desc);
    ms_filter_call_method(member->receiver, MS_FILTER_SET_RECEIVER_MUTEX, mutex);
    ms_filter_call_method(member->receiver, MS_FILTER_SET_RECEIVER_BUFFERIZER, bufferizer);
    member->target_sockaddr = target_sockaddr;
    ms_filter_call_method(member->sender, MS_FILTER_SET_SENDER_TARGET_SOCKADDR, target_sockaddr);
    member->mixer_pin = mixer_pin;
    ms_mutex_lock(&ticker->lock);
    ms_filter_link(member->receiver, 0, stream->mixer, mixer_pin);
    ms_filter_link(stream->mixer, mixer_pin, member->sender, 0);
    tmp = ms_list_append(tmp, member->receiver);;
    ticker->execution_list = ms_list_concat(ticker->execution_list, tmp);
    ms_mutex_unlock(&ticker->lock);
    return 0;
}

static int audio_conference_member_remove(struct audio_manager *audio_manager, struct sockaddr *target_sockaddr)
{

    int index;
    struct audio_stream *stream = audio_manager->stream;
    MSTicker *ticker = stream->ticker;
    struct conference_member *member;
	

    index = conference_member_index_find(audio_manager, target_sockaddr);
    if (index < 0)
        return -1;

    member = stream->member[index];

    ms_mutex_lock(&ticker->lock);
    ticker->execution_list = ms_list_remove(ticker->execution_list, member->receiver);
    ms_filter_unlink(member->receiver, 0, stream->mixer, member->mixer_pin);
    ms_filter_unlink(stream->mixer, member->mixer_pin, member->sender, 0);
    ms_mutex_unlock(&ticker->lock);

    ms_filter_destroy(member->receiver);
    ms_filter_destroy(member->sender);

    free(member);
    stream->member[index] = NULL;

    if (!has_conference_member(audio_manager))
        audio_conference_destroy(audio_manager);

    return 0;
}

static int audio_ring_stop(struct audio_manager *audio_manager);
static int file_play_stop(struct audio_manager *audio_manager);

static int audio_bc_ring_stop(struct audio_manager *audio_manager)
{

    RingStream *stream = audio_manager->bc_ring;
    MSConnectionHelper h;


    if (!stream)
        return -1;

    ms_event_queue_skip(audio_manager->evt_queue);

    ms_ticker_detach(stream->ticker,stream->source);

    ms_connection_helper_start(&h);
    ms_connection_helper_unlink(&h,stream->source,-1,0);
    ms_connection_helper_unlink(&h,stream->tee,0,0);
    ms_connection_helper_unlink(&h,stream->sndwrite,0,-1);
    ms_filter_unlink(stream->tee, 1, stream->sender, 0);

    ms_ticker_destroy(stream->ticker);
    ms_filter_destroy(stream->source);
    ms_filter_destroy(stream->tee);
    ms_filter_destroy(stream->sndwrite);
    ms_filter_destroy(stream->sender);
    ms_free(stream);
    audio_manager->bc_ring = NULL;
    return 0;
}

static int audio_bc_ring_start(struct audio_manager *audio_manager, char *file, MSFilterNotifyFunc func, void * user_data)
{
    RingStream *stream;
    int samplerate;
    MSConnectionHelper h;
    int interval = -1;
    MSSndCard *sndcard;

    if (audio_manager->bc_ring)
	{
        return 0;
	}

    sndcard = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());
    if (!sndcard)
	{
        return -1;
	}

    ms_event_queue_skip(audio_manager->evt_queue);

    stream=(RingStream *)ms_new0(RingStream,1);
    stream->source=ms_filter_new(MS_FILE_PLAYER_ID);
    if (file)
        ms_filter_call_method(stream->source,MS_FILE_PLAYER_OPEN,(void*)file);

    ms_filter_call_method(stream->source,MS_FILE_PLAYER_LOOP,&interval);
    ms_filter_call_method_noarg(stream->source,MS_FILE_PLAYER_START);
    if (func!=NULL)
        ms_filter_set_notify_callback(stream->source,func,user_data);

    stream->tee = ms_filter_new(MS_TEE_ID);
    stream->sender = ms_filter_new_from_desc(&ncs_audio_sender_desc);


    stream->sndwrite=ms_snd_card_create_writer(sndcard);
    ms_filter_call_method(stream->source,MS_FILTER_GET_SAMPLE_RATE,&samplerate);
    ms_filter_call_method(stream->sndwrite,MS_FILTER_SET_SAMPLE_RATE,&samplerate);

    stream->ticker=ms_ticker_new();

    ms_ticker_set_name(stream->ticker,"Audio (ring) MSTicker");
	ms_ticker_set_priority(stream->ticker,MS_TICKER_PRIO_REALTIME);

    ms_connection_helper_start(&h);
    ms_connection_helper_link(&h,stream->source,-1,0);
    ms_connection_helper_link(&h, stream->tee, 0, 0);
    ms_connection_helper_link(&h,stream->sndwrite,0,-1);
    ms_filter_link(stream->tee, 1, stream->sender, 0);

    

    ms_ticker_attach(stream->ticker,stream->source);

    audio_manager->bc_ring = stream;

    return 0;
}



static int audio_ring_start(struct audio_manager *audio_manager, char *file, int interval)
{
    MSSndCard *sndcard_playback;


    if (audio_manager->ring)
        return 0;


    sndcard_playback = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());
    if (!sndcard_playback)
        return -1;

    audio_manager->ring = ring_start(file, interval, sndcard_playback);
    return 0;
}

static int audio_ring_stop(struct audio_manager *audio_manager)
{
    if (audio_manager->ring)
    {
        ring_stop(audio_manager->ring);
        audio_manager->ring = NULL;
    }

    return 0;
}


static void file_play_complete(void *userdata, struct _MSFilter *f, unsigned int id, void *arg)
{
	/*pthread_mutex_lock(&term->mutex);*/
    file_play_stop(term->audio_manager);
	/*pthread_mutex_unlock(&term->mutex);*/
}

static int file_play_start(struct audio_manager *audio_manager, char *file)
{
	MSSndCard *sndcard_playback;
	if (audio_manager->file_ring)
	{
		return 0;
	}
	sndcard_playback = ms_snd_card_manager_get_default_playback_card(ms_snd_card_manager_get());
	if (!sndcard_playback)
	{
		return -1;
	}
	ms_event_queue_skip(audio_manager->evt_queue);
	audio_manager->file_ring = ring_start_with_cb(file, -1, sndcard_playback, file_play_complete, NULL);
	return 0;
}

static int file_play_stop(struct audio_manager *audio_manager)
{
    if (audio_manager->file_ring)
    {
        ms_event_queue_skip(audio_manager->evt_queue);
        ring_stop(audio_manager->file_ring);
        audio_manager->file_ring = NULL;
    }

    return 0;
}

static void mic_set(struct audio_manager *audio_manager, bool_t flag)
{
	if(flag == TRUE)
	{
		audio_manager->set_rt3261reg(audio_manager, 0xc2, 0x0030);
	}
	else
	{
		audio_manager->set_rt3261reg(audio_manager, 0xc2, 0x0000);
	}
}

static bool_t file_playing(struct audio_manager *audio_manager)
{
    if (audio_manager->file_ring)
        return TRUE;
    else
        return FALSE;
}

/*
音量表 : 0，-1，-2，-4，-6，-8，-12，-14，-16，-18，-20，-22，-25，-30，-35，-79 db
*/
static int ncs_vol_table[NCS_MAX_VOL_VALUE+1] = {0,44,49,54,57,59,61,63,65,67,71,73,75,77,78,79};
int ncs_vol_to_pt2558_vol(int vol){
	if(vol<NCS_MIN_VOL_VALUE) vol=NCS_MIN_VOL_VALUE;
	else if(vol>NCS_MAX_VOL_VALUE) vol=NCS_MAX_VOL_VALUE;

//	printf("ncs_vol:%d pt_vol: -%d db  \n",vol,79-ncs_vol_table[vol]);
	return ncs_vol_table[vol];
}

void system_echo(char *value,char *dev){
	if(!value || !dev)
		return;
	
    FILE *fp_val = fopen(dev, "rb+");
    if(!fp_val) return;
    fwrite(value, sizeof(char), strlen(value), fp_val);	
	fclose(fp_val);
}

static void audio_check_codec(struct audio_manager *audio_manager)
{
	audio_manager->codec = RT3261;

	system("echo 0x80 > /proc/n32926_dac_digital_volume");
	system("echo 0x03 > /proc/n32926_dac_analog_volume");
}

static void audio_set_talk_mode(struct audio_manager *audio_manager)
{
	/*system("echo 2e8f00 > /proc/rt3261_reg");*/
	system("echo 46003a > /proc/rt3261_reg");
	system("echo 5201fd > /proc/rt3261_reg");
}

static void audio_set_bc_mode(struct audio_manager *audio_manager)
{
	/*system("echo 2e8f00 > /proc/rt3261_reg");*/
	system("echo 46001e > /proc/rt3261_reg");
	system("echo 5201f7 > /proc/rt3261_reg");
}


static void set_rt3261reg(struct audio_manager *audio_manager, uint8_t reg, uint16_t val)
{
	uint8_t  buf[128];

	sprintf(buf, "echo %02x%02x%02x > /proc/rt3261_reg", reg, val>>8, val&0xff);
	system(buf);
}

static void set_rt3261regEx(struct audio_manager *audio_manager, uint8_t reg, uint8_t bit, bool_t val)
{
	uint8_t  buf[128];

	sprintf(buf, "echo w%02x,%02d,%d > /proc/my_rt3261_fb", reg, bit, val);
	system(buf);
}

static void audio_set_ring_volume(struct audio_manager *audio_manager, int volume)
{
    char cl[128];
    int v;

    if (volume != -1 && volume >= 0 && volume <= 15)
    {
        v = volume;
        audio_manager->ring_volume = v;
    }
    else
        v = audio_manager->ring_volume;

    audio_volume_set(AUDIO_OUT_SET,v);

	//audio_switch(audio_manager, TRUE);
}

static void audio_set_system_volume(struct audio_manager *audio_manager, int volume)
{
    char cl[128];
    int v;

    if (volume != -1 && volume >= 0 && volume <= 15)
    {
        v = volume;
        audio_manager->system_volume = v;
    }
    else
        v = audio_manager->system_volume;

    audio_volume_set(AUDIO_OUT_SET,v);
	audio_switch(audio_manager, TRUE);
}

static void audio_set_talk_out_volume(struct audio_manager *audio_manager, int volume)
{
    char cl[128];
    int v;

    if (volume != -1 && volume >= 0 && volume <= 15)
    {
        v = volume;
        audio_manager->talk_out_volume = v;
    }
    else
        v = audio_manager->talk_out_volume;

	audio_volume_set(AUDIO_OUT_SET,v);
}

static void audio_set_talk_in_volume(struct audio_manager *audio_manager, int volume)
{
    char cl[128];
    int v;

    if (volume != -1 && volume >= 0 && volume <= 15)
    {
        v = volume;
        audio_manager->talk_in_volume = v;
    }
    else
        v = audio_manager->talk_in_volume;
	audio_volume_set(AUDIO_IN_SET,v);
	//audio_switch(audio_manager, TRUE);
}

static void audio_set_bc_out_volume(struct audio_manager *audio_manager, int volume)
{
    char cl[128];
    int v;

    if (volume != -1 && volume >= 0 && volume <= 15)
    {
        v = volume;
        audio_manager->bc_out_volume = v;
    }
    else
        v = audio_manager->bc_out_volume;
	audio_volume_set(AUDIO_OUT_SET,v);
}

static void audio_set_bc_in_volume(struct audio_manager *audio_manager, int volume)
{
    char cl[128];
    int v;

    if (volume != -1 && volume >= 0 && volume <= 15)
    {
        v = volume;
        audio_manager->bc_in_volume = v;
    }
    else
        v = audio_manager->bc_in_volume;

    audio_volume_set(AUDIO_IN_SET,v);
}

static int audio_get_talk_in_volume(struct audio_manager *audio_manager)
{
    return audio_manager->talk_in_volume;
}

static int audio_get_talk_out_volume(struct audio_manager *audio_manager)
{
    return audio_manager->talk_out_volume;
}

static int audio_get_codec(struct audio_manager *audio_manager)
{
    return audio_manager->codec;
}

static int audio_reinit(struct audio_manager *audio_manager)
{
    audio_check_codec(audio_manager);
    return 0;
}

static void audio_channel_init(struct audio_manager *audio_manager)
{
    LpConfig *lpconfig;
    int in_sel;
    int out_sel;
    lpconfig = lp_config_new(INI_CONFIG_FILE);
    if (!lpconfig)
        return;
    // Mic
    in_sel = lp_config_get_int(lpconfig, "terminal", "input_channel_sel", 0);
    if (in_sel == 0)
    {
        if (audio_manager->codec == WM8960)
        {
            audio_manager->orig_input_channel = 1;
            audio_manager->input_channel = 1;
            system("echo 1 > /proc/audio_input_switch");
        }
        else
        {
            audio_manager->orig_input_channel = 3;
            audio_manager->input_channel = 3;
            system("echo 3 > /proc/adc_sel");
        }
        audio_manager->orig_in_chn = MIC_INPUT;
        audio_manager->in_chn = MIC_INPUT;
    }
    else
    {
        // Line in
        if (audio_manager->codec == WM8960)
        {
            audio_manager->orig_input_channel = 3;
            audio_manager->input_channel = 3;
            system("echo 3 > /proc/audio_input_switch");
        }
        else
        {
            audio_manager->orig_input_channel = 9;
            audio_manager->input_channel = 9;
            system("echo 9 > /proc/adc_sel");
        }
        audio_manager->orig_in_chn = LINE_INPUT;
        audio_manager->in_chn = LINE_INPUT;
    }
    // Speaker
    out_sel = lp_config_get_int(lpconfig, "terminal", "output_channel_sel", 0);
    if (out_sel == 0)
    {
		audio_manager->orig_output_channel = 3;
		audio_manager->output_channel = 3;
		system("echo 3 > /proc/dac_sel");

        audio_manager->orig_out_chn = SPK_OUTPUT;
        audio_manager->out_chn = SPK_OUTPUT;
        system("echo 0 > /proc/n32926_line_out_volume"); //濡傛灉鏄壃澹板櫒杈撳嚭锛屽垯鍏抽棴绾胯矾杈撳嚭闊抽噺
    }
    else
    {
		// Line out
		audio_manager->orig_output_channel = 2;
		audio_manager->output_channel = 2;
		system("echo 2 > /proc/dac_sel");

		audio_manager->orig_out_chn = LINE_OUTPUT;
		audio_manager->out_chn = LINE_OUTPUT;
		system("echo 0 > /proc/dac_vol"); //濡傛灉鏄嚎璺緭鍑猴紝鍒欏叧闂壃澹板櫒闊抽噺
    }
    lp_config_destroy(lpconfig);
    SPON_LOG_INFO("=====>%s - in_sel:%d out_sel:%d\n", __func__, in_sel, out_sel);
}

static int audio_init(struct audio_manager *audio_manager)
{
    LpConfig *lpconfig;

    lpconfig = lp_config_new(INI_CONFIG_FILE);

    audio_manager->talk_in_volume  = lp_config_get_int(lpconfig, "talk_cfg", "volume_in", 8);
    audio_manager->talk_out_volume = lp_config_get_int(lpconfig, "talk_cfg", "volume_out", 8);
    /*audio_manager->bc_in_volume    = lp_config_get_int(lpconfig, "broadcast_cfg", "volume_in", 8);*/
    /*audio_manager->bc_out_volume   = lp_config_get_int(lpconfig, "broadcast_cfg", "volume_out", 8);*/
	//TOA测试反馈需要合并两个音量,将广播和对讲音量合并,只有对讲 音量
    audio_manager->bc_in_volume    = audio_manager->talk_in_volume;
    audio_manager->bc_out_volume   = audio_manager->talk_out_volume;
    audio_manager->ring_volume     = lp_config_get_int(lpconfig, "terminal", "volume_ring", 8);
    audio_manager->system_volume   = audio_manager->ring_volume;
	audio_volume_set(AUDIO_OUT_SET, audio_manager->bc_out_volume);

    lp_config_destroy(lpconfig);

    audio_check_codec(audio_manager);

	audio_channel_init(audio_manager);

    receiver_trigger_io_init();

    audio_switch(audio_manager, TRUE);

    audio_manager->evt_queue = ms_event_queue_new();
    ms_set_global_event_queue(audio_manager->evt_queue);

    return 0;
}

struct audio_manager *audio_manager_malloc(void)
{
    struct audio_manager *audio_manager = malloc(sizeof(struct audio_manager));
    if (!audio_manager)
        return NULL;

    memset(audio_manager, 0, sizeof(struct audio_manager));

    audio_manager->init                     = audio_init;
    audio_manager->reinit                   = audio_reinit;
    audio_manager->set_ring_volume          = audio_set_ring_volume;
    audio_manager->set_system_volume        = audio_set_system_volume;
    audio_manager->set_talk_in_volume       = audio_set_talk_in_volume;
    audio_manager->set_talk_out_volume      = audio_set_talk_out_volume;
    audio_manager->set_bc_in_volume         = audio_set_bc_in_volume;
    audio_manager->set_bc_out_volume        = audio_set_bc_out_volume;
    audio_manager->get_talk_out_volume      = audio_get_talk_out_volume;
    audio_manager->get_talk_in_volume      = audio_get_talk_in_volume;
    audio_manager->set_talk_mode            = audio_set_talk_mode;
    audio_manager->set_bc_mode              = audio_set_bc_mode;
    audio_manager->set_rt3261reg            = set_rt3261reg;
    audio_manager->set_rt3261regEx          = set_rt3261regEx;
    audio_manager->get_codec                = audio_get_codec;
    audio_manager->stream_create            = audio_stream_create;
    audio_manager->stream_destroy           = audio_stream_destroy;
    audio_manager->conference_create        = audio_conference_create;
    audio_manager->conference_destroy       = audio_conference_destroy;
    audio_manager->conference_member_add    = audio_conference_member_add;
    audio_manager->conference_member_remove = audio_conference_member_remove;
    audio_manager->ring_start               = audio_ring_start;
    audio_manager->ring_stop                = audio_ring_stop;
    audio_manager->bc_ring_start            = audio_bc_ring_start;
    audio_manager->bc_ring_stop             = audio_bc_ring_stop;
    audio_manager->audio_switch             = audio_switch;
    audio_manager->audio_input_channel_get  = audio_input_channel_get;
    audio_manager->audio_output_channel_get = audio_output_channel_get;
    audio_manager->audio_input_mute         = audio_input_mute;
    audio_manager->audio_input_unmute       = audio_input_unmute;
    audio_manager->file_play_start          = file_play_start;
    audio_manager->file_play_stop           = file_play_stop;
    audio_manager->file_playing             = file_playing;
    audio_manager->mic_set             = mic_set;

    return audio_manager;
}

void audio_manager_free(struct audio_manager *audio_manager)
{
    free(audio_manager);
}

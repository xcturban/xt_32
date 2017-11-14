#include <pthread.h>

#include "ncs_audio_filter.h"
#include "mediastreamer2/msqueue.h"

#include <ncs_event/ncs_event.h>

#include "term.h"

struct receiver_data {
    pthread_mutex_t *mutex;
	MSBufferizer *bufferizer;
};

static void receiver_init(MSFilter *obj)
{
	struct receiver_data *rd = ms_new0(struct receiver_data, 1);
	//打开扬声器电源, 避免白噪声
	term->audio_manager->set_rt3261regEx(term->audio_manager, dec_2_bcd(61), 0, 1);
	obj->data = rd;
}

static void receiver_preprocess(MSFilter *obj)
{
    struct receiver_data *rd = (struct receiver_data *)obj->data;
}

static void receiver_process(MSFilter *obj)
{
    int avail = 0;
    struct receiver_data *rd = (struct receiver_data *)obj->data;
    struct broadcast *bc = term->ncs_manager->bc;

	/*printf("avail 0: %d\n", avail);*/
	pthread_mutex_lock(rd->mutex);
	avail = ms_bufferizer_get_avail(rd->bufferizer);
	avail = 440;
	/*printf("avail 3: %d\n", avail);*/
	if (avail > 0) {
		/*printf("avail 2: %d\n", avail);*/
		mblk_t *om = allocb(avail, 0);
		ms_bufferizer_read(rd->bufferizer, om->b_wptr, avail);
		om->b_wptr += avail;
		/*{*/
			/*static int newMs1 = 0;*/
			/*static int oldMs1 = 0;*/
			/*struct timeval tv1;*/
			/*struct timezone tz;*/
			/*gettimeofday(&tv1, &tz);*/
			/*newMs1 = tv1.tv_usec;*/
			/*if((newMs1-oldMs1) < 0)*/
			/*{*/
				/*printf("ms2= %d, avail:%d\n", 1000000-newMs1+oldMs1, avail);*/

			/*}*/
			/*else*/
			/*{*/
				/*printf("ms2= %d\n", newMs1-oldMs1);*/
			/*}*/
			/*oldMs1 = newMs1;*/
		/*}*/
		ms_queue_put(obj->outputs[0], om);
		terminal_audio_packnum_clear(term);
	}
	pthread_mutex_unlock(rd->mutex);
}

static void receiver_postprocess(MSFilter *obj)
{
	struct receiver_data *rd = (struct receiver_data *)obj->data;
}

static int receiver_mutex_set(MSFilter *obj, void *arg)
{
	struct receiver_data *rd = (struct receiver_data *)obj->data;
    rd->mutex = (pthread_mutex_t *)arg;
	return 0;
}

static int receiver_bufferizer_set(MSFilter *obj, void *arg)
{
	struct receiver_data *rd = (struct receiver_data *)obj->data;
    rd->bufferizer = (MSBufferizer *)arg;
	return 0;
}

static MSFilterMethod receiver_methods[] = {
	{MS_FILTER_SET_RECEIVER_MUTEX, receiver_mutex_set},
	{MS_FILTER_SET_RECEIVER_BUFFERIZER, receiver_bufferizer_set},
	{0,	NULL},
};

static void receiver_uninit(MSFilter *obj)
{
	struct receiver_data *rd = (struct receiver_data *)obj->data;

	ms_free(rd);
}

MSFilterDesc ncs_audio_receiver_desc = {
	.id = MS_FILTER_PLUGIN_ID,
	.name = "ncs_audio_receiver",
	.text = "ncs_audio_receiver",
	.category = MS_FILTER_OTHER,
	.ninputs = 0,
	.noutputs = 1,
    .init = receiver_init,
	.preprocess = receiver_preprocess,
	.process = receiver_process,
	.postprocess = receiver_postprocess,
	.methods = receiver_methods,
    .uninit = receiver_uninit,
};

struct sender_data {
	struct sockaddr *target_sockaddr;
    MSBufferizer bufferizer;
};

static void sender_init(MSFilter *obj)
{
	struct sender_data *sd = ms_new0(struct sender_data, 1);

	//打开扬声器电源, 避免白噪声
	term->audio_manager->set_rt3261regEx(term->audio_manager, dec_2_bcd(61), 0, 1);
    ms_bufferizer_init(&sd->bufferizer);
	obj->data = sd;
}

static void sender_preprocess(MSFilter *obj)
{
    struct sender_data *sd = (struct sender_data *)obj->data;
}

static void sender_process(MSFilter *obj)
{
    int avail = 0;
    uint8_t pcm[NCS_PCM_LEN];
    ncs_event_t *evt;
    struct sender_data *sd = (struct sender_data *)obj->data;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    ms_bufferizer_put_from_queue(&sd->bufferizer, obj->inputs[0]);



    while (ms_bufferizer_read(&sd->bufferizer, pcm, NCS_PCM_LEN) >= NCS_PCM_LEN) {

		if (sd->target_sockaddr)
        	evt = ncs_call_wave_data_event_build(sd->target_sockaddr, pcm, NCS_PCM_LEN);
		else
			evt = ncs_realtime_wave_data_event_build(pcm, NCS_PCM_LEN);
        if (evt)
            ncs_event_send(evt);

        if (ncs_manager->third_party_record) {
            evt = ncs_call_wave_data_event_build(&ncs_manager->third_party_record_addr, pcm, NCS_PCM_LEN);
            if (evt)
                ncs_event_send(evt);
        }
    }
}

static void sender_postprocess(MSFilter *obj)
{
	struct sender_data *sd = (struct sender_data *)obj->data;
}

static int sender_target_sockaddr_set(MSFilter *obj, void *arg)
{
	struct sender_data *sd = (struct sender_data *)obj->data;
    sd->target_sockaddr = (struct sockaddr *)arg;
	return 0;
}

static MSFilterMethod sender_methods[] = {
	{MS_FILTER_SET_SENDER_TARGET_SOCKADDR, sender_target_sockaddr_set},
	{0,	NULL},
};

static void sender_uninit(MSFilter *obj)
{
	struct sender_data *sd = (struct sender_data *)obj->data;

	//关闭扬声器电源, 避免白噪声
	term->audio_manager->set_rt3261regEx(term->audio_manager, dec_2_bcd(61), 0, 0);
    ms_bufferizer_flush(&sd->bufferizer);
	ms_free(sd);
}

MSFilterDesc ncs_audio_sender_desc = {
	.id = MS_FILTER_PLUGIN_ID,
	.name = "ncs_audio_sender",
	.text = "ncs_audio_sender",
	.category = MS_FILTER_OTHER,
	.ninputs = 1,
	.noutputs = 0,
    .init = sender_init,
	.preprocess = sender_preprocess,
	.process = sender_process,
	.postprocess = sender_postprocess,
	.methods = sender_methods,
    .uninit = sender_uninit,
};

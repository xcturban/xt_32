

#include "mediastreamer2/msrtp.h"
#ifdef __NCS_SUPPORT_ONVIF__
#include "rtsp_server.h"
#endif


typedef struct
{
    VideoStream *stream;
    RtpProfile *prof;

    struct sockaddr target_sockaddr;

} ncs_video_stream_t;

typedef struct
{
    RtpSession *s;
    MSFilter *rtpsend;
    int pos;

    struct sockaddr target_sockaddr;

    char remip[20];
    int remport;

    struct list_head list;
} ncs_video_send_sub_stream_t;

static LIST_HEAD(ncs_video_send_sub_stream_list_head);

static void ncs_video_send_sub_stream_add(RtpSession *s, MSFilter *rtpsend, int pos, char *remip, int remport)
{
    ncs_video_send_sub_stream_t *sub = malloc(sizeof(*sub));

    if (sub)
    {
        memset(sub, 0, sizeof(*sub));
        sub->s = s;
        sub->rtpsend = rtpsend;
        sub->pos = pos;
        strcpy(sub->remip, remip);
        sub->remport = remport;

        INIT_LIST_HEAD(&sub->list);

        list_add_tail(&sub->list, &ncs_video_send_sub_stream_list_head);
    }
}

static ncs_video_send_sub_stream_t *ncs_video_send_sub_stream_find(char *remip, int remport)
{
    struct list_head *p;
    struct list_head *head = &ncs_video_send_sub_stream_list_head;
    ncs_video_send_sub_stream_t *sub;

    list_for_each(p, head)
    {
        sub = list_entry(p, ncs_video_send_sub_stream_t, list);
		printf("subip :%s, subport:%d\n", sub->remip, sub->remport);
        if (strcmp(sub->remip, remip) == 0 && sub->remport == remport)
		{
            return sub;
		}
    }

    return NULL;
}


static ncs_video_stream_t *ncs_video_stream_send;
static ncs_video_stream_t *ncs_video_stream_recv;

static int nvs_send_total;

static void ncs_video_stream_stop(VideoStreamDir dir, struct sockaddr *target_sockaddr, char *remip, int remport)
{
#ifdef __NCS_SUPPORT_ONVIF__
    if (!target_sockaddr && dir != VideoStreamSendOnly)
        return;
#else
    if (!target_sockaddr)
	{
        return;
	}
#endif

    if (dir == VideoStreamSendOnly)
    {
		//video_type == disable return;
		if(term->video_type == FALSE)
		{
			SPON_LOG_INFO("vidio_type false\n");
			return;
		}

        ncs_video_send_sub_stream_t *sub = NULL;
        MSTicker *ticker;
        MSFilter *tee;

        if (!ncs_video_stream_send)
		{
            return;
		}

        sub = ncs_video_send_sub_stream_find(remip, remport);
        if (!sub)
		{
            return;
		}

        ticker = ncs_video_stream_send->stream->ticker;
        tee = ncs_video_stream_send->stream->tee;

        list_del(&sub->list);

		ms_mutex_lock(&ticker->lock);

        ms_filter_unlink(tee, sub->pos, sub->rtpsend, 0);

		ms_mutex_unlock(&ticker->lock);

        if (sub->pos == 0)
            rtp_session_unregister_event_queue(ncs_video_stream_send->stream->session, ncs_video_stream_send->stream->evq);

        rtp_session_destroy(sub->s);

        ms_filter_destroy(sub->rtpsend);

        if (sub->pos == 0)
        {
            ncs_video_stream_send->stream->rtpsend = NULL;
            ncs_video_stream_send->stream->session = NULL;
        }

        free(sub);

        nvs_send_total--;
        SPON_LOG_INFO("=====>%s - nvs_send_total:%d\n", __func__, nvs_send_total);

        if (nvs_send_total == 0)
        {
            video_stream_stop(ncs_video_stream_send->stream);
            rtp_profile_destroy(ncs_video_stream_send->prof);
            ms_free(ncs_video_stream_send);
            ncs_video_stream_send = NULL;
        }
    }
    else if (dir == VideoStreamRecvOnly)
    {
        if (!ncs_video_stream_recv)
		{
            return;
		}
        if (memcmp(&ncs_video_stream_recv->target_sockaddr, target_sockaddr, sizeof(*target_sockaddr)))
		{
            return;
		}

        video_stream_stop(ncs_video_stream_recv->stream);
        rtp_profile_destroy(ncs_video_stream_recv->prof);
        ms_free(ncs_video_stream_recv);
        ncs_video_stream_recv = NULL;
    }
}

int h264_decode_started_cb(void)
{
	SendNotifyMessage(system_info.main_hwnd, MSG_NCS_CHATTING_REFRESH, 0, 0);
}

#define PAYLOAD (102)
static int ncs_video_stream_start(VideoStreamDir dir, struct sockaddr *target_sockaddr, char *remip, int remport)
{
    VideoStream *stream;
    RtpProfile *prof;
    struct ncs_manager *ncs_manager = term->ncs_manager;
#ifdef __NCS_SUPPORT_ONVIF__
    if (!target_sockaddr && dir != VideoStreamSendOnly)
        return -1;
#else
    if (!target_sockaddr)
        return -1;
#endif
	Set_h264_decode_started_cb(h264_decode_started_cb);
    if (dir == VideoStreamRecvOnly)
    {
        if (ncs_video_stream_recv)
		{
            return -1;
		}
		ncs_video_stream_recv = (ncs_video_stream_t *)ms_new0(ncs_video_stream_t, 1);
        ncs_video_stream_recv->stream = video_stream_new(ncs_manager->video_port, FALSE);
        ncs_video_stream_recv->prof = rtp_profile_new("Call profile");
        memcpy(&ncs_video_stream_recv->target_sockaddr, target_sockaddr, sizeof(*target_sockaddr));
        stream = ncs_video_stream_recv->stream;
        prof = ncs_video_stream_recv->prof;
        prof->payload[PAYLOAD] = &payload_type_h264;
        stream->dir = dir;
        video_stream_start(stream, prof, remip, 0, 0, PAYLOAD, 60, NULL);
    }
    else if (dir == VideoStreamSendOnly)
    {

        ncs_video_send_sub_stream_t *sub;

        if (!remip || remport <= 0)
            return -1;

        sub = ncs_video_send_sub_stream_find(remip, remport);
        if (sub)
            return 0;

        if (!ncs_video_stream_send)
        {
            ncs_video_stream_send = (ncs_video_stream_t *)ms_new0(ncs_video_stream_t, 1);
            ncs_video_stream_send->stream = video_stream_new(-1, FALSE);
            ncs_video_stream_send->prof = rtp_profile_new("Call profile");
            stream = ncs_video_stream_send->stream;
            prof = ncs_video_stream_send->prof;
            prof->payload[102] = &payload_type_h264;
            stream->dir = dir;

            video_stream_start(stream, prof, remip, remport, remport+1, 102, 60, NULL);
            ncs_video_send_sub_stream_add(stream->session, stream->rtpsend, 0, remip, remport);

        }
        else
        {
            MSTicker *ticker = ncs_video_stream_send->stream->ticker;
            RtpSession *s;
            MSFilter *rtpsend;
            MSFilter *tee = ncs_video_stream_send->stream->tee;
            int i;

            if (nvs_send_total >= tee->desc->noutputs)
                return -1;

            prof = ncs_video_stream_send->prof;

            s = rtp_session_new(RTP_SESSION_SENDONLY);
            rtp_session_set_scheduling_mode(s, 0);
            rtp_session_set_blocking_mode(s, 0);
            rtp_session_set_remote_addr(s, remip, remport);
            rtp_session_set_profile(s, prof);
            rtp_session_set_payload_type(s, 102);


            rtpsend = ms_filter_new(MS_RTP_SEND_ID);
            ms_filter_call_method(rtpsend, MS_RTP_SEND_SET_SESSION, s);

			ms_mutex_lock(&ticker->lock);
            for (i = 1; i < tee->desc->noutputs; i++)
            {
                if (tee->outputs[i] == NULL)
                {
                    rtpsend->ticker = ticker;
                    rtpsend->last_tick = ticker->ticks;
                    ms_filter_link(tee, i, rtpsend, 0);
                    break;
                }
            }
			ms_mutex_unlock(&ticker->lock);
            ncs_video_send_sub_stream_add(s, rtpsend, i, remip, remport);
        }

        nvs_send_total++;
        SPON_LOG_INFO("=====>%s - nvs_send_total:%d\n", __func__, nvs_send_total);
    }

    return 0;
}

#ifdef __NCS_SUPPORT_ONVIF__

void ncs_rtsp_video_stream_start(char *remip, int remport,int rem_rtcp_port)
{
	return ncs_video_stream_start(VideoStreamSendOnly, NULL, remip, remport);
}

void ncs_rtsp_video_stream_stop(char *remip, int remport)
{
	ncs_video_stream_stop(VideoStreamSendOnly, NULL, remip, remport);
}

void ncs_rtsp_init(void)
{
    onvif_server_module_start(ncs_rtsp_video_stream_start,ncs_rtsp_video_stream_stop);
	onvif_rtsp_video_resolution_set(640,480);
}
#endif

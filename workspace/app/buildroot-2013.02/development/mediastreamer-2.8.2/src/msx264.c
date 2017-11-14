/*
mediastreamer2 x264 plugin
Copyright (C) 2006-2010 Belledonne Communications SARL (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msticker.h"
#include "mediastreamer2/msvideo.h"
#include "mediastreamer2/rfc3984.h"

#ifdef _MSC_VER
#include <stdint.h>
#endif

#include "favc_avcodec.h"

#ifndef VERSION
#define VERSION "1.4.1"
#endif


//#define DEBUG_ENTER()  do{printf("=======>Enter %s-%d\n",__func__,__LINE__);}while(0)
#define DEBUG_ENTER()


#define RC_MARGIN 10000 /*bits per sec*/
#define YUV_420_IMAGE_SIZE(width,height) ((width * height) / 2 * 3)


#define SPECIAL_HIGHRES_BUILD_CRF 28

/* the goal of this small object is to tell when to send I frames at startup:
at 2 and 4 seconds*/
typedef struct VideoStarter{
	uint64_t next_time;
	int i_frame_count;
}VideoStarter;



#define	VIDEO_CIF_FMT 	 	0x00
#define	VIDEO_VGA_FMT 	 	0x01
#define	VIDEO_D1_FMT 	 	0x02
#define	VIDEO_720P_FMT  	0x03
#define	VIDEO_1080P_FMT 	0x04

static int video_fmt_type = VIDEO_CIF_FMT;

static void video_starter_init(VideoStarter *vs){
	vs->next_time=0;
	vs->i_frame_count=0;
}

static void video_starter_first_frame(VideoStarter *vs, uint64_t curtime){
	vs->next_time=curtime+2000;
}

static bool_t video_starter_need_i_frame(VideoStarter *vs, uint64_t curtime){
	if (vs->next_time==0) return FALSE;
	if (curtime>=vs->next_time){
		vs->i_frame_count++;
		if (vs->i_frame_count==1){
			vs->next_time+=2000;
		}else{
			vs->next_time=0;
		}
		return TRUE;
	}
	return FALSE;
}

typedef struct _EncData{
	MSVideoSize vsize;
	int bitrate;
	float fps;
	int mode;
	uint64_t framenum;
	Rfc3984Context *packer;
	int keyframe_int;
	VideoStarter starter;
	bool_t generate_keyframe;
//jack
	int favc_quant;
	int qmax;
    int qmin;
	int quant;               // Init Quant
	int frame_rate_base;
   	int gop_size;
	int favc_enc_fd1;	//enc fd
	unsigned char * out_h264_frame;
	FAVC_ENC_PARAM     enc_param;
	avc_workbuf_t	bitstreambuf, yuvframebuf;
}EncData;

int n32926_set_h264_init(MSFilter *f)
{
	EncData * d = (EncData *)f->data;
    // Open driver handler1
    if(d->favc_enc_fd1==0)
      d->favc_enc_fd1= open(FAVC_ENCODER_DEV,O_RDWR);

    if(d->favc_enc_fd1 <= 0)
    {
        printf("H1 Fail to open %s\n",FAVC_ENCODER_DEV);
        return -1;
    }

	// Get Bitstream Buffer Information
	if((ioctl(d->favc_enc_fd1, FAVC_IOCTL_ENCODE_GET_BSINFO, &d->bitstreambuf)) < 0)
	{
		close(d->favc_enc_fd1);
		printf("Get avc Enc bitstream info fail\n");
		return -1;
	}
	printf("H1 Get Enc BS Buffer Physical addr = 0x%x, size = 0x%x,\n",d->bitstreambuf.addr,d->bitstreambuf.size);

    memset(&d->enc_param, 0, sizeof(FAVC_ENC_PARAM));

    d->enc_param.u32API_version = H264VER;
    d->enc_param.u32FrameWidth= d->vsize.width;
    d->enc_param.u32FrameHeight=d->vsize.height;

    d->enc_param.fFrameRate = d->fps;
    d->enc_param.u32IPInterval = d->gop_size; //60, IPPPP.... I, next I frame interval
    d->enc_param.u32MaxQuant       =d->qmax;
    d->enc_param.u32MinQuant       =d->qmin;
    d->enc_param.u32Quant = d->quant; //32
    d->enc_param.u32BitRate = d->bitrate;
    d->enc_param.ssp_output = -1;
    d->enc_param.intra = -1;

    d->enc_param.bROIEnable = 0;
    d->enc_param.u32ROIX = 0;
    d->enc_param.u32ROIY = 0;
    d->enc_param.u32ROIWidth = 0;
    d->enc_param.u32ROIHeight = 0;


    if (ioctl(d->favc_enc_fd1, FAVC_IOCTL_ENCODE_INIT, &d->enc_param) < 0)
    {
        close(d->favc_enc_fd1);
        printf("Handler_1 Error to set FAVC_IOCTL_ENCODE_INIT\n");
        return -1;
    }

    return 0;
}

static int n32926_favc_encode(MSFilter *f, void * data)
{
    AVFrame   *pict=(AVFrame *)data;
	EncData * d = (EncData *)f->data;
	int fileDecriptor = d->favc_enc_fd1;

    memset(&d->enc_param, 0, sizeof(FAVC_ENC_PARAM));
    d->enc_param.pu8YFrameBaseAddr = (unsigned char *)pict->data[0];   //input user continued virtual address (Y), Y=0 when NVOP
    d->enc_param.pu8UFrameBaseAddr = (unsigned char *)pict->data[1];   //input user continued virtual address (U)
    d->enc_param.pu8VFrameBaseAddr = (unsigned char *)pict->data[2];   //input user continued virtual address (V)

	memset(d->out_h264_frame,0,YUV_420_IMAGE_SIZE(d->vsize.width,d->vsize.height));
    d->enc_param.bitstream = d->out_h264_frame;  //output User Virtual address
    d->enc_param.ssp_output = -1;
    d->enc_param.intra = -1;
    d->enc_param.u32IPInterval = 0; // use default IPInterval that set in INIT

    d->enc_param.u32Quant = d->favc_quant;
    d->enc_param.bitstream_size = 0;

    //printf("APP : FAVC_IOCTL_ENCODE_FRAME\n");
    if (ioctl(fileDecriptor, FAVC_IOCTL_ENCODE_FRAME, &d->enc_param) < 0)
    {
        printf("Dev =%d Error to set FAVC_IOCTL_ENCODE_FRAME\n", fileDecriptor);
        return 0;
    }


    return d->enc_param.bitstream_size;
}

void mx264_video_fmt_set(int fmt)
{
	video_fmt_type = fmt;
}

static void enc_init(MSFilter *f){
DEBUG_ENTER();
	EncData *d=ms_new(EncData,1);

	switch(video_fmt_type)
	{
		case VIDEO_CIF_FMT:
			d->vsize.width=MS_VIDEO_SIZE_CIF_W;
			d->vsize.height=MS_VIDEO_SIZE_CIF_H;
			break;
		case VIDEO_VGA_FMT:
			d->vsize.width=MS_VIDEO_SIZE_VGA_W;
			d->vsize.height=MS_VIDEO_SIZE_VGA_H;
			break;
		case VIDEO_D1_FMT:
			d->vsize.width=MS_VIDEO_SIZE_4CIF_W;
			d->vsize.height=MS_VIDEO_SIZE_4CIF_H;
			break;
		case VIDEO_720P_FMT:
			d->vsize.width=MS_VIDEO_SIZE_720P_W;
			d->vsize.height=MS_VIDEO_SIZE_720P_H;
			break;
		case VIDEO_1080P_FMT:
			d->vsize.width=MS_VIDEO_SIZE_1080P_W;
			d->vsize.height=MS_VIDEO_SIZE_1080P_H;
			break;
		default:
			d->vsize.width=MS_VIDEO_SIZE_CIF_W;
			d->vsize.height=MS_VIDEO_SIZE_CIF_H;
			break;
	}

	d->bitrate=512000;
	d->fps=30;
	d->keyframe_int=10; /*10 seconds */
	d->mode=1;
	d->framenum=0;
	d->generate_keyframe=FALSE;
	d->packer=NULL;
	f->data=d;

//jack
	d->favc_quant = 14;
	d->qmax = 51;
    d->qmin = 0;
    d->quant = 0;
    d->frame_rate_base = 1;
    d->gop_size = IPInterval;

    d->favc_enc_fd1 = 0;
//	d->out_h264_frame = (unsigned char *)malloc(YUV_420_IMAGE_SIZE(d->vsize.width ,d->vsize.height));
//	n32926_set_h264_init(f);
DEBUG_ENTER();
}


static void enc_uninit(MSFilter *f){
DEBUG_ENTER();
	EncData *d=(EncData*)f->data;
/*
	 if(d->favc_enc_fd1) {
        close(d->favc_enc_fd1);
    }
	free(d->out_h264_frame);
	d->out_h264_frame = NULL;
    d->favc_enc_fd1 = 0;
*/
	ms_free(d);
DEBUG_ENTER();
}


static void enc_preprocess(MSFilter *f){
DEBUG_ENTER();
	EncData *d=(EncData*)f->data;
	d->packer=rfc3984_new();
	rfc3984_set_mode(d->packer,d->mode);
	rfc3984_enable_stap_a(d->packer,FALSE);
	d->framenum=0;
	video_starter_init(&d->starter);
DEBUG_ENTER();
}

static void h264_frame_to_msgb(unsigned char * frame,int frame_size, MSQueue * nalus){
	mblk_t *m;
	m = allocb(frame_size+10,0);
	memcpy(m->b_wptr,frame,frame_size);
	ms_queue_put(nalus,m);
}

static void enc_process(MSFilter *f){
DEBUG_ENTER();
	EncData *d=(EncData*)f->data;
	unsigned int ts=f->ticker->time*90LL;
	mblk_t *im;
	MSPicture pic;
	MSQueue nalus;
	ms_queue_init(&nalus);
	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		ms_queue_put(&nalus,im);
		rfc3984_pack(d->packer,&nalus,f->outputs[0],ts);
		d->framenum++;
		if (d->framenum==0)
			video_starter_first_frame(&d->starter,f->ticker->time);
		if (ms_yuv_buf_init_from_mblk(&pic,im)==0){
			int num_nals=0;
			if (video_starter_need_i_frame(&d->starter,f->ticker->time))
				d->generate_keyframe=TRUE;

           		num_nals = n32926_favc_encode(f,pic.planes);
			if (num_nals > 0){
				h264_frame_to_msgb(d->out_h264_frame,num_nals,&nalus);
				rfc3984_pack(d->packer,&nalus,f->outputs[0],ts);
				d->framenum++;
				if (d->framenum==0)
					video_starter_first_frame(&d->starter,f->ticker->time);
			}else{
				ms_error("x264_encoder_encode() error.");
			}

		}
//		freemsg(im);
	}
DEBUG_ENTER();
}

static void enc_postprocess(MSFilter *f){
DEBUG_ENTER();
	EncData *d=(EncData*)f->data;
	rfc3984_destroy(d->packer);
	d->packer=NULL;
DEBUG_ENTER();
}

static int enc_get_br(MSFilter *f, void*arg){
	EncData *d=(EncData*)f->data;
	*(int*)arg=d->bitrate;
	return 0;
}

static int enc_set_br(MSFilter *f, void *arg){
	EncData *d=(EncData*)f->data;
	d->bitrate=*(int*)arg;

	if (d->bitrate>=1024000){
		d->vsize.width = MS_VIDEO_SIZE_SVGA_W;
		d->vsize.height = MS_VIDEO_SIZE_SVGA_H;
		d->fps=25;
	}else if (d->bitrate>=512000){
		d->vsize.width = MS_VIDEO_SIZE_VGA_W;
		d->vsize.height = MS_VIDEO_SIZE_VGA_H;
		d->fps=25;
	} else if (d->bitrate>=256000){
		d->vsize.width = MS_VIDEO_SIZE_VGA_W;
		d->vsize.height = MS_VIDEO_SIZE_VGA_H;
		d->fps=15;
	}else if (d->bitrate>=170000){
		d->vsize.width=MS_VIDEO_SIZE_QVGA_W;
		d->vsize.height=MS_VIDEO_SIZE_QVGA_H;
		d->fps=15;
	}else if (d->bitrate>=128000){
		d->vsize.width=MS_VIDEO_SIZE_QCIF_W;
		d->vsize.height=MS_VIDEO_SIZE_QCIF_H;
		d->fps=10;
	}else if (d->bitrate>=64000){
		d->vsize.width=MS_VIDEO_SIZE_QCIF_W;
		d->vsize.height=MS_VIDEO_SIZE_QCIF_H;
		d->fps=7;
	}else{
		d->vsize.width=MS_VIDEO_SIZE_QCIF_W;
		d->vsize.height=MS_VIDEO_SIZE_QCIF_H;
		d->fps=5;
	}


	ms_message("bitrate requested...: %d (%d x %d)\n", d->bitrate, d->vsize.width, d->vsize.height);
	return 0;
}

static int enc_set_fps(MSFilter *f, void *arg){
	EncData *d=(EncData*)f->data;
	d->fps=*(float*)arg;
	return 0;
}

static int enc_get_fps(MSFilter *f, void *arg){
	EncData *d=(EncData*)f->data;
	*(float*)arg=d->fps;
	return 0;
}

static int enc_get_vsize(MSFilter *f, void *arg){
	EncData *d=(EncData*)f->data;
	*(MSVideoSize*)arg=d->vsize;
	PRINT_SIZE(d->vsize);
	return 0;
}

static int enc_set_vsize(MSFilter *f, void *arg){
	EncData *d=(EncData*)f->data;
	d->vsize=*(MSVideoSize*)arg;
	PRINT_SIZE(d->vsize);
	return 0;
}


static int enc_req_vfu(MSFilter *f, void *arg){
	EncData *d=(EncData*)f->data;
	d->generate_keyframe=TRUE;
	return 0;
}

static MSFilterMethod enc_methods[]={
	{	MS_FILTER_GET_VIDEO_SIZE,	enc_get_vsize	},
	{	MS_FILTER_SET_VIDEO_SIZE,	enc_set_vsize	},
	{	0	,			NULL		}
};

/*

static MSFilterMethod enc_methods[]={
	{	MS_FILTER_SET_FPS	,	enc_set_fps	},
	{	MS_FILTER_SET_BITRATE	,	enc_set_br	},
	{	MS_FILTER_GET_BITRATE	,	enc_get_br	},
	{	MS_FILTER_GET_FPS	,	enc_get_fps	},
	{	MS_FILTER_GET_VIDEO_SIZE,	enc_get_vsize	},
	{	MS_FILTER_SET_VIDEO_SIZE,	enc_set_vsize	},
	{	MS_FILTER_REQ_VFU	,	enc_req_vfu	},
	{	0	,			NULL		}
};
*/
#ifndef _MSC_VER

static MSFilterDesc n32926_h264_enc_desc={
//	.id=MS_FILTER_PLUGIN_ID,
	.id=MS_H264_ENC_ID,
	.name="H264Enc",
	.text="A H264 encoder based on n32926 project",
	.category=MS_FILTER_ENCODER,
	.enc_fmt="H264",
	.ninputs=1,
	.noutputs=1,
/*
	.init=enc_init,
	.preprocess=enc_preprocess,
	.process=enc_process,
	.postprocess=enc_postprocess,
	.uninit=enc_uninit,
	.methods=enc_methods
	*/
	.init=enc_init,
	.preprocess=NULL,
	.process=NULL,
	.postprocess=NULL,
	.uninit=enc_uninit,
	.methods=enc_methods
};

#else
static MSFilterDesc n32926_h264_enc_desc={
//	MS_FILTER_PLUGIN_ID,
	MS_H264_ENC_ID,
	"H264Enc",
	"A H264 encoder based on n32926 project",
	MS_FILTER_ENCODER,
	"H264",
	1,
	1,
	enc_init,
	NULL,
	NULL,
	NULL,
	enc_uninit,
	enc_methods
/*
	enc_init,
	enc_preprocess,
	enc_process,
	enc_postprocess,
	enc_uninit,
	enc_methods
	*/

};

#endif

MS2_PUBLIC void n32926_h264_init(void){

	ms_filter_register(&n32926_h264_enc_desc);
}

//MS_FILTER_DESC_EXPORT(n32926_h264_enc_desc)


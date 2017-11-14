#ifndef __N32926_ENC_H__
#define __N32926_ENC_H__

#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msticker.h"
#include "mediastreamer2/msvideo.h"
#include "mediastreamer2/rfc3984.h"

#include "favc_avcodec.h"
#include "ratecontrol.h"
//#define DEBUG_ENTER()  do{ms_message("=======>Enter %s-%d\n",__func__,__LINE__);}while(0)
//#define DEBUG_ENTER()  do{printf("=======>Enter %s-%d\n",__func__,__LINE__);}while(0)
#define DEBUG_ENTER()

typedef struct VideoStarter{
	uint64_t next_time;
	int i_frame_count;
}VideoStarter;

typedef struct _EncData{
	MSVideoSize vsize;
	int bitrate;
	float fps;
	int mode;
	uint64_t framenum;
	Rfc3984Context *packer;
	int keyframe_int;
	bool_t generate_keyframe;
	VideoStarter starter;
//jack
	int favc_quant;
	int qmax;
    int qmin;
	int quant;               // Init Quant
	int frame_rate_base;
   	int gop_size;
	int favc_enc_fd1;	//enc fd
	unsigned int    out_h264_frame;
	FAVC_ENC_PARAM     enc_param;
	avc_workbuf_t	bitstreambuf, yuvframebuf;
	H264RateControl h264_ratec;
}EncData;

#define	VIDEO_CIF_FMT 	 0x00
#define	VIDEO_VGA_FMT 	 0x01
#define	VIDEO_D1_FMT 	 0x02
#define	VIDEO_720P_FMT 	 0x03
#define	VIDEO_1080P_FMT  0x04

//#define RATE_CTL

/*#define H264_ENC_WRITE_FILE*/
#ifdef H264_ENC_WRITE_FILE
#define WRITE_FRAME_COUNT	1000
#define dout_name1         "/home/meidastreamer_enc.264"
#define dinfo_name        "/home/meidastreamer_enc.txt"
FILE      *dout,  *dout_info;
void file_open()
{
    printf("==>file_open \n");
	dout=fopen(dout_name1,"wb");
    printf("Use encoder output name %s\n",dout_name1);

    dout_info = fopen(dinfo_name,"w");
    printf("Use encoder output info_name %s\n",dinfo_name);
}

void file_close()
{
    printf("==>file_close \n");
	sync();
    fclose(dout);
    fclose(dout_info);
}

int file_write(FILE* fd, void * buf,int length)
{
	int writelen = 0;
	writelen  = fwrite((void *)buf, sizeof(char), length, fd);
	printf("==>file_write %d \n",length);
	return writelen;
}
#endif

#define YUV_420_IMAGE_SIZE(width,height) ((width * height) / 2 * 3)

static int video_fmt_type = VIDEO_CIF_FMT;

int n32926_favc_init(EncData *f)
{
	DEBUG_ENTER();
	EncData * d = (EncData *)f;
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

	d->out_h264_frame = (unsigned int)mmap(NULL, d->bitstreambuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, d->favc_enc_fd1, d->bitstreambuf.offset);

	if((void *)d->out_h264_frame == MAP_FAILED)
	{
		printf("Map ENC Bitstream Buffer Failed!\n");
		return -1;;
	}
	printf("H1 Mapped ENC Bitstream Buffer Virtual addr = 0x%x\n",d->out_h264_frame);


    memset(&d->enc_param, 0, sizeof(FAVC_ENC_PARAM));
    d->enc_param.u32API_version = H264VER;
    d->enc_param.u32FrameWidth= d->vsize.width;
    d->enc_param.u32FrameHeight=d->vsize.height;

    d->enc_param.fFrameRate = d->fps;
    d->enc_param.u32MaxQuant       =d->qmax;
    d->enc_param.u32MinQuant       =d->qmin;
    d->enc_param.u32Quant = d->quant; //32


    d->enc_param.u32BitRate = d->bitrate;
    d->enc_param.ssp_output = 0;
    d->enc_param.intra = -1;
	d->enc_param.u32IPInterval = d->gop_size; //60, IPPPP.... I, next I frame interval



    d->enc_param.bROIEnable = 0;
    d->enc_param.u32ROIX = 0;
    d->enc_param.u32ROIY = 0;
    d->enc_param.u32ROIWidth = 0;
    d->enc_param.u32ROIHeight = 0;

#ifdef RATE_CTL
    memset(&d->h264_ratec, 0, sizeof(H264RateControl));
    H264RateControlInit(&d->h264_ratec, d->enc_param.u32BitRate,
        RC_DELAY_FACTOR,RC_AVERAGING_PERIOD, RC_BUFFER_SIZE_BITRATE,
        (int)d->enc_param.fFrameRate,
        (int) d->enc_param.u32MaxQuant,
        (int)d->enc_param.u32MinQuant,
        (unsigned int)d->enc_param.u32Quant,
        d->enc_param.u32IPInterval);
#endif

    if (ioctl(d->favc_enc_fd1, FAVC_IOCTL_ENCODE_INIT, &d->enc_param) < 0)
    {
        close(d->favc_enc_fd1);
        printf("Handler_1 Error to set FAVC_IOCTL_ENCODE_INIT\n");
        return -1;
    }
DEBUG_ENTER();
    return 0;
}

int n32926_favc_encode(EncData *f, void * data)
{

    AVFrame   *pict=(AVFrame *)data;
	EncData * d = (EncData *)f;
	int fileDecriptor = d->favc_enc_fd1;

    d->enc_param.pu8YFrameBaseAddr = (unsigned char *)pict->data[0];   //input user continued virtual address (Y), Y=0 when NVOP
    d->enc_param.pu8UFrameBaseAddr = (unsigned char *)pict->data[1];   //input user continued virtual address (U)
    d->enc_param.pu8VFrameBaseAddr = (unsigned char *)pict->data[2];   //input user continued virtual address (V)

    d->enc_param.bitstream = (void *)d->out_h264_frame;  //output User Virtual address
    d->enc_param.ssp_output = 0;
    d->enc_param.intra = -1;
	d->enc_param.u32IPInterval = d->gop_size;

    d->enc_param.u32Quant = d->favc_quant;
    d->enc_param.bitstream_size = 0;


    if (ioctl(fileDecriptor, FAVC_IOCTL_ENCODE_FRAME, &d->enc_param) < 0)
    {
        printf("Dev =%d Error to set FAVC_IOCTL_ENCODE_FRAME\n", fileDecriptor);
        return 0;
    }
//	printf("size:%d Y:%p U:%p V:%p\n",d->enc_param.bitstream_size,d->enc_param.pu8YFrameBaseAddr,d->enc_param.pu8UFrameBaseAddr,d->enc_param.pu8VFrameBaseAddr);


#ifdef RATE_CTL
    if (d->enc_param.keyframe == 0) {
        //printf("%d %d %d\n", enc_param.u32Quant, enc_param.bitstream_size, 0);
        H264RateControlUpdate(&d->h264_ratec, d->enc_param.u32Quant, d->enc_param.bitstream_size , 0);
    } else  {
        //printf("%d %d %d\n", enc_param.u32Quant, enc_param.bitstream_size, 1);
        H264RateControlUpdate(&d->h264_ratec, d->enc_param.u32Quant, d->enc_param.bitstream_size , 1);
    }
    d->favc_quant = d->h264_ratec.rtn_quant;

//  printf(" favc_quant = %d\n",d->favc_quant);
 #endif

#ifdef H264_ENC_WRITE_FILE
	static int write_frame_count = 0;
	write_frame_count++;
	if(write_frame_count < WRITE_FRAME_COUNT)
	{
		file_write(dout, d->out_h264_frame, d->enc_param.bitstream_size);
		fprintf(dout_info,"%d\n",d->enc_param.bitstream_size);
		printf("===>%d\n", write_frame_count);
	}
#endif
    return d->enc_param.bitstream_size;
}

void n32926_enc_fmt_set(int fmt)
{
	video_fmt_type = fmt;
}

EncData* n32926_enc_init(void){
	DEBUG_ENTER();
	EncData *d=ms_new(EncData,1);

//    d->vsize.width=MS_VIDEO_SIZE_QVGA_W;
//	d->vsize.height=MS_VIDEO_SIZE_QVGA_H;

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

	d->fps=30;
	//d->bitrate=512000;
	d->bitrate=0;
	d->keyframe_int=10; /*10 seconds */
	d->mode=1; //分包发送为1
	d->framenum=0;
	d->generate_keyframe=FALSE;
	d->packer=NULL;

//jack

#ifdef RATE_CTL
    d->quant = 14;               // Init Quant
#else
    d->quant = FIX_QUANT;
#endif

	d->qmax = 51;
    d->qmin = 0;
    d->favc_quant = FIX_QUANT;
    d->frame_rate_base = 1;
    d->gop_size = IPInterval;

    d->favc_enc_fd1 = 0;
	d->out_h264_frame = 0;

	#ifdef H264_ENC_WRITE_FILE
	file_open();
	#endif
	n32926_favc_init(d);
	d->packer=rfc3984_new();
	rfc3984_set_mode(d->packer,d->mode);
	rfc3984_enable_stap_a(d->packer,FALSE);

	DEBUG_ENTER();
	return d;

}


void n32926_enc_uninit(EncData *f){
DEBUG_ENTER();
	EncData *d=(EncData*)f;
	 if(d->favc_enc_fd1) {
        close(d->favc_enc_fd1);
    }

	if(d->out_h264_frame)
		munmap((void *)d->out_h264_frame, d->bitstreambuf.size);
	d->out_h264_frame = 0;
    d->favc_enc_fd1 = 0;


	#ifdef H264_ENC_WRITE_FILE
	file_close();
	#endif
	rfc3984_destroy(d->packer);
	d->packer=NULL;
	ms_free(d);
	d = NULL;
DEBUG_ENTER();
}

void h264_phy_to_av_frame(EncData *f,unsigned char *u32PicPhyAdr,AVFrame * pict){


	int y_image_size = f->vsize.width * f->vsize.height;
    pict->data[0]=(unsigned char *)u32PicPhyAdr;
    pict->data[1]=(unsigned char *)(u32PicPhyAdr + y_image_size);
    pict->data[2]=(unsigned char *)(u32PicPhyAdr + y_image_size +(y_image_size >> 2));

}

mblk_t * h264_frame_to_msgb(unsigned char * frame,int frame_size){
	mblk_t *m;
	m = allocb(frame_size+10,0);
	memcpy(m->b_wptr,frame,frame_size);
	m->b_wptr += frame_size;
	return m;
}

#endif

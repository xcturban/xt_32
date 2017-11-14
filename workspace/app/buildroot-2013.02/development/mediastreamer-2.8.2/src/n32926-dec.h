#ifndef __N32926_DEC_H__
#define __N32926_DEC_H__

#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/ioctl.h>


#include "favc_avcodec.h"
#include "w55fa92_vpe.h"

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <pthread.h>

#define DECODE_OUTPUT_PACKET_YUV422 0

#define MAX_IMG_WIDTH                   1280
#define MAX_IMG_HEIGHT                  720

static int favc_dec_fd = 0;
static int bitstreamsize;
static int decoded_img_width, decoded_img_height;
static int output_vir_buf=0;
static avc_workbuf_t	outputbuf;

static int first_time = 1;
static int first_time_Ex = 1;

static char vpe_device[] = "/dev/vpe";
static int vpe_fd;

//static uint8_t rgb_buf[270*240*2];
//static uint8_t rgb_buf[352*288*2];

static int toggle_flag;

// #define H264_FILE_SAVE
#ifdef H264_FILE_SAVE
static FILE *h264_file_fd = NULL;
#endif


/* VPOST related */
static struct fb_var_screeninfo g_fb_var;
static char *fbdevice = "/dev/fb0";
__u32 g_u32VpostWidth,  g_u32VpostHeight,  fb_bpp;
unsigned int FB_PAddress;
void* FB_VAddress;
unsigned int g_u32VpostBufMapSize = 0;	/* For munmap use */
#define _USE_FBIOPAN_

int fb_fd=-1;
static int video_fmt; //1 cif   >=2 vga 

#define DISPLAY_MODE_CBYCRY		4
#define DISPLAY_MODE_YCBYCR		5
#define DISPLAY_MODE_CRYCBY		6
#define VIDEO_FORMAT_CHANGE		_IOW('v', 50, unsigned int)	//frame buffer format change

#define IOCTL_LCD_GET_DMA_BASE      	_IOR('v', 32, unsigned int *)
#define IOCTL_LCD_ENABLE_INT		_IO('v', 28)
#define IOCTL_LCD_DISABLE_INT		_IO('v', 29)

int GetVideo_fromMedia(void)
{
    return video_fmt;
}


static void FiniFB()
{
    if(fb_fd>0)
    {
        //memset ( FB_VAddress, 0x0, g_u32VpostBufMapSize );
		printf("FiniFB\n");
        usleep(100);
        munmap(FB_VAddress, g_u32VpostBufMapSize);

        close(fb_fd);
        fb_fd = -1;
        g_u32VpostBufMapSize = 0;
        FB_VAddress = NULL;
    }
}

static int InitFB()
{
    fb_fd = open(fbdevice, O_RDWR);
    if (fb_fd == -1)
    {
        perror("open fbdevice fail");
        return -1;
    }
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &g_fb_var) < 0)
    {
        perror("ioctl FBIOGET_VSCREENINFO");
        close(fb_fd);
        fb_fd = -1;
        return -1;
    }
    g_u32VpostWidth = g_fb_var.xres;
    g_u32VpostHeight = g_fb_var.yres;
    fb_bpp = g_fb_var.bits_per_pixel/8;
    printf("FB width =%d, height = %d, bpp=%d\n",g_u32VpostWidth,g_u32VpostHeight,fb_bpp);
    // Get FB Virtual Buffer address
    if (ioctl(fb_fd, IOCTL_LCD_GET_DMA_BASE, &FB_PAddress) < 0)
    {
        perror("ioctl IOCTL_LCD_GET_DMA_BASE ");
        close(fb_fd);
        return -1;
    }

#ifdef _USE_FBIOPAN_
    g_u32VpostBufMapSize = g_u32VpostWidth*g_u32VpostHeight*fb_bpp*2;	/* MAP 2 buffer */
#else
    g_u32VpostBufMapSize = g_u32VpostWidth*g_u32VpostHeight*fb_bpp*1;	/* MAP 1 buffer */
#endif
    FB_VAddress = mmap(NULL, 	g_u32VpostBufMapSize,
                       PROT_READ|PROT_WRITE,
                       MAP_SHARED,
                       fb_fd,
                       0);

    if ( FB_VAddress == MAP_FAILED)
    {
        printf("LCD Video Map failed!\n");
        close(fb_fd);
        return -1;
    }

    memset (FB_VAddress, 0x0, g_u32VpostBufMapSize );

    return 0;
}

static int WaitVPEngine()
{
    unsigned int value;
    int ret=0;
    while ( ioctl (vpe_fd, VPE_WAIT_INTERRUPT, &value) )
    {
        if (errno == EINTR)
        {
            continue;
        }
        else
        {
            printf ("%s: VPE_WAIT_INTERRUPT failed: %s\n", __FUNCTION__, strerror (errno));
            ret = -1;
            break;
        }
    }
    return ret;
}


#define VIDEO_OFFSET_TOP	(132)
#define VIDEO_OFFSET_LEFT	((800 - 352)/2)

static int FormatConversion(char* pSrcBuf, char* pDstBuf, int Srcwidth, int Srcheight, int Tarwidth, int Tarheight)
{
    //int step = 60;
    int top_spacing = VIDEO_OFFSET_TOP;//60*2;
    int margin = VIDEO_OFFSET_LEFT;//5+1+1+1+1;
    //int width = 272 - 2*margin+1-1;
    //int height = 60*4-5;



    unsigned int value, ret =0;
    vpe_transform_t vpe_setting;
    unsigned int ptr_y, ptr_u, ptr_v, step=0;
    ptr_y = (unsigned int)pSrcBuf;

    /* Planar YUV420 */
    ptr_u = ptr_y+MAX_IMG_WIDTH*MAX_IMG_HEIGHT;
    ptr_v = ptr_u+MAX_IMG_WIDTH*MAX_IMG_HEIGHT/4;

    WaitVPEngine();

    // Get Video Decoder IP Register size
    if( ioctl(vpe_fd, VPE_INIT, NULL)  < 0)
    {
        close(vpe_fd);
        printf("VPE_INIT fail\n");
        ret = -1;
    }

    value = 0x5a;
    if((ioctl(vpe_fd, VPE_IO_SET, &value)) < 0)
    {
        close(vpe_fd);
        printf("VPE_IO_SET fail\n");
        ret = -1;
    }

    //value = 1;
    value = 0;
    ioctl(vpe_fd, VPE_SET_MMU_MODE, &value);


    vpe_setting.src_addrPacY = ptr_y;
    vpe_setting.src_addrU = ptr_u;
    vpe_setting.src_addrV = ptr_v;
#if DECODE_OUTPUT_PACKET_YUV422
    vpe_setting.src_format = VPE_SRC_PACKET_YUV422;
#else
    vpe_setting.src_format = VPE_SRC_PLANAR_YUV420;
#endif
    vpe_setting.src_width = (Srcwidth+3) & ~0x03;//Srcwidth;
    vpe_setting.src_height = Srcheight;

    vpe_setting.src_leftoffset = 0;
    vpe_setting.src_rightoffset = ((Srcwidth + 15) & ~0x0F) - Srcwidth;//0;
	vpe_setting.dest_addrPac = (unsigned int)(pDstBuf + (480-margin-Tarwidth)*g_u32VpostWidth*2);
    vpe_setting.dest_format = VPE_DST_PACKET_RGB565;
    vpe_setting.dest_width = Tarwidth;
    vpe_setting.dest_height = Tarheight;

    if (g_u32VpostHeight > Tarheight)    // Put image at the center of panel
    {
        int offset_Y;
		offset_Y = VIDEO_OFFSET_TOP * g_u32VpostWidth * 2;      // For RGB565
        vpe_setting.dest_addrPac = (unsigned int)pDstBuf + offset_Y;
    }

    if (g_u32VpostWidth > Tarwidth )
    {
        vpe_setting.dest_leftoffset = VIDEO_OFFSET_LEFT;
        vpe_setting.dest_rightoffset = VIDEO_OFFSET_LEFT;
    }
    else
    {
        vpe_setting.dest_width = g_u32VpostWidth;
        vpe_setting.dest_height = g_u32VpostHeight;
    }
    vpe_setting.algorithm = VPE_SCALE_DDA;
	vpe_setting.rotation = VPE_OP_UPSIDEDOWN;//VPE_OP_NORMAL;

    if((ioctl(vpe_fd, VPE_SET_FORMAT_TRANSFORM, &vpe_setting)) < 0)
    {
        close(vpe_fd);
		printf("VPE_IO_GET fail\n");
        ret = -1;
    }

    if((ioctl(vpe_fd, VPE_TRIGGER, NULL)) < 0)
    {
        close(vpe_fd);
		printf("VPE_TRIGGER info fail\n");
        ret = -1;
    }
    return ret;
}

//big
static int FormatConversionExx(char* pSrcBuf, char* pDstBuf, int Srcwidth, int Srcheight, int Tarwidth, int Tarheight)
{
    int top_spacing = 10;
    int margin      = 20;



    unsigned int value, ret =0;
    vpe_transform_t vpe_setting;
    unsigned int ptr_y, ptr_u, ptr_v, step=0;
    ptr_y = (unsigned int)pSrcBuf;

    /* Planar YUV420 */
    ptr_u = ptr_y+MAX_IMG_WIDTH*MAX_IMG_HEIGHT;
    ptr_v = ptr_u+MAX_IMG_WIDTH*MAX_IMG_HEIGHT/4;

    WaitVPEngine();

    // Get Video Decoder IP Register size
    if( ioctl(vpe_fd, VPE_INIT, NULL)  < 0)
    {
        close(vpe_fd);
        printf("VPE_INIT fail\n");
        ret = -1;
    }

    value = 0x5a;
    if((ioctl(vpe_fd, VPE_IO_SET, &value)) < 0)
    {
        close(vpe_fd);
        printf("VPE_IO_SET fail\n");
        ret = -1;
    }

    //value = 1;
    value = 0;
    ioctl(vpe_fd, VPE_SET_MMU_MODE, &value);


    vpe_setting.src_addrPacY = ptr_y;
    vpe_setting.src_addrU = ptr_u;
    vpe_setting.src_addrV = ptr_v;
#if DECODE_OUTPUT_PACKET_YUV422
    vpe_setting.src_format = VPE_SRC_PACKET_YUV422;
#else
    vpe_setting.src_format = VPE_SRC_PLANAR_YUV420;
#endif
    vpe_setting.src_width = (Srcwidth+3) & ~0x03;//Srcwidth;
    vpe_setting.src_height = Srcheight;

    vpe_setting.src_leftoffset = 0;
    vpe_setting.src_rightoffset = ((Srcwidth + 15) & ~0x0F) - Srcwidth;//0;
	vpe_setting.dest_addrPac = (unsigned int)(pDstBuf + (480-margin-Tarwidth)*g_u32VpostWidth*2);
    vpe_setting.dest_format = VPE_DST_PACKET_RGB565;
    vpe_setting.dest_width = Tarwidth;
    vpe_setting.dest_height = Tarheight;

    if (g_u32VpostHeight > Tarheight)    // Put image at the center of panel
    {
        int offset_Y;
		offset_Y = 10 * g_u32VpostWidth * 2;      // For RGB565
        vpe_setting.dest_addrPac = (unsigned int)pDstBuf + offset_Y;
    }

    if (g_u32VpostWidth > Tarwidth )
    {
        vpe_setting.dest_leftoffset = 800-20-600;
        vpe_setting.dest_rightoffset = 20;
    }
    else
    {
        vpe_setting.dest_width = g_u32VpostWidth;
        vpe_setting.dest_height = g_u32VpostHeight;
    }
    vpe_setting.algorithm = VPE_SCALE_DDA;
	vpe_setting.rotation = VPE_OP_UPSIDEDOWN;//VPE_OP_NORMAL;

    if((ioctl(vpe_fd, VPE_SET_FORMAT_TRANSFORM, &vpe_setting)) < 0)
    {
        close(vpe_fd);
		printf("VPE_IO_GET fail\n");
        ret = -1;
    }

    if((ioctl(vpe_fd, VPE_TRIGGER, NULL)) < 0)
    {
        close(vpe_fd);
		printf("VPE_TRIGGER info fail\n");
        ret = -1;
    }
    return ret;
}

static int InitVPE(void)
{
    unsigned int version;

    vpe_fd = open(vpe_device, O_RDWR);
    if (vpe_fd < 0)
    {
        printf("open %s error\n", vpe_device);
        return -1;
    }
    else
        printf("open %s successfully\n", vpe_device);

    return 0;
}


static void n32926_h264_decode_uninit(void)
{
    if (output_vir_buf)
    {
        munmap((char *)output_vir_buf, outputbuf.size);
        output_vir_buf=0;
    }

    if(favc_dec_fd)
    {
        close(favc_dec_fd);
        favc_dec_fd = 0;
    }

    first_time    = 1;
    first_time_Ex = 1;
    video_fmt     = 1;

    close(vpe_fd);

    FiniFB();

#ifdef H264_FILE_SAVE
    if (h264_file_fd)
    {
        fclose(h264_file_fd);
        h264_file_fd = NULL;
    }
#endif

}

static int n32926_h264_decode_init(void)
{
    FAVC_DEC_PARAM      tDecParam;
    int                 ret_value;

    printf("enter n32926_h264_decode_init\n");
    if(favc_dec_fd == 0)
        favc_dec_fd = open(FAVC_DECODER_DEV,O_RDWR);

    if(favc_dec_fd <= 0)
    {
        printf("Fail to open %s\n",FAVC_DECODER_DEV);
        return -1;
    }

    if((ioctl(favc_dec_fd, FAVC_IOCTL_DECODE_GET_BSSIZE, &bitstreamsize)) < 0)
    {
        close(favc_dec_fd);
        printf("Get vde bitstream size fail\n");
    }

    printf("bitstreamsize=%d\n", bitstreamsize);

    if((ioctl(favc_dec_fd, FAVC_IOCTL_DECODE_GET_OUTPUT_INFO, &outputbuf)) < 0)
    {
        close(favc_dec_fd);
        printf("Get output buffer size fail\n");
        return -1;
    }
    printf("output buf addr = 0x%x, size=0x%x\n",outputbuf.addr, outputbuf.size);

    output_vir_buf = (unsigned int)mmap(NULL, outputbuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, favc_dec_fd, 0);
    printf("mmap output_vir_buf = 0x%x\n",output_vir_buf);

    if((void *)output_vir_buf == MAP_FAILED)
    {
        printf("Map Output Buffer Failed!\n");
        return -1;;
    }

#if (DECODE_OUTPUT_PACKET_YUV422 ==0)
    if (outputbuf.size ==0)
        return -1;
#endif

    memset(&tDecParam, 0, sizeof(FAVC_DEC_PARAM));
    tDecParam.u32API_version = H264VER;
    tDecParam.u32MaxWidth = -1;
    tDecParam.u32MaxHeight = -1;
//#if OUTPUT_FILE
#if 1
    // For file output or VPE convert planr to packet format
    tDecParam.u32FrameBufferWidth = -1;
    tDecParam.u32FrameBufferHeight = -1;
#else
    // For TV output. H.264 output packet to FB off-screen buffer
    tDecParam.u32FrameBufferWidth = 640;
    tDecParam.u32FrameBufferHeight = 480;
#endif

#if DECODE_OUTPUT_PACKET_YUV422
    tDecParam.u32OutputFmt = 1; // 1->Packet YUV422 format, 0-> planar YUV420 foramt
#else
    tDecParam.u32OutputFmt = 0; // 1->Packet YUV422 format, 0-> planar YUV420 foramt
#endif

    ret_value = ioctl(favc_dec_fd,FAVC_IOCTL_DECODE_INIT,&tDecParam);		// Output : Packet YUV422 or Planar YUV420
    if(ret_value < 0)
    {
        printf("FAVC_IOCTL_DECODE_INIT: memory allocation failed\n");
        return -1;
    }

    InitVPE();

	InitFB();

    toggle_flag = 1;

#ifdef H264_FILE_SAVE
    if (!h264_file_fd)
    {
        h264_file_fd = fopen("/mnt/sd/frames.h264", "w");
        if (!h264_file_fd)
            printf("fopen h264_file failed\n");
    }
#endif



    printf("n32926_h264_decode_init ok\n");
    return 0;
}

static int __n32926_h264_decode(unsigned char *frame, void *data, int size)
{
    AVFrame             *pict=(AVFrame *)data;
    FAVC_DEC_PARAM      tDecParam;
    FAVC_DEC_RESULT		ptResut;
    int  ret_value;
    int j;

#ifdef H264_FILE_SAVE
    if (h264_file_fd)
    {
        int ret = fwrite(frame, size, 1, h264_file_fd);
        if (ret != 1)
            printf("fwrite h264_file failed\n");
    }
#endif

    // set display virtual addr (YUV or RGB)
//YUV422 needn't assign U,V
    memset(&tDecParam, 0, sizeof(FAVC_DEC_PARAM));
    tDecParam.pu8Display_addr[0] = (unsigned int)pict->data[0];
    tDecParam.pu8Display_addr[1] = (unsigned int)pict->data[1];
    tDecParam.pu8Display_addr[2] = (unsigned int)pict->data[2];
    tDecParam.u32Pkt_size =	(unsigned int)size;
    tDecParam.pu8Pkt_buf = frame;

    tDecParam.crop_x = 0;
    tDecParam.crop_y = 0;

#if DECODE_OUTPUT_PACKET_YUV422
    tDecParam.u32OutputFmt = 1; // 1->Packet YUV422 format, 0-> planar YUV420 foramt
#else
    tDecParam.u32OutputFmt = 0;
#endif

    if((ret_value = ioctl(favc_dec_fd, FAVC_IOCTL_DECODE_FRAME, &tDecParam)) != 0)
    {
        printf("FAVC_IOCTL_DECODE_FRAME: Failed.ret=%x\n",ret_value);
        return -1;
    }

    decoded_img_width = tDecParam.tResult.u32Width;
    decoded_img_height = tDecParam.tResult.u32Height;


    if (tDecParam.tResult.isDisplayOut ==0)
        return 0;
    else
        return tDecParam.got_picture;
}

struct _mblk_video_header
{
    uint16_t w, h;
    int pad[3];
};
typedef struct _mblk_video_header mblk_video_header;
static mblk_t *yuv_mblk_create(void *data)
{
    uint8_t *yuv_buf = (uint8_t *)output_vir_buf;
    AVFrame             *pict=(AVFrame *)data;
    int size=(decoded_img_width*decoded_img_height*3)/2;
    const int header_size =sizeof(mblk_video_header);
    const int padding=16;
    mblk_t *msg=allocb(header_size + size+padding,0);
    // write width/height in header
    mblk_video_header* hdr = (mblk_video_header*)msg->b_wptr;
    int len;
    hdr->w = decoded_img_width;
    hdr->h = decoded_img_height;
    msg->b_rptr += header_size;
    msg->b_wptr += header_size;

    len = decoded_img_width*decoded_img_height;
    memcpy(msg->b_wptr, yuv_buf, len);
    msg->b_wptr+=len;
    yuv_buf+=len;
    len = decoded_img_width*decoded_img_height/4;
    memcpy(msg->b_wptr, yuv_buf, len);
    msg->b_wptr+=len;
    yuv_buf+=len;
    memcpy(msg->b_wptr, yuv_buf, len);
    msg->b_wptr+=len;

    return msg;
}

static long long get_now_ms()
{
    long long millisecond = 0;
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    millisecond = tv.tv_sec;
    millisecond = millisecond * 1000 + tv.tv_usec / 1000;
    return millisecond;
}


typedef int (*h264_decode_started_cb)(void);
h264_decode_started_cb __h264_decode_started_cb = NULL;

void Set_h264_decode_started_cb(h264_decode_started_cb func)
{
    __h264_decode_started_cb = func;
}

static void n32926_h264_decode(MSFilter *f, unsigned char *frame, int size)
{
	AVFrame  pict[2], *pict_ptr;

	static int display = 0;
	long long start, end;

	/*static int loop = 0;*/

	pict[0].data[0] = (unsigned char *)outputbuf.addr;
	pict[0].data[1] = (unsigned char *)(outputbuf.addr+ (MAX_IMG_WIDTH*MAX_IMG_HEIGHT));
	pict[0].data[2] = (unsigned char *)(outputbuf.addr+ (MAX_IMG_WIDTH*MAX_IMG_HEIGHT*5/4));

	pict[1].data[0] = (unsigned char *)outputbuf.addr + (outputbuf.size/2) ;
	pict[1].data[1] = (unsigned char *)(outputbuf.addr+ (outputbuf.size/2) + (MAX_IMG_WIDTH*MAX_IMG_HEIGHT));
	pict[1].data[2] = (unsigned char *)(outputbuf.addr+ (outputbuf.size/2) + (MAX_IMG_WIDTH*MAX_IMG_HEIGHT*5/4));


	pict_ptr = &pict[0];

    if (first_time && ((frame[4] & 0x0f) != 0x07))
    {
        return;
    }
    else
    {
        first_time = 0;
    }

	if (__n32926_h264_decode(frame, (void *)pict_ptr, size) == 1)
	{


        uint8_t *yuv_buf = (uint8_t *)output_vir_buf;
        BITMAP bm;

        int top_spacing = VIDEO_OFFSET_TOP;;
		int margin = VIDEO_OFFSET_LEFT;//5+1+1+1+1;
		int width = 352;
		int height = 288;

		char *pDstBuf = NULL;

#ifdef _USE_FBIOPAN_

		if (toggle_flag)
		{
			toggle_flag = 0;
			g_fb_var.yoffset = 0;
			pDstBuf = FB_PAddress+g_u32VpostBufMapSize/2;
		}
		else
		{
			toggle_flag = 1;
			g_fb_var.yoffset = g_fb_var.yres;
			pDstBuf = FB_PAddress;
		}
#else
		pDstBuf = FB_PAddress;
#endif

		if(decoded_img_width >= 640 && decoded_img_height >= 480)
		{
			top_spacing = 10  ;
			margin      = 20  ;
			width       = 600 ;
			height      = 420 ;
            video_fmt   = 2;
			FormatConversionExx((char *)outputbuf.addr, pDstBuf, decoded_img_width, decoded_img_height, width, height);
		}
		else
		{
			top_spacing = VIDEO_OFFSET_TOP;
			margin      = VIDEO_OFFSET_LEFT;
			width       = 352;
			height      = 288;
            video_fmt   = 1;
			FormatConversion((char *)outputbuf.addr, pDstBuf, decoded_img_width, decoded_img_height, width, height);
		}
        if (first_time_Ex)
        {
            if (__h264_decode_started_cb != NULL)
            {
                first_time_Ex = 0;
                __h264_decode_started_cb();
            }
        }

#ifdef _USE_FBIOPAN_
		if (ioctl(fb_fd, FBIOPAN_DISPLAY, &g_fb_var) < 0)
		{
			printf("ioctl FBIOPAN_DISPLAY\n");
		}
#endif
	}
}

#endif

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <common_lib/common_lib.h>

#include "V4L.h"
#include "w55fa92_vpe.h"
#include "system_info.h"
//#include "window_focus.h"
#include "Bmpdec.h"
#include "JpegDec.h"

#include "mediastreamer2/mediastream.h"
#include "mediastreamer2/waveheader.h"

#define DECODE_OUTPUT_PACKET_YUV422 0

#define VIDEO_WIDTH	 640	
#define VIDEO_HEIGHT 480	

#define VIDEO_TO_WIDTH	600
#define VIDEO_TO_HEIGHT	420

static pthread_t tid;
static int thread_start;

static char vpe_device[] = "/dev/vpe";
static int vpe_fd;

#define VIDEO_OFFSET_TOP	70
#define VIDEO_OFFSET_LEFT	230

static int FormatConversion(char* pSrcBuf, char* pDstBuf, int Srcwidth, int Srcheight, int Tarwidth, int Tarheight)
{
    unsigned int value, ret =0;
    vpe_transform_t vpe_setting;
    unsigned int ptr_y, ptr_u, ptr_v, step=0;
    ptr_y = (unsigned int)pSrcBuf;

    /* Planar YUV420 */
    ptr_u = ptr_y+Srcwidth*Srcheight;
    ptr_v = ptr_u+Srcwidth*Srcheight/4;

    // Get Video Decoder IP Register size
    if( ioctl(vpe_fd, VPE_INIT, NULL)  < 0){
        close(vpe_fd);
        SPON_LOG_ERR("=====>%s - VPE_INIT fail\n", __func__);
        ret = -1;
    }

    value = 0x5a;
    if((ioctl(vpe_fd, VPE_IO_SET, &value)) < 0){
        close(vpe_fd);
        SPON_LOG_ERR("=====>%s - VPE_IO_SET fail\n", __func__);
        ret = -1;
    }

    value = 1;
    ioctl(vpe_fd, VPE_SET_MMU_MODE, &value);


    vpe_setting.src_addrPacY = ptr_y;
    vpe_setting.src_addrU = ptr_u;
    vpe_setting.src_addrV = ptr_v;
#if DECODE_OUTPUT_PACKET_YUV422
    vpe_setting.src_format = VPE_SRC_PACKET_YUV422;
#else
    vpe_setting.src_format = VPE_SRC_PLANAR_YUV420;
#endif
    vpe_setting.src_width = Srcwidth;
    vpe_setting.src_height = Srcheight;

    vpe_setting.src_leftoffset = 0;
    vpe_setting.src_rightoffset = 0;
    vpe_setting.dest_addrPac = (unsigned int)pDstBuf;
    vpe_setting.dest_format = VPE_DST_PACKET_RGB565;
    vpe_setting.dest_width = Tarwidth;
    vpe_setting.dest_height = Tarheight;
    vpe_setting.dest_leftoffset = 0;
    vpe_setting.dest_rightoffset = 0;
    vpe_setting.algorithm = VPE_SCALE_DDA;
    vpe_setting.rotation = VPE_OP_NORMAL;

    if((ioctl(vpe_fd, VPE_SET_FORMAT_TRANSFORM, &vpe_setting)) < 0){
        close(vpe_fd);
        SPON_LOG_ERR("=====>%s - VPE_IO_GET fail\n", __func__);
        ret = -1;
    }

    if((ioctl(vpe_fd, VPE_TRIGGER, NULL)) < 0){
        close(vpe_fd);
        SPON_LOG_ERR("====>%s - VPE_TRIGGER info fail\n", __func__);
        ret = -1;
    }

    while (ioctl (vpe_fd, VPE_WAIT_INTERRUPT, &value)) {
        if (errno == EINTR) {
            continue;
        }
        else {
            SPON_LOG_ERR("=====>%s - VPE_WAIT_INTERRUPT failed:%s\n", __func__, strerror (errno));
            ret = -1;
            break;
        }
    }


    return ret;

}


static int InitVPE(void)
{
	unsigned int version;

	vpe_fd = open(vpe_device, O_RDWR);
	if (vpe_fd < 0){
		SPON_LOG_ERR("=====>%s - open %s error\n", __func__, vpe_device);
		return -1;
	}
	else
	{
		SPON_LOG_ERR("=====>%s - open %s successfully\n", __func__, vpe_device);
	}

	return 0;
}

void Capture_video_frame_get(void)
{
	uint8_t *pu8PicPtr;
	uint64_t u64TS;
	uint32_t u32PicPhyAdr;
	struct bmp_file myfile;
	int i=0;
	FILE *fp;

	clock_t start;
	clock_t finish;
	start = clock();

	if (InitV4LDevice(VIDEO_WIDTH, VIDEO_HEIGHT) != ERR_V4L_SUCCESS){
		SPON_LOG_ERR("=====>%s - Init V4L device fail\n", __func__);
		return;
	}
	StopV4LCapture();
	StartV4LCapture();

//	Bmp_init(&myfile);

	while(i < 2)
	{
		if(ReadV4LPicture(&pu8PicPtr, &u64TS, &u32PicPhyAdr) == ERR_V4L_SUCCESS){

			finish = clock();
			printf("%f\n",(double)(finish - start)/CLOCKS_PER_SEC);

			Jpeg_Init(pu8PicPtr);
#if 0
			fp = fopen("send.bmp", "w");
			cvt_420p_to_rgb(320, 240, pu8PicPtr, myfile.bits);

			start = clock();
			printf("%f\n",(double)(start - finish)/CLOCKS_PER_SEC);

			fwrite(&myfile, sizeof(struct bmp_file), 1, fp);
		    fclose(fp);
#endif /* if 0. 2015-9-30 9:39:17 JiangYi */

			start = clock();
			printf("%f\n",(double)(start - finish)/CLOCKS_PER_SEC);

			TriggerV4LNextFrame();
			i++;
		}
	}
//	}
	FinializeV4LDevice();

}



//#define VIDEO_FS_HANDLE 1

static void *local_video_view_thread(void *arg)
{
	PRECT rect = (PRECT)arg;
	uint8_t *pu8PicPtr;
	uint64_t u64TS;
	uint32_t u32PicPhyAdr;
	uint8_t rgb[VIDEO_WIDTH*VIDEO_HEIGHT*2];
	int margin = VIDEO_OFFSET_LEFT;//system_info.margin + 1+1+1+1;;
	int step = system_info.step;
	int y_start = VIDEO_OFFSET_TOP;//2*step;
	int width = VIDEO_TO_WIDTH;//g_rcScr.right - 2*margin +1-1;
	int height = VIDEO_TO_HEIGHT;//step*4-5;
    static int i = 0;
	int ret = 0;
#ifdef VIDEO_FS_HANDLE
    char file[128];
    FILE *fp;

    sprintf(file, "yuv_test_%d.yuv", i);
    fp = fopen(file, "w");
#endif

	if (InitV4LDevice(VIDEO_WIDTH, VIDEO_HEIGHT) != ERR_V4L_SUCCESS){
		SPON_LOG_ERR("=====>%s - Init V4L device fail\n", __func__);
		thread_start = 0;
		return;
	}
	StopV4LCapture();
	StartV4LCapture();

	InitVPE();

	while (thread_start == 1) {

	    if(ReadV4LPicture(&pu8PicPtr, &u64TS, &u32PicPhyAdr) == ERR_V4L_SUCCESS){
			BITMAP bm;

		    TriggerV4LNextFrame();
			FormatConversion(pu8PicPtr, rgb, VIDEO_WIDTH, VIDEO_HEIGHT, width, height);

			InvalidateRect_Ex(system_info.main_hwnd, rect, TRUE); 
            system_info.hdc = BeginPaint(system_info.main_hwnd);
			InitBitmap(system_info.hdc, width, height, width*2, rgb, &bm);

			//sprintf(file, "bmp_%d.bmp", i++);
			//SaveBitmap (system_info.hdc, &bm, file);
			
			ret = FillBoxWithBitmap(system_info.hdc, rect->left, rect->top, width, height, &bm);
			if(ret == 0)
				printf("load failed\n");
            EndPaint(system_info.main_hwnd, system_info.hdc);
            
#ifdef VIDEO_FS_HANDLE
		    fwrite(pu8PicPtr, VIDEO_WIDTH*VIDEO_HEIGHT*3/2, 1, fp);
#endif

	    }
	}
#ifdef VIDEO_FS_HANDLE
    fclose(fp);
    i++;
#endif
	close(vpe_fd);
	FinializeV4LDevice();
	thread_start = 0;

	return NULL;
}

void local_video_start(PRECT pRc)
{
	if (thread_start == 0) {
		thread_start = 1;
		pthread_create(&tid, NULL, local_video_view_thread, pRc);
	}
}


void local_video_stop(void)
{
	if (thread_start == 1) {
		thread_start = 0;
		pthread_join(tid, NULL);
	}
}

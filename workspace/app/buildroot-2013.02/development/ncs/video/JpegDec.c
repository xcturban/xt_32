#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <string.h>
#include "JpegDec.h"

jpeg_param_t s_sJpegParam;
unsigned int s_u32JpegBufSize;
unsigned char  *s_pu8JpegEncBuf = MAP_FAILED;
jpeg_info_t *s_pJpegInfo = NULL;
int s_i32JpegFd = -1;

static int FinializeJpegDevice(void)
{
	if (s_pJpegInfo) {
		free(s_pJpegInfo);
		s_pJpegInfo = NULL;
	}

	if (s_pu8JpegEncBuf !=  MAP_FAILED) {
		munmap(s_pu8JpegEncBuf, s_u32JpegBufSize);
		s_pu8JpegEncBuf = MAP_FAILED;
	}

	if (s_i32JpegFd > 0) {
		close(s_i32JpegFd);
		s_i32JpegFd = -1;
	}

	return 0;
}

static int JpegEncFromUser(unsigned char *pu8SrcPic, unsigned char **pu8JpegEncBuf, unsigned int *pu32JpegEncLen)
{
	int32_t i32Len;

	*pu8JpegEncBuf = NULL;
	*pu32JpegEncLen = 0;

	s_sJpegParam.encode_source_format	= DRVJPEG_ENC_SRC_PLANAR;	/* Source is Planar Format */
	s_sJpegParam.src_bufsize			= s_sJpegParam.encode_width * s_sJpegParam.encode_height * 3/2;
	s_sJpegParam.dst_bufsize			= s_u32JpegBufSize - s_sJpegParam.src_bufsize;

	if ((ioctl(s_i32JpegFd, JPEG_S_PARAM, &s_sJpegParam)) < 0) {
		printf("Set jpeg parame failed\n");
		return -1;
	}
	memcpy(s_pu8JpegEncBuf, pu8SrcPic, s_sJpegParam.src_bufsize);

	if ((ioctl(s_i32JpegFd, JPEG_TRIGGER, NULL)) < 0) {
		printf("Trigger jpeg failed:\n");
		return -1;
	}
	i32Len = read(s_i32JpegFd, s_pJpegInfo, sizeof(jpeg_info_t) + sizeof(__u32));

	if (i32Len < 0) {
		printf("No data can get\n");
		if (s_pJpegInfo->state == JPEG_MEM_SHORTAGE)
			printf("Memory Shortage\n");
		return -1;
	}

	if (s_pJpegInfo->image_size[0] > s_sJpegParam.dst_bufsize)	{
		printf("The encoded bitstream size is more than dst buffer size we gave.\n");
		printf("[Important] The kernel is damaged by H/W Jpeg codec.\n");
		printf("[Important] You should enlarge buffer length of H/W JPEG driver in kernel menuconfig.\n");
		return -1;
	}
	*pu8JpegEncBuf = s_pu8JpegEncBuf + s_sJpegParam.src_bufsize;
	*pu32JpegEncLen = s_pJpegInfo->image_size[0];
	return 0;
}

int Jpeg_Init(unsigned char *src_image)
{
	unsigned char  *pJpgBuffer = NULL;
	unsigned int 	ui32JpegSize = 0;
	unsigned char  *pJpegImgBuffer;

	//pJpegImgBuffer = (unsigned char *)malloc(320*240*2 * sizeof(unsigned char));
	pJpegImgBuffer = src_image;

	s_i32JpegFd = open("/dev/video1", O_RDWR);
	if (s_i32JpegFd < 0)
	{
		printf("open fail!\n");
		return -1;
	}

	if ((ioctl(s_i32JpegFd, JPEG_GET_JPEG_BUFFER, &s_u32JpegBufSize)) < 0)
	{
		close(s_i32JpegFd);
		printf("get jpeg buffer fail\n");
		return -1;
	}

	s_pJpegInfo = malloc(sizeof(jpeg_info_t) + sizeof(__u32));


	if (s_pJpegInfo == NULL) {
		printf("Allocate JPEG info failed\n");
		close(s_i32JpegFd);
		return -1;
	}

	s_pu8JpegEncBuf = mmap(NULL, s_u32JpegBufSize, PROT_READ | PROT_WRITE, MAP_SHARED, s_i32JpegFd, 0);
	if (s_pu8JpegEncBuf == MAP_FAILED) {
		printf("JPEG Map Failed!\n");
		close(s_i32JpegFd);
		return -1;
	}

	memset((void *)&s_sJpegParam, 0, sizeof(s_sJpegParam));
	s_sJpegParam.encode = 1;						/* Encode */

	s_sJpegParam.encode_width = 320;			/* JPEG width */
	s_sJpegParam.encode_height = 240;			/* JPGE Height */


	s_sJpegParam.encode_image_format = DRVJPEG_ENC_PRIMARY_YUV420;

	s_sJpegParam.buffersize = 0;		/* only for continuous shot */
	s_sJpegParam.buffercount = 1;

	s_sJpegParam.scale = 0;		/* Scale function is disabled when scale 0 */
	s_sJpegParam.qscaling = 1;
	s_sJpegParam.qadjust = 0xf;

	int ret = 0;
	if((ret = JpegEncFromUser(pJpegImgBuffer,	&pJpgBuffer, &ui32JpegSize))<0)
	{
		printf("jpegEncFromUser Failed ret = %d\n", ret);
		goto Fail_JpegEncFromUser;
	}

	char file_name[128] = "test_jpeg.jpg";
	int *fp = fopen(file_name, "w");

	if (!fp) {
		fprintf(stdout, "File %s could not be opened for writing", file_name);
		goto Fail_JpegEncFromUser;
	}

	if (ui32JpegSize != fwrite((const void *)pJpgBuffer, 1, ui32JpegSize, fp)) {
		fprintf(stdout, "Write file\n");
		goto Fail_JpegWriteFile;
	}

	fflush(fp);
	fsync(fileno(fp));

Fail_JpegWriteFile:
	fclose(fp);

Fail_JpegEncFromUser:
	FinializeJpegDevice();
	if (pJpegImgBuffer)
		free(pJpegImgBuffer);
	return 0;
}

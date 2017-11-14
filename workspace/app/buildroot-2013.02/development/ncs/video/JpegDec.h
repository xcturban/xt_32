#ifndef MJPEG_H
#define MJPEG_H

typedef unsigned int  __u32;
typedef unsigned char  __u8;

typedef enum jpeg_state {
  JPEG_IDLE = 1,
  JPEG_ENCODING = 2,
  JPEG_ENCODED_IMAGE = 3,
  JPEG_DECODING = 9,
  JPEG_DECODED_HEADER = 10,
  JPEG_DECODED_IMAGE = 11,
  JPEG_DECODE_ERROR = 12,
  JPEG_MEM_SHORTAGE = 15,
  JPEG_DECODE_PARAM_ERROR = 13,
  JPEG_ENCODE_PARAM_ERROR = 14,
  JPEG_CLOSED = 20,
} jpeg_state_t;

typedef struct jpeg_param {
  /*for get/set parameters*/
  __u32 vaddr_src;
  __u32 decopw_vaddr;
  __u32 decopw_en;
  __u32 decopw_TargetBuffersize;
  __u32 vaddr_dst;
  __u32 paddr_src;
  __u32 paddr_dst;
  __u32 src_bufsize;
  __u32 dst_bufsize;
  __u32 dec_stride;
  __u32 windec_en;
  __u32 windec_mcux_start;
  __u32 windec_mcux_end;
  __u32 windec_mcuy_start;
  __u32 windec_mcuy_end;
  __u32 windec_stride;
  /*for set only*/
  __u32 decode_output_format;//DRVJPEG_DEC_PRIMARY_PACKET_YUV420,
  __u32 encode; //0 encode 0 decode
  __u32 scale;  //0 disable 1 enable
  /* encode / decode scale */
  __u32 scaled_width;
  __u32 scaled_height;
  __u32 decInWait_buffer_size;
  /*encode parameters for set only*/
  __u32 encode_width;/*the width that will be encoded image raw data*/
  __u32 encode_height;/*the height that will be encoded image raw data*/
  __u8  qadjust;//the larger the better quality[2-16](0.25Q, 0.5Q, 0.75Q, Q, 1.25Q, 1.5Q, 1.75Q, 2Q, 2.25Q, 2.5Q, 2.75Q, 3Q, 3.25Q, 3.5Q, 3.75Q)
  __u8  qscaling;//the smaller the better quality[1-16]
  __u32 encode_source_format;
  __u32 encode_image_format;
  __u32 buffersize;/*each encode buffer size*/
  __u32 buffercount;/*total encode buffer count*/
} jpeg_param_t;

typedef struct jpeg_info {
  /*decode information*/
  __u32 yuvformat;  /*for decode*/
  __u32 width;    /*for decode*/
  __u32 height;   /*for decode*/
  __u32 dec_stride;
  /*encode information*/
  __u32 bufferend;
  __u32 state;
  __u32 image_size[]; /*image size after encoded*/
} jpeg_info_t;

#define JPEG_TRIGGER                    _IOW('v',120, struct jpeg_param)
#define JPEG_S_PARAM                    _IOW('v',121, struct jpeg_param)
#define JPEG_G_PARAM                    _IOW('v',122, struct jpeg_param)
#define JPEG_STATE                      _IOR('v',123, struct jpeg_param)
#define JPEG_G_INFO                     _IOR('v',124, struct jpeg_info)
#define JPEG_DECIPW_BUFFER_STATE        _IOR('v',125, __u32)
#define JPEG_G_DECIPW_BUFFER_SIZE       _IOR('v',126, __u32)
#define JPEG_DECODE_RESUME              _IOR('v',127, __u32)
#define JPEG_S_DECOPW                   _IOW('v',128,  __u32)
#define JPEG_GET_JPEG_BUFFER            _IOR('v',129, __u32)
#define JPEG_SET_ENCOCDE_RESERVED       _IOW('v',130,  __u32)
#define JPEG_SET_ENC_SRC_FROM_VIN       _IOW('v',131,  __u32)
#define JPEG_FLUSH_CACHE                _IOW('v',132,  __u32)
#define JPEG_SET_ENC_THUMBNAIL          _IOW('v',133, __u32)
#define JPEG_GET_ENC_THUMBNAIL_SIZE     _IOW('v',134, __u32)
#define JPEG_GET_ENC_THUMBNAIL_OFFSET   _IOW('v',135, __u32)

//Define for Encode Image Format
#define DRVJPEG_ENC_PRIMARY_YUV420    0xA0
#define DRVJPEG_ENC_PRIMARY_YUV422    0xA8
#define DRVJPEG_ENC_PRIMARY_GRAY    0xA1
#define DRVJPEG_ENC_THUMBNAIL_YUV420  0x90
#define DRVJPEG_ENC_THUMBNAIL_YUV422  0x98
#define DRVJPEG_ENC_THUMBNAIL_GRAY    0x91

#define DRVJPEG_ENC_SRC_PLANAR  0
#define DRVJPEG_ENC_SRC_PACKET  1

int Jpeg_Init(unsigned char *src_image);

#endif

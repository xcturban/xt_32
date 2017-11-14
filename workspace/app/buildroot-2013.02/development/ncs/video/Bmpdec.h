#ifndef __BMP_DEC_H__
#define __BMP_DEC_H__

struct file_header
{
  unsigned short bfType; // Picture tpye, must set to 19778

  int bfSize; // The file size in bytes

  int bfRev; // Reserved

  int bfOffBits; // the offset from the beginning of the file to the bitmap data.

}__attribute__ ((packed));

struct info_header
{
  int biSize; // info_header's size in bytes

  int biWidth; // width in pixels

  int biHeight;//height in pixels

  short biPlanes; //the number of planes of the target device

  short biBitCount; //the number of bits per pixel

  int biCompression;//the type of compression

  int biSizeImage; //

  int biXPelsPerMeter;//usually set to zero

  int biYPelsPerMeter;//usually set to zero

  int biClrUsed;//the number of colors used in the bitmap

  int biClrImportant;
}__attribute__ ((packed));

struct bmp_file
{
  struct file_header header;
  struct info_header info;
  unsigned char bits[320*240*3];
}__attribute__ ((packed));

void cvt_420p_to_rgb(int width, int height, const unsigned char *src, unsigned char *dst);
int Bmp_init(struct bmp_file *myfile);

#endif

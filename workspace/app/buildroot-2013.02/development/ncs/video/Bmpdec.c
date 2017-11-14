#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Bmpdec.h"

void cvt_420p_to_rgb(int width, int height, const unsigned char *src, unsigned char *dst)
{
	int r, g, b;
	int rdif, gdif, bdif, y;
	int yy, uu, vv;
	int xoff, yoff;
	int numpix = width*height;
	unsigned char *pout = dst + width*height*3;

	for(yoff=0; yoff<height; yoff++){
		for(xoff=0; xoff<width; xoff++){
			yy = *(src+yoff*320+xoff);
	     	uu = *(src+(yoff/2)*160+xoff/2+numpix);
	     	vv = *(src+(yoff/2)*160+xoff/2+numpix+numpix/4);

	   	  	uu -= 128;
	     	vv -= 128;

		     r = yy + vv + ((vv*103)>>8);
		     g = yy - ((uu*88)>>8) - ((vv*183)>>8);
		     b = yy + uu + ((uu*198)>>8);

		     if(r>255) { r = 255; }
		     if(g>255) { g = 255; }
		     if(b>255) { b = 255; }
		     if(r<0) { r = 0;}
		     if(g<0) { g = 0;}
		     if(b<0) { b = 0;}

		     *pout = (unsigned char)b;
		     pout--;
		     *pout = (unsigned char)r;
		     pout--;
		     *pout = (unsigned char)g;
		     pout--;
		 }
	}
}

int Bmp_init(struct bmp_file *myfile)
{
  printf("sizeof short is %d\n", sizeof(short));

  myfile->header.bfType = 19778;
  myfile->header.bfSize = sizeof(struct bmp_file);
  myfile->header.bfRev = 0;
  myfile->header.bfOffBits = 54;

  myfile->info.biSize = 0x28;
  myfile->info.biWidth = 320;
  myfile->info.biHeight = 240;
  myfile->info.biPlanes = 1;
  myfile->info.biBitCount = 24;
  myfile->info.biCompression = 0;
  myfile->info.biSizeImage = 0;
  myfile->info.biClrUsed = 0;
  myfile->info.biClrImportant = 0;
  myfile->info.biXPelsPerMeter = 0;
  myfile->info.biYPelsPerMeter = 0;

  return 0;
}

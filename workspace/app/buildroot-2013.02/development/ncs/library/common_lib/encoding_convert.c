#include <stdio.h>
#include <iconv.h>
#include <string.h>

#include "encoding_convert.h"

int utf8_to_gb2312(char *in, char *out, size_t size) 
{


	iconv_t cd;
	cd = iconv_open("GB2312", "UTF-8");
	if ( cd == (iconv_t)(-1) ) {
		perror("iconv_open failed");
		return 0;
	}

	size_t in_left = strlen(in) + 1;
	char *out_ptr;
	size_t res;

	out_ptr = out;
	res = iconv(cd, &in, &in_left, &out_ptr, &size);
	if ( res == (size_t) (-1) ) {
		perror("iconv failed");
		return 0;
	}

	iconv_close(cd);
	return 1;
}


int gb2312_to_utf8(char *in, char *out, size_t size) 
{
/*
	iconv_t cd;
	cd = iconv_open("UTF-8", "GB2312");
	if ( cd == (iconv_t)(-1) ) {
		perror("iconv_open failed");
		return 0;
	}

	size_t in_left = strlen(in) + 1;
	char *out_ptr;
	size_t res;

	out_ptr = out;
	res = iconv(cd, &in, &in_left, &out_ptr, &size);
	if ( res == (size_t) (-1) ) {
		perror("iconv failed");
		return 0;
	}

	iconv_close(cd);
	return 1;
*/
	return 1;
}

/*
int iconv_test() 
{

	char *utf8_str = "Beijing??—?o?";
	char *gb_str = "?D1úè???o?";
	char out[100];

	printf("---utf8 org : %s \n",utf8_str);
	if (utf8_to_gb2312(utf8_str, out, 100) != 0) {
		printf("new : %s\n", out);
	}
	printf("---gb org : %s \n",gb_str);
	if (gb2312_to_utf8(gb_str, out, 100) != 0) {
		printf("new : %s\n", out);
	}
	
	return 0;
}
*/


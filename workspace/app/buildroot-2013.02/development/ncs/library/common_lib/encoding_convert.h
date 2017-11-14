#ifndef __ENCODING_CONVERT_H__
#define __ENCODING_CONVERT_H__


int utf8_to_gb2312(char *in, char *out, size_t size);
int gb2312_to_utf8(char *in, char *out, size_t size);

#endif

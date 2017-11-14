#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
//#include "Common.h"
char temp_str[100]; /* just for get_value   */

void main(){
	long len;
	char *data;
	char host_id[20];
	char host_ip[100];
	char str[300];
	int i;

	iconv_t cd;
	size_t inleft;
	size_t outleft;
	FILE *fp=fopen("../test.txt","wb");


	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("</head>");

	if(fp==0){
		printf("file open error<br>");
		return;
	}

	
	data = getenv("QUERY_STRING");
	if(data  == NULL){
		printf("data error \n\r");
	}else{
		printf("data1:%s <br>",data);
	//	i=gb23122utf8(str,data);
		cgi_to_html("../xml/audio_cfg.xml","../c2h/audio_cfg.xml.c2h",data,'&');
		fwrite(data,sizeof(char),strlen(data),fp);	
			
		printf("len:%d data:%s \n\r <br>",i,data);
	}

	printf("</html>");
	fclose(fp);	
}


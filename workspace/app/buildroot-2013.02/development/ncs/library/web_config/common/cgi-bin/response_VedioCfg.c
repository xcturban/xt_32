#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
char temp_str[100]; /* just for get_value   */


void main(){
	long len;
	char *data;
	char host_id[20];
	char host_ip[100];
	char str[300];
	char temp_data[300];

	int i;
	for(i=0;i<100;i++){
		str[i]=0;
	}

	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("</head>");
	data = getenv("QUERY_STRING");
	if(data  == NULL){
		printf("data error \n\r");
	}else{
//		gb23122utf8(temp_data,data);
		cgi_to_html("../xml/vedio_cfg.xml","../c2h/vedio_cfg.xml.c2h",data,'&');
	}

//	sprintf(str,"<script>window.location.href=\"VedioCfg.cgi?%s\" </script>",temp_data);
	sprintf(str,"<script>window.location.href=\"VedioCfg.cgi\" </script>");
	printf(str);
	printf("</html>");
	
}


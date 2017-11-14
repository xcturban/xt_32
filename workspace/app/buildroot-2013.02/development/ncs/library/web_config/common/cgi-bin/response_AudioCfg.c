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

	char str_sys_cfg_ini[50];
	char str_audio_cfg_xml[50];
	//char str_audio_cfg_xml_c2h[50];
	char str_audio_cfg_c2h[50];
	char str_audio_cfg_x2i[50];
	char str_audio_cfg_htm[50];

	int i;

	get_file_path(str_sys_cfg_ini,"sys_cfg.ini");
	get_file_path(str_audio_cfg_xml,"audio_cfg.xml");
	//get_file_path(str_audio_cfg_xml_c2h,"audio_cfg_xml.c2h");
	get_file_path(str_audio_cfg_c2h,"audio_cfg.c2h");
	get_file_path(str_audio_cfg_x2i,"audio_cfg.x2i");
	get_file_path(str_audio_cfg_htm,"audio_cfg.htm");


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
		//gb23122utf8(temp_data,data);
		//cgi_to_html(str_audio_cfg_xml,str_audio_cfg_xml_c2h,data,'&');
		cgi_to_html(str_audio_cfg_htm,str_audio_cfg_c2h,data,'&');
		xml2ini(str_sys_cfg_ini, str_audio_cfg_x2i, str_audio_cfg_xml);
	}

//	sprintf(str,"<script>window.location.href=\"AudioCfg.cgi?%s\" </script>",temp_data);
	sprintf(str,"<script>window.location.href=\"AudioCfg.cgi\" </script>");
	printf(str);
	printf("</html>");
	
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#define STR_LEN 100
char temp_str[STR_LEN]; /* just for get_value   */


void main(){
	long len;
	char data[4000];
	char host_id[20];
	char host_ip[100];
	char str[300];
	char temp_data[300];
	char str_temp[50];

	char str_sys_cfg_ini[50];
	char str_board_cfg_xml[50];
	char str_board_cfg_xml_c2h[50];
	char str_board_cfg_x2i[50];

	int i;

	get_file_path(str_sys_cfg_ini,"sys_cfg.ini");
	get_file_path(str_board_cfg_xml_c2h,"board_cfg.xml.c2h");
	get_file_path(str_board_cfg_x2i,"board_cfg.x2i");


	for(i=0;i<100;i++){
		str[i]=0;
	}

	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("</head>");
//	data = getenv("QUERY_STRING");
	scanf("%s",data);
	if(data  == NULL){
		printf("data error \n\r");
	}else{
//		gb23122utf8(temp_data,data);
		if(get_value(str_temp, data, "board_num", '&', 50)==NULL){
			sprintf(str_temp,"0");
		}
		sprintf(str_board_cfg_xml, "board_cfg_%s.xml", str_temp);
		sprintf(str_board_cfg_x2i, "board_cfg_%s.x2i", str_temp);
		cgi_to_html(str_board_cfg_xml,str_board_cfg_xml_c2h,data,'&');
		xml2ini(str_sys_cfg_ini, str_board_cfg_x2i, str_board_cfg_xml);
	}

//	sprintf(str,"<script>window.location.href=\"AudioCfg.cgi?%s\" </script>",temp_data);
	sprintf(str,"<script>window.location.href=\"BoardCfg.cgi\" </script>");
	printf(str);
	printf("</html>");
	
}


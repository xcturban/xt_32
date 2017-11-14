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
	char str_net_cfg_xml[50];
	char str_net_cfg_xml_c2h[50];
	char str_net_cfg_x2i[50];

	int i;

	get_file_path(str_sys_cfg_ini,"sys_cfg.ini");
	get_file_path(str_net_cfg_xml,"net_cfg.xml");
	get_file_path(str_net_cfg_xml_c2h,"net_cfg.xml.c2h");
	get_file_path(str_net_cfg_x2i,"net_cfg.x2i");


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
//		gb23122utf8(temp_data,data); /* the len of str must be twice of data */
//		cgi_to_html("../xml/net_cfg.xml","../c2h/net_cfg.xml.c2h",temp_data,'&');
		cgi_to_html(str_net_cfg_xml,str_net_cfg_xml_c2h,data,'&');
		xml2ini(str_sys_cfg_ini, str_net_cfg_x2i, str_net_cfg_xml);
	}

//	sprintf(str,"<script>window.location.href=\"NetCfg.cgi?%s\" </script>",temp_data);
	sprintf(str,"<script>window.location.href=\"NetCfg.cgi\"</script>");
	printf(str);
	printf("</html>");
	
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

#define STR_LEN 50
#define STR_LONG_LEN 400
void main(){
	long len;
	char *data;
	char str[STR_LONG_LEN];

	char str_ini[STR_LEN];
	char str_htm[STR_LEN];
	char str_x2i[STR_LEN];
	char str_c2h[STR_LEN];
	char str_ret[STR_LONG_LEN];
	char str_retstr[STR_LONG_LEN];
	char str_sh[STR_LEN];
 
	char str_tmp_htm[STR_LEN];
	char str_tmp_x2i[STR_LEN];
	char str_tmp_c2h[STR_LEN];
	char str_tmp_sh[STR_LEN];

	int i;
	char *p;
	int num;

	*str_sh=0;

	get_file_path(str_ini,"sys_cfg.ini");

	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("</head>");
	data = getenv("QUERY_STRING");
	if(data  != NULL){
//		gb23122utf8(temp_data,data);
		
		if(get_value(str_tmp_htm, data, "obj_htm", '&', STR_LEN)==NULL){
			printf("obj_htm not exist\r\n");
		}
		if(get_value(str_tmp_x2i, data, "obj_x2i", '&', STR_LEN)==NULL){
			printf("obj_x2i not exist\r\n");

		}
		if(get_value(str_tmp_c2h, data, "obj_c2h", '&', STR_LEN)==NULL){
			printf("obj_c2h not exist\r\n");

		}
		if(get_value(str_ret, data, "obj_ret", '&', STR_LONG_LEN)==NULL){
			printf("obj_ret not found\r\n");
		}
		if(get_value(str_tmp_sh, data, "obj_sh", '&', STR_LEN)!=NULL){
			sprintf(str_sh, "../shell/%s", str_tmp_sh);
		}
		if((p=strstr(str_tmp_htm,"subboard_cfg_"))!=NULL){
			memcpy(str_retstr, p, strlen(p));
			i=strlen(str_retstr)-strlen("subboard_cfg_");
			memcpy(str_htm, str_retstr+strlen("subboard_cfg_"), i);
			p=strstr(str_htm,".htm");
			*p=0;
			num=atoi(str_htm);
			i=system("cp ../c2h/SubBoardCfg.c2h ../c2h/SubBoardCfgNum.c2h");
			if(i!=0){
				printf("response_Main: system(cp) error\n\r");
				return -1;
			}
			sprintf(str_retstr,"find ../c2h/ -name SubBoardCfgNum.c2h -exec sed -i -r 's/<\\[num\\]>/%d/' {} \\;",num);
			i=system(str_retstr);
			if(i!=0){
				printf("response_Main: system(str_htm) error\n\r");
				return -1;
			}
		}

		get_file_path(str_htm, str_tmp_htm);
		get_file_path(str_x2i, str_tmp_x2i);
		get_file_path(str_c2h, str_tmp_c2h);
		//cgi_to_html(str_true_xml,str_true_c2h,data,'&');
		x2i_to_ini(str_ini, str_x2i, data);
		cgi_to_html_by_ini(str_htm,str_c2h,str_ini);
		if(*str_sh!=0){
			if(system(str_sh)!=0){
				printf("linuxSystem(SHELL_FILE)  error !");
			}	
		}
	}else{
		printf("response_Main.c: data==NULL;\n\r");
	}

	urlDecode(str_retstr, str_ret, STR_LONG_LEN);
//	sprintf(str,"<script>window.location.href=\"AudioCfg.cgi?%s\" </script>",temp_data);
	sprintf(str,"<script>window.location.href=\"%s\" </script>",str_retstr);
	printf(str);
	printf("</html>");
	
}



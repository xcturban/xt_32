#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "ini.h"

#define STR_LEN 50
#define STR_LONG_LEN 400
void main(){
	long len;
	char *data;
	char str[STR_LONG_LEN];
	int ret=0;


	STRUCT_INI *ini_fp;

	char str_ini[STR_LEN];
	char str_x2i[STR_LEN];
	char str_ret[STR_LONG_LEN];
	char str_retstr[STR_LONG_LEN];
	char str_sh[STR_LEN];
	char str_username[STR_LEN];
	char str_password[STR_LEN];
	char str_ini_username[STR_LEN];
	char str_ini_password[STR_LEN];
 
	char str_tmp_ini[STR_LEN];
	char str_tmp_x2i[STR_LEN];
	char str_tmp_sh[STR_LEN];

	int i;
	char *p;
	int num;

	*str_sh=0;


	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("</head>");
	data = getenv("QUERY_STRING");
	if(data  != NULL){
//		gb23122utf8(temp_data,data);
		
		if(get_value(str_tmp_x2i, data, "obj_x2i", '&', STR_LEN)==NULL){
			printf("obj_x2i not exist\r\n");

		}
		if(get_value(str_tmp_ini, data, "obj_ini", '&', STR_LEN)==NULL){
			sprintf(str_tmp_ini, "sys_cfg.ini");
		}

		if(get_value(str_ret, data, "obj_ret", '&', STR_LONG_LEN)==NULL){
			printf("obj_ret not found\r\n");
		}
		if(get_value(str_tmp_sh, data, "obj_sh", '&', STR_LEN)!=NULL){
			sprintf(str_sh, "../shell/%s", str_tmp_sh);
		}


		urlDecode(str_retstr, str_ret, STR_LONG_LEN);

		get_file_path(str_x2i, str_tmp_x2i);
		get_file_path(str_ini, str_tmp_ini);


		ini_fp=ini_new(str_ini);
		if(ini_fp==NULL){
			printf("response_WebCfg ini_fp open error:%s\n", str_ini);
			return 0;
		}
		if(get_value(str_username, data, "web_username_old", '&', STR_LEN)!=NULL){
			if(get_value(str_password, data, "web_password_old", '&', STR_LEN)!=NULL){
				ini_get_value_string(ini_fp, str_ini_username, "web_cfg", "username");
				ini_get_value_string(ini_fp, str_ini_password, "web_cfg", "password");
				if(strcmp(str_ini_username, str_username)){
					ret++;
				}
				if(strcmp(str_ini_password, str_password)){
					ret++;
				}
			}

		}
		ini_release(ini_fp);
		if(ret>0){
			sprintf(str,"<script>window.location.href=\"%s?jmp=false\" </script>",str_retstr);
			printf(str);
			printf("</html>");

			return 0;
		}

		x2i_to_ini(str_ini, str_x2i, data);
		if(*str_sh!=0){
			if(system(str_sh)!=0){
				printf("linuxSystem(%s)  error !", str_sh);
			}	
		}
	}else{
		printf("response_Main.c: data==NULL;\n\r");
	}

	sprintf(str,"<script>window.location.href=\"%s?jmp=true\" </script>",str_retstr);
	printf(str);
	printf("</html>");
	
}



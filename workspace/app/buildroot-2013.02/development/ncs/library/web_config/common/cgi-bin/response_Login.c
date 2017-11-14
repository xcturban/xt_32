#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "ini.h"

#define SHELL_FILE "../shell/response_reboot.sh"

#define bufferSize 512
#define FPATH	"../uploads/"
#define STATUS_FILE  "../uploadStatus.htm"
#define INI_FILE "../ini/sys_cfg.txt"

char temp_str[100]; /* just for get_value   */


void main(){
	char username[bufferSize];
	char password[bufferSize];
	char *contentType;
	int contentLen;
	int i;
	int pos;
	char str[bufferSize];
	char *p;
	char *tmp;
	char str_tmp[bufferSize];
	int hasLogin=0;
	STRUCT_INI *fp_ini;

	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("<script language=javascript src=\"../js/Common.js\" type=\"text/javascript\" ></script>");
	printf("</head>");
	printf("<body>");

	contentType=getenv("CONTENT_TYPE");
	contentLen=atoi(getenv("CONTENT_LENGTH"));
	pos=0;
	i=10;
	fgets(str,bufferSize,stdin);
	if((p=strstr(str,"username="))){
		strcpy(username,p+strlen("username="));
		tmp=strstr(username, "&");
		if(tmp!=NULL){
			*tmp=0;
		}
	}
	if((p=strstr(str,"password="))){
		strcpy(password,p+strlen("password="));
		tmp=strstr(password, "&");
		if(tmp!=NULL){
			*tmp=0;
		}
	}
	fp_ini=ini_new(INI_FILE);
	if(fp_ini){
		ini_get_value_string(fp_ini, str_tmp, "web_cfg", "username");
		if(!strcmp(str_tmp, username)){
			ini_get_value_string(fp_ini, str_tmp, "web_cfg", "password");
			if(!strcmp(str_tmp, password)){
				hasLogin=1;
			}
		}
	}
	ini_release(fp_ini);

	printf("<center>");	
	printf("<div>Checking password ...</div>");
	printf("</center>");
	printf("</body>");
	printf("</html>");
	printf("<script>");
	if(hasLogin==1){
		printf( "function show(){SetCookie(\"admin_has_login\",\"admin\", 0, \"/\"); location=\"/index.htm\";} ");
	}else{
		printf("function show(){location=\"/cache/LoginCheck.htm?jmp=false\" }");
	}
	printf("show();");
	printf("</script>");
	
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

#define SHELL_FILE "../shell/response_reboot.sh"

char temp_str[100]; /* just for get_value   */


void main(){
	long len;
	char *data;
	char host_id[20];
	char host_ip[100];
	char str[300];

	int i;
	for(i=0;i<100;i++){
		str[i]=0;
	}

	printf("Content-type: text/html \n\n");
	printf("<html>");

	printf("<head>");
	printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
	printf("</head>");
	printf("<body onload=show() >");
	printf("<center>");	
	printf("<div>if you look this page ,it means restarting now!</div>");

//	system(SHELL_FILE);

//	if(linuxSystem(SHELL_FILE)!=0){
	if(system(SHELL_FILE)!=0){
		printf("linuxSystem(SHELL_FILE)  error !");
	}	

	printf("</center>");
	printf("</body>");
	printf("</html>");
	printf("<script>");
	printf("function show(){alert(\"restarted successfull!\"); location=\"/cache/Reboot.htm\" ;}");
	printf("</script>");
	
}


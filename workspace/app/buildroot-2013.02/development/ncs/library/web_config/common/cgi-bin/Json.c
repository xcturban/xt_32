#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
char temp_str[100]; /* just for get_value   */


void main(){
	long len;
	char *data=NULL;
	char str_obj_page[400];
	char str_c2h[100];
	char str_ini[100];
	int i;

	printf("Content-type: text/html \n\n");

	for(i=0;i<50;i++){
		str_obj_page[i]=0;
	}


	data = getenv("QUERY_STRING");
	if(data  != NULL){
		if(get_value(str_obj_page, data, "obj_c2h", '&', 50)){
			get_file_path(str_c2h, str_obj_page);
		}else{
			printf("Main.c: main, get value error\r\n");
			exit(1);
		}
		if(get_value(str_obj_page, data, "obj_ini", '&', 50)){
			get_file_path(str_ini, str_obj_page);
		}else{
			printf("Main.c: main, get value error\r\n");
			exit(1);
		}

	}else{
		printf("Main.c: main, data==NULL");
		exit(1);
	}
	cgi_to_browser_by_ini(str_c2h, str_ini);

}


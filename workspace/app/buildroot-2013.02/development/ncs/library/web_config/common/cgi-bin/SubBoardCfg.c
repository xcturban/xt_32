#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

void main(){
	long len;
	char *data;
	char str_obj_page[50];
	char str_c2h[100];
	char str_xml[100];
	int i;

	printf("Content-type: text/html \n\n");

	for(i=0;i<50;i++){
		str_obj_page[i]=0;
	}

	data = getenv("QUERY_STRING");
	if(data  == NULL){
		printf("data error \n\r");
		exit(1);
	}else{
		get_value(str_obj_page, data, "obj_page", "&", 50 );
		sprintf(str_c2h, "../c2h/%s.c2h",str_obj_page);
		sprintf(str_xml, "../xml/%s.xml", str_obj_page);
	}
	
	cgi_to_browser_by_xml(str_c2h, str_xml);

	cgi_to_browser_by_xml("../c2h/BoardCfg.c2h", "../xml/board_cfg.xml");
}

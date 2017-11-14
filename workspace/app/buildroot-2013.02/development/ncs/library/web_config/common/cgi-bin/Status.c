#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"


char temp_str[100]; /* used for get_value */

/*
int main(int argc, char *argv[]){
	if(argc<4){
		printf("command file_name_obj file_name_src argv1=value1,argv2=value2,argv3=value3\n\r");
		return -1;
	}

	cgi_to_html(argv[1],argv[2],argv[3],',');

}
*/

void main(){
	printf("Content-type: text/html \n\n");
	cgi_to_browser_by_xml("../c2h/Status.c2h","../xml/status.xml");
}


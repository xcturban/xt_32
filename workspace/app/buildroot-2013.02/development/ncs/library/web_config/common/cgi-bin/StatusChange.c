#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"


char temp_str[100]; /* used for get_value */

int main(int argc, char *argv[]){
	if(argc<2){
		printf("command argv1=value1,argv2=value2,argv3=value3\n\r");
		return -1;
	}

	cgi_to_html("../xml/status.xml","../c2h/status.xml.c2h",argv[1],',');

}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

#define STR_LEN 50
int main(int argc, char argv[]){

	char str_c2h[STR_LEN];
	char str_lan[STR_LEN];
	char str_ini[STR_LEN];
 
	if(argc<5){
		printf("set_language file.c2h file.lan file.ini\n ");
		return 0;
	}

	get_file_path(str_c2h, argv[1]);
	get_file_path(str_lan, argv[2]);
	get_file_path(str_ini, argv[3]);

	lan_to_cgi_by_ini(str_c2h, str_lan, str_ini);
}


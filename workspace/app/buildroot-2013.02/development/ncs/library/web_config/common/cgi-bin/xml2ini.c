#include <stdio.h>
#include <string.h>
#include "ini.h"
#include "xml.h"
#include "xml2ini.h"

#define STR_XML_ARGV_LEN   50
#define STR_INI_SEC_LEN    50
#define STR_INI_ARGV_LEN   50

int xml2ini(char *str_ini, char *str_x2i, char *str_xml){
	char str_xml_argv[50];
	char str_ini_sec[50];
	char str_ini_argv[50];
	char str_xml_value[50];
 	int idx;
	int i;
	FILE *xml_fp=xml_new(str_xml);
	FILE *x2i_fp=fopen(str_x2i,"rb");
	STRUCT_INI *ini_fp=ini_new(str_ini);

	if(xml_fp==NULL){
		printf("xml open error!\n\r");
		return 0;
	}
	if(x2i_fp==NULL){
		printf("x2i open error!\n\r");
		return 0;
	}
	if(ini_fp==NULL){
		printf("ini open error!\n\r");
		return 0;
	}

	while(fgets(str_xml_argv, STR_XML_ARGV_LEN, x2i_fp)!=NULL){
		if(fgets(str_ini_sec, STR_INI_SEC_LEN, x2i_fp)!=NULL){
			if(fgets(str_ini_argv, STR_INI_ARGV_LEN, x2i_fp)!=NULL){
				i=strlen(str_xml_argv);
				if(str_xml_argv[i-2]==0x0d){
					str_xml_argv[i-2]=0;
				}else{
					str_xml_argv[i-1]=0;
				}
				i=strlen(str_ini_sec);
				if(str_ini_sec[i-2]==0x0d){
					str_ini_sec[i-2]=0;
				}else{
					str_ini_sec[i-1]=0;
				}
				i=strlen(str_ini_argv);
				if(str_ini_argv[i-2]==0x0d){
					str_ini_argv[i-2]=0;
				}else{
					str_ini_argv[i-1]=0;
				}
				xml_get_value_string( str_xml_value, str_xml_argv, xml_fp);
			//	printf("\r\n xml argv:___%s___ \r\n", str_xml_argv);
			//	printf("xml value: %s \r\n", str_xml_value);
				ini_put_value_string(ini_fp, str_ini_sec, str_ini_argv, str_xml_value);
				if(fgets(str_ini_argv, STR_INI_ARGV_LEN, x2i_fp)==NULL){
					break;
				}
			}else{
				break;
			}
		}else{
			break;
		}
	}

	xml_release(xml_fp);
	fclose(x2i_fp);
	ini_release(ini_fp);
	return 0;
}


/*

void main(void){
	xml2ini("test.ini", "test.x2i", "test.xml");
	printf("\r\nokokkokokkkkkkkkkkkkkkkkk\n\r");

}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <iconv.h>
#include "Common.h"
#include "ini.h"
#include "xml.h"

#define WEB_CONFIG_INI_FILE "../ini/web_config.ini"
#define STR_LEN 555

char temp_str[100]; /* used for get_value */

char *get_file_path(char *path, char *file)
{
    STRUCT_INI *fp = ini_new(WEB_CONFIG_INI_FILE);
    ini_get_value_string(fp, path, "web_config", file);
    return path;
}

int hex2int(char *argv)
{
    int ret;
    char h = toupper(argv[0]);
    char l = toupper(argv[1]);
    int vh = isalpha(h) ? (10 + (h - 'A')) : (h - '0');
    int vl = isalpha(l) ? (10 + (l - 'A')) : (l = '0');
    ret = (vh << 4) + vl;
    return ret;
}

char *get_value(char *value, char *argv, char *argv_sub, char div_char, int str_len)
{
    int i;
    int j;
    int k;
    char *p_tmp;
    const char *c_tmp = "&? ";
    char *str_argv_sub;
    j = strlen(argv_sub);
    str_argv_sub = malloc(j + 3);
    memcpy(str_argv_sub + 1, argv_sub, j);
    str_argv_sub[j + 1] = '=';
    str_argv_sub[j + 2] = 0;

    p_tmp = strstr(argv, (str_argv_sub + 1));
    if(p_tmp != argv)
    {
        j = strlen(c_tmp);
        for(i = 0; i < j; i++)
        {
            str_argv_sub[0] = c_tmp[i];
            p_tmp = strstr(argv, str_argv_sub);
            if(p_tmp != NULL)
            {
                break;
            }
        }
        if(p_tmp == NULL)
        {
            return NULL;/* not found anything */
        }
        p_tmp++;
    }

    free(str_argv_sub);
    i = strlen(argv_sub) + 1;
    j = strlen(p_tmp + i);
    if(j > str_len)
    {
        j = str_len;
    }
    memcpy(value, p_tmp + i, j);
    value[j] = 0;
    if((p_tmp = strchr(value, div_char)) == NULL)
    {
        if(strlen(value) >= str_len)
        {
            value[str_len] = 0;
        }
        else
        {
            value[strlen(value)] = 0;
        }
    }
    else
    {
        *p_tmp = 0;
    }
    return value;
}

/*
int get_value(char *value, char *argv, char *argv_sub, char div_char){
	int i;
	int j;
	int k;
	j=0;
	char temp[50];
	for(i=0;i<strlen(argv);i++){
		for(k=0;k<strlen(argv_sub);k++){
			temp[k]=argv[i+k];
		}
		temp[k]=0;
		if(!strcmp(temp,argv_sub)){
			if(i==0 || argv[i-1]==' ' || argv[i-1]==div_char){
				if(argv[i+strlen(argv_sub)]=='='){
					i+=strlen(argv_sub)+1;
					do{

						if(argv[i]=='%' && argv[i+1]!=div_char && argv[i+1]!=0 && argv[i+2]!=div_char && argv[i+2]!=0){
							value[j]=(char)(hex2int(argv+i+1));
							i+=3;
							j++;
						}else if(argv[i]=='+'){
							i++;
						}else{

							value[j]=argv[i];
							j++;
							i++;
						}
						if(i>strlen(argv)){
							break;
						}
					}while(argv[i-1]!=div_char && argv[i-1]!=0);
					value[j-1]=0;
					return 0;
				}

			}
		}
	}
	return -1;
}
*/

int get_value_from_xml(char *value, char *file_name_xml, char *argv)
{
    FILE *fp;
    int temp;
    int temp_begin;
    char arg_name[50];
    char arg_name_neg[50];
    char temp_name[50];
    int i;
    int j;
    char *arg_value = value;


    if(strlen(argv) > 45)
    {
        printf("the argv is too long :error!\n\r");
        return -1;
    }
    arg_name[0] = '<';
    arg_name_neg[0] = '<';
    arg_name_neg[1] = '/';
    for(i = 1; i < strlen(argv) + 1; i++)
    {
        arg_name[i] = argv[i - 1];
        arg_name_neg[i + 1] = argv[i - 1];
    }
    arg_name[i] = '>';
    arg_name_neg[i + 1] = '>';
    arg_name[i + 1] = 0;
    arg_name_neg[i + 2] = 0;
    if((fp = fopen(file_name_xml, "r")) == NULL)
    {
        printf("file open error!\n\r");
        return -1;
    }
    temp = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(temp_name, strlen(arg_name) + 1, fp);
        if(!strcmp(temp_name, arg_name))
        {
            temp += strlen(arg_name);
            j = 0;
            do
            {
                fseek(fp, temp, SEEK_SET);
                fgets(&arg_value[j], strlen(arg_name_neg) + 1, fp);
                j++;
                temp++;
                if(j == 48)
                {
                    break;
                }
            }
            while(strcmp(&arg_value[j - 1], arg_name_neg) && (!feof(fp)));
            if(j < 48)
            {
                fclose(fp);
                arg_value[j - 1] = 0;
                return 0; /* find it so return means successful... */
            }
            else
            {
                fclose(fp);
                arg_value[0] = 0;
                return -1; /* you have two choice, 1 for pass, the other tell error. */
            }
        }
        temp++;
    }
    fclose(fp);
    arg_value[0] = 0;
    return -1; /* find none of yous */
}

int cgi_to_html(char *file_name_obj, char *file_name_src, char *argv_table, char div_char)
{
    char arg_name[50];
    char arg_val[50];
    int i, j, k, m;
    FILE *fp;
    FILE *fp_htm;
    int temp;
    int begin_temp;
    int end_temp;
    int temp_htm;
    char str[100];

    if((fp = fopen(file_name_src, "rb")) == NULL)
    {
        printf("file open error!\n\r");
        return -1;
    }
    if((fp_htm = fopen(file_name_obj, "wb")) == NULL)
    {
        printf("file temp_temp open error!\n\r");
        return -1;
    }

    temp = 0;
    temp_htm = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(arg_name, strlen("<?cgi ") + 1, fp);
        if(!strcmp(arg_name, "<?cgi "))
        {
            begin_temp = temp;
            temp += strlen("<?cgi ");
            i = -1;
            do
            {
                i += 1;
                fseek(fp, temp + i, SEEK_SET);
                fgets(&arg_name[i], strlen("?>") + 1, fp);
            }
            while(strcmp(&arg_name[i], "?>"));
            arg_name[i - 1] = 0; /* note the space followed the ?> */
            end_temp = temp + i - 2;
            //	printf("*****************************\n\r");
            if(get_value(temp_str, argv_table, arg_name, div_char, 100) < 0)
            {
                temp_str[0] = 0;
            }
            fwrite(temp_str, sizeof(char), strlen(temp_str), fp_htm);
            //	sprintf(str, "the temp_str: %s str argv[2]: %s str arg_name: %s \n\r",temp_str,argv[2],arg_name);
            //	printf(str);
            //	printf("****************************\n\r");
            temp = end_temp + 4 - 1; /* note after the if block there is a temp++ */
        }
        else
        {
            fputc(arg_name[0], fp_htm);
        }
        temp++;
    }

    fclose(fp);
    fclose(fp_htm);

    sync();
    return 0;

}

int cgi_to_html_by_ini(char *file_name_obj, char *file_name_src, char *file_name_ini)
{
    char arg_name[STR_LEN];
    char arg_val[STR_LEN];
    int i, j, k, m;
    FILE *fp;
    FILE *fp_htm;
    int temp;
    int begin_temp;
    int end_temp;
    int temp_htm;
    char str[STR_LEN];
    STRUCT_INI *fp_ini;

    if((fp = fopen(file_name_src, "rb")) == NULL)
    {
        printf("file open error!\n\r");
        return -1;
    }
    if((fp_htm = fopen(file_name_obj, "wb")) == NULL)
    {
        printf("file temp_temp open error!\n\r");
        return -1;
    }
    if((fp_ini = ini_new(file_name_ini)) == NULL)
    {
        printf("common.c: cgi_to_html_by_ini() , ini file open error!\r\n");
        return -1;
    }

    temp = 0;
    temp_htm = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(arg_name, strlen("<?cgi ") + 1, fp);
        if(!strcmp(arg_name, "<?cgi "))
        {
            begin_temp = temp;
            temp += strlen("<?cgi ");
            i = -1;
            do
            {
                i += 1;
                fseek(fp, temp + i, SEEK_SET);
                fgets(&arg_name[i], strlen("?>") + 1, fp);
            }
            while(strcmp(&arg_name[i], "?>"));
            arg_name[i - 1] = 0; /* note the space followed the ?> */
            end_temp = temp + i - 2;
            i = strstr(arg_name, ".") - arg_name;
            arg_name[i] = 0;
            //	printf("*****************************\n\r");
            if(ini_get_value_string(fp_ini, temp_str, arg_name, arg_name + i + 1) == 0)
            {
                temp_str[0] = 0;
            }
            fwrite(temp_str, sizeof(char), strlen(temp_str), fp_htm);
            //	sprintf(str, "the temp_str: %s str argv[2]: %s str arg_name: %s \n\r",temp_str,argv[2],arg_name);
            //	printf(str);
            //	printf("****************************\n\r");
            temp = end_temp + 4 - 1; /* note after the if block there is a temp++ */
        }
        else
        {
            fputc(arg_name[0], fp_htm);
        }
        temp++;
    }

    fclose(fp);
    fclose(fp_htm);
    ini_release(fp_ini);

    sync();
    return 0;

}

int lan_to_cgi_by_ini(char *file_name_obj, char *file_name_src, char *file_name_ini)
{
    char arg_name[STR_LEN];
    char arg_val[STR_LEN];
    int i, j, k, m;
    FILE *fp;
    FILE *fp_htm;
    int temp;
    int begin_temp;
    int end_temp;
    int temp_htm;
    char str[STR_LEN];
    STRUCT_INI *fp_ini;

    if((fp = fopen(file_name_src, "rb")) == NULL)
    {
        printf("file open error!\n\r");
        return -1;
    }
    if((fp_htm = fopen(file_name_obj, "wb")) == NULL)
    {
        printf("file temp_temp open error!\n\r");
        return -1;
    }
    if((fp_ini = ini_new(file_name_ini)) == NULL)
    {
        printf("common.c: cgi_to_html_by_ini() , ini file open error!\r\n");
        return -1;
    }

    temp = 0;
    temp_htm = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(arg_name, strlen("<?cgi string=") + 1, fp);
        if(!strcmp(arg_name, "<?cgi string="))
        {
            begin_temp = temp;
            temp += strlen("<?cgi string=");
            i = -1;
            do
            {
                i += 1;
                fseek(fp, temp + i, SEEK_SET);
                fgets(&arg_name[i], strlen("?>") + 1, fp);
            }
            while(strcmp(&arg_name[i], "?>"));
            arg_name[i - 1] = 0; /* note the space followed the ?> */
            end_temp = temp + i - 2;
            i = strstr(arg_name, ".") - arg_name;
            arg_name[i] = 0;
            //	printf("*****************************\n\r");
            if(ini_get_value_string(fp_ini, temp_str, arg_name, arg_name + i + 1) == 0)
            {
                temp_str[0] = 0;
            }
            fwrite(temp_str, sizeof(char), strlen(temp_str), fp_htm);
            //	sprintf(str, "the temp_str: %s str argv[2]: %s str arg_name: %s \n\r",temp_str,argv[2],arg_name);
            //	printf(str);
            //	printf("****************************\n\r");
            temp = end_temp + 4 - 1; /* note after the if block there is a temp++ */
        }
        else
        {
            fputc(arg_name[0], fp_htm);
        }
        temp++;
    }
    fclose(fp);
    fclose(fp_htm);
    ini_release(fp_ini);

    sync();
    return 0;

}



int cgi_to_browser_by_ini(char *file_name_src, char *file_name_ini)
{
    char arg_name[50];
    char arg_val[50];
    int i, j, k, m;
    FILE *fp;
    int temp;
    int begin_temp;
    int end_temp;
    int temp_htm;
    char str[100];
    char *p;
    STRUCT_INI *fp_ini;

    if((fp = fopen(file_name_src, "rb")) == NULL)
    {
        printf("file open error!\n\r");
        return -1;
    }
    if((fp_ini = ini_new(file_name_ini)) == NULL)
    {
        printf("common.c: cgi_to_html_by_ini() , ini file open error!\r\n");
        return -1;
    }

    temp = 0;
    temp_htm = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(arg_name, strlen("<?cgi ") + 1, fp);
        if(!strcmp(arg_name, "<?cgi "))
        {
            begin_temp = temp;
            temp += strlen("<?cgi ");
            i = -1;
            do
            {
                i += 1;
                fseek(fp, temp + i, SEEK_SET);
                fgets(&arg_name[i], strlen("?>") + 1, fp);
            }
            while(strcmp(&arg_name[i], "?>"));
            arg_name[i - 1] = 0;

            end_temp = temp + i - 2;
            p = strstr(arg_name, ".");
            if((p < (&arg_name[i - 1])) && (p != NULL))
            {
                *p = 0;
                p++;
                if(ini_get_value_string(fp_ini, temp_str, arg_name, p) == NULL)
                {
                    temp_str[0] = 0;
                }
                else
                {
                    i = strlen(temp_str);
                    if(temp_str[i - 1] == '\n')
                    {
                        temp_str[i - 1] = 0;
                    }
                    printf(temp_str);
                }

            }
            temp = end_temp + 4 - 1;

        }
        else
        {
            putchar(arg_name[0]);
        }
        temp++;
    }

    fclose(fp);
    ini_release(fp_ini);

    sync();
    return 0;

}

int cgi_to_browser(char *file_name_src, char *argv_table, char div_char)
{
    char arg_name[50];
    char arg_val[50];
    int i, j, k, m;
    FILE *fp;
    FILE *fp_htm;
    int temp;
    int begin_temp;
    int end_temp;
    int temp_htm;
    char str[100];

    if((fp = fopen(file_name_src, "r")) == NULL)
    {
        printf("file open error!\n\r");
        return -1;
    }

    temp = 0;
    temp_htm = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(arg_name, strlen("<?cgi ") + 1, fp);
        if(!strcmp(arg_name, "<?cgi "))
        {
            begin_temp = temp;
            temp += strlen("<?cgi ");
            i = -1;
            do
            {
                i += 1;
                fseek(fp, temp + i, SEEK_SET);
                fgets(&arg_name[i], strlen("?>") + 1, fp);
            }
            while(strcmp(&arg_name[i], "?>"));
            arg_name[i - 1] = 0;
            end_temp = temp + i + 1;
            //	printf("*****************************\n\r");
            if(get_value(temp_str, argv_table, arg_name, div_char, 100) == NULL)
            {
                temp_str[0] = 0;
            }
            //write(temp_str,sizeof(char),strlen(temp_str));
            printf(temp_str);
            //	sprintf(str, "the temp_str: %s str argv[2]: %s str arg_name: %s \n\r",temp_str,argv[2],arg_name);
            //	printf(str);
            //	printf("****************************\n\r");
            temp = end_temp + 1;
        }
        else
        {
            //putc(arg_name[0]);
            arg_name[1] = 0;
            printf(arg_name);
        }
        temp++;
    }

    fclose(fp);

    sync();
    return 0;

}

int cgi_to_browser_by_xml(char *file_name_src, char *file_name_xml)
{
    char arg_name[50];
    char arg_val[50];
    int i, j, k, m;
    FILE *fp;
    FILE *fp_htm;
    int temp;
    int begin_temp;
    int end_temp;
    int temp_htm;
    char str[100];
    FILE *fp_xml;

    if((fp = fopen(file_name_src, "r")) == NULL)
    {
        printf("Common.c: file open error!\n\r");
        return -1;
    }
    if((fp_xml = xml_new(file_name_xml)) == NULL)
    {
        printf("Common.c: xml file open error!\r\n");
        return -2;
    }

    temp = 0;
    temp_htm = 0;
    while(!feof(fp))
    {
        fseek(fp, temp, SEEK_SET);
        fgets(arg_name, strlen("<?cgi ") + 1, fp);
        if(!strcmp(arg_name, "<?cgi "))
        {
            begin_temp = temp;
            temp += strlen("<?cgi ");
            i = -1;
            do
            {
                i += 1;
                fseek(fp, temp + i, SEEK_SET);
                fgets(&arg_name[i], strlen("?>") + 1, fp);
            }
            while(strcmp(&arg_name[i], "?>"));
            arg_name[i - 1] = 0;
            end_temp = temp + i + 1;
            //	printf("*****************************\n\r");
            //		if(get_value_from_xml(temp_str, file_name_xml, arg_name)<0){
            //			temp_str[0]=0;
            //		}
            if(xml_get_value_string(temp_str, arg_name, fp_xml) == NULL)
            {
                temp_str[0] = 0;
            }
            //write(temp_str,sizeof(char),strlen(temp_str));
            printf(temp_str);
            //	sprintf(str, "the temp_str: %s str argv[2]: %s str arg_name: %s \n\r",temp_str,argv[2],arg_name);
            //	printf(str);
            //	printf("****************************\n\r");
            temp = end_temp;
        }
        else
        {
            //putc(arg_name[0]);
            arg_name[1] = 0;
            printf(arg_name);
        }
        temp++;
    }

    xml_release(fp_xml);
    fclose(fp);

    sync();
    return 0;

}

const char HEX2DEC[256] =
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 1 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 2 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9, -1, -1, -1, -1, -1, -1,

    /* 4 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 5 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 6 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 7 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 8 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 9 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* A */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* B */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* C */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* D */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* E */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* F */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

void uri_decode(const char * psrc, const int len, char * pres, int * plen)
{
    // Note from RFC1630:  "Sequences which start with a percent sign
    // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
    // for future extension"
    const int SRC_LEN = len ;
    const unsigned char * const SRC_END = psrc + SRC_LEN ;
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%'

    char * const pstart = (char *)malloc(SRC_LEN) ;
    char * pend = pstart ;

    while (psrc < SRC_LAST_DEC)
    {
        if (*psrc == '%')
        {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(psrc + 1)])
                    && -1 != (dec2 = HEX2DEC[*(psrc + 2)]))
            {
                *pend++ = (dec1 << 4) + dec2;
                psrc += 3;
                continue;
            }
        }
        *pend++ = *psrc++;
    }

    // the last 2- chars
    while (psrc < SRC_END)
    {
        *pend++ = *psrc++;
    }
    *plen = (pend - pstart) ;
    memcpy(pres, pstart, *plen) ;
    free(pstart) ;
}

int x2i_to_ini(char *file_name_ini, char *file_name_x2i, char *data)
{
    char str_ini_argv[STR_LEN];
    char str_ini_sec[STR_LEN];
    char str_ini_key[STR_LEN];
    char str_ini_value[STR_LEN];
    char str_ini_our[STR_LEN];
    int idx;
    int i;
    int nlen;
    FILE *x2i_fp = fopen(file_name_x2i, "rb");
    STRUCT_INI *ini_fp = ini_new(file_name_ini);

    if(x2i_fp == NULL)
    {
        printf("Common: x2i_to_ini: x2i open error!; file_name_x2i:%s \n\r", file_name_x2i);
        return 0;
    }
    if(ini_fp == NULL)
    {
        printf("ini open error!\n\r");
        return 0;
    }

    while(fgets(str_ini_argv, STR_LEN, x2i_fp) != NULL)
    {
        if(fgets(str_ini_sec, STR_LEN, x2i_fp) != NULL)
        {
            if(fgets(str_ini_key, STR_LEN, x2i_fp) != NULL)
            {
                i = strlen(str_ini_argv);
                if(str_ini_argv[i - 2] == 0x0d)
                {
                    str_ini_argv[i - 2] = 0;
                }
                else
                {
                    str_ini_argv[i - 1] = 0;
                }
                i = strlen(str_ini_sec);
                if(str_ini_sec[i - 2] == 0x0d)
                {
                    str_ini_sec[i - 2] = 0;
                }
                else
                {
                    str_ini_sec[i - 1] = 0;
                }
                i = strlen(str_ini_key);
                if(str_ini_key[i - 2] == 0x0d)
                {
                    str_ini_key[i - 2] = 0;
                }
                else
                {
                    str_ini_key[i - 1] = 0;
                }
                get_value(str_ini_value, data, str_ini_argv, '&', STR_LEN);
                //printf("\r\n ini argv:___%s___ \r\n", str_ini_argv);
                //printf("ini value: %s \r\n", str_ini_argv);
                //printf("data: %s \r\n", data);
                //urlDecode(str_ini_our, str_ini_argv, STR_LEN);
                memset(str_ini_our, 0, STR_LEN);
                uri_decode(str_ini_value, strlen(str_ini_value), str_ini_our, &nlen);
                //printf("str_ini_our: %s : len = %d\r\n", str_ini_our, nlen);// str_ini_value 即所传上来的字符串，在此处理中文
                ini_put_value_string(ini_fp, str_ini_sec, str_ini_key, str_ini_our);
                if(fgets(str_ini_argv, STR_LEN, x2i_fp) == NULL)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    fclose(x2i_fp);
    ini_release(ini_fp);

    sync();
    return 0;
}


int linuxSystem(char *cmdFile)
{
    FILE *fp;
    char cmdStr[512];
    int i;
    if((fp = fopen(cmdFile, "r")) == NULL)
    {
        printf("file open error!");
        return -1;
    }

    i = 0;
    while((cmdStr[i] = fgetc(fp)) != EOF)
    {
        if(cmdStr[i] == '\n')
        {
            cmdStr[i] = 0;
            system(cmdStr);
            i = -1;
        }
        i++;
    }

    return 0;
}

/*
int gb23122utf8(char *outstr, char *instr){
	int ret;
	int inleft;
	int outleft;
	iconv_t cd;

	ret=0;
	inleft=strlen(instr);
	outleft=inleft*2;
	cd = iconv_open("UTF-8","GB2312");
	if(0==cd){
		return -2;
	}
	ret = iconv(cd, &instr, &inleft, &outstr, &outleft);
	iconv_close(cd);
	return ret;
}
*/

char *urlEncode(char *outstr, char *instr, int str_len)
{
    int len_instr;
    char *ret = outstr;
    len_instr = strlen(instr);
    while((*instr != 0) && (str_len--))
    {
        if(*instr == '?')
        {
            *(outstr++) = '1';
            *(outstr++) = '1';
        }
        else if(*instr == '=')
        {
            *(outstr++) = '1';
            *(outstr++) = '2';
        }
        else if(*instr == '&')
        {
            *(outstr++) = '1';
            *(outstr++) = '3';
        }
        else if(*instr == ';')
        {
            *(outstr++) = '1';
            *(outstr++) = '4';
        }
        else if(*instr == "/")
        {
            *(outstr++) = '1';
            *(outstr++) = '5';
        }
        else
        {
            *(outstr++) = '0';
            *(outstr++) = *instr;
        }
        instr++;
    }
    *outstr = 0;
    return ret;
}

char *urlDecode(char *outstr, char *instr, int str_len)
{
    int len_instr;
    char *ret = outstr;
    len_instr = strlen(instr);
    while((*instr != 0) && ((str_len -= 2) > 0))
    {
        if(*instr == '0')
        {
            instr++;
            *(outstr++) = *instr++;
        }
        else if(*instr == '1')
        {
            instr++;
            switch(*instr)
            {
                case '1':
                    *(outstr++) = '?';
                    break;
                case '2':
                    *(outstr++) = '=';
                    break;
                case '3':
                    *(outstr++) = '&';
                    break;
                case '4':
                    *(outstr++) = ';';
                    break;
                case '5':
                    *(outstr++) = '/';
                    break;
            }
            instr++;
        }
    }
    *outstr = 0;
    return ret;
}

/*
void main(){
	LinuxSystem("test.txt");
}*/
/*
void main(){
	cgi_to_html("./update_test.htm","../c2h/HardUpdateState.c2h","update_state=ok&update_time=123456",'&');
}

void main(){
	cgi_to_html("../net_cfg.xml","../net_cfg.c2h","host_ip=18.18.18.18&host_mac=10.10.10.10.10.10.10",'&');
}

void main(){
	char str[50];
	char str2[100];
	get_value_from_xml(str, "../net_cfg.xml", "host_ip");
	sprintf(str2,"the value:%s\n\r",str);
	printf(str2);

}
void main(){
	char str[50];
	STRUCT_INI *fp;
	fp=ini_new("test.ini");
	ini_get_value_string(fp, str, "seg", "key");
	printf("_____%s_____\n\r",str);
//	cgi_to_html_by_ini("test.htm","test.c2h", "../ini/sys_cfg.ini");
//	cgi_to_html_by_ini("test.htm","../c2h/AudioCfg.c2h", "../ini/sys_cfg.ini");
}
void main(){
	char str[50];
	get_value(str, "bbb=cc&dbb=aa&jaf=sfa", "bbb", '&', 50);
	printf(str);
}

void main(){
	x2i_to_ini("board_cfg_1.ini", "board_cfg_1.x2i", "board_1_key_0=1_0_%E6%88%91%E7%9A%84&board_1_key_1=2_0_&board_1_key_2=3_0_&board_1_key_3=4_0_&board_1_key_4=5_0_&board_1_key_5=6_0_&board_1_key_6=7_0_&board_1_key_7=8_0_&board_1_key_8=9_0_&board_1_key_9=10_0_&board_1_key_10=11_0_&board_1_key_11=12_0_&board_1_key_12=13_0_&board_1_key_13=14_0_&board_1_key_14=15_0_&board_1_key_15=16_0_");
}
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

#define STR_LEN 100
int main(int argc, char *argv[])
{
    char *str_htm;
    char *str_c2h;
    char *str_ini;

    char str_tmp_ini[STR_LEN];
    char str_tmp_c2h[STR_LEN];
    char str_tmp_htm[STR_LEN];
    char str_tmp[STR_LEN];
    FILE *c2h_fp;
    int i;
    char *p;

    if(argc != 4)
    {
        printf("format:  cmd htmName c2hName iniName\n\r");
        printf(" if htmName=0 , then c2hName means script \n\r");
        printf(" iniName=1, then lan to c2h \n\r");
        printf(" iniName=0, then c2h to htm \n\r");
        return -1;
    }
    str_htm = argv[1];
    str_c2h = argv[2];
    str_ini = argv[3];
    printf("  %s %s %s \n\r", str_htm, str_c2h, str_ini);
    if(strcmp(str_htm, "0") == 0)
    {
        c2h_fp = fopen(str_c2h, "rb");
        if(c2h_fp == NULL)
        {
            printf("c2h_fp open error!; file_name:%s \n\r", str_c2h);
            return 0;
        }
        while(fgets(str_tmp_ini, STR_LEN, c2h_fp) != NULL)
        {
            if(fgets(str_tmp_c2h, STR_LEN, c2h_fp) != NULL)
            {
                if(fgets(str_tmp_htm, STR_LEN, c2h_fp) != NULL)
                {
                    i = strlen(str_tmp_ini) - 1;
                    str_tmp_ini[i] = 0;
                    i = strlen(str_tmp_c2h) - 1;
                    str_tmp_c2h[i] = 0;
                    i = strlen(str_tmp_htm) - 1;
                    str_tmp_htm[i] = 0;
                    printf("htm:%s ; c2h:%s ; ini:%s \n\r", str_tmp_htm, str_tmp_c2h, str_tmp_ini);
                    if((p = strstr(str_tmp_htm, "SubBoardCfg")) != NULL)
                    {
                        memcpy(str_tmp, p, strlen(p));
                        i = strlen(str_tmp) - strlen("SubBoardCfg");
                        memcpy(str_tmp, str_tmp + strlen("SubBoardCfg"), i);
                        p = strstr(str_tmp, ".htm");
                        *p = 0;
                        i = atoi(str_tmp);
                        sprintf(str_tmp, "find ../c2h/ -name SubBoardCfgNum.c2h -exec sed -i -r 's/<\\[num\\]>/%d/' {} \\;", i);
                        i = system("cp ../c2h/SubBoardCfg.c2h ../c2h/SubBoardCfgNum.c2h");
                        if(i != 0)
                        {
                            printf("response_Main: system(cp) error\n\r");
                            return -1;
                        }
                        i = system(str_tmp);
                        if(i != 0)
                        {
                            printf("response_Main: system(str_htm) error\n\r");
                            return -1;
                        }
                    }
                    switch(atoi(str_ini))
                    {
                        case 0:
                            printf("cgi_to_html_by_ini: %s %s %s\n", str_tmp_htm, str_tmp_c2h, str_tmp_ini);
                            cgi_to_html_by_ini(str_tmp_htm, str_tmp_c2h, str_tmp_ini);
                            break;
                        case 1:
                            printf("lan_to_cgi_by_ini: %s %s %s\n", str_tmp_htm, str_tmp_c2h, str_tmp_ini);
                            lan_to_cgi_by_ini(str_tmp_htm, str_tmp_c2h, str_tmp_ini);
                            break;
                    }
                    if(fgets(str_tmp_ini, STR_LEN, c2h_fp) == NULL)
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
        fclose(c2h_fp);
    }
    else
    {
        cgi_to_html_by_ini(str_htm, str_c2h, str_ini);
    }

}



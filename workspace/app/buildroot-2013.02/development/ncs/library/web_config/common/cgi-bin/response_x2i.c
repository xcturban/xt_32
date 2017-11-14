#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

#define STR_LEN 50
#define STR_LONG_LEN 400
void main()
{
    long len;
    char *data;
    char str[STR_LONG_LEN];

    char str_ini[STR_LEN];
    char str_x2i[STR_LEN];
    char str_ret[STR_LONG_LEN];
    char str_retstr[STR_LONG_LEN];
    char str_sh[STR_LEN];

    char str_tmp_x2i[STR_LEN];
    char str_tmp_sh[STR_LEN];
    char str_tmp_ini[STR_LEN];

    *str_sh = 0;


    printf("Content-type: text/html \n\n");
    printf("<html>");

    printf("<head>");
    printf("<meta http-equiv=content-type content=\"text/html; charset=utf-8\">");
    printf("</head>");
    data = getenv("QUERY_STRING");
    if(data  != NULL)
    {
//		gb23122utf8(temp_data,data);

        if(get_value(str_tmp_x2i, data, "obj_x2i", '&', STR_LEN) == NULL)
        {
            printf("obj_x2i not exist\r\n");
        }
        if(get_value(str_tmp_ini, data, "obj_ini", '&', STR_LEN) == NULL)
        {
            sprintf(str_tmp_ini, "sys_cfg.ini");
        }
        if(get_value(str_ret, data, "obj_ret", '&', STR_LONG_LEN) == NULL)
        {
            printf("obj_ret not found\r\n");
        }
        if(get_value(str_tmp_sh, data, "obj_sh", '&', STR_LEN) != NULL)
        {
            sprintf(str_sh, "../shell/%s", str_tmp_sh);
        }


        get_file_path(str_x2i, str_tmp_x2i);
        get_file_path(str_ini, str_tmp_ini);

        x2i_to_ini(str_ini, str_x2i, data);
        if(*str_sh != 0)
        {
            if(system(str_sh) != 0)
            {
                //printf("linuxSystem(SHELL_FILE)  error !");
            }
        }
    }
    else
    {
        printf("response_x2i.c: data==NULL;\n\r");
    }

    urlDecode(str_retstr, str_ret, STR_LONG_LEN);
    sprintf(str, "<script>window.location.href=\"%s\" </script>", str_retstr);
    printf(str);
    printf("</html>");

}



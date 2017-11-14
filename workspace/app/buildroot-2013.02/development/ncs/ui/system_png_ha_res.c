#include "system_png_ha_res.h"

static TYPE_PNG_RES_SL *s_p_png_ha_res_array = NULL;

void init_png_res_sl(void)
{
	int i = 0;
	s_p_png_ha_res_array = malloc(PNG_HA_RES_SIZE*sizeof(TYPE_PNG_RES_SL));

	if(!s_p_png_ha_res_array)
	{
		return ;
	}
	for(i = 0; i < PNG_HA_RES_SIZE; i++)
	{
		memset(&s_p_png_ha_res_array[i], 0, sizeof(TYPE_PNG_RES_SL));
	}
}

int calc_addr_png_ha_res_ex(int Num, int Cnt)
{
	int Tmp = 0;

	Tmp = Num + 2*Cnt -1;
	if(Tmp >= PNG_HA_RES_SIZE)
	{
		Tmp = 0;
	}
	return Tmp;
}

int calc_addr_png_ha_res(char *ptr)
{
	int Tmp = 0;
	int i = 0;

	while(ptr[i] != '\0')
	{
		Tmp = Tmp*131 + ptr[i];
		i++;
	}
	if(Tmp < 0)
	{
		Tmp *=(-1);
	}
	return (Tmp%PNG_HA_RES_SIZE);
}

BITMAP *png_ha_res_find_by_name(char* name)
{
	if(name == NULL)
	{
		return NULL;
	}
	int Tmp = calc_addr_png_ha_res(name);
	int i = 0;

	if(s_p_png_ha_res_array[Tmp].flag)
	{
		if(strcmp(s_p_png_ha_res_array[Tmp].pic_name, name) == 0)
		{
			return &s_p_png_ha_res_array[Tmp].bitmap;
		}
	}

	while(1)
	{
		Tmp = calc_addr_png_ha_res_ex(Tmp, ++i);
		if(s_p_png_ha_res_array[Tmp].flag)
		{
			if(strcmp(s_p_png_ha_res_array[Tmp].pic_name, name) == 0)
			{
				return &s_p_png_ha_res_array[Tmp].bitmap;
			}
		}
		if(i >= PNG_HA_RES_SIZE)
		{
			break;
		}
	}
	return &s_p_png_ha_res_array[0].bitmap;
}
/***************************************************************************************************
 * 函 数 名: system_bmp_res_loading_ex
 * 版    权: 长沙世邦通信技术有限公司，未经允许，不得转载
 * 日    期: 2016-12-02 09:30
 * 文 件 名: ui/system_bmp_res.c
 * 功    能: 
 * 参    数: 无
 * 返 回 值: 无
****************************************************************************************************/
bool_t system_bmp_res_loading_ex(HDC hdc)
{
    DIR    *dir;
    struct  dirent *ptr;
    struct stat s_info;
    char * path = SYSTEM_PNG_RES_PATH;
    char * a_path = NULL;
    char file_dir[512];
    int file_count = 0;
	long int Tmp = 0;
	int i = 0;

    bmp_res_t * afile = NULL;
    a_path = absolute_path_new(path);
    SPON_LOG_INFO("=====>system_bmp_res_loading begin ... \n");

    if(!a_path)
    {
        SPON_LOG_ERR("create error !\n");
        return FALSE;
    }
    stat(a_path,&s_info);
	if(S_ISDIR(s_info.st_mode))
    {
		dir = opendir(a_path);
        while((ptr = readdir(dir)) != NULL)
        {
            memcpy(file_dir,a_path,512);
            strcat(file_dir,ptr->d_name);
            stat(file_dir,&s_info);
			if(S_ISREG(s_info.st_mode))
            {
				if(strstr(file_dir,".png") || strstr(file_dir,".bmp") )
				{
					Tmp = calc_addr_png_ha_res(ptr->d_name);
					if(!s_p_png_ha_res_array[Tmp].flag)
					{
						s_p_png_ha_res_array[Tmp].flag = TRUE;
						LoadBitmap(hdc, &s_p_png_ha_res_array[Tmp].bitmap, file_dir);
						strcpy(s_p_png_ha_res_array[Tmp].pic_name, ptr->d_name);
						/*printf("name :%s\n", ptr->d_name);*/
					}
					else
					{
						i = 0;
						while(1)
						{
							Tmp = calc_addr_png_ha_res_ex(Tmp, ++i);
							if(!s_p_png_ha_res_array[Tmp].flag)
							{
								s_p_png_ha_res_array[Tmp].flag = TRUE;
								LoadBitmap(hdc, &s_p_png_ha_res_array[Tmp].bitmap, file_dir);
								strcpy(s_p_png_ha_res_array[Tmp].pic_name, ptr->d_name);
								/*printf("name[i] :%s\n", ptr->d_name);*/
								break;
							}
							if(i >= PNG_HA_RES_SIZE)
							{
								break;
							}
						}
					}
				}
            }
        }
    }
    closedir(dir);
    free(a_path);
    SPON_LOG_INFO("=====>system_bmp_res_loading end ... \n");
    return TRUE;
}




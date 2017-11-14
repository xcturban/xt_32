#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

#include "system_info.h"
#include "system_bmp_res.h"

static HDC hdc_screen;
static struct list_head * bmp_res_list_head;
static pthread_mutex_t image_load_mutex;

int str_hash_value_get(char * str)
{
    int hash = 0;
    int i=0;
    int len = strlen(str);
    for(i=0; i<len; i++)
        hash += (i+1)*str[i];

    return hash;
}

int bmp_list_count(struct list_head *head)
{

    int count = 0;
    struct list_head *p;

    list_for_each(p, head)
    count++;

    SPON_LOG_INFO("=====>%s - count:%d\n", __func__, count);
    return count;
}

bmp_res_t *bmp_res_new(void)
{
    bmp_res_t *bmp = malloc(sizeof(bmp_res_t));
    //SPON_LOG_INFO(bmp);

    memset(bmp, 0, sizeof(bmp_res_t));
    return bmp;
}

bool_t system_bmp_res_unload(struct list_head * head)
{
    if(!head) return FALSE;

    struct list_head *p = NULL;
    bmp_res_t *bmp = NULL;
    /*pthread_mutex_lock(&image_load_mutex);*/
    while((p = head->next) && (p != head))
    {
		/*if(!strcmp(bmp.pic_name, "vol_1.png")|| \*/
				/*!strcmp(bmp.pic_name, "vol.png")|| \*/
				/*!strcmp(bmp.pic_name, "online.png")||\*/
				/*!strcmp(bmp.pic_name, "offline.png")*/
				/*)*/
		/*{*/
			/*continue;*/
		/*}*/
        common_list_del(p);
        bmp = list_entry(p, bmp_res_t, list);
        UnloadBitmap(&bmp->bitmap);
        free(bmp);
    }
    /*pthread_mutex_unlock(&image_load_mutex);*/
    return TRUE;
}

bool_t system_bmp_res_load_to_list(bmp_res_t * afile)
{
    if(!afile ) return FALSE;
    /*pthread_mutex_lock(&image_load_mutex);*/
    common_list_append(&afile->list, bmp_res_list_head);
    /*pthread_mutex_unlock(&image_load_mutex);*/
}

bmp_res_t* system_bmp_load(char * file)
{
    struct stat s_info;
    char * path = SYSTEM_PNG_RES_PATH;
    char * a_path = NULL;
    char file_dir[512];
    int file_count = 0;
    bmp_res_t * afile = NULL;

    if(!file)
    {
        SPON_LOG_ERR("file not exist !\n");
        return FALSE;
    }

    strcpy(file_dir,path);
    strcat(file_dir,file);
    stat(file_dir,&s_info);

	if(S_ISREG(s_info.st_mode))
    {
        if(strstr(file_dir,".png") || strstr(file_dir,".bmp") )
        {
            afile = bmp_res_new();
            if (!afile)
            {
                SPON_LOG_ERR("file malloc error !\n");
                return NULL;
            }
            LoadBitmap(hdc_screen, &afile->bitmap, file_dir);
            strcpy(afile->pic_name,file);
            afile->id = str_hash_value_get(file);
//			printf("loading res:[%s] id:[%d]\n",file,afile->id);
            system_bmp_res_load_to_list(afile);
        }
    }
    return afile;
}

bool_t system_bmp_res_init(struct list_head *head)
{
    static bool_t is_system_bmp_res_init = FALSE;

    if (!is_system_bmp_res_init)
    {
        common_list_head_init(head);
        bmp_res_list_head = head;
        is_system_bmp_res_init = TRUE;
        /*pthread_mutex_init(&image_load_mutex, NULL);*/
    }

    return is_system_bmp_res_init;
}

bool_t system_bmp_res_load(HDC hdc,struct list_head *head)
{
    /*system_bmp_res_init(hdc,head);*/
}

bmp_res_t *bmp_is_loaded(struct list_head *head,char* name)
{
//	struct list_head *head = bmp_res_list_head;
    struct list_head *p = NULL;
    bmp_res_t *afile = NULL;
    int hash = str_hash_value_get(name);

    if (!head) return NULL;
    list_for_each(p, head)
    {
        afile = list_entry(p, bmp_res_t, list);
        if (afile->id == hash)
            return afile;
    }
    return NULL;
}

bmp_res_t *bmp_find_by_name(struct list_head *head,char* name)
{
    bmp_res_t *afile = bmp_is_loaded(head, name);
	if(!afile)
	{
		SPON_LOG_ERR("not find %s", afile->pic_name);
		/*afile = system_bmp_load(name);*/
	}

    return afile;
}


bool_t system_bmp_res_loading(HDC hdc,struct list_head *head)
{
    static bool_t is_loaded = FALSE;

    if(is_loaded)
        return FALSE;

    DIR    *dir;
    struct  dirent *ptr;
    struct stat s_info;
    char * path = SYSTEM_PNG_RES_PATH;
    char * a_path = NULL;
    char file_dir[512];
    int file_count = 0;

    bmp_res_t * afile = NULL;
    /*system_bmp_res_init(hdc,head);*/
    system_bmp_res_unload(bmp_res_list_head);

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
					if(!bmp_is_loaded(bmp_res_list_head,ptr->d_name))
					{
						afile = bmp_res_new();
						if (!afile) continue;
						LoadBitmap(hdc_screen, &afile->bitmap, file_dir);
						strcpy(afile->pic_name,ptr->d_name);
						afile->id = str_hash_value_get(ptr->d_name);
						system_bmp_res_load_to_list(afile);
					}
				}
            }
        }
    }
    closedir(dir);
    free(a_path);
    SPON_LOG_INFO("=====>system_bmp_res_loading end ... \n");
    is_loaded = TRUE;
    return TRUE;
}


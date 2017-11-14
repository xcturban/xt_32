#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <assert.h>

#include <common_lib/common_lib.h>

#include "file.h"

// #include "private.h"
#include "system_info.h"
TYPE_BROADCAST_FILE_PLAY s_file_list_obj;

char *file_list_obj_get_play_path(TYPE_BROADCAST_FILE_PLAY *pobj)
{
	return pobj->path;
}

void file_list_obj_set_play_path(TYPE_BROADCAST_FILE_PLAY *pobj, char *pDecPath)
{
	strcpy(pobj->path, pDecPath);
}

int file_list_count(struct list_head *head)
{

    int count = 0;
    struct list_head *p;

	list_for_each(p, head)
		count++;

    SPON_LOG_INFO("=====>%s - count:%d\n", __func__, count);

    return count;
}


file_t *file_find_by_id(struct list_head *head,int id)
{
	struct list_head *p;
	file_t *file;
    if(!head)
        return NULL;
    list_for_each(p, head)
    {
        file = list_entry(p, file_t, list);
        if (file->id ==  id)
            return file;
    }

    return NULL;
}


file_t *file_new()
{
    int ret = -1;
    file_t *file;

    file = malloc(sizeof(file_t));
    if (file == NULL)
    {
        SPON_LOG_ERR("=====>%s - file malloc failed:%s\n", __func__, strerror(errno));
        return NULL;
    }
    memset(file, 0, sizeof(file_t));
    return file;
}

void file_delete(file_t *file)
{
    free(file);
}

void file_list_destroy(struct list_head * head )
{
    if(!head)
        return;
#if 0
    list_destroy(head,file_t,list);
#else
    struct list_head *p;
    file_t *file;
    while(p = head->next)
    {
        if(p != head)
        {
            list_del(p);
            file = list_entry(p, file_t, list);
            file_delete(file);
        }
        else
            break;
    }
#endif
}


static int file_broadcast_list_init(struct list_head * head)
{
    struct list_head *p;
    file_t *file;
    file_t *file_new;
    int count = 0;
    /*INIT_LIST_HEAD(head);*/

    list_for_each(p, &s_file_list_obj.file_player_list)
    {
        file = list_entry(p, file_t, list);
        if (file->rate ==  22050 && file->nchannels == 1)
        {
            file_new = malloc(sizeof(file_t));
            memcpy(file_new, file, sizeof(file_t));
            file_new->dir = FILE_DIR_BROADCAST;
            file_new->id = count++;
            INIT_LIST_HEAD(&file_new->list);
            list_add_tail(&file_new->list, head);
        }
    }
    return file_list_count(head);
}

int file_wave_list_create(struct list_head *head)
{
    DIR    *dir;
    struct  dirent *ptr;
    struct dirent **namelist;
    int n;
    struct stat info;
    char * path = file_list_obj_get_play_path(&s_file_list_obj);
    char * a_path = NULL;
    char file_dir[512];
    int file_wave_count = 0;
    wave_info wave;

    file_t * afile = NULL;

    a_path = absolute_path_new(path);

    if(!a_path)
    {
        SPON_LOG_ERR("=====>%s - wave file list is no exist!\n", __func__);
        return -1;
    }

    stat(a_path,&info);

    if(S_ISDIR(info.st_mode))
    {
        n = scandir(a_path, &namelist, 0, alphasort);
        if (n < 0)
        {
            printf("not found !\n");
        }
        while(n--)
        {
            memcpy(file_dir,a_path,512);
            strcat(file_dir, namelist[n]->d_name);
            stat(file_dir,&info);
            if(S_ISREG(info.st_mode))
            {
                if(wave_open(file_dir,&wave) == 0)
                {
                    afile = file_new(&wave);
                    if(!afile)
                        continue;
                    afile->hsize           = wave.hsize;
                    afile->nchannels       = wave.nchannels;
                    afile->bps             = wave.bps;
                    afile->rate            = wave.rate;
                    afile->fsize           = info.st_size;
                    afile->time_sec        = (info.st_size-wave.hsize)/wave.bps;
                    afile->type            = WAVE;
                    afile->dir             = FILE_DIR_PLAY;
                    afile->file_sel_status = NOT_FILE_SEL;
                    /*utf8_to_gb2312(ptr->d_name,afile->name,strlen(ptr->d_name));*/
                    strcpy(afile->name, namelist[n]->d_name);
                    strcpy(afile->path,file_dir);
                    afile->id = ++file_wave_count;
                    list_add_tail(&afile->list,head);
                }
            }
        }
        free(namelist);
    }
    return file_list_count(head);
}

void file_list_update_sel_statue(struct list_head *head, int id, file_sel_t statue)
{
    struct list_head *p;
    file_t *file;

    list_for_each(p, &s_file_list_obj.file_broadcast_list)
    {
        file = list_entry(p, file_t, list);
		if(file->id == id)
		{
			file->file_sel_status = statue;
			break;
		}

        /*if (file->rate ==  22050 && file->nchannels == 1)*/
        /*{*/
            /*file_new = malloc(sizeof(file_t));*/
            /*memcpy(file_new,file,sizeof(file_t));*/
            /*file_new->dir = FILE_DIR_BROADCAST;*/
            /*file_new->id = ++count;*/
            /*INIT_LIST_HEAD(&file_new->list);*/
            /*list_add_tail(&file_new->list, head);*/
        /*}*/
    }


}

void file_list_dst(void)
{
    file_list_destroy(&s_file_list_obj.file_player_list);
    file_list_destroy(&s_file_list_obj.file_broadcast_list);
	file_set_list_init_flag(FALSE);
}

void file_list_init(void)
{
    file_wave_list_create(&s_file_list_obj.file_player_list);
    file_broadcast_list_init(&s_file_list_obj.file_broadcast_list);
	file_set_list_init_flag(TRUE);
	SPON_LOG_INFO("USB file list init ok\n");
}

void file_set_list_init_flag(bool_t flag)
{
	s_file_list_obj.flag_init_complete = flag;
}

void file_list_printf(void)
{
    struct list_head *p;
    file_t *file;
	int i = 0;

    list_for_each(p, &s_file_list_obj.file_player_list)
    {
        file = list_entry(p, file_t, list);
		printf("list [%d]:%s\n",i++, file->name);

        /*if (file->rate ==  22050 && file->nchannels == 1)*/
        /*{*/
            /*file_new = malloc(sizeof(file_t));*/
            /*memcpy(file_new,file,sizeof(file_t));*/
            /*file_new->dir = FILE_DIR_BROADCAST;*/
            /*file_new->id = ++count;*/
            /*INIT_LIST_HEAD(&file_new->list);*/
            /*list_add_tail(&file_new->list, head);*/
        /*}*/
    }
}


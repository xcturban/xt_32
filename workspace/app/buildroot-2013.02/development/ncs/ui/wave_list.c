#include "wave_list.h"

int read_wav_header(int fd, wave_info * d)
{
    char header1[sizeof(riff_t)];
    char header2[sizeof(format_t)];
    char header3[sizeof(data_t)];
    int count;

    riff_t *riff_chunk     = (riff_t*)header1;
    format_t *format_chunk = (format_t*)header2;
    data_t *data_chunk     = (data_t*)header3;

    unsigned long len=0;

    len = read(fd, header1, sizeof(header1)) ;
    if (len != sizeof(header1))
    {
        goto not_a_wav;
    }

    if (0 != strncmp(riff_chunk->riff, "RIFF", 4) || 0 != strncmp(riff_chunk->wave, "WAVE", 4))
    {
        goto not_a_wav;
    }

    len = read(fd, header2, sizeof(header2)) ;
    if (len != sizeof(header2))
    {
        printf("Wrong wav header: cannot read file \n");
        goto not_a_wav;
    }

    d->rate=le_uint32(format_chunk->rate);
    d->nchannels=le_uint16(format_chunk->channel);
    d->bps = le_uint32(format_chunk->bps);

    if (format_chunk->len-0x10>0)
    {
        lseek(fd,(format_chunk->len-0x10),SEEK_CUR);
    }

    d->hsize=sizeof(wave_header_t)-0x10+format_chunk->len;

    len = read(fd, header3, sizeof(header3)) ;
    if (len != sizeof(header3))
    {
        printf("Wrong wav header: cannot read file \n");
        goto not_a_wav;
    }
    count=0;
    while (strncmp(data_chunk->data, "data", 4)!=0 && count<30)
    {
        printf("skipping chunk=%s len=%i \n", data_chunk->data, data_chunk->len);
        lseek(fd,data_chunk->len,SEEK_CUR);
        count++;
        d->hsize=d->hsize+len+data_chunk->len;

        len = read(fd, header3, sizeof(header3)) ;
        if (len != sizeof(header3))
        {
            printf("Wrong wav header: cannot read file \n");
            goto not_a_wav;
        }
    }
#ifdef WORDS_BIGENDIAN
    if (le_uint16(format_chunk->blockalign)==le_uint16(format_chunk->channel) * 2)
        d->swap=TRUE;
#endif
    return 0;

not_a_wav:
    /*rewind*/
    lseek(fd,0,SEEK_SET);
    d->hsize=0;
    return -1;
}

int read_wav_header_ex(char *pFilePath, wave_info_ex * d)
{
	char header1[sizeof(riff_t)];
	char header2[sizeof(format_t)];
	char header3[sizeof(data_t)];
	unsigned long len = 0;
	uint16_t count    = 0;
	FILE *fd          = NULL;

	riff_t *riff_chunk     = (riff_t*)header1;
	format_t *format_chunk = (format_t*)header2;
	data_t *data_chunk     = (data_t*)header3;



    fd = fopen(pFilePath, "r");
	if(fd == NULL)
	{
		printf("open %s failed\n", pFilePath); 
		return -1;
	}
	len = fread(header1, 1, sizeof(header1), fd);
	if (len != sizeof(header1))
	{
		goto not_a_wav;
	}

	if (0 != strncmp(riff_chunk->riff, "RIFF", 4) || 0 != strncmp(riff_chunk->wave, "WAVE", 4))
	{
		goto not_a_wav;
	}

	len = fread(header2, 1, sizeof(header2), fd) ;
	if (len != sizeof(header2))
	{
		printf("Wrong wav header: cannot fread file \n");
		goto not_a_wav;
	}

	d->rate    = format_chunk->rate;
	d->channel = format_chunk->channel;

	if (format_chunk->len-0x10>0)
	{
		fseek(fd, (format_chunk->len-0x10), SEEK_CUR);
	}

	d->header_size = sizeof(wave_header_t)-0x10+format_chunk->len;

	len = fread(header3, 1, sizeof(header3), fd) ;
	if (len != sizeof(header3))
	{
		printf("Wrong wav header: cannot fread file \n");
		goto not_a_wav;
	}
	count=0;
	while (strncmp(data_chunk->data, "data", 4)!=0 && count<30)
	{
		printf("skipping chunk=%s len=%i \n", data_chunk->data, data_chunk->len);
		fseek(fd, data_chunk->len, SEEK_CUR);
		count++;
		d->header_size=d->header_size+len+data_chunk->len;

		len = fread(header3, 1, sizeof(header3), fd) ;
		if (len != sizeof(header3))
		{
			printf("Wrong wav header: cannot fread file \n");
			goto not_a_wav;
		}
	}
	d->dataLen = data_chunk->len;
    fclose(fd);
	return 0;

not_a_wav:
	/*rewind*/
	fseek(fd, 0, SEEK_SET);
	d->header_size = 0;
    fclose(fd);
	return -1;
}

int wave_open(void *arg, wave_info * info)
{

    const char *file=(const char*)arg;
    int fd;
    if ((fd=open(file,O_RDONLY))==-1)
    {
        perror("Failed to open \n");
        return -1;
    }
    memset(info,0,sizeof(wave_info));
    if (read_wav_header(fd,info) != 0)
    {
//		printf("File %s wav header could be found.\n",file);
        close(fd);
        return -1;
    }

//	printf("%s opened: rate=%i,channel=%i \n",file,info.rate,info.nchannels);
    close(fd);
    return 0;
}

char *absolute_path_new(char * from)
{
    char * path = (char *)malloc(512);
    memset(path,0,512);
    //获取当前目录绝对路径
	if (NULL == realpath(from, path))
    {
        /*printf("absolute_path_new Error\n");*/
        free(path);
        return NULL;
    }
    strcat(path, "/");
    /*printf("absolute_path_new:%s\n", path);*/
    return path;
}


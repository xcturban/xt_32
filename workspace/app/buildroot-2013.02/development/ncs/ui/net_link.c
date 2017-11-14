#include <stdio.h>
#include "net_link.h"
#include <sys/stat.h>

bool_t usb_link(void)
{
	char * path = FILE_USB_DIR;
	struct stat s_info;
	char * a_path = absolute_path_new(path);

	if(!a_path) 
	{
		return FALSE;
	}
	stat(a_path,&s_info);
	free(a_path);
	if(S_ISDIR(s_info.st_mode)) 
	{
		return TRUE;
	}
	return FALSE;
}

/*bool_t sdcard_link(void){*/
	/*char * path = FILE_SDCARD_DIR;*/
	/*char * a_path = absolute_path_new(path);*/
	/*if(!a_path) return FALSE;*/
     /*struct stat s_info;*/
	/*stat(a_path,&s_info);*/
	/*free(a_path);*/
	/*if(S_ISDIR(s_info.st_mode)) */
		/*return TRUE;*/
	/*return FALSE;*/
/*}*/

bool_t net_link(void)
{
	char v;
	int ret;
	FILE *fp = fopen("/sys/class/net/eth0/carrier", "r");

	if (!fp)
		return FALSE;

	ret = fread(&v, sizeof(v), 1, fp);
	if (ret != 1) {
		fclose(fp);
		return FALSE;
	}

	fclose(fp);

	if (v == '1')
		return TRUE;
	else if (v == '0')
		return FALSE;
	
	return FALSE;
}

void ncs_get_mac_address(char *address)
{
	int ret;
	FILE *fp = fopen("/sys/class/net/eth0/address", "r");

	if (!fp)
		return NULL;
	ret = fread(address, 1, 17, fp) ;
	fclose(fp);
}



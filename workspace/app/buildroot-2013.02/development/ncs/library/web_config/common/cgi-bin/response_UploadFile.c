#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEN   50
#define bufferSize 512
#define FPATH	"../uploads/"
#define STATUS_FILE  "../uploadStatus.htm"
#define SHELL_FILE "../shell/response_Firmware.sh"
#define SYS_LOG "firmware.ini"
/*
int writeStatus(char *fileName,char *status){
	FILE *fp;
	if((fp=fopen(fileName,"w"))==NULL){
		return -1; // means error open file error 
	}
	fwrite(status,sizeof(char),strlen(status),fp);
	fclose(fp);
	return 0;
}
*/

void main(){
	char *contentType;
	int contentLen;
	char *buffer;
	int temp;
	int i;
	int pos;
	char str[bufferSize];
	char oldStr[bufferSize*2];
	short oldStrFull;
	char *p;
	char fname[200];
	char ffname[512];
	int fSize;
	int bSize;
	FILE *fp;
	char boundry[100];
	int completedSize;
	int scroll_current_pos;
	int scroll_old_pos;
	char strUploadFileStatus[20];
 

//int a,b,c,d,e;

	char str_ini[STR_LEN];
	char str_htm[STR_LEN];
	char str_c2h[STR_LEN];
 
	get_file_path(str_ini, SYS_LOG);

	printf("Content-type: text/html \n\n");
	contentType=getenv("CONTENT_TYPE");
	contentLen=atoi(getenv("CONTENT_LENGTH"));
	pos=0;
	i=10;
	fgets(boundry,100,stdin);
	pos+=strlen(boundry);
	while(i--){
		if(fgets(str,bufferSize,stdin)){
			pos+=strlen(str);
			if(strstr(str,"filename=")){
				p=strstr(str,"filename=");
				strcpy(fname,p+strlen("filename=\""));
				*p=0;
				break;
			}
		}
	}
	fgets(str,bufferSize,stdin);
	pos+=strlen(str);
	fgets(str,bufferSize,stdin);
	pos+=strlen(str);
	fSize=contentLen-pos-strlen(boundry)-4;
	completedSize=0;
	bSize=4096;
	memcpy(ffname,FPATH,strlen(FPATH));
	memcpy(ffname+strlen(FPATH),fname,strlen(fname));
	*(ffname+strlen(FPATH)+strlen(fname)-strlen("\"\n\r"))=0;
	if(!(fp=fopen(ffname,"wb"))){
		printf("file open error <br>");
		return;
	}
	oldStrFull=0;
	scroll_old_pos=0;
	do{
		if(fSize>bufferSize){
			bSize=bufferSize;
			fSize-=bufferSize;
		}else{
			bSize=fSize;
			fSize=0;
		}
		fread(str,sizeof(char),bSize,stdin);
		if(oldStrFull==0){
			memcpy(oldStr, str, bSize);
			oldStrFull=1;
			if(fSize!=0){
				continue;
			}
		}
		memcpy(oldStr+bufferSize,str,bSize);
		if(strstr(oldStr,boundry)){
			pos=strstr(oldStr,boundry)-oldStr-2; /* the 2 means \r\n  */
			fwrite(oldStr, sizeof(char), pos, fp);
			break;
		}
		fwrite(oldStr, sizeof(char), bufferSize, fp);
                if(fSize<=0){
			fwrite(oldStr+bufferSize, sizeof(char), bSize, fp);
			break;
		}
//		completedSize+=bufferSize;
//		sprintf(strUploadFileStatus,"%d",completedSize);
//		writeStatus(STATUS_FILE,strUploadFileStatus);
		memcpy(oldStr,str,bufferSize);
	}while(fSize>0);
	fclose(fp);


        sprintf(str, "%s", SHELL_FILE);
	if(system(str)!=0){
		printf("linuxSystem(SHELL_FILE)  error !");
	}

	printf("<body>");
	printf("<script>parent.location='/cache/Firmware.htm?jmp=true'; </script>");
	printf("</body>");
//	writeStatus(STATUS_FILE,"over");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"

#define STR_LEN 400
#define STR_FILE_NAME_C2H "../c2h/Log.c2h"

int file_line_cnt(FILE *fp){
	int ret=0;
	char str[STR_LEN];
	while(!feof(fp)){
		fgets(str, STR_LEN, fp);
		if(strstr(str,"\n")){
			ret++;
		}
	}
	ret++;
	return ret;
}

char *file_line2pos(FILE *fp, int line){
	int ret=0;
	char str[STR_LEN];
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp)){
		if(ret==line){
			return ftell(fp);
		}
		fgets(str, STR_LEN, fp);
		if(strstr(str, "\n")){
			ret++;
		}
	}
	return NULL;
}

void main(){
	long len;
	char *data=NULL;
	char str[STR_LEN];
	int page_size=0;
	int page_index=0;
	int line_cnt=0;
	int i;
	FILE *fp;
	char *pos;
	FILE *fp_c2h;
	int i_c2h;
	int temp;
	int begin_temp;
	int end_temp;
	char *str_file_path_log[50];
	char *str_file_path_c2h[50];
 

        char tmp_c2h[STR_LEN];
        char tmp_log[STR_LEN];
        char str_c2h[STR_LEN];
        char str_log[STR_LEN];

	printf("Content-type: text/html \n\n");

	data = getenv("QUERY_STRING");
	if(data  != NULL){
		if(get_value(str, data, "page_size", '&', 50)){
			page_size=atoi(str);
		}else{
			printf("Main.c: main, get value error\r\n");
			exit(1);
		}
		if(get_value(str, data, "page_index", '&', 50)){
			page_index=atoi(str);
		}else{
			printf("Main.c: main, get value error\r\n");
			exit(1);
		}
  		if(get_value(tmp_c2h, data, "obj_c2h", '&', 50)){
		}else{
			printf("Log.c main, get value error c2h\r\n");
			exit(1);
		}
    		if(get_value(tmp_log, data, "obj_log", '&', 50)){
		}else{
			printf("Log.c main, get value error log\r\n");
			exit(1);
		}

	}else{
		printf("Main.c: main, data==NULL");
		exit(1);
	}

	get_file_path(str_file_path_c2h, tmp_c2h);
	if((fp_c2h=fopen(str_file_path_c2h, "r"))==NULL){
		printf("file open error!\n\r");
		return -1;
	}
	temp=0;
	while(!feof(fp_c2h)){
		fseek(fp_c2h,temp,SEEK_SET);
		fgets(str,strlen("<?cgi ")+1,fp_c2h);
		if(!strcmp(str,"<?cgi ")){
			begin_temp=temp;
			temp+=strlen("<?cgi ");
			i_c2h=-1;
			do{
				i_c2h+=1;
				fseek(fp_c2h,temp+i_c2h,SEEK_SET);
				fgets(&str[i_c2h],strlen("?>")+1,fp_c2h);
			}while(strcmp(&str[i_c2h],"?>"));
			str[i_c2h-1]=0;
			end_temp=temp+i_c2h+1;
		//	printf("*****************************\n\r");

			if(!strcmp(str, "fun_file_content")){
				get_file_path(str_file_path_log, tmp_log);
				fp=fopen(str_file_path_log, "r");
				if(fp==NULL){
					printf("Log.c main, file open error: 1\r\n");
					exit(1);
				}
				line_cnt=file_line_cnt(fp);	
				i=page_size*page_index;
				if(i<=line_cnt){
					pos=file_line2pos(fp, i);
				}else{
					printf("Log.c main, file pos beyond :1 \r\n");	
					exit(1);
				}
				fseek(fp, pos, SEEK_SET);
				i=0;
				printf("<div class=class_row><div class=class_col1>%d</div><div class=class_col2>", page_index*page_size+i+1);
				while(!feof(fp)){
					if(fgets(str, STR_LEN, fp)==NULL){
						break;
					}
					printf(str);
					if(strstr(str, "\n")){
						i++;
						if(i>=page_size){
							break;
						}
						printf("</div></div><div class=class_row><div class=class_col1>%d</div><div class=class_col2>", page_index*page_size+i+1);
					}
				}
				printf("</div></div>");
				fclose(fp);
			}else if(!strcmp(str, "page_index")){
				printf("%d", page_index);
			}else if(!strcmp(str, "page_cnt")){
				printf("%d", line_cnt/page_size + (line_cnt%page_size==0?0:1));
			}

		//	printf(str);
		//	printf("****************************\n\r");	
			temp=end_temp+1;
		}else{
			//putc(arg_name[0]);
			str[1]=0;
			printf(str);
		}
		temp++;
	}
	fclose(fp_c2h); 

}



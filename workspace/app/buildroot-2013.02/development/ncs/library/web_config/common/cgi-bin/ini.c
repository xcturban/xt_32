#include <stdio.h>
#include <string.h>
#include "ini.h"

#define STR_LEN 255

STRUCT_INI *ini_new(const char *str_ini){
	STRUCT_INI *ret=(STRUCT_INI *)malloc(sizeof(STRUCT_INI));
	if(ret==NULL){
		printf("malloc error!\r\n");
		exit(1);
	}
	ret->fp=fopen(str_ini, "a+b");
	ret->fname=str_ini;
	return ret;
}

int ini_release(STRUCT_INI *fp_ini){
	fclose(fp_ini->fp);
	sync(fp_ini->fp);
	fp_ini->fname=NULL;
	free(fp_ini);
	return 0;
}

long ini_get_sec_pos(FILE *fp_ini, char *sec){
	char str_tmp[STR_LEN];
	int idx;
	int str_len;
	long tmp;
	fseek(fp_ini, 0, SEEK_SET);
	while(!feof(fp_ini)){
		fgets(str_tmp, STR_LEN, fp_ini);
		idx=strlen(str_tmp);
		str_len=idx-1;/* if there s '\n\r\0'  */
		if((str_tmp[0]=='[')&&(str_tmp[str_len-1]==']')){
			str_tmp[str_len-1]=0;
			if(strcmp(&str_tmp[1], sec)==0){
				tmp=(long)(ftell(fp_ini));	
				fseek(fp_ini, 0, SEEK_SET);
				return tmp;
			}	
		}
	}
	fseek(fp_ini, 0, SEEK_SET);
	return -1;
}

long ini_get_sec_end_pos(FILE *fp_ini, long start_pos){
	char str_tmp[STR_LEN];
	int idx;
	int str_len;
	fseek(fp_ini, start_pos, SEEK_SET);
	while(!feof(fp_ini)){
		fgets(str_tmp, STR_LEN, fp_ini);
		idx=strlen(str_tmp);
		str_len=idx-1;/* if there s '\n\r\0'  */
		if((str_tmp[0]=='[')&&(str_tmp[str_len-1]==']')){
			return (long)(ftell(fp_ini)-idx);
		}
	}
	return (long)ftell(fp_ini);
}


long ini_get_key_pos(FILE *fp_ini, long start_pos, long end_pos, char *key){
	char str_tmp[STR_LEN];
	int idx;
	int str_len;
	long tmp;
	str_len=strlen(key);
	fseek(fp_ini, start_pos, SEEK_SET);
	while(!feof(fp_ini)){
		fgets(str_tmp, STR_LEN, fp_ini);
		idx=strlen(str_tmp);
		if(str_tmp[str_len]=='='){
			str_tmp[str_len]=0;
			if(strcmp(str_tmp, key)==0){
				tmp=(long)(ftell(fp_ini)-idx+str_len+1);
				fseek(fp_ini, 0, SEEK_SET);
				return tmp;
			}	
		}
		if(ftell(fp_ini)>=end_pos){
			break;
		}
	}
	fseek(fp_ini, 0, SEEK_SET);
	return -1;
}

int ini_file1_to_file2(FILE *fp_file2, FILE *fp_file1, long start_pos, long end_pos){
	char str_tmp[STR_LEN];
	int idx;
	fseek(fp_file1, start_pos, SEEK_SET);
	while(fgets(str_tmp, STR_LEN, fp_file1)!=NULL){
		if(end_pos!=NULL){
			idx=end_pos-ftell(fp_file1);
			if(idx<=0){
				str_tmp[strlen(str_tmp)+idx]=0;
				fputs(str_tmp, fp_file2);
				break;
			}
		}
		fputs(str_tmp, fp_file2);
	}
	return 0;
}

long ini_new_sec(STRUCT_INI *fp_ini, char *sec){
	char str_tmp[STR_LEN];
	int idx;
	int str_len;

	if(ini_get_sec_pos(fp_ini->fp, sec)>=0){
		return 0;
	}
	fclose(fp_ini->fp);
	fp_ini->fp=fopen(fp_ini->fname, "ab");
	if(fp_ini->fp==NULL){
		printf("ini.c: ini_new_sec,file open error\r\n");
		exit(1);
	}
	fseek(fp_ini->fp, 0, SEEK_END);
	fputc('[',fp_ini->fp);
	fputs(sec, fp_ini->fp);
	fputc(']',fp_ini->fp);
	fputs("\n\n",fp_ini->fp);
	fclose(fp_ini->fp);
	fp_ini->fp=fopen(fp_ini->fname, "rb");
	return 1;
}

long ini_new_key(STRUCT_INI *fp_ini, long start_pos, long end_pos, char *key){
	char str_tmp[STR_LEN];
	int idx;
	int str_len;
	FILE *fp_tmp=tmpfile();

	if(ini_get_key_pos(fp_ini->fp, start_pos, end_pos, key)>=0){
		return 0;
	}
	fseek(fp_ini->fp, start_pos, SEEK_SET);
	while(!feof(fp_ini->fp)){
		fgets(str_tmp, STR_LEN, fp_ini->fp);
		idx=strlen(str_tmp);
		if(idx<=1){
			break;
		}
	}
	start_pos=ftell(fp_ini->fp);
	ini_file1_to_file2(fp_tmp, fp_ini->fp, 0, start_pos-1);
	fputs(key, fp_tmp);
	fputs("=\n\n", fp_tmp);
	ini_file1_to_file2(fp_tmp, fp_ini->fp, start_pos, NULL);

	fclose(fp_ini->fp);
	fp_ini->fp=fopen(fp_ini->fname, "wb");
	if(fp_ini->fp==NULL){
		printf("ini.c: ini_net_key,file open error\r\n");
		exit(1);
	}
	ini_file1_to_file2(fp_ini->fp, fp_tmp, 0, NULL);
	fclose(fp_ini->fp);
	fp_ini->fp=fopen(fp_ini->fname, "rb");
	return 1;
}



char *ini_put_value_string(STRUCT_INI *fp_ini, char *sec, char *key, char *value){
	long pos_sec;
	long pos_sec_end;
	long pos_key;
	char str_tmp[STR_LEN];
	FILE *fp_tmp=tmpfile();

	if(!fp_tmp){
		printf("tmpfile error!\n\r");
		return NULL;
	}
	pos_sec=ini_get_sec_pos(fp_ini->fp, sec);
	if(pos_sec<0){
		ini_new_sec(fp_ini, sec);
		pos_sec=ini_get_sec_pos(fp_ini->fp, sec);
	}
	pos_sec_end=ini_get_sec_end_pos(fp_ini->fp, pos_sec);
	pos_key=ini_get_key_pos(fp_ini->fp, pos_sec, pos_sec_end, key);
	if(pos_key<0){
		ini_new_key(fp_ini, pos_sec, pos_sec_end, key);
		pos_key=ini_get_key_pos(fp_ini->fp, pos_sec, pos_sec_end, key);
	}
	if(pos_key<0){
		return -1;
	}

	ini_file1_to_file2(fp_tmp, fp_ini->fp, 0, pos_key);
	fputs(value, fp_tmp);
	fputs("\n", fp_tmp);
	ini_file1_to_file2(fp_tmp, fp_ini->fp, ftell(fp_ini->fp), NULL);
	fclose(fp_ini->fp);
	fp_ini->fp=fopen(fp_ini->fname, "wb");
	if(fp_ini->fp==NULL){
		printf("fp_ini->fp open error! \r\n");
		exit(1);
	}
	ini_file1_to_file2(fp_ini->fp, fp_tmp, 0, NULL);
	fclose(fp_ini->fp);
	fp_ini->fp=fopen(fp_ini->fname, "rb");	
	return value;
}

char *ini_get_value_string(STRUCT_INI *fp_ini, char *value, char *sec, char *key){
	long pos_sec;
	long pos_sec_end;
	long pos_key;
	char str_tmp[STR_LEN];
	
	pos_sec=ini_get_sec_pos(fp_ini->fp, sec);
	pos_sec_end=ini_get_sec_end_pos(fp_ini->fp, pos_sec);
	pos_key=ini_get_key_pos(fp_ini->fp, pos_sec, pos_sec_end, key);
	fseek(fp_ini->fp, pos_key, SEEK_SET);
	fgets(value, STR_LEN, fp_ini->fp);
	value[strlen(value)-1]=0;
	return value;
}

/*
char *ini_get_value_string(FILE *fp_ini, char *value, char *sec, char *key){
	char str_tmp[STR_LEN];
	while(!feof(fp_ini)){
		fgets(str_tmp, STR_LEN, fp_ini);
		
	}
}
*/
/*
void main(void){
	long start_pos;
	long end_pos;
	char value_str[50];
	STRUCT_INI *fp=ini_new("test.ini");
	ini_new_sec(fp, "ppppp");
//	start_pos=ini_get_sec_pos(fp->fp, "sec");
//	end_pos=ini_get_sec_end_pos(fp->fp, "sec");
//	printf("  kkkkkkkkkkk start:%ld \r\n",start_pos);
//	printf("  kkkkkkkkkkk end  :%ld \r\n",end_pos);
//	ini_new_key(fp, start_pos, end_pos, "wahaha" );
//	ini_put_value_string(fp, "sec2", "key2", "ggggggggggg");
	ini_get_value_string(fp, value_str, "sec", "key");
	printf("the value_str:%s \r\n", value_str);
	ini_release(fp);
}
*/
/*
	printf("             sssss: %ld \n\r", ini_get_sec_pos(fp, "good"));
	printf("ssssssssssssssssssssssssssssssssok \n\r");

*/
/*
	printf("            ssss:%ld \n\r", ini_get_key_pos(fp, 0, 100, "ip"));
	printf("ssssssssssssssssssssssssssssss\n\r");

*/
/*
	FILE *fp1=ini_new("test1.ini");
	ini_file1_to_file2(fp, fp1, 2, 5);
	ini_release(fp1);

*/
/*
	char str[50];
	ini_put_value_string(fp, "sec", "key", "123456789");
	ini_get_value_string(fp, str, "sec", "key" );
	printf("aaaa : -%s-  \r\n",str);
	ini_release(fp);


}
*/

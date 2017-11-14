#include <stdio.h>
#include <string.h>
#include "xml.h"

FILE *g_fp=NULL;

FILE *xml_new(char *xml_file_name){
	FILE *fp;
	fp=fopen(xml_file_name,"rb");
	return fp;
}

int xml_release(FILE *fp){
	fclose(fp);
	return 0;
}

char *xml_get_value_string(char *value, char *argv, FILE *fp){
	int temp;
	int temp_begin;
	char arg_name[50];
	char arg_name_neg[50]; 
	char temp_name[50];
	int i;
	int j;
	char *arg_value=value;


	if(strlen(argv)>45){
		printf("xml.c: xml_get_value_string,the argv is too long :error!\n\r");
		printf(": %s \r\n",argv);
		return -1; 
	}	
	arg_name[0]='<';
	arg_name_neg[0]='<';
	arg_name_neg[1]='/';
	for(i=1;i<strlen(argv)+1;i++){
		arg_name[i]=argv[i-1];
		arg_name_neg[i+1]=argv[i-1];
	}
	arg_name[i]='>';
	arg_name_neg[i+1]='>';
	arg_name[i+1]=0;
	arg_name_neg[i+2]=0;
	if(fp==NULL){
		printf("xml.c: xml_get_value_string,xml_file open error!\n\r");
		return NULL;
	}
	temp=0;
	while(!feof(fp)){
		fseek(fp,temp,SEEK_SET);
		fgets(temp_name,strlen(arg_name)+1,fp);
		if(!strcmp(temp_name,arg_name)){
			temp+=strlen(arg_name);
			j=0;
			do{
				fseek(fp,temp,SEEK_SET);
				fgets(&arg_value[j],strlen(arg_name_neg)+1,fp);
				j++;
				temp++;
				if(j==48){
					break;
				}
			}while(strcmp(&arg_value[j-1],arg_name_neg)&&(!feof(fp)));
			if(j<48){
				arg_value[j-1]=0;
				return value; /* find it so return means successful... */
			}else{
				arg_value[0]=0;
				return -1; /* you have two choice, 1 for pass, the other tell error. */
			}
		}
		temp++;	
	}
	arg_value[0]=0;
	return NULL; /* find none of yous */
}

/*
void main(void){
	char str[100];
	FILE *fp=xml_new("test.xml");
	xml_get_value_string(str, "sec_key", fp);
	printf("value: %s \r\n",str);
	xml_release(fp);
}


*/

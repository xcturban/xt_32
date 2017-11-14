#include "language.h"
#include "system_info.h"
#include "term.h"

#include <common_lib/common_lib.h>

/**
 * get_str_from_id - ��ȡ�ַ���
 * @id: �ַ���id��
 * @str: ������ȡ�����ַ���
 */
char *get_str_from_id(str_id_t id, char *str)
{
	char buf[256];
	FILE *fp;
	int __id;
	int i;
	int len;
	char *p_begin;
	char *p_end;
	char space='	';

	if (term->language == ENGLISH)
	 	sprintf(buf, "%s/language_en.txt", SYSTEM_LANGUAGE_PATH);
	else if (term->language == CHINESE)
		sprintf(buf, "%s/language_cn.txt", SYSTEM_LANGUAGE_PATH);
	else
		sprintf(buf, "%s/language_cn.txt", SYSTEM_LANGUAGE_PATH);

	fp = fopen(buf, "r");
	if (!fp) {
		SPON_LOG_ERR("open file %s failed\n", buf);
		return NULL;
	}

	/*
	get space value from line 2 of the file 
	if the space is tab key as you know 	
	*/
	/*
	i=0;
	while (!feof(fp)) {
		fgets(buf, sizeof(buf), fp);
		space=buf[0];
		if(++i<2){
			break;
		}
	}
	*/

	while (!feof(fp)) {
		/* ÿ�δ��ļ��ж�һ�� */
		fgets(buf, sizeof(buf), fp);
		len=strlen(buf);
		/* ȥ�����з� */
		buf[len - 2] = 0;

		/* get id */
		sscanf(buf, "%d", &__id);
		if (__id == id) {
			fclose(fp);
			
			/* find the first space */
			for(i=0; i<len; i++){
				if(buf[i]==space){
					break;
				}
			}
			/* find the second str */
			for(; i<len; i++){
				if(buf[i]!=space){
					break;
				}
			}
			//SPON_LOG_ERR("string**** buf:%s i:%d len:%d\n\r", buf, i, strlen(buf));
			strcpy(str, &buf[i]);
			return str;
		}			
	}

	SPON_LOG_ERR("string not found %d\n", id);
	fclose(fp);
	return NULL;
}

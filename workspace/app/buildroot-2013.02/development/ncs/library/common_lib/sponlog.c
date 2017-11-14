#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include "sponlog.h"

#define MAX_LOGINFO_NUM     256
#define MAX_LOGINFO_LEN     256

/*
#ifdef CFG_IN_NAND1_1
#define LOGINFO_PATH        "/mnt/nand1-1/loginfo"
#else
#define LOGINFO_PATH        "/var/www/ini/loginfo"
#endif
*/
#define LOGINFO_PATH        "/mnt/nand1-2/loginfo"

#define BITS_PER_ULONG 		32
#define DBG_VERSION_ARRAY_SIZE	(MAX_DBG_VERSION / BITS_PER_ULONG)

static char *usr_loginfo[MAX_LOGINFO_NUM];
static unsigned int sys_logout_level = 0;
static unsigned int sys_logout_type = 0;
static unsigned long sysdbg_logout_bitmap[DBG_VERSION_ARRAY_SIZE] = {0};
const char* sys_log_module = NULL;

unsigned int sys_language = 0;		//ϵͳ���Ա�ʶ�� 0-Ӣ�� 1-����

/*
    web��ϵͳ��־
*/
void user_log(unsigned int LOG_IDX, ...)
{
	return;
    if(LOG_IDX >= MAX_LOGINFO_NUM)
        return;

    va_list ap;
    va_start(ap, LOG_IDX);
	if(sys_language == 1)
	    vsyslog(LOG_NOTICE, usr_loginfo[LOG_IDX], ap);
	else
		vsyslog(LOG_NOTICE, usr_loginfo[LOG_IDX+3], ap);
}


void web_user_log(unsigned int LOG_IDX, ...)
{
    if(LOG_IDX >= MAX_LOGINFO_NUM)
        return;

    va_list ap;
    va_start(ap, LOG_IDX);
	vsyslog(LOG_NOTICE, sys_log_info_en[LOG_IDX], ap);
}


#if 0
static void set_bit(unsigned int offset, unsigned long *addr)
{
	unsigned long mask = 1UL << (offset & (BITS_PER_ULONG - 1));
	unsigned long *p = addr + (offset >> (sizeof(unsigned long) + 1));
	unsigned long old = *p;
	*p = old | mask;
}
#endif


static int test_bit(unsigned int offset, unsigned long *addr)
{
	unsigned long mask = 1UL << (offset & (BITS_PER_ULONG - 1));
	unsigned long *p = addr + (offset >> (sizeof(unsigned long) + 1));
	unsigned long old = *p;
	return old & mask;
}

#if 0

static void clear_bit(unsigned int offset, unsigned long *addr)
{
	unsigned long mask = 1UL << (offset & (BITS_PER_ULONG - 1));
	unsigned long *p = addr + (offset >> (sizeof(unsigned long) + 1));
	unsigned long old = *p;
	*p = old & ~mask;
}


/* �ر�ָ���汾�ĵ�����Ϣ */
static void disable_dbg_version(unsigned int dbg_version)
{
	if(dbg_version >= MAX_DBG_VERSION)
		return;

	clear_bit(dbg_version, sysdbg_logout_bitmap);
}



/* ����ָ���汾�ĵ�����Ϣ */
static void enable_dbg_version(unsigned int dbg_version)
{
	if(dbg_version >= MAX_DBG_VERSION)
		return;

	set_bit(dbg_version, sysdbg_logout_bitmap);
}
#endif

int is_dbg_version_enable(unsigned int dbg_version)
{
	if(dbg_version >= MAX_DBG_VERSION)
		return 0;

	return test_bit(dbg_version, sysdbg_logout_bitmap);
}


/*******************************************************************************
* ��������: SPON_LOG_PRINT      // �������ơ�
* ˵��:          // �������ܡ����ܵȵ�������
* �������:      // �������˵��������ÿ������������
*                // �á�ȡֵ˵�����������ϵ��
* �������:      // �����������˵����
* ����ֵ:        // ��������ֵ��˵����
* ����:          // ����˵����ѡ���
*******************************************************************************/
void  SPON_LOG_PRINT(int logout_level,int dbg_version, const char *fmt, ...)
{
	if(logout_level == SPON_LOGLVL_DBG && !is_dbg_version_enable(dbg_version))
		return;

	if(logout_level <= sys_logout_level)
	{
		
		va_list ap;
		va_start(ap, fmt);
		if(sys_logout_type & SPON_LOGTYPE_PRINTF)
		{
            vprintf(fmt, ap);
		}
		if(sys_logout_type & SPON_LOGTYPE_SYSLOG)
		{
			vsyslog(LOG_INFO, fmt, ap);
		}
  		va_end(ap);
	}

	return;
}

/******************************************************************************
 �������� : SPON_LOG_INIT
 �������� : ��ʼ����־���
 ������� : log_type		��־�������: ���� ����
            logout_level	��־�������
 ������� : ��
 �� �� ֵ : ��
******************************************************************************/
void SPON_LOG_INIT(const char *module_name, unsigned int log_type, unsigned int logout_level)
{
    if(!module_name)
        sys_log_module = "";
    else
        sys_log_module = module_name;

	sys_logout_type = log_type;
	sys_logout_level = logout_level;
}


void user_log_init(unsigned int language)
{
	sys_language = language;

    int i = 0;
    FILE *fd_loginfo;

    for(i = 0; i < MAX_LOGINFO_LEN; i++)
    {
        usr_loginfo[i] = (char *)malloc(MAX_LOGINFO_LEN);
        memset(usr_loginfo[i], 0, MAX_LOGINFO_LEN);
    }

    fd_loginfo = fopen(LOGINFO_PATH, "r");
    if(!fd_loginfo)
    {
        SPON_LOG_ERR("Read: Open %s fail\n", LOGINFO_PATH);
        return;
    }

    i = 0;
    while (!feof(fd_loginfo) && i < MAX_LOGINFO_NUM)
    {
        fgets(usr_loginfo[i], MAX_LOGINFO_LEN, fd_loginfo);
        i++;
    }

    fclose(fd_loginfo);
}



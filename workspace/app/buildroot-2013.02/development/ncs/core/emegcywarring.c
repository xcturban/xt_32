
#include "emegcywarring.h"
#include "lpconfig.h"
#include "NAS8532.h"
#include "system_info.h"
#include "ortp/ortp.h"
#include "term.h"
#include "utils.h"


TYPE_EMEGCY_WARRING_INFO *g_pEmegcyWarring;

TYPE_EMEGCY_WARRING_INFO* EmegcyWarringMalloc(void)
{
	TYPE_EMEGCY_WARRING_INFO *pInfo = (TYPE_EMEGCY_WARRING_INFO*)malloc(sizeof(TYPE_EMEGCY_WARRING_INFO)); 

	pInfo->Init = Init;
	pInfo->Init(pInfo);
	return pInfo;
}


void SendEmegcyWarringToServer(TYPE_EMEGCY_WARRING_INFO *pInfo, int Index, ENUM_EMEGCY_WARRING_STATUE statue)
{
	ncs_event_t *evt;
	char Buf[] = { 0x36, 0x01, 0x01, 0, 0, 0, 0xff};
	//当前状态为停止状态，则可以发送开始消息
	if(statue == ENUM_EMEGCY_WARRING_STATUE_PLAY)
	{
		if(pInfo->statue == ENUM_EMEGCY_WARRING_STATUE_STOP)
		{
			Buf[1] = 0x01;
			Buf[2] = Index;
			evt = PackProcWithBuf(Buf);
			if (evt)
			{
				ncs_event_send(evt);
			}
		}
	}
	else if(statue == ENUM_EMEGCY_WARRING_STATUE_STOP)
	{
		if(pInfo->statue == ENUM_EMEGCY_WARRING_STATUE_PLAY)
		{
			Buf[1] = 0x00;
			Buf[2] = Index;
			evt = PackProcWithBuf(Buf);
			if (evt)
			{
				ncs_event_send(evt);
			}
		}
	}
}

void RevEmegcyWarringFromServer(TYPE_EMEGCY_WARRING_INFO *pInfo, int Index, ENUM_EMEGCY_WARRING_STATUE statue)
{
	if(statue == ENUM_EMEGCY_WARRING_STATUE_PLAY)
	{
		pInfo->CurWarringIndex = Index;
	}
	else if(statue == ENUM_EMEGCY_WARRING_STATUE_STOP)
	{
		pInfo->CurWarringIndex = -1;
	}
	pInfo->statue = statue;
}

void PluginFlushGui(TYPE_EMEGCY_WARRING_INFO *pInfo, int Index, ENUM_EMEGCY_WARRING_STATUE statue)
{
}

void ParseWebConfig(TYPE_EMEGCY_WARRING_INFO *pInfo)
{
    LpConfig *lpconfig;
	char *pTemp;
	int i = 0;
	char Buf[10];
	char TmpBuf[100] = {0};

    lpconfig = lp_config_new(INI_WARRINGCFG_FILE);
    if (lpconfig == NULL)
    {
        SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
        return -1;
    }
	for(i = 0; i < SIZE_EMEGCY_WARRING_NUMBER; i++)
	{
		sprintf(Buf, "key_%d", i);
		pTemp = lp_config_get_string(lpconfig, "alone_1", Buf, "undefined");
		utf8_to_gb2312(pTemp, TmpBuf, strlen(pTemp)+1);
		if(strlen(TmpBuf) < 29)
		{
			strcpy(pInfo->Name[i], TmpBuf);
		}
		else
		{
			strncpy(pInfo->Name[i], TmpBuf, 29);
		}
	}
	lp_config_destroy(lpconfig);
}

static void Init(TYPE_EMEGCY_WARRING_INFO* pInfo)
{
	pInfo->SendEmegcyWarringToServer  = SendEmegcyWarringToServer;
	pInfo->RevEmegcyWarringFromServer = RevEmegcyWarringFromServer;
	pInfo->PluginFlushGui             = PluginFlushGui;
	pInfo->ParseWebConfig             = ParseWebConfig;
	pInfo->CurWarringIndex            = -1;
	pInfo->statue                     = ENUM_EMEGCY_WARRING_STATUE_STOP;
	pInfo->ParseWebConfig(pInfo);
}


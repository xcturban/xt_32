/*
***********************************************************************
* Name		: plugins.h 插件函数库
* Author	: Ryan  胡松平
* Reversion	: 1.0
* Data  	: 2013/9/12
* Mail  	: yishuiyuntian@gmail.com
* Company	: Changsha SPON Ltd.Corp
***********************************************************************
*/

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#include "Types.h"


//**********************************************************************************
// 									   结构定义
//**********************************************************************************
typedef struct _ADPCM_STRUCT
{
	long	valprev;	
	long	index;		
} ADPCM_STRUCT;



//**********************************************************************************
// 									  外部函数声明
//**********************************************************************************


extern void adpcm_encode(short indata[], char outdata[], int len);						//len为indata的short大小
extern void adpcm_decode(char indata[], short outdata[], int len);						//len为outdata的short大小
extern INT FADPCM_Encode(PSHORT pData, PBYTE pOutBuffer, INT iLength);
extern INT FADPCM_Decode(PBYTE pInBuffer, PSHORT pData, INT iLength);

extern VOID SteroToMono(PSHORT inDat, PSHORT outDat, DWORD dwSize, BYTE ucChanel);		//dwSize为outDat 的short大小
extern VOID MonoToStero(PSHORT inDat, PSHORT outDat, DWORD dwSize, BYTE ucChannel);		//dwSize为inDat 的short大小

extern VOID PCM_32KTo8K(PSHORT dst,  const SHORT *src, DWORD dwSize); 					//dwSize为Src 的SHORT大小
extern VOID PCM_8KTo32K(PSHORT dst,  const SHORT *src, DWORD dwSize);					//dwSize为Src 的SHORT大小
extern VOID PCM_16KTo8K(PSHORT dst,  const SHORT *src, DWORD dwSize);					//dwSize为Src 的SHORT大小
extern VOID PCM_8KTo16K(PSHORT dst,  const SHORT *src, DWORD dwSize);					//dwSize为Src 的SHORT大小


#endif





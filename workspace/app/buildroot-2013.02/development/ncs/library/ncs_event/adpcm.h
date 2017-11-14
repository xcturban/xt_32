/*
***********************************************************************
* Name		: plugins.h ���������
* Author	: Ryan  ����ƽ
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
// 									   �ṹ����
//**********************************************************************************
typedef struct _ADPCM_STRUCT
{
	long	valprev;	
	long	index;		
} ADPCM_STRUCT;



//**********************************************************************************
// 									  �ⲿ��������
//**********************************************************************************


extern void adpcm_encode(short indata[], char outdata[], int len);						//lenΪindata��short��С
extern void adpcm_decode(char indata[], short outdata[], int len);						//lenΪoutdata��short��С
extern INT FADPCM_Encode(PSHORT pData, PBYTE pOutBuffer, INT iLength);
extern INT FADPCM_Decode(PBYTE pInBuffer, PSHORT pData, INT iLength);

extern VOID SteroToMono(PSHORT inDat, PSHORT outDat, DWORD dwSize, BYTE ucChanel);		//dwSizeΪoutDat ��short��С
extern VOID MonoToStero(PSHORT inDat, PSHORT outDat, DWORD dwSize, BYTE ucChannel);		//dwSizeΪinDat ��short��С

extern VOID PCM_32KTo8K(PSHORT dst,  const SHORT *src, DWORD dwSize); 					//dwSizeΪSrc ��SHORT��С
extern VOID PCM_8KTo32K(PSHORT dst,  const SHORT *src, DWORD dwSize);					//dwSizeΪSrc ��SHORT��С
extern VOID PCM_16KTo8K(PSHORT dst,  const SHORT *src, DWORD dwSize);					//dwSizeΪSrc ��SHORT��С
extern VOID PCM_8KTo16K(PSHORT dst,  const SHORT *src, DWORD dwSize);					//dwSizeΪSrc ��SHORT��С


#endif





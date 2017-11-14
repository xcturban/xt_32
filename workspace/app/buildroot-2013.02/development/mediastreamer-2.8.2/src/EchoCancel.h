/********************************************************************
	Copyright (C) 2011
	created:	2011/11/12
	filename: 	EchoCancel.h
	author:		PanLinFeng
	
	purpose:	回声抵消声明头文件
*********************************************************************/

//----------------- 1. 宏及结构本声明 -----------------

//----------------- 2. 变量声明 -----------------------

//----------------- 3. 函数声明 -----------------------
void		EchoInit(int nSamplesPerBlock, int nSamplesRate);		// 初始化,nSamplesPerBlock为回声处理时的采样个数,不能大于256,建议在128~256
void		EchoCancel(short *psOut, const short *psMic, const short *psSpeaker);


/********************************************************************
	Copyright (C) 2011
	created:	2011/11/12
	filename: 	EchoCancel.h
	author:		PanLinFeng
	
	purpose:	������������ͷ�ļ�
*********************************************************************/

//----------------- 1. �꼰�ṹ������ -----------------

//----------------- 2. �������� -----------------------

//----------------- 3. �������� -----------------------
void		EchoInit(int nSamplesPerBlock, int nSamplesRate);		// ��ʼ��,nSamplesPerBlockΪ��������ʱ�Ĳ�������,���ܴ���256,������128~256
void		EchoCancel(short *psOut, const short *psMic, const short *psSpeaker);


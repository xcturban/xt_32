/*
***********************************************************************
* Name		: plugins.c 插件函数库
* Author	: Ryan  胡松平
* Reversion	: 1.0
* Data  	: 2013/9/12
* Mail  	: yishuiyuntian@gmail.com
* Company	: Changsha SPON Ltd.Corp
***********************************************************************
*/
#include "adpcm.h"


#ifndef __STDC__
#define signed
#endif


//**********************************************************************************
// 									 引用声明
//**********************************************************************************

//**********************************************************************************
// 								   内部常量定义
//**********************************************************************************
// Intel ADPCM step variation table
//static long indexTable[16] = {  //占用内存空间
const long indexTable[16] = { //占用Flash空间
    -1,	-1,	-1,	-1,	2,	4,	6,	8,
	-1,	-1,	-1,	-1,	2,	4,	6,	8,
};

//static long stepsizeTable[89] = { //占用内存空间
const long stepsizeTable[89] = {//占用Flash空间
	7,		8,		9,		10,		11,		12,		13,		14,		16,		17,
	19,		21,		23,		25,		28,		31,		34,		37,		41,		45,
	50,		55,		60,		66,		73,		80,		88,		97,		107,	118,
	130,	143,	157,	173,	190,	209,	230,	253,	279,	307,
	337,	371,	408,	449,	494,	544,	598,	658,	724,	796,
	876,	963,	1060,	1166,	1282,	1411,	1552,	1707,	1878,	2066,
	2272,	2499,	2749,	3024,	3327,	3660,	4026,	4428,	4871,	5358,
	5894,	6484,	7132,	7845,	8630,	9493,	10442,	11487,	12635,	13899,
	15289,	16818,	18500,	20350,	22385,	24623,	27086,	29794,	32767
};

//**********************************************************************************
// 								   内部变量定义
//**********************************************************************************

//**********************************************************************************
// 								   内部函数声明
//**********************************************************************************

//**********************************************************************************
// 									 函数定义
//**********************************************************************************
static ADPCM_STRUCT encode_status;
static ADPCM_STRUCT decode_status;
void adpcm_encode(short indata[], char outdata[], int len)
{
	ADPCM_STRUCT *state = &encode_status;
    short		*inp;				/* Input buffer pointer */
    signed char *outp;				/* output buffer pointer */
    long		val;				/* Current input sample value */
    long		sign;				/* Current adpcm sign bit */
    long		delta;				/* Current adpcm output value */
    long		diff;				/* Difference between val and valprev */
    long		step;				/* Stepsize */
    long		valpred;			/* Predicted output value */
    long		vpdiff;				/* Current change to valpred */
    long		index;				/* Current step change index */
    long		outputbuffer = 0;	/* place to keep previous 4-bit value */
    long		bufferstep;			/* toggle between outputbuffer/output */

    outp		=	(signed char *)outdata;
    inp			=	indata;

    valpred		=	state->valprev;
    index		=	state->index;
    step		=	stepsizeTable[index];
    
    bufferstep	=	1;

    for (; len > 0; --len) {
		val		=	*inp++;
	
		/* Step 1 - compute difference with previous value */
		diff	=	val - valpred;
		sign	=	(diff < 0) ? 8 : 0;
		if ( sign ) {
			diff	=	(-diff);
		}
	
		/* Step 2 - Divide and clamp */
		/* Note:
		** This code *approximately* computes:
		**    delta = diff*4/step;
		**    vpdiff = (delta+0.5)*step/4;
		** but in shift step bits are dropped. The net result of this is
		** that even if you have fast mul/div hardware you cannot put it to
		** good use since the fixup would be too expensive.
		*/
		delta	=	0;
		vpdiff	=	(step >> 3);
		
		if (diff >= step) {
			delta	=	4;
			diff	-=	step;
			vpdiff	+=	step;
		}
		step	>>=	1;
		if (diff >= step) {
			delta	|=	2;
			diff	-=	step;
			vpdiff	+=	step;
		}
		step	>>=	1;
		if (diff >= step) {
			delta	|=	1;
			vpdiff	+=	step;
		}
	
		/* Step 3 - Update previous value */
		if (sign) {
			valpred	-=	vpdiff;
		} else {
			valpred	+=	vpdiff;
		}
	
		/* Step 4 - Clamp previous value to 16 bits */
		if (valpred > 32767) {
			valpred	=	32767;
		} else if ( valpred < -32768 ) {
			valpred	=	-32768;
		}
	
		/* Step 5 - Assemble value, update index and step values */
		delta	|=	sign;
		
		index	+=	indexTable[delta];
		if (index < 0) {
			index = 0;
		}
		if (index > 88) {
			index = 88;
		}
		step	=	stepsizeTable[index];
	
		/* Step 6 - Output value */
		if (bufferstep) {
			outputbuffer	=	(delta << 4) & 0xf0;
		} else {
			*outp++	=	(delta & 0x0f) | outputbuffer;
		}
		bufferstep	=	!bufferstep;
    }

    /* Output last step, if needed */
    if (!bufferstep)
      *outp++	=	outputbuffer;
    
    state->valprev	=	valpred;
    state->index	=	index;
}

void adpcm_decode(char indata[], short outdata[], int len)
{
	ADPCM_STRUCT *state = &decode_status;
    signed char		*inp;				/* Input buffer pointer */
    short			*outp;				/* output buffer pointer */
    long			sign;				/* Current adpcm sign bit */
    long			delta;				/* Current adpcm output value */
    long			step;				/* Stepsize */
    long			valpred;			/* Predicted value */
    long			vpdiff;				/* Current change to valpred */
    long			index;				/* Current step change index */
    long			inputbuffer = 0;	/* place to keep next 4-bit value */
    long			bufferstep;			/* toggle between inputbuffer/input */

    outp	=	outdata;
	inp		=	(signed char *)indata;

    valpred	=	state->valprev;
    index	=	state->index;
    step	=	stepsizeTable[index];

    bufferstep	=	0;
    
	for (; len > 0; --len) {
		
		/* Step 1 - get the delta value */
		if (bufferstep) {
			delta	=	inputbuffer & 0xf;
		} else {
			inputbuffer	=	*inp++;
			delta	=	(inputbuffer >> 4) & 0xf;
		}
		bufferstep	=	!bufferstep;
	
		/* Step 2 - Find new index value (for later) */
		index	+=	indexTable[delta];
		if (index < 0) {
			index = 0;
		}
		if (index > 88) {
			index = 88;
		}
	
		/* Step 3 - Separate sign and magnitude */
		sign	=	delta & 8;
		delta	=	delta & 7;
	
		/* Step 4 - Compute difference and new predicted value */
		/*
		** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
		** in adpcm_coder.
		*/
		vpdiff	=	step >> 3;
		if (delta & 4) {
			vpdiff	+=	step;
		}
		if (delta & 2) {
			vpdiff	+=	step>>1;
		}
		if (delta & 1) {
			vpdiff	+=	step>>2;
		}
	
		if (sign) {
			valpred	-=	vpdiff;
		} else {
			valpred	+=	vpdiff;
		}
	
		/* Step 5 - clamp output value */
		if (valpred > 32767) {
			valpred = 32767;
		} else if (valpred < -32768) {
			valpred = -32768;
		}
	
		/* Step 6 - Update step value */
		step	=	stepsizeTable[index];
	
		/* Step 7 - Output value */
		*outp++	=	valpred;
    }

    state->valprev	=	valpred;
    state->index	=	index;
}

STATIC CONST SHORT g_pAdptive[16] = {230, 230, 230, 230, 307, 409, 512, 614, 768, 614, 512, 409, 307, 230, 230, 230};

INT FADPCM_Encode(PSHORT pData, PBYTE pOutBuffer, INT iLength)
{
	PBYTE  pOutBase = pOutBuffer;
    PSHORT pDataEnd = &pData[iLength];
	PWORD  pWords = (PWORD)pOutBuffer;
    LONG   lSample;
	CHAR   chHigh;
	CHAR   chLow;
	LONG   lDelta;
	WORD   wChkSum = 0;

	*pOutBuffer++ = 0;
	*pOutBuffer++ = lDelta = 128;
	*pOutBuffer++ = 0;
	lSample = *pData++;
	*pOutBuffer++ = LOBYTE(*pData); 
	*pOutBuffer++ = HIBYTE(*pData);
	*pOutBuffer++ = LOBYTE(lSample); 
	*pOutBuffer++ = HIBYTE(lSample);
	lSample = *pData++;

   	while (pData < pDataEnd) {
		chHigh = (CHAR)((*pData++ - lSample) / lDelta);
		chHigh = min(chHigh, 7);
		chHigh = max(chHigh, -8);

		lSample += lDelta * chHigh;
		lSample = min(lSample, 32767);
		lSample = max(lSample, -32768);

		chHigh += (chHigh < 0) ? 0x10 : 0;
		
		lDelta = (lDelta * g_pAdptive[chHigh]) / 256;
		lDelta = max(lDelta, 16);

		chLow = (CHAR)((*pData++ - lSample) / lDelta);
		chLow = min(chLow, 7);
		chLow = max(chLow, -8);

		lSample += lDelta * chLow;
		lSample = min(lSample, 32767);
		lSample = max(lSample, -32768);

		chLow  += (chLow < 0) ? 0x10 : 0;
		
		lDelta = (lDelta * g_pAdptive[chLow]) / 256;
		lDelta = max(lDelta, 16);
	
		*pOutBuffer++ = ((chHigh << 4) | chLow);
	}

	while ((DWORD)pWords < (DWORD)pOutBuffer)
		wChkSum += *pWords++;

	wChkSum += LOBYTE(*pWords);

	return ((DWORD)pOutBuffer - (DWORD)pOutBase);
} 

INT FADPCM_Decode(PBYTE pInBuffer, PSHORT pData, INT iLength)
{
    PBYTE  pInBufferEnd = (PBYTE)((DWORD)pInBuffer++ + iLength);
	PSHORT   pInBase = pData;
    LONG   lSample;
    LONG   lDelta;
	CHAR   ucCode;
	
	lDelta = (SHORT)MAKEWORD(pInBuffer[0], pInBuffer[1]);
	pInBuffer += sizeof(WORD);
	*pData++ = (SHORT)MAKEWORD(pInBuffer[2], pInBuffer[3]);
	lSample = (SHORT)MAKEWORD(pInBuffer[0], pInBuffer[1]);
	*pData++ = lSample;
	pInBuffer += 2 * sizeof(WORD);

	while (pInBuffer < pInBufferEnd) {
		ucCode = (*pInBuffer >> 4);
		lSample += lDelta * ((ucCode > 7) ? (ucCode - 0x10) : ucCode);
		lSample = min(lSample, 32767);
		lSample = max(lSample, -32768);
		*pData++ = (SHORT)lSample;
		lDelta = (lDelta * g_pAdptive[ucCode]) / 256;
		lDelta = max(lDelta, 16);

		ucCode = (*pInBuffer++ & 0x0F);
		lSample += lDelta * ((ucCode > 7) ? (ucCode - 0x10) : ucCode);
		lSample = min(lSample, 32767);
		lSample = max(lSample, -32768);
		*pData++ = (SHORT)lSample;
		lDelta = (lDelta * g_pAdptive[ucCode]) / 256;
		lDelta = max(lDelta, 16);
	}

	return ((DWORD)pData - (DWORD)pInBase);
}


VOID SteroToMono(PSHORT inDat, PSHORT outDat, DWORD dwSize, BYTE ucChanel)
{
	inDat += ucChanel ? 1 : 0;
	
	while(dwSize--) {
		*outDat++ = (SHORT)(*inDat);
		inDat += 2;
	}	
}

VOID MonoToStero(PSHORT inDat, PSHORT outDat, DWORD dwSize, BYTE ucChannel)
{
	outDat += ucChannel ? 1 : 0;
	
	if (inDat != NULL) {
		while(dwSize--) {
			*outDat = (SHORT)(*inDat++);
			outDat += 2;
		}	
	}
	else {
		while(dwSize--) {
			*outDat = (SHORT)(0);
			outDat += 2;
		}	
	}
}


VOID PCM_32KTo8K(PSHORT dst,  const SHORT *src, DWORD dwSize) { 
   while(dwSize) { 
      *dst++ = *src; 
      src += 4; 
      dwSize -= 4; 
   } 
}

VOID PCM_8KTo32K(PSHORT dst,  const SHORT *src, DWORD dwSize) { 
   SHORT tmp; 
   SHORT tmpSub; 
   STATIC SHORT lastData = 0;

   while(dwSize--) { 
      tmp = *src++; 
      tmpSub = (tmp - lastData) / 4; 
      *dst++ = lastData; 
      *dst++ = lastData + (tmpSub * 1); 
      *dst++ = lastData + (tmpSub * 2); 
      *dst++ = lastData + (tmpSub * 3); 
      lastData = tmp; 
   } 
}

VOID PCM_16KTo8K(PSHORT dst,  const SHORT *src, DWORD dwSize) { 
   while(dwSize) { 
	*dst++ = *src; 
	src += 2; 
	dwSize -= 2;
   } 
}

VOID PCM_8KTo16K(PSHORT dst,  const SHORT *src, DWORD dwSize) { 
   SHORT tmp; 
   SHORT tmpSub; 
   STATIC SHORT lastData = 0;

   while(dwSize--) { 
	tmp = *src++; 
	tmpSub = (tmp - lastData) / 2; 
	*dst++ = lastData; 
	*dst++ = lastData + (tmpSub * 1); 
	lastData = tmp; 
   } 
}














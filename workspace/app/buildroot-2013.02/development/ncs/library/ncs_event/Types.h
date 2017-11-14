/*
 ********************************************************************************************************
 * Copyright (C) 2003-2012, Changsha Spon Electronic Co., Ltd
 ********************************************************************************************************
 * Filename     : Types.h
 * Author       : WangCheng <WangCheng@spon.com.cn>
 * Created      : 2012/08/02
 * Description  : 
 ********************************************************************************************************
 * $Id:$ 
 ********************************************************************************************************
 */
#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

typedef void                    VOID, *PVOID;

typedef signed char             CHAR, *PCHAR;

typedef signed int              INT, *PINT;

typedef signed short            SHORT, *PSHORT;  

typedef signed long             LONG, *PLONG;    

typedef signed long long        LONGLONG, *PLONGLONG;    

typedef unsigned char           UCHAR, *PUCHAR;

typedef unsigned int            UINT, *PUINT;

typedef unsigned short          USHORT, *PUSHORT;

typedef unsigned long           ULONG, *PULONG;

typedef unsigned int            BOOL, *PBOOL;

typedef unsigned char           BYTE, *PBYTE;

typedef unsigned short          WORD, *PWORD;

typedef unsigned short          WCHAR, *PWCHAR;

typedef unsigned long           DWORD, *PDWORD;

typedef unsigned long long      QWORD, *PQWORD;

typedef float                   FLOAT, *PFLOAT;

typedef double                  DOUBLE, *PDOUBLE;

typedef WORD                    COLORREF, *PCOLORREF;

#define STATIC					static
#define VOID					void
#define CONST					const
#define bool BOOL

#ifndef FALSE
#define FALSE                        0
#endif

#ifndef TRUE
#define TRUE                         1
#endif

#ifndef NULL
#define NULL                         ((void *)0)
#endif

#ifndef min
#define min(a,b)                     ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)                     ((a) > (b) ? (a) : (b))
#endif

#define MAKEWORD(a,b)                ((WORD)(((BYTE)( (DWORD)(a)&0xFF )) \
	                                     |((WORD)((BYTE)((DWORD)(b)&0xFF)))<<8))

#define MAKELONG(a,b)                ((DWORD)(((WORD)((DWORD)(a)&0xFFFF)) \
	                                     |((DWORD)((WORD)((DWORD)(b)&0xFFFF)))<<16))

#define LOWORD(l)                    ((WORD)((DWORD)(l)&0xFFFF))
#define HIWORD(l)                    ((WORD)((DWORD)(l)>>16))
#define LOBYTE(w)                    ((BYTE)((DWORD)(w)&0xFF))
#define HIBYTE(w)                    ((BYTE)((DWORD)(w)>>8))

#define CopyMemory        CpyMemory
#define FillMemory        SetMemory

#define SET_BITS(DAT, BITMASK)	  ((DAT) |= (BITMASK))
#define CLR_BITS(DAT, BITMASK)	  ((DAT) &= ~(BITMASK))

typedef struct {
    INT iLeft;
    INT iTop;
    INT iWidth;
    INT iHeight;
} RECT, *PRECT;

typedef struct {
    INT iX;
    INT iY;
} POINT, *PPOINT;

typedef struct {
	INT iWidth;
	INT iHeight;
} SIZE, *PSIZE;

typedef struct {
    WORD wYear; 
    WORD wMonth; 
    WORD wDayOfWeek; 
    WORD wDay; 
    WORD wHour; 
    WORD wMinute; 
    WORD wSecond; 
    WORD wMilliseconds; 
} DATETIME, *PDATETIME;

#endif /* __TYPES_H__ */




#ifndef EMEGCYWARRING_H
#define EMEGCYWARRING_H

#define SIZE_EMEGCY_WARRING_NAME 30
#define SIZE_EMEGCY_WARRING_NUMBER 9

typedef enum
{
	ENUM_EMEGCY_WARRING_STATUE_PLAY,
	ENUM_EMEGCY_WARRING_STATUE_STOP,
}ENUM_EMEGCY_WARRING_STATUE;

typedef struct _emegcy_warring_info
{
	char Name[SIZE_EMEGCY_WARRING_NUMBER][SIZE_EMEGCY_WARRING_NAME];
	ENUM_EMEGCY_WARRING_STATUE statue;
	int CurWarringIndex;
	void (*SendEmegcyWarringToServer)(struct _emegcy_warring_info *, int , ENUM_EMEGCY_WARRING_STATUE);
	void (*RevEmegcyWarringFromServer)(struct _emegcy_warring_info *, int , ENUM_EMEGCY_WARRING_STATUE);
	void (*PluginFlushGui)(struct _emegcy_warring_info *, int, ENUM_EMEGCY_WARRING_STATUE);
	void (*Init)(struct _emegcy_warring_info *);
	void (*ParseWebConfig)(struct _emegcy_warring_info *);
}TYPE_EMEGCY_WARRING_INFO;

extern TYPE_EMEGCY_WARRING_INFO *g_pEmegcyWarring;
static void Init(TYPE_EMEGCY_WARRING_INFO* pInfo);



#endif


#ifndef GUIPUB_H
#define GUIPUB_H

#include <minigui/common.h>
#include "user_msg.h"
#include "system_info.h"

#define WIDGET_IDX(h,l)		((h << 8) | l)
#define WIDGET_LOW(V)		(V & 0xff)
#define WIDGET_HIGH(V)		((V >> 8) & 0xff)

typedef struct
{
	int id;
	RECT rc;
	void *pData0;
	void *pData1;
	void (*pReFlash)(PRECT, void*);
	void (*pHandle)(HWND hWnd, int message, int wParam, int lParam);
}TYPE_GUI_AREA_HANDLE;


typedef struct
{
	void (*PreProcess)(void*);
	TYPE_GUI_AREA_HANDLE *pAreaPage;
}TYPE_GUI_HANDLE;


typedef struct
{
	int enable;
	int id;
	RECT rc;
	void *pData0;
	void *pData1;
	void (*pReFlash)(PRECT, void*, void*, int);
	void (*pHandle)(HWND hWnd, int message, int wParam, int lParam);
}TYPE_GUI_AREA_PTR;

extern const TYPE_GUI_AREA_HANDLE gui_welcome_page_statuebar_item[];
int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
int VolWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static inline void HandleCreate(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd)
{
	int i = 0;

	system_info.hdc = BeginPaint(hWnd);

	//必须刷新状态栏
	for(i = 0; ; i++)
	{
		if(gui_welcome_page_statuebar_item[i].id == -1)
		{
			break;
		}
		if(gui_welcome_page_statuebar_item[i].pReFlash)
		{
			gui_welcome_page_statuebar_item[i].pReFlash((PRECT)&gui_welcome_page_statuebar_item[i].rc, gui_welcome_page_statuebar_item[i].pData0);
		}
	}
	for(i = 0; ; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(pGui_Area_Handle[i].pReFlash)
		{
			g_pWinPrivatePrt  = &pGui_Area_Handle[i];
			pGui_Area_Handle[i].pReFlash(&pGui_Area_Handle[i].rc, pGui_Area_Handle[i].pData0);
		}
	}
	EndPaint(hWnd, system_info.hdc);
}

/***************************************************************************************************
 * 函 数 名: HandleCreateEx
 * 日    期: 2016-10-12 10:39
 * 文 件 名: ui/guipub.h
****************************************************************************************************/
static inline void HandleCreateEx(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd)
{
	int i = 0;

	if(pGui_Area_Handle == NULL)
	{
		return;
	}

	/*system_info.hdc = BeginPaint(hWnd);*/
	for(i = 0; ; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(pGui_Area_Handle[i].pReFlash)
		{
			g_pWinPrivatePrt  = &pGui_Area_Handle[i];
			pGui_Area_Handle[i].pReFlash(&pGui_Area_Handle[i].rc, pGui_Area_Handle[i].pData0);
		}
	}
	/*EndPaint(hWnd, system_info.hdc);*/
}
/***************************************************************************************************
 * 函 数 名: HandleCreateExx
 * 日    期: 2016-11-02 15:47
 * 文 件 名: ui/guipub.h
 * 功    能: 带count数量的创建函数,由参数传入
 * 参    数: TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int Count
 * 返 回 值: 无
****************************************************************************************************/
static inline void HandleCreateExx(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int Count)
{
	int i = 0;

	/*system_info.hdc = BeginPaint(hWnd);*/
	//必须刷新状态栏
	for(i = 0; ; i++)
	{
		if(gui_welcome_page_statuebar_item[i].id == -1)
		{
			break;
		}
		if(gui_welcome_page_statuebar_item[i].pReFlash)
		{
			g_pWinPrivatePrt  = &gui_welcome_page_statuebar_item[i];
			gui_welcome_page_statuebar_item[i].pReFlash((PRECT)&gui_welcome_page_statuebar_item[i].rc, gui_welcome_page_statuebar_item[i].pData0);
		}
	}
	for(i = 0; i < Count; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(pGui_Area_Handle[i].pReFlash)
		{
			g_pWinPrivatePrt  = &pGui_Area_Handle[i];
			pGui_Area_Handle[i].pReFlash(&pGui_Area_Handle[i].rc, pGui_Area_Handle[i].pData0);
		}
	}
	/*EndPaint(hWnd, system_info.hdc);*/
}


static inline void HandleCreatePtr(TYPE_GUI_AREA_PTR** pGui_Area_Handle, HWND hWnd, int size)
{
	int i = 0;
	int j = 0;
	system_info.hdc = BeginPaint(hWnd);

	for(i = 0; ; i++)
	{
		if(gui_welcome_page_statuebar_item[i].id == -1)
		{
			break;
		}
		if(gui_welcome_page_statuebar_item[i].pReFlash)
		{
			gui_welcome_page_statuebar_item[i].pReFlash((PRECT)&gui_welcome_page_statuebar_item[i].rc, gui_welcome_page_statuebar_item[i].pData0);
		}
	}
	for(j = 0; j < size; j++)
	{
		for(i = 0; ; i++)
		{
			if(pGui_Area_Handle[j][i].id == -1 || pGui_Area_Handle[j] == NULL)
			{
				break;
			}
			if(pGui_Area_Handle[j][i].pReFlash)
			{
				g_pWinPrivatePrt  = &pGui_Area_Handle[j][i];
				pGui_Area_Handle[j][i].pReFlash(&pGui_Area_Handle[j][i].rc, pGui_Area_Handle[j][i].pData0, pGui_Area_Handle[j][i].pData1, pGui_Area_Handle[j][i].enable);
			}
		}
	}
	
	EndPaint(hWnd, system_info.hdc);
}

static inline void SetWidgetEnable(TYPE_GUI_AREA_PTR* pGui_Area_Handle, bool_t enable)
{
	if(!pGui_Area_Handle)
	{
		return;
	}
	
	if(pGui_Area_Handle->id == -1)
	{
		return;
	}
	else
	{
		pGui_Area_Handle->enable = enable;
	}
}

static inline TYPE_GUI_AREA_PTR* HandleWidgetInfo(TYPE_GUI_AREA_PTR** pGui_Area_Handle,int i,int j)
{
	return &pGui_Area_Handle[i][j];
}

static inline void HandleWidgetsUpdateProcPtr(TYPE_GUI_AREA_PTR** pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam)
{
	int i = 0,j = 0;

	int w_low  = WIDGET_LOW(wParam);
	int w_high = WIDGET_HIGH(wParam);
	int l_low  = WIDGET_LOW(lParam);
	int l_high = WIDGET_HIGH(lParam);
	RECT rc;
	if(!pGui_Area_Handle)
	{
		return;
	}
	
	if(w_high == 0xff)
	{
		InvalidateRect_Ex(hWnd, &pGui_Area_Handle[w_low][l_high].rc, TRUE);
		system_info.hdc = BeginPaint(hWnd);
		for(i = 0; ; i++)
		{
			if(pGui_Area_Handle[w_low][i].id == -1)
			{
				break;
			}

			g_pWinPrivatePrt = &pGui_Area_Handle[w_low][i];
			if(pGui_Area_Handle[w_low][i].pReFlash)
			{
				pGui_Area_Handle[w_low][i].pReFlash(&pGui_Area_Handle[w_low][i].rc, pGui_Area_Handle[w_low][i].pData0, pGui_Area_Handle[w_low][i].pData1, pGui_Area_Handle[w_low][i].enable);
			}			
		}
		EndPaint(hWnd, system_info.hdc);

		w_low += 1;
		SetRect(&rc, 560-15, 110, 560+35*4, 200+80+56+80);
		InvalidateRect_Ex(hWnd, &rc, TRUE);
		system_info.hdc = BeginPaint(hWnd);
		for(i = 0; ; i++)
		{
			if(pGui_Area_Handle[w_low][i].id == -1)
			{
				break;
			}

			g_pWinPrivatePrt = &pGui_Area_Handle[w_low][i];
			if(pGui_Area_Handle[w_low][i].pReFlash)
			{
				pGui_Area_Handle[w_low][i].pReFlash(&pGui_Area_Handle[w_low][i].rc, pGui_Area_Handle[w_low][i].pData0, pGui_Area_Handle[w_low][i].pData1, pGui_Area_Handle[w_low][i].enable);
			}			
		}
		EndPaint(hWnd, system_info.hdc);
	}
	else
	{
		g_pWinPrivatePrt = &pGui_Area_Handle[w_low][l_low];
		if(pGui_Area_Handle[w_low][l_low].pReFlash)
		{
			InvalidateRect_Ex(hWnd, &pGui_Area_Handle[w_low][l_low].rc, TRUE);
			system_info.hdc = BeginPaint(hWnd);
			pGui_Area_Handle[w_low][l_low].pReFlash(&pGui_Area_Handle[w_low][l_low].rc, pGui_Area_Handle[w_low][l_low].pData0, pGui_Area_Handle[w_low][l_low].pData1, pGui_Area_Handle[w_low][l_low].enable);
			EndPaint(hWnd, system_info.hdc);
		}
	}
}

static inline void HandleWidgetsHandleProcPtr(TYPE_GUI_AREA_PTR** pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam)
{
	int i = lParam,j = wParam;

	if(!pGui_Area_Handle)
	{
		return;
	}
	if(pGui_Area_Handle[j][i].id == -1)
	{
		return;
	}
	if(pGui_Area_Handle[j][i].pHandle)
	{
		pGui_Area_Handle[j][i].pHandle(hWnd, message, 0, 0);
		return;
	}
}

static inline void HandlePageProcPtr(TYPE_GUI_AREA_PTR** pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam,int size)
{
	int i = 0,j = 0;
	static int index = -1;
	static TYPE_GUI_AREA_PTR* pGui = NULL;

	g_pWinPrivatePrt = NULL;
	if(!pGui_Area_Handle)
	{
		return;
	}
	for(j = 0;j < size;j++)
	{
		for(i = 0; ; i++)
		{
			if(pGui_Area_Handle[j][i].id == -1 || pGui_Area_Handle[j]== NULL)
			{
				break;
			}
			if(message == MSG_LBUTTONDOWN||message == MSG_LBUTTONUP)
			{
				if(message == MSG_LBUTTONDOWN)
				{
					if(PtInRect(&pGui_Area_Handle[j][i].rc, LOWORD(lParam), HIWORD (lParam)))
					{
						g_pWinPrivatePrt = &pGui_Area_Handle[j][i];
						if(pGui_Area_Handle[j][i].pHandle)
						{
							index = i;
							pGui = pGui_Area_Handle[j];
							pGui_Area_Handle[j][i].pHandle(hWnd, message, wParam, lParam);
							break;
						}
					}
				}
				else
				{
					if(index != -1)
					{
						if(pGui == pGui_Area_Handle[j])
						{
							g_pWinPrivatePrt = &pGui_Area_Handle[j][index];
							if(pGui_Area_Handle[j][index].pHandle)
							{
								pGui_Area_Handle[j][index].pHandle(hWnd, MSG_LBUTTONUP, wParam, lParam);
							}
							index = -1;
							pGui = NULL;
						}
					}
					break;
				}
			}
			else
			{
				int w_low = WIDGET_LOW(wParam);
				int l_low = WIDGET_LOW(lParam);
			
				if(l_low  == pGui_Area_Handle[j][i].id && w_low == j)
				{
					g_pWinPrivatePrt = &pGui_Area_Handle[j][i];
					if(pGui_Area_Handle[j][i].pHandle)
					{
						pGui_Area_Handle[j][i].pHandle(hWnd, message, wParam, lParam);
						break;
					}
				}
			}

		}
	}
}

static inline void HandleStatueBarPageProc(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam)
{
	int i = 0;
	g_pWinPrivatePrt = NULL;

	for(i = 0;; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(wParam == pGui_Area_Handle[i].id)
		{
			g_pWinPrivatePrt = &pGui_Area_Handle[i];
			if(pGui_Area_Handle[i].pHandle)
			{
				pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
			}
			break;
		}
	}
}

static inline void HandlePageProc(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam)
{
	int i = 0;
	static int index = -1;
	static TYPE_GUI_AREA_HANDLE* pGui = NULL;

	g_pWinPrivatePrt = NULL;
	if(!pGui_Area_Handle)
	{
		return;
	}
	for(i = 0; ; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(message == MSG_LBUTTONDOWN||message == MSG_LBUTTONUP)
		{
			//弹起消息在后面处理
			if(message == MSG_LBUTTONDOWN)
			{
				if(PtInRect(&pGui_Area_Handle[i].rc, LOWORD(lParam), HIWORD (lParam)))
				{
					g_pWinPrivatePrt = &pGui_Area_Handle[i];
					if(pGui_Area_Handle[i].pHandle)
					{
						index = i;
						pGui = pGui_Area_Handle;//为了避免消息错位
						pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
						break;
					}
				}
			}
			else
			{
				if(index != -1)
				{
					if(pGui == pGui_Area_Handle) //为了避免消息错位
					{
						g_pWinPrivatePrt = &pGui_Area_Handle[index];
						if(pGui_Area_Handle[index].pHandle)
						{
							pGui_Area_Handle[index].pHandle(hWnd, MSG_LBUTTONUP, wParam, lParam);
						}
						index = -1;
						pGui = NULL;
					}
				}
				break;
			}
		}
		else
		{
			if(wParam == pGui_Area_Handle[i].id)
			{
				g_pWinPrivatePrt = &pGui_Area_Handle[i];
				if(pGui_Area_Handle[i].pHandle)
				{
					pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
				}
				break;
			}
		}
	}
}

//处理了父控件之后,还继续处理子控件
static inline void HandlePageProcXXXX(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam)
{
	int i = 0;
	static int index = -1;
	static TYPE_GUI_AREA_HANDLE* pGui = NULL;

	g_pWinPrivatePrt = NULL;
	if(!pGui_Area_Handle)
	{
		return;
	}
	for(i = 0; ; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(message == MSG_LBUTTONDOWN||message == MSG_LBUTTONUP)
		{
			//弹起消息在后面处理
			if(message == MSG_LBUTTONDOWN)
			{
				if(PtInRect(&pGui_Area_Handle[i].rc, LOWORD(lParam), HIWORD (lParam)))
				{
					g_pWinPrivatePrt = &pGui_Area_Handle[i];
					if(pGui_Area_Handle[i].pHandle)
					{
						index = i;
						pGui = pGui_Area_Handle;//为了避免消息错位
						pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
					}
				}
			}
			else
			{
				if(index != -1)
				{
					if(pGui == pGui_Area_Handle) //为了避免消息错位
					{
						g_pWinPrivatePrt = &pGui_Area_Handle[index];
						if(pGui_Area_Handle[index].pHandle)
						{
							pGui_Area_Handle[index].pHandle(hWnd, MSG_LBUTTONUP, wParam, lParam);
						}
						index = -1;
						pGui = NULL;
					}
				}
			}
		}
		else
		{
			if(wParam == pGui_Area_Handle[i].id)
			{
				g_pWinPrivatePrt = &pGui_Area_Handle[i];
				if(pGui_Area_Handle[i].pHandle)
				{
					pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
				}
				break;
			}
		}
	}
}

/***************************************************************************************************
 * 函 数 名: HandlePageProcEx
 * 日    期: 2016-11-02 16:52
 * 文 件 名: ui/guipub.h
 * 功    能: 带数量的处理函数
 * 参    数: 无
 * 返 回 值: 无
****************************************************************************************************/
static inline void HandlePageProcEx(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam, int Count)
{
	int i = 0;
	static int index = -1;
	static TYPE_GUI_AREA_HANDLE* pGui = NULL;

	g_pWinPrivatePrt = NULL;
	for(i = 0; i< Count; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break;
		}
		if(message == MSG_LBUTTONDOWN||message == MSG_LBUTTONUP)
		{
			//弹起消息在后面处理
			if(message == MSG_LBUTTONDOWN)
			{
				if(PtInRect(&pGui_Area_Handle[i].rc, LOWORD(lParam), HIWORD (lParam)))
				{
					g_pWinPrivatePrt = &pGui_Area_Handle[i];
					if(pGui_Area_Handle[i].pHandle)
					{
						index = i;
						pGui = pGui_Area_Handle;//为了避免消息错位
						pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
					}
					break;
				}
			}
			else
			{
				if(index != -1)
				{
					if(pGui == pGui_Area_Handle) //为了避免消息错位
					{
						g_pWinPrivatePrt = &pGui_Area_Handle[index];
						if(pGui_Area_Handle[index].pHandle)
						{
							pGui_Area_Handle[index].pHandle(hWnd, MSG_LBUTTONUP, wParam, lParam);
						}
						index = -1;
						pGui = NULL;
					}
				}
				break;
			}
		}
		else
		{
			if(wParam == pGui_Area_Handle[i].id)
			{
				g_pWinPrivatePrt = &pGui_Area_Handle[i];
				if(pGui_Area_Handle[i].pHandle)
				{
					pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
				}
				break;
			}
		}
	}
}

static inline SetStatueFunc(pStatueFunc func)
{
	if(g_StauteFun != func)
	{
		g_StauteFunOld = g_StauteFun;
		if(!g_StauteFunOld)
		{
			g_StauteFunOld = func;
		}
		g_StauteFun  = func;
		g_firstEnter = TRUE;
	}
}

static inline EnterWin(HWND hWnd, pStatueFunc func)
{
	SetStatueFunc(func);
	InvalidateRect(hWnd, NULL, TRUE);
	if(func != WelComeWinProc)
	{
		system_info.flagCreate = FALSE;
	}
	else
	{
		set_screen_id(NCS_IDLE_SCREEN_ID);
	}
	if(func == VolWinProc)
	{
		g_vol_change_win_flag = TRUE;
	}
	else
	{
		g_vol_change_win_flag = FALSE;
	}
	SendNotifyMessage(hWnd, MSG_CREATE, 0, 0);
}

//带反向消息处理
static inline void HandlePageProcXX(TYPE_GUI_AREA_HANDLE* pGui_Area_Handle, HWND hWnd, int message, int wParam, int lParam)
{
	int i = 0;
	static int index = -1;
	static TYPE_GUI_AREA_HANDLE* pGui = NULL;

	g_pWinPrivatePrt = NULL;
	if(!pGui_Area_Handle)
	{
		return;
	}
	for(i = 0; ; i++)
	{
		if(pGui_Area_Handle[i].id == -1)
		{
			break; 
		}
		if(message == MSG_LBUTTONDOWN||message == MSG_LBUTTONUP||message == MSG_MOUSEMOVE)
		{
			//弹起消息在后面处理
			if(message == MSG_LBUTTONDOWN)
			{
				if(PtInRect(&pGui_Area_Handle[i].rc, LOWORD(lParam), HIWORD (lParam)))
				{
					g_pWinPrivatePrt = &pGui_Area_Handle[i];
					if(pGui_Area_Handle[i].pHandle)
					{
						index = i;
						pGui = pGui_Area_Handle;//为了避免消息错位
						pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
					}
					/*break;*/
				}
			}
			/*else if(message == MSG_MOUSEMOVE)*/
			/*{*/
				/*if(PtInRect(&pGui_Area_Handle[i].rc, LOWORD(lParam), HIWORD (lParam)))*/
				/*{*/
					/*g_pWinPrivatePrt = &pGui_Area_Handle[i];*/
					/*if(pGui_Area_Handle[i].pHandle)*/
					/*{*/
						/*index = i;*/
						/*pGui = pGui_Area_Handle;//为了避免消息错位*/
						/*pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);*/
					/*}*/
					/*[>break;<]*/
				/*}*/
			/*}*/
			else
			{
				if(index != -1)
				{
					if(pGui == pGui_Area_Handle) //为了避免消息错位
					{
						g_pWinPrivatePrt = &pGui_Area_Handle[index];
						if(pGui_Area_Handle[index].pHandle)
						{
							pGui_Area_Handle[index].pHandle(hWnd, MSG_LBUTTONUP, wParam, lParam);
						}
						index = -1;
						pGui = NULL;
					}
				}
				/*else*/
				/*{*/
					/*g_vol_change_win_flag = 0;*/
					/*EnterWin(hWnd, g_StauteFunOld);*/
				/*}*/
				break;
			}
		}
		else
		{
			if(wParam == pGui_Area_Handle[i].id)
			{
				g_pWinPrivatePrt = &pGui_Area_Handle[i];
				if(pGui_Area_Handle[i].pHandle)
				{
					pGui_Area_Handle[i].pHandle(hWnd, message, wParam, lParam);
				}
				break;
			}
		}
	}
}
#endif




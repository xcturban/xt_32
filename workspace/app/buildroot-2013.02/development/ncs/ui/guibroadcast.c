#include  "guibroadcast.h"
#include "ncs_queue.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guibroadcast_button_display(PRECT pRc, void* pData);
static void guibroadcast_btn_func(HWND hWnd, int message, int wParam, int lParam);
static int guibroadcast_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static GUIBROADCAST g_guibroadcast_ctrl;

void guibroadcast_newscreen(HWND hWnd,task_t type)
{
	SPON_LOG_INFO("guibroadcast_newscreen type %d\n",type);
	set_screen_id(MSG_NCS_BROADCAST_WIN);
	set_system_task(TASK_NCS_BROADCAST_INCOMING);
	WEB_SYS_LOG(USRLOG_BCIN_START);
    g_guibroadcast_ctrl.Type = type;
	EnterWin(hWnd, guibroadcast_screen_proc);
}

#define GUIBROADCAST_INFO_L		((LCD_WIDTH - 581)/2)
#define GUIBROADCAST_INFO_T		((LCD_HEIGH - 356)/2)
#define GUIBROADCAST_INFO_R		((LCD_WIDTH + 581)/2)
#define GUIBROADCAST_INFO_B		((LCD_HEIGH + 356)/2)

const TYPE_GUI_AREA_HANDLE guibroadcast_data_items[] =
{
	{0  , {GUIBROADCAST_INFO_L , GUIBROADCAST_INFO_T , GUIBROADCAST_INFO_R , GUIBROADCAST_INFO_B } , "incommingbroadcast.png" , NULL          , guibroadcast_button_display , NULL                  } ,
	{1  , {BTN_BACK_L          , BTN_BACK_T          , BTN_BACK_R          , BTN_BACK_B          } , BTN_BACK_UP              , BTN_BACK_DOWN , guibroadcast_button_display , guibroadcast_btn_func } ,
	{-1 , {0                   , 0                   , 0                   , 0                   } , NULL                     , NULL          , NULL                        , NULL                  } ,
};

void guibroadcast_handup(HWND hWnd)
{
    struct broadcast *bc = term->ncs_manager->bc;

	set_system_task(TASK_NONE);
	WEB_SYS_LOG(USRLOG_BCIN_END);
	bc->ncs_wav_broadcast_task_stop(bc);
	EnterWin(hWnd, WelComeWinProc);
}

static void guibroadcast_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData1);
            EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0);
            EndPaint(hWnd, system_info.hdc);
			guibroadcast_handup(hWnd);
			SetSysStatueCloseBroadcastFlag(TRUE);
            break;
    }

}

static void guibroadcast_button_display(PRECT pRc, void* pData)
{	
	BITMAP * bitmap = NULL;


	bitmap  = png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
}

static int guibroadcast_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;
	TYPE_GUI_AREA_HANDLE* data_item;


	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
	}
	switch(message)
	{
		case MSG_CREATE:
			data_item = guibroadcast_data_items;
			HandleCreate((TYPE_GUI_AREA_HANDLE*)data_item, hWnd);
			break;
		case MSG_TIMER:
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			{
				TYPE_GUI_AREA_HANDLE* data_item;
				data_item = guibroadcast_data_items;
				HandlePageProc((TYPE_GUI_AREA_HANDLE*)data_item, hWnd, message, wParam, lParam);
			}
			break;
		case MSG_PAINT:
			return 0;
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			return 0;
		case MSG_ERASEBKGND:
			bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
			FillBoxWithBitmap((HDC)wParam,0,0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
			return 0;
		case MSG_KEYUP:
			switch (wParam)
			{
				case kKeyBcStop:
				/*case kKeyTaskIdle:*/
					guibroadcast_handup(hWnd);
					break;
			}
			break;
		case MSG_DESTROY:
			return 0;
		default:
			break;
	}
	return 1;
}

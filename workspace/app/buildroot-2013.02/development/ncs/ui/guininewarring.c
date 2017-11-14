
#include <sys/ioctl.h>
#include "language.h"
#include "guiwelcomwin.h"
#include "gui_call_incomming.h"
#include "gui_calloutcomming.h"
#include "gui_conference_window.h"
#include "utils.h"
#include "term.h"
#include "system_bmp_res.h"
#include "guipub.h"
#include "user_msg.h"
#include "system_info.h"
#include "emegcywarring.h"

#define M_POS_X 110
#define M_POS_Y 70
#define M_W_UNIT 180
#define M_H_UNIT 90
#define BLANCK_GRE_SIZE 20
#define BOTTOM_PIC_Y (M_POS_Y + 3*M_H_UNIT+2*BLANCK_GRE_SIZE + 25)
#define BOTTOM_PIC_X (M_POS_X+M_W_UNIT-50)

extern PLOGFONT logfont_ttf_verdana;
static void PaintGre(PRECT pRc, void* pData);
static void PaintGre1(PRECT pRc, void* pData);
static void PaintGre2(PRECT pRc, void* pData);
static void Gre_Proc(HWND hWnd, int message, int wParam, int lParam);
static void ButtonProc(HWND hWnd, int message, int wParam, int lParam);
static void PaintButton(PRECT pRc, void* pData);
static int fillcolor = 0; 

static char TmpName[18] = {0};

const TYPE_GUI_AREA_HANDLE gui_nine_item[] =
{
    {1   , {M_POS_X                                , M_POS_Y                              , M_POS_X + M_W_UNIT                      , M_POS_Y + M_H_UNIT                     } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,
    {2   , {M_POS_X+M_W_UNIT +BLANCK_GRE_SIZE      , M_POS_Y                              , M_POS_X + 2*M_W_UNIT+BLANCK_GRE_SIZE    , M_POS_Y + M_H_UNIT                     } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,
    {3   , {M_POS_X+2*M_W_UNIT + 2*BLANCK_GRE_SIZE , M_POS_Y                              , M_POS_X + 3*M_W_UNIT +2*BLANCK_GRE_SIZE , M_POS_Y + M_H_UNIT                     } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,

    {4   , {M_POS_X                                , M_POS_Y+M_H_UNIT+BLANCK_GRE_SIZE     , M_POS_X + M_W_UNIT                      , M_POS_Y + 2*M_H_UNIT+BLANCK_GRE_SIZE   } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,
    {5   , {M_POS_X+M_W_UNIT +BLANCK_GRE_SIZE      , M_POS_Y+M_H_UNIT+BLANCK_GRE_SIZE     , M_POS_X + 2*M_W_UNIT+BLANCK_GRE_SIZE    , M_POS_Y + 2*M_H_UNIT+BLANCK_GRE_SIZE   } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,
    {6   , {M_POS_X+2*M_W_UNIT+2*BLANCK_GRE_SIZE   , M_POS_Y+M_H_UNIT+BLANCK_GRE_SIZE     , M_POS_X + 3*M_W_UNIT +2*BLANCK_GRE_SIZE , M_POS_Y + 2*M_H_UNIT+BLANCK_GRE_SIZE   } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,

    {7   , {M_POS_X                                , M_POS_Y+2*M_H_UNIT+2*BLANCK_GRE_SIZE , M_POS_X + M_W_UNIT                      , M_POS_Y + 3*M_H_UNIT+2*BLANCK_GRE_SIZE } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,
    {8   , {M_POS_X+M_W_UNIT+BLANCK_GRE_SIZE       , M_POS_Y+2*M_H_UNIT+2*BLANCK_GRE_SIZE , M_POS_X + 2*M_W_UNIT + BLANCK_GRE_SIZE  , M_POS_Y + 3*M_H_UNIT+2*BLANCK_GRE_SIZE } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,
    {9   , {M_POS_X+2*M_W_UNIT+2*BLANCK_GRE_SIZE   , M_POS_Y+2*M_H_UNIT+2*BLANCK_GRE_SIZE , M_POS_X + 3*M_W_UNIT+2*BLANCK_GRE_SIZE  , M_POS_Y + 3*M_H_UNIT+2*BLANCK_GRE_SIZE } , "gre_down.png"   , "gre_up.png"       , PaintGre    , Gre_Proc   } ,

    {10  , {BOTTOM_PIC_X                           , BOTTOM_PIC_Y                         , BOTTOM_PIC_X+180                        , BOTTOM_PIC_Y+56                        } , "bc_stop_up.png" , "bc_stop_down.png" , PaintButton , ButtonProc } ,
    {11  , {BOTTOM_PIC_X+240                       , BOTTOM_PIC_Y                         , BOTTOM_PIC_X+2*240                      , BOTTOM_PIC_Y+56                        } , "return_up.png"  , "return_down.png"  , PaintButton , ButtonProc } ,
    { -1 , {DATE_LEFT                              , DATE_TOP                             , DATE_RIGHT                              , DATE_BOTTOOM                           } , NULL             , NULL               , NULL        , NULL       } ,
};

static void ButtonProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	TYPE_EMEGCY_WARRING_INFO *pInfo = g_pEmegcyWarring;

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
			switch(p->id)
			{
				case 10:
					pInfo->SendEmegcyWarringToServer(pInfo, pInfo->CurWarringIndex, ENUM_EMEGCY_WARRING_STATUE_STOP);
					break;
				case 11:
					if(pInfo->statue == ENUM_EMEGCY_WARRING_STATUE_STOP)
					{
						EnterWin(hWnd, WelComeWinProc);
					}
					break;
			}
			break;
	}
}

static void PaintButton(PRECT pRc, void* pData)
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

static void PaintGre(PRECT pRc, void* pData)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	TYPE_EMEGCY_WARRING_INFO *pInfo = g_pEmegcyWarring;
	RECT rc;
	BITMAP * bitmap = NULL;
	char Buf[100];



	memcpy(&rc, &p->rc, sizeof(RECT));

	if(pInfo->CurWarringIndex == p->id)
	{
		if(TmpName[0] != 0)
		{
			bitmap  = png_ha_res_find_by_name(TmpName);
		}
	}
	else
	{
		bitmap  = png_ha_res_find_by_name(pData);
	}
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
    /*draw_box(system_info.hdc, pRc->left, pRc->top, pRc->right, pRc->bottom, COLOR_lightwhite, fillcolor);*/
    SetBkMode(system_info.hdc, BM_TRANSPARENT);
    SetTextColor(system_info.hdc, COLOR_lightwhite);
	SelectFont(system_info.hdc, logfont_ttf_verdana); //选择字体，字体在之前就要创建好
	if(strlen(pInfo->Name[p->id-1]) > 8)
	{
		rc.left+=10;
		rc.top+=10;
		DrawText(system_info.hdc, pInfo->Name[p->id-1], -1, &rc, DT_LEFT);
	}
	else
	{
		rc.top+=26;
		DrawText(system_info.hdc, pInfo->Name[p->id-1], -1, &rc, DT_CENTER);
	}
}

static void PaintGre1(PRECT pRc, void* pData)
{
    draw_box(system_info.hdc, pRc->left, pRc->top, pRc->right, pRc->bottom, COLOR_lightwhite, RGB2Pixel(system_info.hdc, 0, 0, 255));
}
static void PaintGre2(PRECT pRc, void* pData)
{
    draw_box(system_info.hdc, pRc->left, pRc->top, pRc->right, pRc->bottom, COLOR_lightwhite, RGB2Pixel(system_info.hdc, 0, 120, 255));
}

static void Gre_Proc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	TYPE_EMEGCY_WARRING_INFO *pInfo = g_pEmegcyWarring;
	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    switch(message)
    {
        case MSG_KEYUP:
			if(lParam == KKEYWARINGBEGIN || lParam == KKEYWARINGOVER)
			{
				system_info.hdc = BeginPaint(hWnd);
				p->pReFlash(&p->rc, p->pData0);
				EndPaint(hWnd, system_info.hdc);
			}
			break;
        case MSG_LBUTTONDOWN:
			/*fillcolor = RGB2Pixel(system_info.hdc, 0, 220, 255);*/
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData1);
            EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
			/*fillcolor = RGB2Pixel(system_info.hdc, 38, 38, 38);*/
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0);
            EndPaint(hWnd, system_info.hdc);

			pInfo->SendEmegcyWarringToServer(pInfo, p->id, ENUM_EMEGCY_WARRING_STATUE_PLAY);
            break;
    }
}


int guininewarring_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap;
	TYPE_EMEGCY_WARRING_INFO *pInfo = g_pEmegcyWarring;
	
	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
		fillcolor = RGB2Pixel(system_info.hdc, 38, 38, 38);
	}
	
	switch(message)
    {
		case MSG_CREATE:
			pthread_mutex_lock(&term->mutex);
            HandleCreate(gui_nine_item, hWnd);
			pthread_mutex_unlock(&term->mutex);
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			pthread_mutex_lock(&term->mutex);
            HandlePageProc(gui_nine_item, hWnd, message, wParam, lParam);
			pthread_mutex_unlock(&term->mutex);
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
        case MSG_NCS_SWITCH_WND:
			if(pInfo->statue == ENUM_EMEGCY_WARRING_STATUE_STOP)
			{
				//按红色按钮触发紧急报警，只有在空闲状态下才能触发紧急报警。
				EnterNewScreen(hWnd, (int)lParam);
			}
            break;
		case MSG_KEYUP:
			switch (lParam)
			{
				case KKEYWARINGBEGIN:
					pthread_mutex_lock(&term->mutex);
					strcpy(TmpName, "gre_ing-hong.png");
					HandlePageProc(gui_nine_item, hWnd, message, wParam, lParam);
					pthread_mutex_unlock(&term->mutex);
					break;
				case KKEYWARINGOVER:
					pthread_mutex_lock(&term->mutex);
					TmpName[0] = 0;
					HandlePageProc(gui_nine_item, hWnd, message, wParam, lParam);
					pthread_mutex_unlock(&term->mutex);
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




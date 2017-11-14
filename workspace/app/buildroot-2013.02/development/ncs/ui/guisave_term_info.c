
#include "guisave_term_info.h"

static void PaintButtonEx(PRECT pRc, void* pData);
static void Button_Proc(HWND hWnd, int message, int wParam, int lParam);
static void PaintRect(PRECT pRc, void* pData);
int SettingWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
void system_config_save(void);
void put_net_opt(void);

const TYPE_GUI_AREA_HANDLE gui_save_info_pager_item[] =
{
	{0  , {95+118     , 85+48  , 95+118+349    , 85+48+195} , "tip_win.png" , NULL           , PaintRect     , NULL} , 
	{1  , {95+118+60  , 85+160 , 95+118+60+94  , 85+160+48} , "ok.png"      , "ok_1.png"     , PaintButtonEx , Button_Proc} , 
	{2  , {95+118+220 , 85+160 , 95+118+220+94 , 85+160+48} , "cancel.png"  , "cancel_1.png" , PaintButtonEx , Button_Proc} , 
	{-1 , {0          , 0      , 0             , 0}         , NULL          , NULL           , NULL          , NULL} , 
};


static void Button_Proc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

    switch(message)
    {
        case MSG_LBUTTONDOWN:
			InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData1);
            EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
            switch(p->id)
            {
                case 1:
                    put_net_opt();
                    system_config_save();
                    system("reboot");
                    break;
                case 2:
                    EnterWin(hWnd, SettingWinProc);
                    break;
            }
            break;
    }
}

	
static void PaintButtonEx(PRECT pRc, void* pData)
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

static void PaintRect(PRECT pRc, void* pData)
{
	BITMAP * bitmap = NULL;

	uint8_t Buf[] ={"配置保存将要重启系统，是否需要保存?"};
	uint8_t Buf2[] ={"Save Configuare Will Reboot System, Is Save ?\n"};
	RECT rect ;

	memcpy(&rect, pRc, sizeof(RECT));
	rect.top +=80;
	rect.left +=70;
	bitmap  = png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	if(term->language == 1 )
	{
		DrawText(system_info.hdc, Buf, -1, &rect, DT_LEFT);
	}
	else
	{
		DrawText(system_info.hdc, Buf2, -1, &rect, DT_LEFT);
	}
}

int SaveInfoWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;

	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
	}
	switch(message)
    {
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
            HandlePageProcXXXX((TYPE_GUI_AREA_HANDLE*)gui_save_info_pager_item, hWnd, message, wParam, lParam);
            break;
        case MSG_PAINT:
            HandleCreate((TYPE_GUI_AREA_HANDLE*)gui_save_info_pager_item, hWnd);
            return 0;
        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
        case MSG_ERASEBKGND:
            bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
            FillBoxWithBitmap((HDC)wParam,0,0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
            return 0;
        case MSG_DESTROY:
            return 0;
        default:
            break;
    }
	return 1;
}


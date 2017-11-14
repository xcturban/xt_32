#include  "guilocalvideo.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guilocalvideo_button_display(PRECT pRc, void* pData);
static int guilocalvideo_stream_display(PRECT pRc, void* pData);
static void guilocalvideo_title_display(PRECT pRc, void* pData);
static void guilocalvideo_btn_func(HWND hWnd, int message, int wParam, int lParam);
static int guilocalvideo_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static GUILOCALVIDEO g_guilocalvideo_ctrl;

void guilocalvideo_newscreen(HWND hWnd)
{
    //g_guilocalvideo_ctrl.Type = type;
	EnterWin(hWnd, guilocalvideo_screen_proc);
}


const TYPE_GUI_AREA_HANDLE guilocalvideo_data_items[] =
{
	{0  , {0          , 40         , LCD_WIDTH  , 65         } , NULL        , NULL          , guilocalvideo_title_display  , NULL                   } ,
	{1  , {20         , 50         , 20+600     , 50+420     } , NULL        , NULL          , guilocalvideo_stream_display , NULL                   } ,
	{2  , {BTN_BACK_L , BTN_BACK_T , BTN_BACK_R , BTN_BACK_B } , BTN_BACK_UP , BTN_BACK_DOWN , guilocalvideo_button_display , guilocalvideo_btn_func } ,
	{-1 , {0          , 0          , 0          , 0          } , NULL        , NULL          , NULL                         , NULL                   } ,
};

static int guilocalvideo_stream_display(PRECT pRc, void* pData)
{
	SPON_LOG_INFO("VIDEO DISPLAY\n");
	SetBrushColor(system_info.hdc, 0x12345678);//0x12345678
	FillBox(system_info.hdc, pRc->left, pRc->top, pRc->right-pRc->left, pRc->bottom-pRc->top);

	local_video_start(pRc);
    return 0;
}

static void guilocalvideo_stop(HWND hWnd)
{
    local_video_stop();
	set_system_task(TASK_NONE);
	EnterWin(hWnd, WelComeWinProc);
}

static void guilocalvideo_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;
	struct call_manager *call_manager = term->call_manager;
	call_t *call = call_manager->current_call_get(call_manager);
	
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
			guilocalvideo_stop(hWnd);
            break;
    }

}

static void guilocalvideo_button_display(PRECT pRc, void* pData)
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

static void guilocalvideo_title_display(PRECT pRc, void* pData)
{
	char buf[128] = {0};
	sprintf(buf, "本地视频\n");

	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	DrawText(system_info.hdc, buf, -1, pRc, DT_CENTER);
}


static int guilocalvideo_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
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
			data_item = guilocalvideo_data_items;
			HandleCreate((TYPE_GUI_AREA_HANDLE*)data_item, hWnd);
			break;
		case MSG_TIMER:
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			{
				TYPE_GUI_AREA_HANDLE* data_item;
				data_item = guilocalvideo_data_items;
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
				case kKeyTaskIdle:
					guilocalvideo_stop(hWnd);
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

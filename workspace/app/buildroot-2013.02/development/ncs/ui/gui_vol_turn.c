
#include "gui_vol_turn.h"
#include "audio_local.h"
#include "common.h"
#include "gui_call_chatting.h"

static TYPE_CLTL_VOL_TURN s_vol_turn_ctl;
static void PaintVolRect(PRECT pRc, void* pData);
static void PaintVolSP_Proc(HWND hWnd, int message, int wParam, int lParam);
static void PaintVolRect_Proc(HWND hWnd, int message, int wParam, int lParam);
static void PaintVolBmp(PRECT pRc, void * pData);
static void ButtonProc(HWND hWnd, int message, int wParam, int lParam);
static void PaintButton(PRECT pRc, void* pData);
static void PaintVolRect_Proc0(HWND hWnd, int message, int wParam, int lParam);

const TYPE_GUI_AREA_HANDLE gui_welcome_page_vol_turn_item[] =
{
    {0  , {(800-465)/2 -20        , 100        , (800-465)/2+465+20       , 100+240      } , "vol_turn_bar.png" , NULL            , PaintVolRect , PaintVolRect_Proc0 } , 
    {1  , {(800-293)/2+30     , 175        , (800-293)/2+30+293    , 175+34       } , "vol_turn_bar.png" , NULL            , PaintVolBmp  , PaintVolRect_Proc  } , 
    {2  , {(800-293)/2+30     , 260        , (800-293)/2+30+293    , 260+34       } , "vol_turn_bar.png" , NULL            , PaintVolBmp  , PaintVolRect_Proc  } , 
    {3  , {(800-293)/2-10     , 173        , (800-293)/2-10+40     , 173+40       } , "sub.png"          , "sub_down.png"  , PaintVolBmp  , PaintVolSP_Proc  } , 
    {4  , {(800-293)/2+30+293 , 173        , (800-293)/2+30+293+40 , 173+40       } , "plus.png"         , "plus_down.png" , PaintVolBmp  , PaintVolSP_Proc  } , 
    {5  , {(800-293)/2-10     , 257        , (800-293)/2-10+40     , 257+40       } , "sub.png"          , "sub_down.png"  , PaintVolBmp  , PaintVolSP_Proc  } , 
    {6  , {(800-293)/2+30+293 , 257        , (800-293)/2+30+293+40 , 257+40       } , "plus.png"         , "plus_down.png" , PaintVolBmp  , PaintVolSP_Proc  } , 
    {7  , {BTN_BACK_L         , BTN_BACK_T , BTN_BACK_R            , BTN_BACK_B   } , BTN_BACK_UP        , BTN_BACK_DOWN   , PaintButton  , ButtonProc         } , // 返回
    {-1 , {DATE_LEFT          , DATE_TOP   , DATE_RIGHT            , DATE_BOTTOOM } , NULL               , NULL            , NULL         , NULL               } , 
};

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

static void ButtonProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	int ret = 0;

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
			g_vol_change_win_flag = 0;
			EnterWin(hWnd, g_StauteFunOld);
		default:
			break;
	}
}

static void initVol_turn_ctl(void)
{
	int b_cur_pos = 0; 
	int b_distance = 0;
	struct audio_manager *audio_manager = term->audio_manager;

	b_distance = gui_welcome_page_vol_turn_item[1].rc.right-6-37-gui_welcome_page_vol_turn_item[1].rc.left+8;
	s_vol_turn_ctl.b_p_vol_win_ptr = NULL;
	s_vol_turn_ctl.vol_value[0]       = audio_manager->get_talk_out_volume(audio_manager);
	s_vol_turn_ctl.vol_value[1]       = audio_manager->get_talk_in_volume(audio_manager);
	s_vol_turn_ctl.rc[0].left         = gui_welcome_page_vol_turn_item[1].rc.left+8+b_distance *s_vol_turn_ctl.vol_value[0]/15; 
	s_vol_turn_ctl.rc[0].top          = gui_welcome_page_vol_turn_item[1].rc.top;
	s_vol_turn_ctl.rc[0].right        = s_vol_turn_ctl.rc[0].left+37;
	s_vol_turn_ctl.rc[0].bottom       = s_vol_turn_ctl.rc[0].top+37;
	//限制范围
	if(s_vol_turn_ctl.rc[0].left  < gui_welcome_page_vol_turn_item[1].rc.left+8)
	{
		s_vol_turn_ctl.rc[0].left = gui_welcome_page_vol_turn_item[1].rc.left+8;
		s_vol_turn_ctl.rc[0].right = s_vol_turn_ctl.rc[0].left+37;
	}
	if(s_vol_turn_ctl.rc[0].right > gui_welcome_page_vol_turn_item[1].rc.right)
	{
		s_vol_turn_ctl.rc[0].right = gui_welcome_page_vol_turn_item[1].rc.right;
		s_vol_turn_ctl.rc[0].left = s_vol_turn_ctl.rc[0].right-37;
	}
	b_distance = gui_welcome_page_vol_turn_item[2].rc.right-6-37-gui_welcome_page_vol_turn_item[2].rc.left+8;
	s_vol_turn_ctl.rc[1].left         =gui_welcome_page_vol_turn_item[2].rc.left+8+b_distance *s_vol_turn_ctl.vol_value[1]/15; 
	s_vol_turn_ctl.rc[1].top          = gui_welcome_page_vol_turn_item[2].rc.top;
	s_vol_turn_ctl.rc[1].right        = s_vol_turn_ctl.rc[1].left+37;
	s_vol_turn_ctl.rc[1].bottom       = s_vol_turn_ctl.rc[1].top+37;
	//限制范围
	if(s_vol_turn_ctl.rc[1].left  < gui_welcome_page_vol_turn_item[2].rc.left+8)
	{
		s_vol_turn_ctl.rc[1].left  = gui_welcome_page_vol_turn_item[2].rc.left+8;
		s_vol_turn_ctl.rc[1].right = s_vol_turn_ctl.rc[1].left+37;
	}
	else if(s_vol_turn_ctl.rc[1].right > gui_welcome_page_vol_turn_item[2].rc.right)
	{
		s_vol_turn_ctl.rc[1].right = gui_welcome_page_vol_turn_item[2].rc.right;
		s_vol_turn_ctl.rc[1].left = s_vol_turn_ctl.rc[1].right-37;
	}
	s_vol_turn_ctl.vol_slider_down_left = 0;
	s_vol_turn_ctl.f_first_down = FALSE;
}

static void PaintVolBmpTmp(PRECT pRc, void * pData)
{
	BITMAP * bitmap = NULL;
    TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	char Buf[128];
	struct audio_manager *audio_manager = term->audio_manager;
	RECT rect;

    bitmap  = png_ha_res_find_by_name(pData);
    FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	bitmap  = png_ha_res_find_by_name("vol_slider.png");
	SetTextColor(system_info.hdc, RGB2Pixel(system_info.hdc, 255, 0, 0));
	SetBkColor(system_info.hdc, system_info.status_bar_bg_color);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	if(p->id == 1)
	{
		FillBoxWithBitmap(system_info.hdc, s_vol_turn_ctl.rc[0].left, s_vol_turn_ctl.rc[0].top,  bitmap->bmWidth, bitmap->bmHeight, bitmap);
		memcpy(&rect,  &s_vol_turn_ctl.rc[0], sizeof(RECT));
		rect.left +=3;
		rect.top +=8;
		sprintf(Buf, "%d", audio_manager->talk_out_volume);
	}
	else if(p->id == 2)
	{
        FillBoxWithBitmap(system_info.hdc, s_vol_turn_ctl.rc[1].left, s_vol_turn_ctl.rc[1].top,  bitmap->bmWidth, bitmap->bmHeight, bitmap);
		memcpy(&rect,  &s_vol_turn_ctl.rc[1], sizeof(RECT));
		rect.left +=3;
		rect.top +=8;
		sprintf(Buf, "%d",audio_manager->talk_in_volume);
	}
	DrawText(system_info.hdc, Buf, -1, &rect, DT_CENTER);
}

static void PaintVolBmp(PRECT pRc, void * pData)
{
	BITMAP * bitmap = NULL;
	char Buf[128];

    RECT rect;
	struct audio_manager *audio_manager = term->audio_manager;

    TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
    bitmap  = png_ha_res_find_by_name(pData);
    FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);

    if(p->id == 1)
    {
		memcpy(&rect, &p->rc, sizeof(RECT));
		SetTextColor(system_info.hdc, COLOR_lightwhite);
		SetBkColor(system_info.hdc, system_info.status_bar_bg_color);
		rect.left -= 120;
		rect.top += 10;
		if(term->language == 1)
		{
			sprintf(Buf, "输出音量");
		}
		else
		{
			sprintf(Buf, "Output Vol");
		}
		DrawText(system_info.hdc, Buf, -1, &rect, DT_LEFT);

		bitmap  = png_ha_res_find_by_name("vol_slider.png");
		FillBoxWithBitmap(system_info.hdc, s_vol_turn_ctl.rc[0].left, s_vol_turn_ctl.rc[0].top,  bitmap->bmWidth, bitmap->bmHeight, bitmap);

		SetTextColor(system_info.hdc, RGB2Pixel(system_info.hdc, 255, 0, 0));
		SetBkColor(system_info.hdc, system_info.status_bar_bg_color);
		SetBkMode(system_info.hdc, BM_TRANSPARENT);
		memcpy(&rect,  &s_vol_turn_ctl.rc[0], sizeof(RECT));
		rect.left +=3;
		rect.top +=8;
		sprintf(Buf, "%d", audio_manager->talk_out_volume);
    }
    else if(p->id == 2)
    {
		memcpy(&rect, &p->rc, sizeof(RECT));
		SetTextColor(system_info.hdc, COLOR_lightwhite);
		SetBkColor(system_info.hdc, system_info.status_bar_bg_color);
		rect.left -= 120;
		rect.top += 10;


		if(term->language == 1)
		{
			sprintf(Buf, "输入音量");
		}
		else
		{
			sprintf(Buf, "Input Vol");
		}
		DrawText(system_info.hdc, Buf, -1, &rect, DT_LEFT);
		bitmap  = png_ha_res_find_by_name("vol_slider.png");
		FillBoxWithBitmap(system_info.hdc, s_vol_turn_ctl.rc[1].left, s_vol_turn_ctl.rc[1].top,  bitmap->bmWidth, bitmap->bmHeight, bitmap);

		SetTextColor(system_info.hdc, RGB2Pixel(system_info.hdc, 255, 0, 0));
		SetBkColor(system_info.hdc, system_info.status_bar_bg_color);
		SetBkMode(system_info.hdc, BM_TRANSPARENT);
		memcpy(&rect,  &s_vol_turn_ctl.rc[1], sizeof(RECT));
		rect.left +=3;
		rect.top +=8;
		sprintf(Buf, "%d",audio_manager->talk_in_volume);
    }
	DrawText(system_info.hdc, Buf, -1, &rect, DT_CENTER);
}

static void PaintVolRect(PRECT pRc, void* pData)
{	
	/*SetBrushColor(system_info.hdc, system_info.title_bar_bg_color);*/
	RECT rect;
	memcpy(&rect, pRc, sizeof(RECT));
	SetPenColor(system_info.hdc, system_info.status_bar_bg_color);
	SetBrushColor(system_info.hdc, system_info.status_bar_bg_color);
	RoundRect(system_info.hdc, pRc->left,
			pRc->top,	pRc->right, pRc->bottom, 8, 8);
	SetBrushColor(system_info.hdc, system_info.title_bar_bg_color);
	//划圆角
	SetPenColor(system_info.hdc, system_info.status_bar_bg_color);
	FillArcEx(system_info.hdc, pRc->left, pRc->top, 8*2, 8*2, 90*64, 90*64);
	FillArcEx(system_info.hdc, pRc->right-16, pRc->top, 8*2, 8*2, 0, 90*64);
	FillBox(system_info.hdc, pRc->left+8, pRc->top, pRc->right-pRc->left-16, 16);
	FillBox(system_info.hdc, pRc->left, pRc->top+8, pRc->right-pRc->left+1, 30);

	SetPenColor(system_info.hdc, system_info.status_bar_bg_color);
	ArcEx(system_info.hdc, pRc->left, pRc->top, 16, 16, 90*64, 90*64);
	ArcEx(system_info.hdc, pRc->right-16, pRc->top, 16, 16, 0, 90*64);
	SetPenColor(system_info.hdc, COLOR_lightwhite);

	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	rect.top+=10;
	if(term->language == 1)
	{
		DrawText(system_info.hdc, "音量控制", -1, &rect, DT_CENTER);
	}
	else
	{
		DrawText(system_info.hdc, "Vol Ctrl", -1, &rect, DT_CENTER);
	}
}

static void PaintVolRect_Proc0(HWND hWnd, int message, int wParam, int lParam)
{

}

static void PaintVolSP_Proc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	struct audio_manager *audio_manager = term->audio_manager;
	int ret = 0;

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
                case 3: //输出音量－
                    ret = audio_manager->get_talk_out_volume(audio_manager);
                    if(ret)
                    {
                        ret--;
                        audio_manager->set_talk_out_volume(audio_manager, ret);
                        //刷新滑块，直接调用p->id ==2的刷新函数
                        p = &gui_welcome_page_vol_turn_item[1];
                        g_pWinPrivatePrt = p;
                        initVol_turn_ctl();
                        InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
                        system_info.hdc = BeginPaint(hWnd);
                        p->pReFlash(&p->rc, p->pData0);
                        EndPaint(hWnd, system_info.hdc);
						SendNotifyMessage(hWnd, MSG_NCS_STATUE_BAR_PAGE, 1, 0);
                    }
                    break;
                case 4://输出音量＋
                    ret = audio_manager->get_talk_out_volume(audio_manager);
                    if(ret < 15)
                    {
                        ret++;
                        audio_manager->set_talk_out_volume(audio_manager, ret);
                        //刷新滑块，直接调用p->id ==2的刷新函数
                        p = &gui_welcome_page_vol_turn_item[1];
                        g_pWinPrivatePrt = p;
                        initVol_turn_ctl();
                        InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
                        system_info.hdc = BeginPaint(hWnd);
                        p->pReFlash(&p->rc, p->pData0);
                        EndPaint(hWnd, system_info.hdc);
						SendNotifyMessage(hWnd, MSG_NCS_STATUE_BAR_PAGE, 1, 0);
                    }
                    break;
                case 5: //输入音量－
                    ret = audio_manager->get_talk_in_volume(audio_manager);
                    if(ret)
                    {
                        ret--;
                        audio_manager->set_talk_in_volume(audio_manager, ret);
                        //刷新滑块，直接调用p->id ==2的刷新函数
                        p = &gui_welcome_page_vol_turn_item[2];
                        g_pWinPrivatePrt = p;
                        initVol_turn_ctl();
                        InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
                        system_info.hdc = BeginPaint(hWnd);
                        p->pReFlash(&p->rc, p->pData0);
                        EndPaint(hWnd, system_info.hdc);
                    }
                    break;
                case 6://输入音量＋
                    ret = audio_manager->get_talk_in_volume(audio_manager);
                    if(ret < 15)
                    {
                        ret++;
                        audio_manager->set_talk_in_volume(audio_manager, ret);
                        //刷新滑块，直接调用p->id ==2的刷新函数
                        p = &gui_welcome_page_vol_turn_item[2];
                        g_pWinPrivatePrt = p;
                        initVol_turn_ctl();
                        InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
                        system_info.hdc = BeginPaint(hWnd);
                        p->pReFlash(&p->rc, p->pData0);
                        EndPaint(hWnd, system_info.hdc);
                    }
                    break;
                default:
                    break;
            }
		default:
			break;
	}
}

static void PaintVolRect_Proc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	struct audio_manager *audio_manager = term->audio_manager;
    RECT rect;
    int x, y;
	char Buf[128];
	int b_cur_pos;
    int b_distance = 0;
    static int s_fix_distance = 0;

    switch(message)
    {
        case MSG_LBUTTONDOWN:
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            if(PtInRect(&s_vol_turn_ctl.rc[0], x, y))
			{
				if(!s_vol_turn_ctl.f_first_down)
				{
					s_vol_turn_ctl.f_first_down = TRUE;
					s_fix_distance = x-s_vol_turn_ctl.rc[0].left;
				}
			}
			else
			{
				if(PtInRect(&p->rc, x, y))
				{
					if(p->id == 1)
					{
						s_vol_turn_ctl.rc[0].left = x-5;
						s_vol_turn_ctl.rc[0].right = s_vol_turn_ctl.rc[0].left+37;
						if(s_vol_turn_ctl.rc[0].left  < gui_welcome_page_vol_turn_item[1].rc.left+8)
						{
							s_vol_turn_ctl.rc[0].left = gui_welcome_page_vol_turn_item[1].rc.left+8;
							s_vol_turn_ctl.rc[0].right = s_vol_turn_ctl.rc[0].left+37;
						}
						if(s_vol_turn_ctl.rc[0].right > gui_welcome_page_vol_turn_item[1].rc.right)
						{
							s_vol_turn_ctl.rc[0].right = gui_welcome_page_vol_turn_item[1].rc.right;
							s_vol_turn_ctl.rc[0].left = s_vol_turn_ctl.rc[0].right-37;
						}

						b_distance = gui_welcome_page_vol_turn_item[1].rc.right-37-gui_welcome_page_vol_turn_item[1].rc.left+8;
						b_cur_pos = s_vol_turn_ctl.rc[0].left-gui_welcome_page_vol_turn_item[1].rc.left+8;
						s_vol_turn_ctl.vol_value[0]       = (b_cur_pos*15/b_distance);
						audio_manager->set_talk_out_volume(audio_manager, s_vol_turn_ctl.vol_value[0]);

						InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
						system_info.hdc = BeginPaint(hWnd);
						PaintVolBmpTmp(&p->rc, p->pData0);
						EndPaint(hWnd, system_info.hdc);
						SendNotifyMessage(hWnd, MSG_NCS_STATUE_BAR_PAGE, 1, 0);
					}
					else if(p->id ==  2)
					{
						s_vol_turn_ctl.rc[1].left = x-5;
						s_vol_turn_ctl.rc[1].right = s_vol_turn_ctl.rc[1].left+37;

						if(s_vol_turn_ctl.rc[1].left  < gui_welcome_page_vol_turn_item[2].rc.left+8)
						{
							s_vol_turn_ctl.rc[1].left  = gui_welcome_page_vol_turn_item[2].rc.left+8;
							s_vol_turn_ctl.rc[1].right = s_vol_turn_ctl.rc[1].left+37;
						}
						else if(s_vol_turn_ctl.rc[1].right > gui_welcome_page_vol_turn_item[2].rc.right)
						{
							s_vol_turn_ctl.rc[1].right = gui_welcome_page_vol_turn_item[2].rc.right;
							s_vol_turn_ctl.rc[1].left = s_vol_turn_ctl.rc[1].right-37;
						}
						b_distance = gui_welcome_page_vol_turn_item[2].rc.right-37-gui_welcome_page_vol_turn_item[2].rc.left+8;
						b_cur_pos = s_vol_turn_ctl.rc[1].left-gui_welcome_page_vol_turn_item[2].rc.left+8;
						s_vol_turn_ctl.vol_value[1]       = (b_cur_pos*15/b_distance);
						/*audio_volume_set(AUDIO_IN_SET, s_vol_turn_ctl.vol_value[1]);*/
						audio_manager->set_talk_in_volume(audio_manager, s_vol_turn_ctl.vol_value[1]);
						InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
						system_info.hdc = BeginPaint(hWnd);
						PaintVolBmpTmp(&p->rc, p->pData0);
						EndPaint(hWnd, system_info.hdc);
					}
				}
			}
            break;
        case MSG_LBUTTONUP:
            s_fix_distance = 0;
			s_vol_turn_ctl.f_first_down = FALSE;
            break;
		case MSG_MOUSEMOVE:
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			if(PtInRect(&s_vol_turn_ctl.rc[0], x, y))
			{
				if(!s_vol_turn_ctl.f_first_down)
				{
					s_vol_turn_ctl.f_first_down = TRUE;
					s_fix_distance = x-s_vol_turn_ctl.rc[0].left;
				}
				s_vol_turn_ctl.rc[0].left = x-s_fix_distance;
				s_vol_turn_ctl.rc[0].right = s_vol_turn_ctl.rc[0].left+37;
				if(s_vol_turn_ctl.rc[0].left  < gui_welcome_page_vol_turn_item[1].rc.left+8)
				{
					s_vol_turn_ctl.rc[0].left = gui_welcome_page_vol_turn_item[1].rc.left+8;
					s_vol_turn_ctl.rc[0].right = s_vol_turn_ctl.rc[0].left+37;
				}
				if(s_vol_turn_ctl.rc[0].right > gui_welcome_page_vol_turn_item[1].rc.right)
				{
					s_vol_turn_ctl.rc[0].right = gui_welcome_page_vol_turn_item[1].rc.right;
					s_vol_turn_ctl.rc[0].left = s_vol_turn_ctl.rc[0].right-37;
				}
				b_distance = gui_welcome_page_vol_turn_item[1].rc.right-37-gui_welcome_page_vol_turn_item[1].rc.left+8;
				b_cur_pos = s_vol_turn_ctl.rc[0].left-gui_welcome_page_vol_turn_item[1].rc.left+8;
				s_vol_turn_ctl.vol_value[0]       = (b_cur_pos*15/b_distance);
				audio_manager->set_talk_out_volume(audio_manager, s_vol_turn_ctl.vol_value[0]);
				InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
				system_info.hdc = BeginPaint(hWnd);
				p->pReFlash(&p->rc, p->pData0);
				PaintVolBmpTmp(&p->rc, p->pData0);
				EndPaint(hWnd, system_info.hdc);
				SendNotifyMessage(hWnd, MSG_NCS_STATUE_BAR_PAGE, 1, 0);
			}
			else if(PtInRect(&s_vol_turn_ctl.rc[1], x, y))
			{
				if(!s_vol_turn_ctl.f_first_down)
				{
					s_vol_turn_ctl.f_first_down = TRUE;
					s_fix_distance = x-s_vol_turn_ctl.rc[1].left;
				}
				s_vol_turn_ctl.rc[1].left = x-s_fix_distance;
				s_vol_turn_ctl.rc[1].right = s_vol_turn_ctl.rc[1].left+37;
				if(s_vol_turn_ctl.rc[1].left  < gui_welcome_page_vol_turn_item[2].rc.left+8)
				{
					s_vol_turn_ctl.rc[1].left  = gui_welcome_page_vol_turn_item[2].rc.left+8;
					s_vol_turn_ctl.rc[1].right = s_vol_turn_ctl.rc[1].left+37;
				}
				else if(s_vol_turn_ctl.rc[1].right > gui_welcome_page_vol_turn_item[2].rc.right)
				{
					s_vol_turn_ctl.rc[1].right = gui_welcome_page_vol_turn_item[2].rc.right;
					s_vol_turn_ctl.rc[1].left = s_vol_turn_ctl.rc[1].right-37;
				}
				b_distance = gui_welcome_page_vol_turn_item[2].rc.right-37-gui_welcome_page_vol_turn_item[2].rc.left+8;
				b_cur_pos = s_vol_turn_ctl.rc[1].left-gui_welcome_page_vol_turn_item[2].rc.left+8;
				s_vol_turn_ctl.vol_value[1]       = (b_cur_pos*15/b_distance);
				audio_manager->set_talk_in_volume(audio_manager, s_vol_turn_ctl.vol_value[1]);
				InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
				system_info.hdc = BeginPaint(hWnd);
				p->pReFlash(&p->rc, p->pData0);
				PaintVolBmpTmp(&p->rc, p->pData0);
				EndPaint(hWnd, system_info.hdc);
			}
			break;
    }
}

void VolBackToWelcome(HWND hWnd)
{
    struct broadcast *bc = term->ncs_manager->bc;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct audio_manager *audio_manager = term->audio_manager;

	if (bc->bc_incoming(bc))
	{
		bc->ncs_wav_broadcast_task_stop(bc);
	}

	set_system_task(TASK_NONE);
	EnterWin(hWnd, WelComeWinProc);
}

int VolWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;


	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
		initVol_turn_ctl();
	}
	switch(message)
    {
		case MSG_CREATE:
			HandleCreate((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_vol_turn_item, hWnd);
			break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
		case MSG_MOUSEMOVE:
			HandlePageProcXX((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_vol_turn_item, hWnd, message, wParam, lParam);
            break;
		case MSG_PAINT:
			return 0;
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			return 0;
		case MSG_KEYUP:
			switch (wParam)
			{
				case kKeyBcStop:
				case kKeyTaskIdle:
					VolBackToWelcome(hWnd);
					return 0;
				case kKeyCallStart:
					//这里改变老状态函数,目的是当设备设置自动接听,还未接听前
					//点击了音量按钮,返回会导致进入的不是最新的状态界面
					g_StauteFunOld = guicall_chatting_proc;
					break;
			}
			break;
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


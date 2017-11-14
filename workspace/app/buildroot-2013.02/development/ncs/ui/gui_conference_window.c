
#include "gui_conference_window.h"
#include "gui_call_chatting.h"
#include "gui_calloutcomming.h"
#include "guicominputdigtal.h"
#include "gui_call_incomming.h"
 
static TYPE_GUI_AREA_HANDLE *gui_callconfrence_ptr = NULL;
static int s_flag_t = 0;
static void gui_second_call_text(PRECT pRc, void* pData);
static void gui_first_call_text(PRECT pRc, void* pData);
static void guicall_first_end(PRECT pRc, void* pData);
static void guicall_first_func(HWND hWnd, int message, int wParam, int lParam);
static void guicall_conference_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guicall_videostreamer_display(PRECT pRc, void* pData);
int guicall_conference_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);


TYPE_GUI_AREA_HANDLE gui_conference_data_items[] =
{
	{0  , {(800-352)/2     , 60  , (800-352)/2+352     , 60+288                                     } , "call_videostreamer.png" , NULL                   , guicall_videostreamer_display , NULL                        } ,
	{1  , {30              , 140 , 30+150              , 140+20                                     } , NULL                     , NULL                   , gui_first_call_text           , NULL                        } ,
	{2  , {30              , 170 , 30+46               , 170+80                                     } , "call_accept.png"        , "call_accept_down.png" , guicall_first_end             , guicall_conference_btn_func } ,
	{3  , {120             , 170+CALL_END_Y_OFFSET , 120+46              , 170+80+CALL_END_Y_OFFSET } , "call_end.png"           , "call_end_down.png"    , guicall_first_end             , guicall_conference_btn_func } ,
	{4  , {(800-352)/2+362 , 140 , (800-352)/2+362+160 , 140+20                                     } , NULL                     , NULL                   , gui_second_call_text          , NULL                        } ,
	{5  , {(800-352)/2+362 , 170+CALL_END_Y_OFFSET , (800-352)/2+362+46  , 170+80+CALL_END_Y_OFFSET } , "call_end.png"           , "call_end_down.png"    , guicall_first_end             , guicall_conference_btn_func } ,
	{-1 , {0               , 0   , 0                   , 0                                          } , NULL                     , NULL                   , NULL                          , NULL                        } ,
};

TYPE_GUI_AREA_HANDLE gui_conference_data_itemsExxx[] =
{
	{0  , {20         , 50     , 20+600        , 50+420    } , "call_videostreamer.png" , NULL                   , guicall_videostreamer_display , NULL                        } , 
	{1  , {10+640     , 50     , 10+640+150    , 50+20     } , NULL                     , NULL                   , gui_first_call_text           , NULL                        } , 
	{2  , {10+640     , 80     , 650+46        , 80+80     } , "call_accept.png"        , "call_accept_down.png" , guicall_first_end             , guicall_conference_btn_func } , 
	{3  , {10+640+100 , 80     , 650+46+100+46 , 80+80     } , "call_end.png"           , "call_end_down.png"    , guicall_first_end             , guicall_conference_btn_func } , 
	{4  , {10+640     , 200-30 , 10+640+150    , 200-30+20 } , NULL                     , NULL                   , gui_second_call_text          , NULL                        } , 
	{5  , {10+640     , 200    , 650+46        , 200+80    } , "call_end.png"           , "call_end_down.png"    , guicall_first_end             , guicall_conference_btn_func } , 
	{-1 , {0          , 0      , 0             , 0         } , NULL                     , NULL                   , NULL                          , NULL                        } , 
};

static void gui_first_call_text(PRECT pRc, void* pData)
{
	char bmp_buf[128];
	char text_buf[128];
	char tmp_buf[128];
	SIZE sz;
	BITMAP bm, bm1;
	char *name;
	int len;
	task_t task = get_system_task();
	struct call_manager *call_manager = term->call_manager;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	call_t *call[2];

    if(call_manager->calls_total_get(call_manager) == 1)
    {
        return;
    }
	call_manager->nr_calls_get(call_manager, call, 2);

	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);


	name = ncs_manager->ncs_term_name_get_by_id(ncs_manager, call[0]->target_id);
	if (call[0]->started)
		get_str_from_id(TALKING_STR_ID, tmp_buf);
	else
		get_str_from_id(CALLING_STR_ID, tmp_buf);

	if (name)
	{
		char short_name[40] = {0};

		if (strlen(name))
		{
			if (strlen(name) > 16)
				strncpy(short_name, name, 16);
			else
				strcpy(short_name, name);
		}

		if (call[0]->board_id > 0)
			sprintf(text_buf, "%s-%d-%d %s", short_name, call[0]->target_id, call[0]->board_id, tmp_buf);
		else
			sprintf(text_buf, "%s-%d %s", short_name, call[0]->target_id, tmp_buf);
	}
	DrawText(system_info.hdc, text_buf, -1, pRc, DT_LEFT);
}

static void gui_call_update_text(HWND hWnd, int wParam)
{
	TYPE_GUI_AREA_HANDLE *p = NULL;

	p = &gui_callconfrence_ptr[1];
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    system_info.hdc = BeginPaint(hWnd);
    p->pReFlash(&p->rc, p->pData0);
    EndPaint(hWnd, system_info.hdc);

	p = &gui_callconfrence_ptr[4];
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    system_info.hdc = BeginPaint(hWnd);
    p->pReFlash(&p->rc, p->pData0);
    EndPaint(hWnd, system_info.hdc);
}

static void conference_drop_call(call_t *call_drop, call_t *call_ref)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;

	if (call_drop)
	{
		ncs_manager->ncs_call_stop(ncs_manager, call_drop, FALSE);
	}

	if (call_ref)
	{
		ncs_manager->ncs_video_request(ncs_manager, call_ref);

		if (call_ref->started)
		{
			if (call_ref->type == SDK_MONITOR || call_ref->type == MONITOR)
				set_system_task(TASK_NCS_MONITOR_OUTGOING);
			else
			{
				if (call_ref->dir == IN)
					set_system_task(TASK_NCS_CALL_INCOMING);
				else
					set_system_task(TASK_NCS_CALL_OUTGOING);
			}
            EnterWin(system_info.main_hwnd, guicall_chatting_proc);
		}
		else
		{
			if (call_ref->dir == IN)
			{
				set_system_task(TASK_NCS_CALL_INCOMING);
                EnterWin(system_info.main_hwnd, guicall_incomming_proc);
			}
			else
			{
				set_system_task(TASK_NCS_CALL_OUTGOING);
                EnterWin(system_info.main_hwnd, guicall_out_proc);
			}
		}
	}
}

static void gui_second_call_text(PRECT pRc, void* pData)
{
	char bmp_buf[128];
	char text_buf[128];
	char tmp_buf[128];
	SIZE sz;
	BITMAP bm, bm1;
	int len;
	task_t task = get_system_task();
	char *name;
	struct call_manager *call_manager = term->call_manager;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	call_t *call[2];
    if(call_manager->calls_total_get(call_manager) == 1)
    {
        return;
    }
	call_manager->nr_calls_get(call_manager, call, 2);

	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);


	name = ncs_manager->ncs_term_name_get_by_id(ncs_manager, call[1]->target_id);
	if (call[1]->started)
		get_str_from_id(TALKING_STR_ID, tmp_buf);
	else
		get_str_from_id(CALLING_STR_ID, tmp_buf);

	if (name)
	{
		char short_name[40] = {0};

		if (strlen(name))
		{
			if (strlen(name) > 16)
				strncpy(short_name, name, 16);
			else
				strcpy(short_name, name);
		}

		if (call[1]->board_id > 0)
			sprintf(text_buf, "%s-%d-%d %s", short_name, call[1]->target_id, call[1]->board_id, tmp_buf);
		else
			sprintf(text_buf, "%s-%d %s", short_name, call[1]->target_id, tmp_buf);
	}
	DrawText(system_info.hdc, text_buf, -1, pRc, DT_LEFT);
}

static void guicall_first_end(PRECT pRc, void* pData)
{
    BITMAP * bitmap = NULL;
    bitmap  = png_ha_res_find_by_name(pData);

    printf("pdata :%s\n", pData);
    if(bitmap == NULL)
    {
        SPON_LOG_ERR("file: %s load error\n", pData);
    }
    else
    {
        FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
    }
}

static void guicall_first_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
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
            break;
    }
}

static void guicall_conference_accept(HWND hWnd)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
    //下面是替换第３项的图片到第二项，将接听按钮去掉,做了一个静态标志
    gui_callconfrence_ptr[2].pData0 = gui_callconfrence_ptr[3].pData0;
    gui_callconfrence_ptr[2].pData1 = gui_callconfrence_ptr[3].pData1;

    //先清除３个区域
    p = &gui_callconfrence_ptr[1];
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    p = &gui_callconfrence_ptr[2];
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    p = &gui_callconfrence_ptr[3];
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新

    system_info.hdc = BeginPaint(hWnd);
    p = &gui_callconfrence_ptr[1];
    p->pReFlash(&p->rc, p->pData0);
    p = &gui_callconfrence_ptr[2];
    p->pReFlash(&p->rc, p->pData0);
    EndPaint(hWnd, system_info.hdc);
}

static void guicall_conference_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;
	struct call_manager *call_manager = term->call_manager;
	call_t *call[2];
    int i = 0;
    int ret = call_manager->nr_calls_get(call_manager, call, 2);
    if(ret < 0)
    {
        call[0] = NULL;
        call[1] = NULL;
    }
	
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
            if(p->id == 2)
            {
                if(!s_flag_t)
                {
                    for (i = 0; i < 2; i++)
                    {
                        if (call[i] && call[i]->dir == IN && !call[i]->started)
                        {
                            ncs_manager->ncs_call_start(ncs_manager, call[i], FALSE);
                        }
                    }
                    guicall_conference_accept(hWnd);
					s_flag_t = TRUE;
                }
                else
				{
					if(call[0])
					{
						conference_drop_call(call[0], call[1]);
					}
				}
            }
            else if(p->id == 3)
            {
                if(call[0])
                {
                    conference_drop_call(call[0], call[1]);
                }
            }
            else if(p->id == 5)
            {
                if(call[1])
                {
                    conference_drop_call(call[1], call[0]);
                }
            }
            else if(p->id == 6)
            {
                ncs_manager->ncs_call_stop_all(ncs_manager, FALSE);
                set_system_task(TASK_NONE);
                EnterWin(hWnd, WelComeWinProc);
            }
            break;
    }
}

static void guicall_videostreamer_display(PRECT pRc, void* pData)
{
	BITMAP * bitmap = NULL;

	struct call_manager *call_manager = term->call_manager;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;

	if (ncs_manager->ncs_video_recv_start(ncs_manager) == FALSE)
	{
		SPON_LOG_INFO("=================>>>>>>>>>>picture DISPLAY\n");
		bitmap	= png_ha_res_find_by_name(pData);
		if(bitmap == NULL)
		{
			SPON_LOG_ERR("file: %s load error\n", pData);
		}
		else
		{
			FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
		}
	}
	else
	{
		SPON_LOG_INFO("=================>>>>>>>>>>VIDEO DISPLAY\n");
		SetBrushColor(system_info.hdc, 0x12345678);//0x12345678
		FillBox(system_info.hdc, pRc->left, pRc->top, pRc->right-pRc->left, pRc->bottom-pRc->top);
	}
}

static void gui_call_incomming_disp_screen_func(HWND hWnd, int wParam)
{
	TYPE_GUI_AREA_HANDLE *p = NULL;

	p = &gui_callconfrence_ptr[0];

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    system_info.hdc = BeginPaint(hWnd);
    p->pReFlash(&p->rc, p->pData0);
    EndPaint(hWnd, system_info.hdc);
}

int guicall_conference_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;
	task_t task = get_system_task();
    call_t *tmp = NULL;
    char Buf0[30] = {"call_accept.png"};
    char Buf1[30] = {"call_accept_down.png"};
    RECT rc= { 0, 0, 800, 480};

	if(g_firstEnter)
	{
		SPON_LOG_INFO("===============> guicall_conference_proc \n");
		g_firstEnter = FALSE;
        if(GetVideo_fromMedia() >= 2)
        {
            gui_callconfrence_ptr = gui_conference_data_itemsExxx;
        }
        else
        {
            gui_callconfrence_ptr = gui_conference_data_items;
        }
        //这里是配合后面修改了图片之后改回来, 防止第二次进入三方对讲出问题
        /*gui_callconfrence_ptr[2].pData0 = Buf0;*/
        /*gui_callconfrence_ptr[2].pData1 = Buf1;*/
        gui_callconfrence_ptr[2].pData0 = "call_accept.png";
        gui_callconfrence_ptr[2].pData1 = "call_accept_down.png";
	}
	
	switch(message)
    {
		case MSG_CREATE:
            s_flag_t = 0;
            HandleCreate(gui_callconfrence_ptr, hWnd);
            //这个时候如果是主动发起三方对讲，直接显示拒绝，不需要再显示接听按钮
            if(task == TASK_NCS_CONFERENCE)
            {
                guicall_conference_accept(hWnd);
				s_flag_t = TRUE;
            }
            break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
            HandlePageProc(gui_callconfrence_ptr, hWnd, message, wParam, lParam);
			break;
		case MSG_PAINT:
			return 0;
		case MSG_NCS_CHATTING_REFRESH:
            if(GetVideo_fromMedia() >= 2)
            {
                gui_callconfrence_ptr = gui_conference_data_itemsExxx;
                //这里是配合后面修改了图片之后改回来, 防止第二次进入三方对讲出问题
                gui_callconfrence_ptr[2].pData0 = "call_accept.png";
                gui_callconfrence_ptr[2].pData1 = "call_accept_down.png";

                InvalidateRect_Ex(hWnd, &rc, TRUE);
                gui_callconfrence_ptr = (TYPE_GUI_AREA_HANDLE*)gui_conference_data_itemsExxx;
                HandleCreate(gui_callconfrence_ptr, hWnd);
                if(task == TASK_NCS_CONFERENCE)
                {
                    guicall_conference_accept(hWnd);
					s_flag_t = TRUE;
                }
            }
            else
            {
                gui_callconfrence_ptr = gui_conference_data_items;
                //这里是配合后面修改了图片之后改回来, 防止第二次进入三方对讲出问题
                gui_callconfrence_ptr[2].pData0 = "call_accept.png";
                gui_callconfrence_ptr[2].pData1 = "call_accept_down.png";
                gui_call_incomming_disp_screen_func(hWnd, wParam);
            }
            break;
        case MSG_ERASEBKGND:
            bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
            FillBoxWithBitmap((HDC)wParam, 0,0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
            return 0;
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			return 0;
		case MSG_KEYUP:
            switch(wParam)
            {
                case kKeyCallStart:
                    gui_call_update_text(hWnd, wParam);
					guicall_conference_accept(hWnd);
					s_flag_t = TRUE;
                    break;
                /*case kKeyCallStop:*/
                /*case kKeyCallBusy:*/
                /*case kKeyCallReject:*/
                case kKeyIpRequestFailed:
                    SPON_LOG_INFO("=====>%s\n", __func__);
                    tmp = term->call_manager->current_call_get(term->call_manager);
                    if(tmp->dir == IN)
                    {
                        set_system_task(TASK_NCS_CALL_INCOMING);
                    }
                    else
                    {
                        set_system_task(TASK_NCS_CALL_OUTGOING);
                    }
                    EnterWin(hWnd, guicall_chatting_proc);
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


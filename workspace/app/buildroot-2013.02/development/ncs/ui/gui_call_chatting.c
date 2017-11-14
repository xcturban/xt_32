
#include "gui_call_chatting.h"
#include "gui_conference_window.h"

static void guicall_videostreamer_display(PRECT pRc, void* pData);
static void guicall_chatting_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guicall_monitor_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guicall_target_info_display(PRECT pRc, void* pData);

#define GUICHAT_START_X		200

static TYPE_GUI_AREA_HANDLE *gui_callchat_ptr = NULL;

const TYPE_GUI_AREA_HANDLE guichat_data_items[] =
{
	{0  , {(800-352)/2           , 60  , (800-352)/2+352       , 60+288 } , "call_videostreamer.png" , NULL                       , guicall_videostreamer_display , NULL                      } , //gui_call_chatting_disp_screen_func
	{1  , {GUICHAT_START_X       , 387 , GUICHAT_START_X+114   , 387+93 } , "door_open_up.png"       , "door_open_down.png"       , guicall_button_display        , guicall_chatting_btn_func } ,
	{2  , {GUICHAT_START_X+114   , 387+CALL_END_Y_OFFSET , GUICHAT_START_X+114*2 , 387+93+CALL_END_Y_OFFSET } , "call_forward.png"       , "call_forward_down.png"    , guicall_button_display        , guicall_chatting_btn_func } ,
	{3  , {GUICHAT_START_X+114*2 , 387 , GUICHAT_START_X+114*3 , 387+93 } , "call_conference.png"    , "call_conference_down.png" , guicall_button_display        , guicall_chatting_btn_func } ,
	{4  , {GUICHAT_START_X+114*3 , 387 , GUICHAT_START_X+114*4 , 387+93 } , "call_end.png"           , "call_end_down.png"        , guicall_button_display        , guicall_chatting_btn_func } ,
	{5  , {0                     , 36  , 800                   , 60     } , NULL                     , NULL                       , guicall_target_info_display   , NULL                      } ,
	{-1 , {0                     , 0   , 0                     , 0      } , NULL                     , NULL                       , NULL                          , NULL                      } ,
};

const TYPE_GUI_AREA_HANDLE guichat_data_itemsExxx[] =
{
	{0  , {20     , 50              , 20+600 , 50+420        } , "call_videostreamer.png" , NULL                       , guicall_videostreamer_display , NULL                      } , //gui_call_chatting_disp_screen_func
	{1  , {20+640 , 50              , 660+46 , 50+80         } , "door_open_up.png"       , "door_open_down.png"       , guicall_button_display        , guicall_chatting_btn_func } ,
	{2  , {20+640 , 50+80+30        , 660+46 , 50+80+30+80   } , "call_forward.png"       , "call_forward_down.png"    , guicall_button_display        , guicall_chatting_btn_func } ,
	{3  , {20+640 , 50+80+30*2+80   , 660+46 , 50+80+60+80*2 } , "call_conference.png"    , "call_conference_down.png" , guicall_button_display        , guicall_chatting_btn_func } ,
	{4  , {20+640 , 50+80+30*3+80*2 , 660+46 , 50+80+60+80*3 } , "call_end.png"           , "call_end_down.png"        , guicall_button_display        , guicall_chatting_btn_func } ,
	{5  , {0      , 36              , 800    , 60            } , NULL                     , NULL                       , guicall_target_info_display   , NULL                      } ,
	{-1 , {0      , 0               , 0      , 0             } , NULL                     , NULL                       , NULL                          , NULL                      } ,
};

const TYPE_GUI_AREA_HANDLE guimonitor_data_items[] =
{
	{0  , {(800-352)/2 , 60  , (800-352)/2+352 , 60+288 } , "call_videostreamer.png" , NULL                , guicall_videostreamer_display , NULL                     } , //gui_call_chatting_disp_screen_func
	{1  , {(800-50)/2  , 387 , (800+50)/2      , 387+93 } , "call_end.png"           , "call_end_down.png" , guicall_button_display        , guicall_monitor_btn_func } ,
	{2  , {0           , 36  , 800             , 60     } , NULL                     , NULL                , guicall_target_info_display   , NULL                     } ,
	{-1 , {0           , 0   , 0               , 0      } , NULL                     , NULL                , NULL                          , NULL                     } ,
};

const TYPE_GUI_AREA_HANDLE guimonitor_data_itemsExxx[] =
{
	{0  , {20     , 50              , 20+600 , 50+420        } , "call_videostreamer.png" , NULL                       , guicall_videostreamer_display , NULL                      } , //gui_call_chatting_disp_screen_func
	{1  , {20+640 , 50              , 660+46 , 50+80         } , "call_end.png"           , "call_end_down.png" , guicall_button_display        , guicall_monitor_btn_func } ,
	{2  , {0           , 36  , 800             , 60     } , NULL                     , NULL                , guicall_target_info_display   , NULL                     } ,
	{-1 , {0           , 0   , 0               , 0      } , NULL                     , NULL                , NULL                          , NULL                     } ,
};

static void guicall_handup(HWND hWnd)
{
	task_t task = get_system_task();
    struct call_manager *call_manager = term->call_manager;
	call_t *call = call_manager->current_call_get(call_manager);
    struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;
	

	if (task == TASK_SIP_CALL_INCOMING || task == TASK_SIP_CALL_OUTGOING)
	{
		term->sip_manager->call_stop();
	}
	else
	{
		if (call)
		{
			ncs_manager->ncs_call_stop(ncs_manager, call, FALSE);
		}
	}
	SPON_LOG_INFO("task %d\n", task);

	WEB_SYS_LOG(USRLOG_TALKEND);
	
	if(term->calllog_flag == 0 && (task == TASK_NCS_MONITOR_OUTGOING || task == TASK_SIP_CALL_OUTGOING ||  task == TASK_NCS_CALL_OUTGOING))
	{
		SPON_LOG_INFO("=======>>>55\n");
		InputDigtal_NewScreen(hWnd);
		SPON_LOG_INFO("=======>>>57\n");
	}
	else
	{
		set_system_task(TASK_NONE);
		EnterWin(hWnd, WelComeWinProc);
	}
	SPON_LOG_INFO("=======>>>64\n");
}

static void guicall_target_info_display(PRECT pRc, void* pData)
{
	char buf[128] = {0};
	char tmp_buf[128];
	task_t task = get_system_task();
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct call_manager *call_manager = term->call_manager;
    call_t *call = call_manager->current_call_get(call_manager);
	char *name;
	
	if(call == NULL)
	{
		return;
	}

	name = ncs_manager->ncs_term_name_get_by_id(ncs_manager, call->target_id);
	
	if(term->customer_type == CUSTOMER_LNJY)
	{
		if(strlen(ncs_kfwqName_get()))
		{
			name = ncs_kfwqName_get();
		}
	}
	if (task == TASK_NCS_CALL_INCOMING || task == TASK_NCS_CALL_OUTGOING)
	{
		get_str_from_id(TALKING_STR_ID, tmp_buf);
		if(name)
		{
			if (call->board_id > 0)
				sprintf(buf, "%s-%s-%d-%d",tmp_buf, name, call->target_id, call->board_id);
			else
				sprintf(buf, "%s-%s-%d", tmp_buf, name, call->target_id%1000);
		}
		else
		{
			if (call->board_id > 0)
				sprintf(buf, "%d-%d %s", call->target_id, call->board_id, tmp_buf);
			else
				sprintf(buf, "%d %s", call->target_id, tmp_buf);
		}
		
	}
	else if (task == TASK_NCS_MONITOR_OUTGOING)
	{
		get_str_from_id(MONITING_STR_ID, tmp_buf);
		if (name)
		{
			if (call->board_id > 0)
				sprintf(buf, "%s-%d-%d %s", name, call->target_id, call->board_id, tmp_buf);
			else
				sprintf(buf, "%s-%d %s", name, call->target_id, tmp_buf);
		}
		else
		{
			if (call->board_id > 0)
				sprintf(buf, "%d-%d %s", call->target_id, call->board_id, tmp_buf);
			else
				sprintf(buf, "%d %s", call->target_id, tmp_buf);
		}
	}
	
	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	DrawText(system_info.hdc, buf, -1, pRc, DT_CENTER);
}

static void guicall_chatting_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;
	struct call_manager *call_manager = term->call_manager;
	call_t *call = call_manager->current_call_get(call_manager);

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
	if(message == MSG_LBUTTONDOWN || message ==MSG_LBUTTONUP )
	{
		SPON_LOG_INFO("message == down up\n");
	}
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
			if(p->id == 1)
			{
				if (task == TASK_NCS_CALL_INCOMING || task == TASK_NCS_CALL_OUTGOING)
				{
					if (call)
					{
						ncs_manager->ncs_term_status_ctl(ncs_manager, call, 0, 0, 1, 5);
						ncs_manager->ncs_term_status_ctl(ncs_manager, call, 0, 1, 1, 5);
					}
				}
			}
			else if(p->id == 4)
			{
				guicall_handup(hWnd);
			}
			else if(p->id == 2)
			{
				set_system_task(TASK_NCS_CALL_TRANSFER);
				InputDigtal_NewScreen(hWnd);
			}
			else if(p->id == 3)
			{
				set_system_task(TASK_NCS_CONFERENCE);
				InputDigtal_NewScreen(hWnd);
			}
            break;
    }
}

static void guicall_monitor_btn_func(HWND hWnd, int message, int wParam, int lParam)
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
			if(p->id == 1)
			{
                guicall_handup(hWnd);
			}
            break;
    }
}
static void guicall_button_display(PRECT pRc, void* pData)
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

static void guicall_info_display(PRECT pRc, void* pData)
{
	char buf[128] = {0};

    sprintf(buf, "通话中\n");
	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	DrawText(system_info.hdc, buf, -1, pRc, DT_CENTER);
}

static void gui_call_chatting_disp_screen_func(HWND hWnd, int wParam)
{
	TYPE_GUI_AREA_HANDLE *p = NULL;
	/*task_t task = get_system_task();*/
	
	/*if(task == TASK_NCS_MONITOR_OUTGOING)*/
	/*{*/
		/*p = &guimonitor_data_items[wParam];*/
	/*}*/
	/*else*/
	/*{*/
		/*p = &guichat_data_items[wParam];*/
	/*}*/
    p = &gui_callchat_ptr[wParam];

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    system_info.hdc = BeginPaint(hWnd);
    p->pReFlash(&p->rc, p->pData0);
    EndPaint(hWnd, system_info.hdc);
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

int guicall_chatting_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;
    RECT rc= { 0, 0, 800, 480 };
	task_t task = get_system_task();

	if(g_firstEnter)
	{
		SPON_LOG_INFO("===============> guicall_chatting_proc \n");
		g_firstEnter = FALSE;
	}
	
	switch(message)
    {
		case MSG_CREATE:
            if(GetVideo_fromMedia() >= 2)
            {
                if(task == TASK_NCS_MONITOR_OUTGOING)
                {
                    gui_callchat_ptr =(TYPE_GUI_AREA_HANDLE*)guimonitor_data_itemsExxx; 
                }
                else
                {
                    gui_callchat_ptr =(TYPE_GUI_AREA_HANDLE*)guichat_data_itemsExxx; 
                }
            }
            else
            {
				WEB_SYS_LOG(USRLOG_TALKSTART);
                if(task == TASK_NCS_MONITOR_OUTGOING)
                {
                    gui_callchat_ptr =(TYPE_GUI_AREA_HANDLE*)guimonitor_data_items; 
                }
                else
                {
                    gui_callchat_ptr =(TYPE_GUI_AREA_HANDLE*)guichat_data_items; 
                }
            }
            HandleCreate(gui_callchat_ptr, hWnd);
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			pthread_mutex_lock(&term->mutex);
            HandlePageProc(gui_callchat_ptr, hWnd, message, wParam, lParam);
			pthread_mutex_unlock(&term->mutex);
			break;
		case MSG_NCS_CHATTING_REFRESH:
            if(GetVideo_fromMedia() >= 2)
            {
                InvalidateRect_Ex(hWnd, &rc, TRUE);
                if(task == TASK_NCS_MONITOR_OUTGOING)
                {
                    gui_callchat_ptr =(TYPE_GUI_AREA_HANDLE*)guimonitor_data_itemsExxx; 
                }
                else
                {
                    gui_callchat_ptr =(TYPE_GUI_AREA_HANDLE*)guichat_data_itemsExxx; 
                }
                HandleCreate(gui_callchat_ptr, hWnd);
            }
            else
            {
                gui_call_chatting_disp_screen_func(hWnd, wParam);
            }
			break;
		case MSG_PAINT:
			return 0;
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
				case kKeyCallStop:
				case kKeyCallBusy:
				case kKeyCallReject:
				//case kKeyIpRequestFailed:
					if(term->calllog_flag == 0 && (task == TASK_NCS_MONITOR_OUTGOING || task == TASK_SIP_CALL_OUTGOING ||  task == TASK_NCS_CALL_OUTGOING))
					{
						InputDigtal_NewScreen(hWnd);
					}
					else
					{
						//audio_channel_mode_set(AUDIO_CHANNEL_IDLE);
						set_system_task(TASK_NONE);
						EnterWin(hWnd, WelComeWinProc);
					}
					break;
                case kKeyIpRequestFailed:
					SPON_LOG_INFO("recieve kKeyIpRequestFailed\n");
					if(term->calllog_flag == 0)
					{
						InputDigtal_NewScreen(hWnd);
					}
					else
					{
						//audio_channel_mode_set(AUDIO_CHANNEL_IDLE);
						set_system_task(TASK_NONE);
						EnterWin(hWnd, WelComeWinProc);
					}
					break;
				case kKeyCallIn:
					SPON_LOG_INFO("recieve kKeyCallIn\n");
					EnterWin(hWnd, guicall_conference_proc);
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

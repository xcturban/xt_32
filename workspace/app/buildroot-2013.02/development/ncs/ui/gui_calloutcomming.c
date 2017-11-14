
#include "gui_calloutcomming.h"
#include "guicominputdigtal.h"
#include "gui_call_chatting.h"


static void gui_call_outgoing_target_info_display(PRECT pRc, void* pData);


const TYPE_GUI_AREA_HANDLE guioutgoingcall_data_items[] =
{
    {0  , {(800-352)/2       , 60  , (800-352)/2+352      , 60+288 } , "call_videostreamer.png" , NULL                , guicall_videostreamer_display         , NULL             } ,
    {1  , {LCD_WIDTH/2-114/2 , 390 , LCD_WIDTH/2-114/2+80 , 390+90 } , "call_end.png"           , "call_end_down.png" , guicall_button_display                , guicall_btn_func } ,
    {2  , {0                 , 36  , 800                  , 60     } , NULL                     , NULL                , gui_call_outgoing_target_info_display , NULL             } ,
    {-1 , {0                 , 0   , 0                    , 0      } , NULL                     , NULL                , NULL                                  , NULL             } ,
};

const TYPE_GUI_AREA_HANDLE guioutgoingcall_data_itemsExxx[] =
{
    {0  , {20       , 50  , 20+600      , 50+420               } , "call_videostreamer.png" , NULL                , guicall_videostreamer_display         , NULL             } ,
    {1  , {20+640 , 50, 660+46 , 50+80                         } , "call_end.png"           , "call_end_down.png" , guicall_button_display                , guicall_btn_func } ,
    {2  , {0                 , 36  , 800                  , 60 } , NULL                     , NULL                , gui_call_outgoing_target_info_display , NULL             } ,
    {-1 , {0                 , 0   , 0                    , 0  } , NULL                     , NULL                , NULL                                  , NULL             } ,
};

static TYPE_GUI_AREA_HANDLE *gui_callout_ptr = NULL;

static void gui_call_outgoing_target_info_display(PRECT pRc, void* pData)
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
	get_str_from_id(CALLING_STR_ID, tmp_buf);
	if(term->customer_type == CUSTOMER_LNJY)
	{
		if(strlen(ncs_kfwqName_get()))
		{
			name = ncs_kfwqName_get();
		}
	}
	if(name)
	{
		if (call->board_id > 0)
			sprintf(buf, "%s-%s-%d %d",tmp_buf, name, call->target_id, call->board_id);
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
		

	
	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	DrawText(system_info.hdc, buf, -1, pRc, DT_CENTER);
}

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
	if(term->calllog_flag == 0 && (task == TASK_NCS_MONITOR_OUTGOING || task == TASK_SIP_CALL_OUTGOING ||  task == TASK_NCS_CALL_OUTGOING))
	{
		InputDigtal_NewScreen(hWnd);
	}
	else
	{
		set_system_task(TASK_NONE);
		EnterWin(hWnd, WelComeWinProc);
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

static void guicall_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;
	struct call_manager *call_manager = term->call_manager;
	call_t *call = call_manager->current_call_get(call_manager);
	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE);
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
				/*if (task == TASK_NCS_CALL_INCOMING) */
				/*{*/
					/*if (call) */
					/*{*/
						/*[>guicall_incomingcall_answer_proc(hWnd, wParam);<]*/
						/*EnterWin(hWnd, guicall_chatting_proc);*/
						/*ncs_manager->ncs_call_start(ncs_manager, call, FALSE);*/
						/*if (term->customer_type == CUSTOMER_JXWGS)*/
						/*{*/
							/*close_emergency_alarm(call->target_id, 0);*/
						/*}*/
					/*}*/
				/*}*/
				/*else if (task == TASK_SIP_CALL_INCOMING) */
				/*{*/
					/*term->sip_manager->call_start();*/
				/*}*/
				/*else if(p->id == 1)*/
				{
					guicall_handup(hWnd);
				}
			}
            break;
    }
}

static void gui_call_outgoing_disp_screen_func(HWND hWnd, int wParam)
{
	TYPE_GUI_AREA_HANDLE *p = NULL;

	p = &gui_callout_ptr[wParam];

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //ÏÈÇå³ýÇøÓò, ÔÙË¢ÐÂ
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

int guicall_out_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;
	task_t task = get_system_task();
    RECT rc= { 0, 0, 800, 480 };
	
	if(g_firstEnter)
	{
		SPON_LOG_INFO("===============> Enter guicall_out_proc\n");
		g_firstEnter = FALSE;
	}
	
	switch(message)
    {
        case MSG_CREATE:
            gui_callout_ptr = (TYPE_GUI_AREA_HANDLE*)guioutgoingcall_data_items;
            HandleCreate(gui_callout_ptr, hWnd);
            break;
		case MSG_NCS_CHATTING_REFRESH:
            if(GetVideo_fromMedia() >= 2)
            {
                InvalidateRect_Ex(hWnd, &rc, TRUE);
                gui_callout_ptr = (TYPE_GUI_AREA_HANDLE*)guioutgoingcall_data_itemsExxx;
                HandleCreate(gui_callout_ptr, hWnd);
            }
            else
            {
                gui_call_outgoing_disp_screen_func(hWnd, wParam);
            }
            break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
			pthread_mutex_lock(&term->mutex);
            HandlePageProc(gui_callout_ptr, hWnd, message, wParam, lParam);
			pthread_mutex_unlock(&term->mutex);
            break;
        case MSG_PAINT:
            return 0;
        case MSG_ERASEBKGND:
            bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
            FillBoxWithBitmap((HDC)wParam, 0, 0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
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
					if(term->calllog_flag == 0 && (task == TASK_NCS_MONITOR_OUTGOING || task == TASK_SIP_CALL_OUTGOING ||  task == TASK_NCS_CALL_OUTGOING))
					{
						InputDigtal_NewScreen(hWnd);
					}
					else					
					{
	                    set_system_task(TASK_NONE);
	                    EnterWin(hWnd, WelComeWinProc);
					}
                    return 0;
                case kKeyIpRequestFailed:
					if(term->calllog_flag == 0)
					{
						InputDigtal_NewScreen(hWnd);
					}
					else
					{
	                    set_system_task(TASK_NONE);
	                    EnterWin(hWnd, WelComeWinProc);
					}
                    return 0;
                case kKeyCallStart:
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

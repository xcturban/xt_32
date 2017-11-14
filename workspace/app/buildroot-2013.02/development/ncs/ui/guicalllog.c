#include  "guicalllog.h"
#include "gui_calloutcomming.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guicalllog_button_display(PRECT pRc, void* pData);
static void guicalllog_context_display(PRECT pRc, void* pData);
static void guicalllog_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guicalllog_back_func(HWND hWnd, int message, int wParam, int lParam);

static void guicalllog_info_func(HWND hWnd, int message, int wParam, int lParam);

static int guicalllog_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static GUICALLLOG g_guicalllog_ctrl;
static GUICALLLOG *calllog_ptr = &g_guicalllog_ctrl;
void guicalllog_set_callback(GUICALLLOG *ptr, calllog_item item)
{
	if(item == CALLLOG_MISSED)
	{
		ptr->del_callback = missed_call_del;
		ptr->get_id_callback = missed_call_id_get;
		ptr->get_count_callback = missed_call_count_get;	
	}
	else if(item == CALLLOG_DIALED)
	{
		ptr->del_callback = NULL;
		ptr->get_id_callback = dialed_call_id_get;
		ptr->get_count_callback = dialed_call_count_get;	
	}
	else if(item == CALLLOG_RECEIVED)
	{
		ptr->del_callback = NULL;
		ptr->get_id_callback = received_call_id_get;
		ptr->get_count_callback = received_call_count_get;	
	}
	
	ptr->item = item;
	ptr->totalNum = ptr->get_count_callback();
}

void guicalllog_newscreen(HWND hWnd)
{
	calllog_ptr->select_idx = CALLLOG_UNVALID_ID;
	term->calllog_flag = 1;
	guicalllog_set_callback(calllog_ptr, CALLLOG_MISSED);

	EnterWin(hWnd, guicalllog_screen_proc);
}


const TYPE_GUI_AREA_HANDLE guicalllog_data_items[] =
{
    {0   , {CALLLOG_TAB_L                 , CALLLOG_TAB_T  , CALLLOG_TAB_L+CALLLOG_TAB_W   , CALLLOG_TAB_B  } , "missedcall_up.png"       ,  "missedcall_down.png",  guicalllog_button_display  , guicalllog_btn_func  } ,
    {1   , {CALLLOG_TAB_L+CALLLOG_TAB_W   , CALLLOG_TAB_T  , CALLLOG_TAB_L+CALLLOG_TAB_W*2 , CALLLOG_TAB_B  } , "dialedcall_down.png"     , "dialedcall_up.png"     , guicalllog_button_display  , guicalllog_btn_func  } ,
    {2   , {CALLLOG_TAB_L+CALLLOG_TAB_W*2 , CALLLOG_TAB_T  , CALLLOG_TAB_L+CALLLOG_TAB_W*3 , CALLLOG_TAB_B  } , "receivedcall_down.png"   , "receivedcall_up.png"   , guicalllog_button_display  , guicalllog_btn_func  } ,
    {3   , {BTN_BACK_L                    , BTN_BACK_T     , BTN_BACK_R                    , BTN_BACK_B     } , BTN_BACK_UP             , BTN_BACK_DOWN             , guicalllog_button_display  , guicalllog_back_func } ,
    {4   , {CALLLOG_INFO_L                , CALLLOG_INFO_T , CALLLOG_INFO_R                , CALLLOG_INFO_B } , "calllog_context.png"   , NULL                      , guicalllog_context_display , guicalllog_info_func } ,
    {-1  , {0                             , 0              , 0                             , 0              } , NULL                    , NULL                      , NULL                       , NULL                 } ,
};

static int guicalllog_select_id(int x, int y)
{
	int idx = CALLLOG_UNVALID_ID,i = 0;
	int total = calllog_ptr->totalNum;

	for(;i<total;i++)
	{
		if(y > (i*CALLLOG_LINE_OFFSET+CALLLOG_INFO_T+CALLLOG_INFO_OFFSET) && y <= ((i+1)*CALLLOG_LINE_OFFSET+CALLLOG_INFO_T+CALLLOG_INFO_OFFSET))
		{
			idx = i;
			break;
		}
	}
	
	//SPON_LOG_INFO("guicalllog_select_id %d\n",idx);
	return idx;
}
static void guicalllog_call_outgoing(HWND hWnd, int idx)
{
	int target_id;
	int board_id;
	int total = calllog_ptr->totalNum;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	
	if(idx != CALLLOG_UNVALID_ID && total != 0)
	{
		//missed_call_id_get(idx, &target_id, &board_id);
		if(calllog_ptr->get_id_callback)
		{
			calllog_ptr->get_id_callback(total- idx, &target_id, &board_id);
		}

		if(calllog_ptr->del_callback)
		{
			calllog_ptr->del_callback(idx);
		}
		//missed_call_del(idx);
		if (target_id > 0)
		{
			int ret = ncs_manager->ncs_ip_request(ncs_manager, target_id, board_id, TALK);
			
			SPON_LOG_INFO("guicalllog_call_outgoing %d\n",ret);
			if (ret >= 0)
			{
				set_system_task(TASK_NCS_CALL_OUTGOING);			
				EnterWin(hWnd, guicall_out_proc);
			}
		}
	}
}

static void guicalllog_return(HWND hWnd)
{
	SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_IDLE_SCREEN_ID);
}

static void guicalllog_info_func(HWND hWnd, int message, int wParam, int lParam)
{
	int idx = CALLLOG_UNVALID_ID;
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	
    switch(message)
    {
        case MSG_LBUTTONDOWN:
			if(calllog_ptr->select_idx == CALLLOG_UNVALID_ID)
			{
				idx = guicalllog_select_id(LOWORD(lParam), HIWORD (lParam));
				if(idx != CALLLOG_UNVALID_ID)
				{
					calllog_ptr->select_idx= idx;
				}
			}
            break;
        case MSG_LBUTTONUP:
			if(calllog_ptr->select_idx != CALLLOG_UNVALID_ID)
			{
				//InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
	            system_info.hdc = BeginPaint(hWnd);
	            p->pReFlash(&p->rc, p->pData0);
	            EndPaint(hWnd, system_info.hdc);
				guicalllog_call_outgoing(hWnd, calllog_ptr->select_idx);
				calllog_ptr->select_idx = CALLLOG_UNVALID_ID;
			}
            break;
    }

}

static void guicalllog_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_HANDLE *pre = &guicalllog_data_items[calllog_ptr->item];
    
    switch(message)
    {
        case MSG_LBUTTONDOWN:
			if(calllog_ptr->item != p->id)
			{
				InvalidateRect_Ex(hWnd, &pre->rc, TRUE); 
	            system_info.hdc = BeginPaint(hWnd);
	            p->pReFlash(&pre->rc, (pre->id == 0 ? pre->pData1 : pre->pData0));
	            EndPaint(hWnd, system_info.hdc);
				
				InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
	            system_info.hdc = BeginPaint(hWnd);
	            p->pReFlash(&p->rc, (p->id == 0 ? p->pData0 : p->pData1));
	            EndPaint(hWnd, system_info.hdc);
				calllog_ptr->item = p->id + CALLLOG_MISSED;

				guicalllog_set_callback(calllog_ptr, calllog_ptr->item);
				SendNotifyMessage(hWnd, MSG_NCS_INPUT_DIGTAL_PAGE, 0, 0);
			}
            break;
        case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, (p->id == 0 ? p->pData0 : p->pData1));
            EndPaint(hWnd, system_info.hdc);
			//SendNotifyMessage(hWnd, MSG_NCS_INPUT_DIGTAL_PAGE, 0, 0);
            break;
    }
}

static void guicalllog_back_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

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
			guicalllog_return(hWnd);
            break;
    }
}
static void guicalllog_button_display(PRECT pRc, void* pData)
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
static void calllog_list_empty_window(HDC hdc,PRECT pRc)
{
	RECT rect;
	char empty[][32] = {"missedcall_empty.png", "dialedcall_empty.png" ,"receivedcall_empty.png"};

	rect.top = pRc->top+110;
	rect.left= pRc->left+190;
	rect.bottom = rect.top+30;
	rect.right= rect.left+172;

	guicalllog_button_display(&rect, empty[calllog_ptr->item]);

	return 0;
}


static void guicalllog_context_display(PRECT pRc, void* pData)
{
	char text_buf[128];
	int step = CALLLOG_LINE_OFFSET;
	int y_start = pRc->top + CALLLOG_INFO_OFFSET;
	int text_height = GetFontHeight(system_info.hdc);
	int i;
	char *name;
	LpConfig *cfg;
	char section[128];
	int total = calllog_ptr->totalNum;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	
	char total_str[][32] = {"missed_call_total","dialed_call_total","received_call_total"};
	char filename[][32] = {INI_MISSED_CALL_FILE,INI_DIALED_CALL_FILE,INI_RECEIVED_CALL_FILE};
	char section_str[][32] = {"missed_call_","dialed_call_","received_call_"};
	
	guicalllog_button_display(pRc, pData);
	
	if (total == 0) {
		calllog_list_empty_window(system_info.hdc,pRc);
		return 0;
	}

	//SetPenColor(system_info.hdc, COLOR_lightwhite);
	//SetTextColor(system_info.hdc, COLOR_lightwhite);
	cfg = lp_config_new(filename[calllog_ptr->item]);

	if (cfg == NULL)
	{
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	total = lp_config_get_int(cfg, total_str[calllog_ptr->item], "total", 0);

	for (i = 0; i < total; i++)
	{
		int target_id, board_id;
		char *date;

		//sprintf(section, "missed_call_%d", total - i);
		sprintf(section, "%s%d", section_str[calllog_ptr->item], total - i);		
		target_id = lp_config_get_int(cfg, section, "target_id", 0);
		board_id = lp_config_get_int(cfg, section, "board_id", 0);
		date = lp_config_get_string(cfg, section, "date", NULL);
		
		if (target_id > 0)
		{
			name = ncs_manager->ncs_term_name_get_by_id(ncs_manager, target_id);
			if (name)
			{
				if (board_id > 0)
					sprintf(text_buf, "ID:%s - %d - %d   Date:%s", name, target_id, board_id, date);
				else
					sprintf(text_buf, "ID:%s - %d   Date:%s", name, target_id, date);
			}
			else
			{
				if (board_id > 0)
					sprintf(text_buf, "ID:%d - %d   Date:%s", target_id, board_id, date);
				else
					sprintf(text_buf, "ID:%d   Date:%s", target_id, date);
			}
			
			SetTextColor(system_info.hdc, COLOR_lightwhite);
			SetBkMode(system_info.hdc, BM_TRANSPARENT);
			//DrawText(system_info.hdc, text_buf, -1, &text_rect, DT_CENTER);
			TextOut(system_info.hdc, pRc->left+30, (step - text_height) / 2 + y_start, text_buf);
			//SPON_LOG_INFO("text_buf %s x %d y %d\n",text_buf,pRc->left+30, (step - text_height) / 2 + y_start);
		}


		if (i != MAX_CALLLOG_NUM-1)
		{
			y_start += step;
			SetPenColor(system_info.hdc, COLOR_lightwhite);
			MoveTo(system_info.hdc, pRc->left+7, y_start);
			LineTo(system_info.hdc, pRc->right-20, y_start);
		}
		
	}

	lp_config_destroy(cfg);

	return 0;
}

static void guicalllog_context_update(HWND hWnd)
{
	TYPE_GUI_AREA_HANDLE *p = &guicalllog_data_items[4];

	InvalidateRect_Ex(hWnd, &(p->rc), TRUE);
	system_info.hdc = BeginPaint(hWnd);
	p->pReFlash(&(p->rc), p->pData0);
	EndPaint(hWnd, system_info.hdc);
}

static int guicalllog_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;


	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
	}
	
	switch(message)
    {
        case MSG_CREATE:
            break;
		case MSG_TIMER:
			break;
		case MSG_NCS_INPUT_DIGTAL_PAGE:
			guicalllog_context_update(hWnd);
			break;			
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
			{
				TYPE_GUI_AREA_HANDLE* data_item;
				data_item = guicalllog_data_items;
				HandlePageProc((TYPE_GUI_AREA_HANDLE*)data_item, hWnd, message, wParam, lParam);
			}
            break;
		case MSG_PAINT:
			{
				TYPE_GUI_AREA_HANDLE* data_item;
				data_item = guicalllog_data_items;
				HandleCreate((TYPE_GUI_AREA_HANDLE*)data_item, hWnd);
			}			
			return 0;
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			return 0;
		case MSG_ERASEBKGND:
            bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
            FillBoxWithBitmap((HDC)wParam,0,0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
			return 0;
			break;
		case MSG_NCS_SWITCH_WND:
			EnterNewScreen(hWnd,(int)lParam);
			break;
		case MSG_DESTROY:
			return 0;
		default:
			break;
    }
	return 1;
}

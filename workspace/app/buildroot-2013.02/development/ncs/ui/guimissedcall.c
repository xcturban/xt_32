#include  "guimissedcall.h"
#include "gui_calloutcomming.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guimissedcall_button_display(PRECT pRc, void* pData);
static void guimissedcall_info_display(PRECT pRc, void* pData);
static void guimissedcall_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guimissedcall_info_func(HWND hWnd, int message, int wParam, int lParam);

static int guimissedcall_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static GUIMISSEDCALL g_guimissedcall_ctrl;

void guimissedcall_newscreen(HWND hWnd)
{
	g_guimissedcall_ctrl.select_idx = MISSED_CALL_UNVALID_ID;
	g_guimissedcall_ctrl.totalNum = missed_call_count_get();
	EnterWin(hWnd, guimissedcall_screen_proc);
}


const TYPE_GUI_AREA_HANDLE guimissedcall_data_items[] =
{
    //{0  , {0						, 40					, 143					, 4						}, "wjld_title.png"			, NULL                , guimissedcall_button_display	, NULL}                  , 
	{0	, {GUIMISSEDCALL_INFO_L 	, GUIMISSEDCALL_INFO_T	, GUIMISSEDCALL_INFO_R	, GUIMISSEDCALL_INFO_B	}, "missedcall_sub_bg.png"	, NULL, guimissedcall_info_display		, guimissedcall_info_func}, 
    {1  , {BTN_BACK_L, BTN_BACK_T ,	BTN_BACK_R,BTN_BACK_B 	}, BTN_BACK_UP		, BTN_BACK_DOWN, guimissedcall_button_display 	, guimissedcall_btn_func}, 
    {-1 , {0						, 0                  	, 0  					, 0						}, NULL           	, NULL                , NULL                        	, NULL}                  , 
};

static int guimissedcall_select_id(int x, int y)
{
	int idx = MISSED_CALL_UNVALID_ID,i = 0;
	int total = g_guimissedcall_ctrl.totalNum;

	for(;i<total;i++)
	{
		if(y > (i*MISSED_CALL_LINE_OFFSET+GUIMISSEDCALL_INFO_T+MISSED_CALL_INFO_OFFSET) && y <= ((i+1)*MISSED_CALL_LINE_OFFSET+GUIMISSEDCALL_INFO_T+MISSED_CALL_INFO_OFFSET))
		{
			idx = i;
			break;
		}
	}
	
	//SPON_LOG_INFO("guimissedcall_select_id %d\n",idx);
	return idx;
}
static void guimissedcall_call_outgoing(HWND hWnd, int idx)
{
	int target_id;
	int board_id;
	int total = g_guimissedcall_ctrl.totalNum;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	
	if(idx != MISSED_CALL_UNVALID_ID && total != 0)
	{
		missed_call_id_get(idx, &target_id, &board_id);
		missed_call_del(idx);
		if (target_id > 0)
		{
			int ret = ncs_manager->ncs_ip_request(ncs_manager, target_id, board_id, TALK);
			
			SPON_LOG_INFO("guimissedcall_call_outgoing %d\n",ret);
			if (ret >= 0)
			{
				set_system_task(TASK_NCS_CALL_OUTGOING);			
				EnterWin(hWnd, guicall_out_proc);
			}
		}
	}
}

static void guimissedcall_return(HWND hWnd)
{
	//set_system_task(TASK_NONE);
	//EnterWin(hWnd, WelComeWinProc);
	SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_IDLE_SCREEN_ID);
}

static void guimissedcall_info_func(HWND hWnd, int message, int wParam, int lParam)
{
	int idx = MISSED_CALL_UNVALID_ID;
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	
    switch(message)
    {
        case MSG_LBUTTONDOWN:
			if(g_guimissedcall_ctrl.select_idx == MISSED_CALL_UNVALID_ID)
			{
				idx = guimissedcall_select_id(LOWORD(lParam), HIWORD (lParam));
				if(idx != MISSED_CALL_UNVALID_ID)
				{
					g_guimissedcall_ctrl.select_idx= idx;
				}
			}
            break;
        case MSG_LBUTTONUP:
			if(g_guimissedcall_ctrl.select_idx != MISSED_CALL_UNVALID_ID)
			{
				//InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
	            system_info.hdc = BeginPaint(hWnd);
	            p->pReFlash(&p->rc, p->pData0);
	            EndPaint(hWnd, system_info.hdc);
				guimissedcall_call_outgoing(hWnd, g_guimissedcall_ctrl.select_idx);
				g_guimissedcall_ctrl.select_idx = MISSED_CALL_UNVALID_ID;
			}
            break;
    }

}

static void guimissedcall_btn_func(HWND hWnd, int message, int wParam, int lParam)
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
			guimissedcall_return(hWnd);
            break;
    }
}
static void guimissedcall_button_display(PRECT pRc, void* pData)
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
static void missed_call_list_empty_window(HDC hdc,PRECT pRc)
{
	RECT rect;

	rect.top = pRc->top+190;
	rect.left= pRc->left+190;
	rect.bottom = rect.top+30;
	rect.right= rect.left+172;
	
	guimissedcall_button_display(&rect, "missedcall_empty.png");
	
	return 0;
}


static void guimissedcall_info_display(PRECT pRc, void* pData)
{
	char text_buf[128];
	int margin = system_info.margin;
	int step = MISSED_CALL_LINE_OFFSET;
	int round_size = system_info.round_size;
	int y_start = pRc->top + MISSED_CALL_INFO_OFFSET;

	int text_height = GetFontHeight(system_info.hdc);
	int i;
	char *name;
	LpConfig *cfg;
	char section[128];
	int total = g_guimissedcall_ctrl.totalNum;
	struct ncs_manager *ncs_manager = term->ncs_manager;

	guimissedcall_button_display(pRc, pData);
	
	if (total == 0) {
		missed_call_list_empty_window(system_info.hdc,pRc);
		return 0;
	}

	//RoundRect(hdc, pRc->left, pRc->top, pRc->right, pRc->bottom, round_size, round_size);

	cfg = lp_config_new(INI_MISSED_CALL_FILE);

	if (cfg == NULL)
	{
		SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));
		return -1;
	}

	total = lp_config_get_int(cfg, "missed_call_total", "total", 0);

	for (i = 0; i < total; i++)
	{
		int target_id, board_id;
		char *date;

		sprintf(section, "missed_call_%d", total - i);
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
			TextOut(system_info.hdc, pRc->left+30, (step - text_height) / 2 + y_start, text_buf);

		}


		if (1)//i != total - 1)
		{
			y_start += step;
			SetPenColor(system_info.hdc, COLOR_lightwhite);
			MoveTo(system_info.hdc, pRc->left+7, y_start);
			LineTo(system_info.hdc, pRc->right-15, y_start);
		}
		
	}

	lp_config_destroy(cfg);

	return 0;
}

static int guimissedcall_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
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
			data_item = guimissedcall_data_items;
			HandleCreate((TYPE_GUI_AREA_HANDLE*)data_item, hWnd);
			break;
		case MSG_TIMER:
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			{
				TYPE_GUI_AREA_HANDLE* data_item;
				data_item = guimissedcall_data_items;
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

#include "guicominputdigtal.h"
#include "gui_calloutcomming.h"
#include "gui_call_chatting.h"
#include "gui_conference_window.h"


extern const TYPE_GUI_AREA_HANDLE gui_com_input_digtal_item[];
TYPE_CTRL_INPUT_DIGTAL g_input_ctrl_type;
static void PaintButton(PRECT pRc, void* pData);
static void PaintKeyBar(PRECT pRc, void* pData);
static void PaintOkButton(PRECT pRc, void* pData);
static void PaintPhoneBookImage(PRECT pRc, void* pData);
static void PaintInfomation(PRECT pRc, void* pData);
static void ButtonProc(HWND hWnd, int message, int wParam, int lParam);
int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
int PhoneBookWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void PhonebookButtonProc(HWND hWnd, int message, int wParam, int lParam);

void Init(TYPE_CTRL_INPUT_DIGTAL* pCtrl, int type)
{
    pCtrl->Type = type;
    pCtrl->EditCurPos = 0;

    if(type == 3)
	{
		set_system_task(TASK_NCS_MONITOR_OUTGOING);
	}
	else if(type == 5)
	{
		set_system_task(TASK_NCS_CALL_TRANSFER);
	}
	else if(type == 6)
	{
		set_system_task(TASK_NCS_CONFERENCE);
	}
    else
    {
        if (term->dial_rapid == DIAL_NCS)
        {
            set_system_task(TASK_NCS_CALL_OUTGOING);
        }
        else
        {
            set_system_task(TASK_SIP_CALL_OUTGOING);
        }
    }
}

static void InputEditBuf(TYPE_CTRL_INPUT_DIGTAL* pCtrl, char c)
{
    pCtrl->EditTextBuf[pCtrl->EditCurPos++] = c;
}

static void DeleteEditBuf(TYPE_CTRL_INPUT_DIGTAL* pCtrl)
{
    if(pCtrl->EditCurPos)
    {
        pCtrl->EditTextBuf[pCtrl->EditCurPos--] = '\0';
    }
}

static void EditDrawText(TYPE_CTRL_INPUT_DIGTAL* pCtrl, HWND hWnd, int FuncType, int pos)
{
    /*int i = 0;*/
    int pos_x = 0;
    RECT rc;
    char a = 9;
	BITMAP * bitmap = NULL;
    char Buf[30];

    if(FuncType == 1)
    {
		pos_x = gui_com_input_digtal_item[15].rc.left+(pos-1)*21+6;
		SetRect(&rc, pos_x, gui_com_input_digtal_item[15].rc.top+15, pos_x+18, gui_com_input_digtal_item[15].rc.top+15+20);
		InvalidateRect_Ex(hWnd, &rc, TRUE);

        system_info.hdc = BeginPaint(hWnd);
        if(pCtrl->Type == 5)
        {
            sprintf(Buf, "%s.png", "10mi");
        }
        else
        {
            if(pCtrl->EditTextBuf[pos-1]>= 0x30 && pCtrl->EditTextBuf[pos-1]<=0x39)
            {
                sprintf(Buf, "T%d.png", pCtrl->EditTextBuf[pos-1]-0x30);
            }
            else if(pCtrl->EditTextBuf[pos-1] == '*')
            {
                sprintf(Buf, "%s.png", "Tmi");
            }
            else if(pCtrl->EditTextBuf[pos-1] == '#')
            {
                sprintf(Buf, "%s.png", "Tjingh");
            }
        }
        bitmap  = png_ha_res_find_by_name( Buf);
        FillBoxWithBitmap(system_info.hdc, rc.left, rc.top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
        EndPaint(hWnd, system_info.hdc);
    }
    else if(FuncType == 2)
    {
		pos_x = gui_com_input_digtal_item[15].rc.left+(pos-1)*21+6;
		SetRect(&rc, pos_x, gui_com_input_digtal_item[15].rc.top+15, pos_x+18, gui_com_input_digtal_item[15].rc.top+15+20);
		//InvalidateRect_Ex(hWnd, &rc, TRUE);

        //system_info.hdc = BeginPaint(hWnd);
        if(pCtrl->Type == 5)
        {
            sprintf(Buf, "%s.png", "10mi");
        }
        else
        {
            if(pCtrl->EditTextBuf[pos-1]>= 0x30 && pCtrl->EditTextBuf[pos-1]<=0x39)
            {
                sprintf(Buf, "T%d.png", pCtrl->EditTextBuf[pos-1]-0x30);
            }
            else if(pCtrl->EditTextBuf[pos-1] == '*')
            {
                sprintf(Buf, "%s.png", "Tmi");
            }
            else if(pCtrl->EditTextBuf[pos-1] == '#')
            {
                sprintf(Buf, "%s.png", "Tjingh");
            }
        }
        bitmap  = png_ha_res_find_by_name( Buf);
        FillBoxWithBitmap(system_info.hdc, rc.left, rc.top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
        //EndPaint(hWnd, system_info.hdc);
    }
    else if(FuncType == -1)
    {
		pos_x = gui_com_input_digtal_item[15].rc.left+(pos-1)*21+6;
		SetRect(&rc, pos_x, gui_com_input_digtal_item[15].rc.top+15, pos_x+18, gui_com_input_digtal_item[15].rc.top+15+20);
		InvalidateRect_Ex(hWnd, &rc, TRUE);

        system_info.hdc = BeginPaint(hWnd);
        EndPaint(hWnd, system_info.hdc);
    }
}


const TYPE_GUI_AREA_HANDLE gui_com_input_digtal_item[] =
{
    {0  , {240+75     , 120+75+75+75      , 240+75+68    , 120+75+75+75+68}      , "input_up_0.png"      , "input_dp_0.png"         , PaintButton     , ButtonProc}          , // 0
    {1  , {240        , 120               , 240+65       , 120+68}               , "input_up_1.png"      , "input_dp_1.png"         , PaintButton     , ButtonProc}          , // 1
    {2  , {240+75     , 120               , 240+75+68    , 120+68}               , "input_up_2.png"      , "input_dp_2.png"         , PaintButton     , ButtonProc}          , // 2
    {3  , {240+75+75  , 120               , 240+75+75+68 , 120+68}               , "input_up_3.png"      , "input_dp_3.png"         , PaintButton     , ButtonProc}          , // 3
    {4  , {240        , 120+75            , 240+68       , 120+75+68}            , "input_up_4.png"      , "input_dp_4.png"         , PaintButton     , ButtonProc}          , // 4
    {5  , {240+75     , 120+75            , 240+75+68    , 120+75+68}            , "input_up_5.png"      , "input_dp_5.png"         , PaintButton     , ButtonProc}          , // 5
    {6  , {240+75+75  , 120+75            , 240+75+75+68 , 120+75+68}            , "input_up_6.png"      , "input_dp_6.png"         , PaintButton     , ButtonProc}          , // 6
    {7  , {240        , 120+75+75         , 240+68       , 120+75+75+68}         , "input_up_7.png"      , "input_dp_7.png"         , PaintButton     , ButtonProc}          , // 7
    {8  , {240+75     , 120+75+75         , 240+75+68    , 120+75+75+68}         , "input_up_8.png"      , "input_dp_8.png"         , PaintButton     , ButtonProc}          , // 8
    {9  , {240+75+75  , 120+75+75         , 240+75+75+68 , 120+75+75+68}         , "input_up_9.png"      , "input_dp_9.png"         , PaintButton     , ButtonProc}          , // 9
    {10 , {240        , 120+75+75+75      , 240+68       , 120+75+75+75+68}      , "input_up_mi.png"     , "input_dp_mi.png"        , PaintButton     , ButtonProc}          , // *
    {11 , {240+75+75  , 120+75+75+75      , 240+75+75+68 , 120+75+75+75+68}      , "input_up_jh.png"     , "input_dp_jh.png"        , PaintButton     , ButtonProc}          , // #
    {12 , {210        , 120+75+75+75+68+4 , 210+293      , 120+75+75+75+68+4+58} , BTN_OK_UP             , BTN_OK_DOWN              , PaintOkButton   , ButtonProc}          , // È·ÈÏ
    {13 , {210+298-70 , 69+3              , 210+298-30   , 69+33}                , "pre_arrow.png"       , "pre_arrow_2.png"        , PaintButton     , ButtonProc}          , // É¾³ý
    {14 , {BTN_BACK_L , BTN_BACK_T        , BTN_BACK_R   , BTN_BACK_B	}          , BTN_BACK_UP           , BTN_BACK_DOWN            , PaintButton     , ButtonProc}          , // ·µ»Ø
    {15 , {210        , 60                , 210+298      , 60+55}                , "key-bar.png"         , NULL                     , PaintKeyBar     , NULL}                , // ¼üÅÌÌõ
    {16 , {800-132    , 60                , 800          , 60+56}                , "key-phonenumber.png" , "key-phonenumber-up.png" , PaintButton     , PhonebookButtonProc} , // ºÅÂð²¾
    {17 , {210        , 40                , 210+298      , 60}                   , NULL                  , NULL                     , PaintInfomation , NULL}                , // ¼üÅÌÌõ
    {-1 , {0          , 0                 , 0            , 0}                    , NULL                  , NULL                     , NULL            , NULL}                , 
};

static void PaintInfomation(PRECT pRc, void* pData)
{
    SetPenColor(system_info.hdc, COLOR_lightwhite);
    SetTextColor(system_info.hdc, COLOR_lightwhite);
    SetBkMode(system_info.hdc, BM_TRANSPARENT);
	if(term->language == 1)
	{
		DrawText(system_info.hdc, "ÇëÊäÈë:ID+#+Ãæ°åID", -1, pRc, DT_LEFT);
	}
	else
	{
		DrawText(system_info.hdc, "Please Input:ID + # + Board ID", -1, pRc, DT_LEFT);
	}
}
static void PaintOkButton(PRECT pRc, void* pData)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	char *data;
	int down = 0;
	char talk[2][64] = {"talk_up.png","talk_down.png"};
	char monitor[2][64] = {"monitor_up.png","monitor_down.png"};

	if(strcmp((char*)pData,BTN_OK_DOWN) == 0)
	{
		down = 1;
	}
	if(task == TASK_NCS_MONITOR_OUTGOING)
	{
		data = monitor[down];
	}
	else
	{
		data = talk[down];
	}
	PaintButton(pRc, data);
}


static int inputdigital_str_parse(int *data, int max_len, char delimiter)
{
    char *p_start = g_input_ctrl_type.EditTextBuf;
    char *p_end = g_input_ctrl_type.EditTextBuf;
    char tmp[128];
    int j = 0;

    while (1)
    {
        /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í±ï¿½Ê¾ï¿½ï¿½ï¿½Ü½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
        if (*p_end == delimiter)
        {
            if (p_end != p_start)
            {
                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, p_start, p_end-p_start);
                data[j++] = atoi(tmp);
                if (j >= max_len)
                    return j;
            }
            p_end++;
            p_start = p_end;
        }
        else if (*p_end >= '0' && *p_end <= '9')
        {
            /* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ç·Ö¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Îªï¿½ï¿½ï¿½Ö£ï¿½ï¿½ï¿½ï¿½ï¿½Ç°Ç°ï¿½ï¿½Ò»ï¿½ï¿½ï¿½Ö·ï¿½ */
            p_end++;
        }
        else
        {
            /* ï¿½ï¿½ï¿½ï¿½ï¿½Ö·ï¿½ï¿½ï¿½Ä©Î² */
            if (p_end != p_start)
            {
                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, p_start, p_end-p_start);
                data[j++] = atoi(tmp);
            }
            break;
        }
    }

    return j;
}

static void inputdigital_start_sip_call(void)
{
    SPON_LOG_INFO("=====>%s - g_input_ctrl_type.EditTextBuf:%s\n", __func__, g_input_ctrl_type.EditTextBuf);

    term->sip_manager->call_request(g_input_ctrl_type.EditTextBuf, strlen(g_input_ctrl_type.EditTextBuf));

    //destroy_edit_window();

    //ENTER_CALL_OUTGOING_WINDOW();
    return;
}


static int inputdigital_start_ncs_call(void)
{
    int id[2] = {0};
    int ret;
    struct ncs_manager *ncs_manager = term->ncs_manager;

	if(strcmp(g_input_ctrl_type.EditTextBuf, "*#12#") == 0)
	{
		return -2;
	}
	else
	{
		inputdigital_str_parse(id, 2, '#');

		ret = ncs_manager->ncs_ip_request(ncs_manager, id[0], id[1], TALK);

		if(ret < 0)
			return -1;
		return 0;
	}
}

static int inputdigital_start_ncs_conference_call(void)
{
    int id[2] = {0};
    int ret = 0;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    inputdigital_str_parse(id, 2, '#');
    ret = ncs_manager->ncs_ip_request(ncs_manager, id[0], id[1], TALK);
    if(ret < 0)
        return -1;
	return 0;
}


static void inputdigital_start_ncs_call_transfer(void)
{
    struct ncs_manager *ncs_manager = term->ncs_manager;

    ncs_manager->ncs_call_transfer(ncs_manager, atoi(g_input_ctrl_type.EditTextBuf));
}

static int inputdigital_start_ncs_monitor(void)
{
    int id[2] = {0};
    int ret = 0;
    struct ncs_manager *ncs_manager = term->ncs_manager;

    inputdigital_str_parse(id, 2, '#');

    ret = ncs_manager->ncs_ip_request(ncs_manager, id[0], id[1], MONITOR);
    if(ret < 0)
        return -1;
	return 0;
}

static void PhonebookButtonProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	int ret = 0;
	
	//if(task == TASK_NCS_MONITOR_OUTGOING)
	//{
	//	return;
	//}

	InvalidateRect_Ex(hWnd, &p->rc, TRUE); //ÏÈÇå³ýÇøÓò, ÔÙË¢ÐÂ
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
			EnterWin(hWnd, PhoneBookWinProc);
			break;
        default:
            break;
	}
}


static void ButtonProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();
	int ret = 0;

	InvalidateRect_Ex(hWnd, &p->rc, TRUE); //ÏÈÇå³ýÇøÓò, ÔÙË¢ÐÂ
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
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
					if(g_input_ctrl_type.EditCurPos<10)//13
					{
						if(p->id == 10)
						{
							InputEditBuf(&g_input_ctrl_type, '*');
						}
						else if(p->id == 11)
						{
							InputEditBuf(&g_input_ctrl_type, '#');
						}
						else
						{
							InputEditBuf(&g_input_ctrl_type, 0x30+p->id);
						}
						EditDrawText(&g_input_ctrl_type, hWnd, 1, g_input_ctrl_type.EditCurPos);
					}
					break;
				case 12:
					if(!g_input_ctrl_type.EditCurPos)
					{
						break;
					}
					switch(task)
					{
						case TASK_SIP_CALL_OUTGOING:
							inputdigital_start_sip_call();
							EnterWin(hWnd, guicall_out_proc);
							break;
						case TASK_NCS_CALL_OUTGOING:
							ret = inputdigital_start_ncs_call();
							if(ret >= 0)
							{
								EnterWin(hWnd, guicall_out_proc);
							}
							else if(ret == -2)
							{
								guilocalvideo_newscreen(hWnd);
							}
							break;
						case TASK_NCS_MONITOR_OUTGOING:
	               			ret = inputdigital_start_ncs_monitor();
							if(ret >= 0)
							{
								EnterWin(hWnd, guicall_chatting_proc);
							}
							break;
						case TASK_NCS_CONFERENCE:
							ret = inputdigital_start_ncs_conference_call();
							if(ret >= 0)
							{
								EnterWin(hWnd, guicall_conference_proc);
							}
							break;
						case TASK_NCS_CALL_TRANSFER:
							inputdigital_start_ncs_call_transfer();
							//EnterWin(hWnd, guicall_chatting_proc);
							break;
						default:
							break;
					}
					break;
				case 13: //É¾³ý
					if(g_input_ctrl_type.EditCurPos)
					{
						EditDrawText(&g_input_ctrl_type, hWnd, -1, g_input_ctrl_type.EditCurPos);
						DeleteEditBuf(&g_input_ctrl_type);
					}
					break;
				case 14:
					if (task == TASK_NCS_CALL_TRANSFER || task == TASK_NCS_CONFERENCE)
					{
						EnterWin(hWnd, guicall_chatting_proc);
						break;
					}
					set_system_task(TASK_NONE);
					EnterWin(hWnd, WelComeWinProc);
					break;
				case 16:
					EnterWin(hWnd, PhoneBookWinProc);
					break;
        default:
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

static void PaintKeyBar(PRECT pRc, void* pData)
{
	BITMAP * bitmap = NULL;
	int i = 0;

    bitmap  = png_ha_res_find_by_name(pData);
    if(bitmap == NULL)
    {
        SPON_LOG_ERR("file: %s load error\n", pData);
    }
    else
    {
        FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
    }
#if 1
	for(i=0;i<g_input_ctrl_type.EditCurPos;i++)
	{
		EditDrawText(&g_input_ctrl_type, system_info.main_hwnd, 2, i+1);
	}
#endif	
}

static void PaintPhoneBookImage(PRECT pRc, void* pData)
{
	task_t task = get_system_task();
	
	if(task == TASK_NCS_MONITOR_OUTGOING)
	{
		return;
	}
	PaintButton(pRc, pData);
}

int InputDigtalWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;

	HDC hdc;
	static bool_t flag = 0 ;

	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
		SPON_LOG_INFO("===============> Enter InputDigtalWinProc\n");
	}
	switch(message)
	{
		case MSG_CREATE:
			HandleCreate((TYPE_GUI_AREA_HANDLE*)gui_com_input_digtal_item, hWnd);
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
		case MSG_NCS_INPUT_DIGTAL_PAGE:
			HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_com_input_digtal_item, hWnd, message, wParam, lParam);
			break;
		case MSG_PAINT:
			return 0;
		case MSG_ERASEBKGND:
			bitmap  = png_ha_res_find_by_name("othermainwindowbg.png");
			FillBoxWithBitmap((HDC)wParam, 0, 0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
			return 0;
		case MSG_KEYUP:
			switch(wParam)
			{
				case kKeyCallStop:
                    //audio_channel_mode_set(AUDIO_CHANNEL_IDLE);
                    set_system_task(TASK_NONE);
                    EnterWin(hWnd, WelComeWinProc);
                    break;
			}
		case MSG_CLOSE:
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);
			return 0;
		case MSG_DESTROY:
			return 0;
		default:
			break;
	}
	return 1;
}

void InputDigtal_NewScreen(HWND hWnd)
{
	g_input_ctrl_type.EditCurPos = 0;
	memset(g_input_ctrl_type.EditTextBuf,0,sizeof(g_input_ctrl_type.EditTextBuf));
	EnterWin(hWnd, InputDigtalWinProc);
}


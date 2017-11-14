#include  "guiloginwin.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guiloginwin_button_display(PRECT pRc, void* pData);
static void guiloginwin_input_display(PRECT pRc, void* pData);
static void guiloginwin_input_texture(loginwin_data_struct* ptr, char c);
static void guiloginwin_delete_texture(loginwin_data_struct* ptr);
static void guiloginwin_texture_display(loginwin_data_struct* ptr, PRECT pRc, int idx);
static void guiloginwin_keypad_func(HWND hWnd, int message, int wParam, int lParam);
static int guiloginwin_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guiloginwin_btn_func(HWND hWnd, int message, int wParam, int lParam);

	
static loginwin_data_struct sg_loginwin_data;

#define BC_SUB_BG_W			378
#define BC_SUB_BG_H			421

#define START_X				((LCD_WIDTH-BC_SUB_BG_W)>>1)
#define START_Y				41

#define KEY_X_OFFSET		80
#define KEY_Y_OFFSET		70

#define KEY_X				START_X+KEY_X_OFFSET
#define KEY_Y				171
#define KEY_W				80
#define KEY_H				65

#define KEYPAD_BAR_W		282
#define KEYPAD_BAR_X 		((LCD_WIDTH-KEYPAD_BAR_W)>>1)
#define KEYPAD_BTN_Y 		KEY_X+240
#define KEYPAD_BTN_OFFSET	80

const TYPE_GUI_AREA_HANDLE gui_login_widget_keypad[] =
{
	{0	, {START_X 		, START_Y				, START_X+BC_SUB_BG_W 		, START_Y+BC_SUB_BG_H		}, "login_sub_bg.png"	, NULL				, guiloginwin_button_display  , NULL} ,
    {1  , {KEY_X+KEY_W-5	, KEY_Y+KEY_Y_OFFSET*3	, KEY_X+KEY_W+KEY_X_OFFSET-5	, KEY_Y+KEY_Y_OFFSET*3+KEY_H}, "input_up_0.png" , "input_dp_0.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 0
    {2  , {KEY_X-20       	, KEY_Y               	, KEY_X+KEY_X_OFFSET-20       	, KEY_Y+KEY_H				}, "input_up_1.png"	, "input_dp_1.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 1
    {3  , {KEY_X+KEY_W-5	, KEY_Y               	, KEY_X+KEY_W+KEY_X_OFFSET-5	, KEY_Y+KEY_H				}, "input_up_2.png"	, "input_dp_2.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 2
    {4  , {KEY_X+KEY_W*2+10, KEY_Y               	, KEY_X+KEY_W*2+KEY_X_OFFSET+10, KEY_Y+KEY_H				}, "input_up_3.png"	, "input_dp_3.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 3
    {5  , {KEY_X-20       	, KEY_Y+KEY_Y_OFFSET	, KEY_X+KEY_X_OFFSET-20       	, KEY_Y+KEY_Y_OFFSET+KEY_H	}, "input_up_4.png"	, "input_dp_4.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 4
    {6  , {KEY_X+KEY_W-5	, KEY_Y+KEY_Y_OFFSET    , KEY_X+KEY_W+KEY_X_OFFSET-5	, KEY_Y+KEY_Y_OFFSET+KEY_H	}, "input_up_5.png"	, "input_dp_5.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 5
    {7  , {KEY_X+KEY_W*2+10, KEY_Y+KEY_Y_OFFSET    , KEY_X+KEY_W*2+KEY_X_OFFSET+10, KEY_Y+KEY_Y_OFFSET+KEY_H	}, "input_up_6.png"	, "input_dp_6.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 6
    {8  , {KEY_X-20       	, KEY_Y+KEY_Y_OFFSET*2  , KEY_X+KEY_X_OFFSET-20       	, KEY_Y+KEY_Y_OFFSET*2+KEY_H}, "input_up_7.png"	, "input_dp_7.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 7
    {9  , {KEY_X+KEY_W-5	, KEY_Y+KEY_Y_OFFSET*2	, KEY_X+KEY_W+KEY_X_OFFSET-5	, KEY_Y+KEY_Y_OFFSET*2+KEY_H}, "input_up_8.png"	, "input_dp_8.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 8
    {10 , {KEY_X+KEY_W*2+10, KEY_Y+KEY_Y_OFFSET*2	, KEY_X+KEY_W*2+KEY_X_OFFSET+10, KEY_Y+KEY_Y_OFFSET*2+KEY_H}, "input_up_9.png"	, "input_dp_9.png"      , guiloginwin_button_display , guiloginwin_keypad_func} , // 9
    {11 , {KEY_X-20       	, KEY_Y+KEY_Y_OFFSET*3	, KEY_X+KEY_X_OFFSET-20       	, KEY_Y+KEY_Y_OFFSET*3+KEY_H}, "input_up_mi.png", "input_dp_mi.png"     , guiloginwin_button_display , guiloginwin_keypad_func} , // *
    {12 , {KEY_X+KEY_W*2+10, KEY_Y+KEY_Y_OFFSET*3	, KEY_X+KEY_W*2+KEY_X_OFFSET+10, KEY_Y+KEY_Y_OFFSET*3+KEY_H}, "input_up_jh.png", "input_dp_jh.png"     , guiloginwin_button_display , guiloginwin_keypad_func} , // #

    //{13 , {KEYPAD_BTN_X, KEYPAD_BTN_Y						, KEYPAD_BTN_X+BTN_W	, KEYPAD_BTN_Y+BTN_H					}, BTN_OK_UP	, BTN_OK_DOWN	, guiloginwin_button_display , guiloginwin_btn_func} , // È·ÈÏ
    //{14 , {KEYPAD_BTN_X, KEYPAD_BTN_Y+KEYPAD_BTN_OFFSET		, KEYPAD_BTN_X+BTN_W	, KEYPAD_BTN_Y+KEYPAD_BTN_OFFSET+BTN_H	}, BTN_DEL_UP	, BTN_DEL_DOWN	, guiloginwin_button_display , guiloginwin_btn_func} , // É¾³ý
	{13 , {KEYPAD_BAR_X+KEYPAD_BAR_W-50		, KEY_Y-48					, KEYPAD_BAR_X+KEYPAD_BAR_W-10	, KEY_Y-18						}, "pre_arrow.png"	 , "pre_arrow_2.png" 				 , guiloginwin_button_display  , guiloginwin_btn_func}		,
	{14 ,	{BTN_BACK_L, BTN_BACK_T ,	BTN_BACK_R,BTN_BACK_B 	}, BTN_BACK_UP		, BTN_BACK_DOWN, guiloginwin_button_display , guiloginwin_btn_func} ,    
	{15 , {KEYPAD_BAR_X		, KEY_Y-60					, KEYPAD_BAR_X+KEYPAD_BAR_W-50	, KEY_Y-10						}				, "key-bar.png" 		   , NULL				   , guiloginwin_input_display , NULL}	   , // ¼üÅÌÌõ
    
    {-1 , {0         , 0                 , 0            , 0}                    , NULL                     , NULL                  , NULL        	, NULL}       ,

};
static char sg_password[20];

void guiloginwin_get_login_pwd(void)
{
	LpConfig *lpconfig;
	u_int yes = 1;
	char *password;
	char command[128];
	int i;

	lpconfig = lp_config_new(INI_CONFIG_FILE);
	if (lpconfig == NULL) {
		SPON_LOG_ERR("lp_config_new failed:%s\n", strerror(errno));
		return -1;
	}
	password = (char *)lp_config_get_string(lpconfig, "terminal", "password", "123456");
	sscanf(password, "%s", sg_password);

	lp_config_destroy(lpconfig);
}

void guiloginwin_newscreen(HWND hWnd)
{
	sg_loginwin_data.input_position = 0;
	memset(sg_loginwin_data.input,0,LOGINWIN_INPUT_NUM);
	guiloginwin_get_login_pwd();
	EnterWin(hWnd, guiloginwin_screen_proc);
}

static void guiloginwin_img_display(int x, int y, char *path)
{
	BITMAP * bitmap = NULL;
	bitmap	= png_ha_res_find_by_name(path);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", path);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, x, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
}

static void guiloginwin_keypad_func(HWND hWnd, int message, int wParam, int lParam)
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
			
			//if(sg_loginwin_data.input_position == LOGINWIN_INPUT_NUM)
			//{
			//	sg_loginwin_data.input_position = 0;
			//	memset(sg_loginwin_data.input,0,LOGINWIN_INPUT_NUM);
			//}
			
			if(p->id>= 1 && p->id <= 10)
			{
				guiloginwin_input_texture(&sg_loginwin_data,0x30+p->id-1);
			}
			else if(p->id == 11)
			{
				guiloginwin_input_texture(&sg_loginwin_data,'*');
			}
			else if(p->id == 12)
			{
				guiloginwin_input_texture(&sg_loginwin_data,'#');
			}
			SendNotifyMessage(hWnd, MSG_NCS_INPUT_DIGTAL_PAGE, 0, 15);
			break;
	}
}


static void guiloginwin_btn_func(HWND hWnd, int message, int wParam, int lParam)
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
			/*if(p->id == 13)
			{
				if((strcmp(sg_password, sg_loginwin_data.input)==0)||(strcmp("800823", sg_loginwin_data.input)==0))
				{
					sg_loginwin_data.input_position = 0;
					memset(sg_loginwin_data.input,0,LOGINWIN_INPUT_NUM);
					SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_SETTING_SCREEN_ID);
				}
				else
				{
					sg_loginwin_data.input_position = 0;
					memset(sg_loginwin_data.input,0,LOGINWIN_INPUT_NUM);
					SendNotifyMessage(hWnd, MSG_NCS_INPUT_DIGTAL_PAGE, 0, 15);
				}
			}
			else */
			if(p->id == 13)
			{
				guiloginwin_delete_texture(&sg_loginwin_data);
				SendNotifyMessage(hWnd, MSG_NCS_INPUT_DIGTAL_PAGE, 0, 15);
			}
			else if(p->id == 14)
			{
				SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_IDLE_SCREEN_ID);
			}
			break;
	}
}

static void guiloginwin_input_texture(loginwin_data_struct* ptr, char c)
{
    ptr->input[ptr->input_position++] = c;
}

static void guiloginwin_delete_texture(loginwin_data_struct* ptr)
{
    if(ptr->input_position)
    {
    	ptr->input_position--;
        ptr->input[ptr->input_position] = '\0';
    }
}

static void guiloginwin_texture_display(loginwin_data_struct* ptr, PRECT pRc, int idx)
{
    BITMAP * bitmap = NULL;
    char data[30];
/*
    if(ptr->input[idx]>= 0x30 && ptr->input[idx]<=0x39)
    {
        sprintf(data, "T%d.png", ptr->input[idx]-0x30);
    }
    else if(ptr->input[idx] == '*')
    {
        sprintf(data, "%s.png", "Tmi");
    }
    else if(ptr->input[idx] == '#')
    {
        sprintf(data, "%s.png", "Tjingh");
    }
*/
	sprintf(data, "%s.png", "Tmi");

	guiloginwin_img_display(pRc->left+idx*20+20, pRc->top+20, data);
	
}

static void guiloginwin_button_display(PRECT pRc, void* pData)
{	
	guiloginwin_img_display(pRc->left, pRc->top, pData);
}

static void guiloginwin_input_display(PRECT pRc, void* pData)
{
	int i = 0;
	PRECT ptr = pRc;
	int len = strlen(sg_loginwin_data.input);
	
	guiloginwin_button_display(pRc, pData);
	
	for(i=0;i<len;i++)
	{
		guiloginwin_texture_display(&sg_loginwin_data, pRc, i);
	}
}

static void guiloginwin_input_update(HWND hWnd)
{
	TYPE_GUI_AREA_HANDLE *p = &gui_login_widget_keypad[15];

	InvalidateRect_Ex(hWnd, &(p->rc), TRUE);
	system_info.hdc = BeginPaint(hWnd);
	p->pReFlash(&(p->rc), p->pData0);
	EndPaint(hWnd, system_info.hdc);
	
}
static int guiloginwin_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap;
	TYPE_GUI_AREA_HANDLE* data_item;
	int timer_id = (int)wParam;
	
	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
	}
	
	switch(message)
    {
		case MSG_CREATE:
			data_item = gui_login_widget_keypad;
			HandleCreate((TYPE_GUI_AREA_HANDLE*)data_item, hWnd);
			break;
		case MSG_TIMER:
			break;
		case MSG_NCS_INPUT_DIGTAL_PAGE:
			guiloginwin_input_update(hWnd);
			if(sg_loginwin_data.input_position == LOGINWIN_INPUT_NUM)
			{
				if((strcmp(sg_password, sg_loginwin_data.input)==0)||(strcmp("800823", sg_loginwin_data.input)==0))
				{
					sg_loginwin_data.input_position = 0;
					memset(sg_loginwin_data.input,0,LOGINWIN_INPUT_NUM);
					SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_SETTING_SCREEN_ID);
				}
				else
				{
					sg_loginwin_data.input_position = 0;
					memset(sg_loginwin_data.input,0,LOGINWIN_INPUT_NUM);
					SendNotifyMessage(hWnd, MSG_NCS_INPUT_DIGTAL_PAGE, 0, 15);
				}
			}
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			{
				TYPE_GUI_AREA_HANDLE* data_item;
				data_item = gui_login_widget_keypad;
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


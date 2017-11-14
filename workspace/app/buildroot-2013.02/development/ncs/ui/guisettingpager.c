#include "guisettingpager.h"
#include "lpconfig.h"
#include "language.h"

#define TIMERID_SETTING 102
TYPE_CTRL_SETTING_DLG g_Seting_Ctrl =
{
	0, 0, 0, 0, 0
};

TYPE_GUI_AREA_HANDLE *g_plist_widget = NULL;
static void PaintButton(PRECT pRc, void* pData);
static void PaintTermSettingEditWithCursor(int index);
static void PaintButtonEx(PRECT pRc, void* pData);
static void ButtonProcEx(HWND hWnd, int message, int wParam, int lParam);
static void MoveToxy(int x, int Max);
static void PaintItemByOneW(int curPos, int index, char a);
static void PaintTermSettingId(PRECT pRc, void* pData);
static void PaintTermSettingLocalIp(PRECT pRc, void* pData);
static void PaintTermSettingServerIp(PRECT pRc, void* pData);
static void PaintTermSettingMask(PRECT pRc, void* pData);
static void PaintTermSettingGw(PRECT pRc, void* pData);
int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void ButtonProc(HWND hWnd, int message, int wParam, int lParam);
static void PaintTermSetting(PRECT pRc, void* pData);
static void PaintTermKeyPad(PRECT pRc, void* pData);
static void CopyInfoToEditBar(void);
static void PaintTermSettingPrc(HWND hWnd, int message, int wParam, int lParam);
static void PaintTermKeyPadProc(HWND hWnd, int message, int wParam, int lParam);
static void KeyValueProc(int message, int wParam, int lParam);
static void PaintItemByOneWEx(int curPos, int index, char a);
static void PaintTermSettingLocalIp(PRECT pRc, void* pData);
static void ButtonProcOther(HWND hWnd, int message, int wParam, int lParam);
int SaveInfoWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void PaintTermConfigInfo(PRECT pRc, void* pData);
static void PaintTermMacAddrInfo(PRECT pRc, void* pData);
static void PaintTermConfigVersion(PRECT pRc, void* pData);
static void PaintTermConfigId(PRECT pRc, void* pData);
static void PaintTermConfigSip(PRECT pRc, void* pData);

static void reset_factory_settings(void);
static void ResetFactoryCancleProc(HWND hWnd, int message, int wParam, int lParam);
static void ResetFactoryOkProc(HWND hWnd, int message, int wParam, int lParam);
const TYPE_GUI_AREA_HANDLE gui_com_setting_pager_item[] =
{
	{0  , {43 , 75      , 43+157 , 75+60}   , "set_btn_down.png" , "set_btn_up.png" , PaintButton , ButtonProc} , //0
	{1  , {43 , 75+60   , 43+157 , 75+60*2} , "set_btn_up.png" , "set_btn_down.png" , PaintButton , ButtonProc} , //1
	{2  , {43 , 75+60*2 , 43+157 , 75+60*3} , "set_btn_up.png" , "set_btn_down.png" , PaintButton , ButtonProc} , //2
	{-1 , {0   , 0                    , 0       , 0}                       , NULL                   , NULL                   , NULL       , NULL}       ,
};

const TYPE_GUI_AREA_HANDLE gui_setting_retang[] =
{
	{0  , {80+110 , 73 , 80+110+394 , 73+318} , "setting_retang.png" , "terminal_info.png" , PaintButtonEx , NULL} , //框
	{-1  , {80+110 , 73 , 80+110+394 , 73+318} , NULL , NULL , PaintButtonEx , NULL} , //框
};

const TYPE_GUI_AREA_HANDLE gui_setting_other_array[] =
{
	{0 , {BTN_BACK_L, BTN_BACK_T ,	BTN_BACK_R,BTN_BACK_B 	} , BTN_BACK_UP         , BTN_BACK_DOWN, PaintButtonEx , ButtonProcOther} , // 返回
	{-1 , {0       , 0  , 0            , 0}      , NULL                    , NULL , NULL        , NULL}       ,
};


char g_pager_widget0_File[][32]= {"term_id_1.png", "term_id_2.png"};

TYPE_GUI_AREA_HANDLE gui_com_setting_pager_widget0[] =
{
	{0  , {95+118 , 85        , 95+118+349 , 85+48}   , "term_id_1.png"        , "term_id_2.png"        , PaintTermSettingId       , PaintTermSettingPrc} ,
	{1  , {95+118 , 85+48*1+3 , 95+118+349 , 85+48*2} , "term_ip_2.png"        , "term_ip_1.png"        , PaintTermSettingLocalIp  , PaintTermSettingPrc} ,
	{2  , {95+118 , 85+48*2+3 , 95+118+349 , 85+48*3} , "term_server_ip_2.png" , "term_server_ip_1.png" , PaintTermSettingServerIp , PaintTermSettingPrc} ,
	{3  , {95+118 , 85+48*3+3 , 95+118+349 , 85+48*4} , "term_gw_2.png"        , "term_gw_1.png"        , PaintTermSettingGw       , PaintTermSettingPrc} ,
	{4  , {95+118 , 85+48*4+3 , 95+118+349 , 85+48*5} , "term_mask_2.png"      , "term_mask_1.png"      , PaintTermSettingMask     , PaintTermSettingPrc} ,
	{5  , {95+123 , 85+48*5+3 , 95+118+349 , 85+48*6} , "term_save_2.png"      , "term_save_1.png"      , PaintButtonEx            , ButtonProcEx}          ,
	{-1 , {0      , 0         , 0          , 0}       , NULL                   , NULL                   , NULL                     , NULL}                ,
};

const TYPE_GUI_AREA_HANDLE gui_com_setting_pager_widget1[] =
{
	{0  , {95+118 , 85        , 95+118+349 , 85+48 }   , NULL , NULL , PaintTermMacAddrInfo   , NULL         } , 
	{1  , {95+118 , 85+48*1+3 , 95+118+349 , 85+48*2 } , NULL , NULL , PaintTermConfigInfo    , NULL         } , 
	{2  , {95+118 , 85+48*2+3 , 95+118+349 , 85+48*3 } , NULL , NULL , PaintTermConfigInfo    , NULL         } , 
	{3  , {95+118 , 85+48*3+3 , 95+118+349 , 85+48*4 } , NULL , NULL , PaintTermConfigVersion , NULL }         , 
	{4  , {95+118 , 85+48*4+3 , 95+118+349 , 85+48*5 } , NULL , NULL , PaintTermConfigId      , NULL        }  , 
	{5  , {95+118 , 85+48*5+3 , 95+118+349 , 85+48*6 } , NULL , NULL , PaintTermConfigSip     , NULL        }  , 
	{-1 , {0      , 0         , 0          , 0       } , NULL , NULL , NULL                   , NULL     }     , 
};

const TYPE_GUI_AREA_HANDLE gui_com_setting_pager_widget2[] =
{
	{0   , {95+118       , 85+48  , 95+118+349      , 85+48+195 } , "ask_win.png" , NULL           , PaintButtonEx , NULL                   } ,
	//{1 , {95+118+60+70 , 85+150 , 95+118+60+70+94 , 85+150+48 } , "ok.png"      , "ok_1.png"     , PaintButtonEx , ResetFactoryOkProc     } ,
	{1   , {95+118+60    , 85+150 , 95+118+60+94    , 85+150+48 } , "ok.png"      , "ok_1.png"     , PaintButtonEx , ResetFactoryOkProc     } ,
	{2   , {95+118+220   , 85+150 , 95+118+220+94   , 85+150+48 } , "cancel.png"  , "cancel_1.png" , PaintButtonEx , ResetFactoryCancleProc } ,
	{-1  , {0            , 0      , 0               , 0         } , NULL          , NULL           , NULL          , NULL                   } ,
};

const TYPE_GUI_AREA_HANDLE gui_com_setting_pager_keypad[] =
{
	{1  , {600       , 75  , 600+56       , 75+57}  , "keypad_dp_1.png"  , "keypad_up_1.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{2  , {600+56    , 75  , 600+56+56    , 75+57}  , "keypad_dp_2.png"  , "keypad_up_2.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{3  , {600+56+56 , 75  , 600+56+56+56 , 75+57}  , "keypad_dp_3.png"  , "keypad_up_3.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,

	{4  , {600       , 135 , 600+56       , 135+57} , "keypad_dp_4.png"  , "keypad_up_4.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{5  , {600+56    , 135 , 600+56+56    , 135+57} , "keypad_dp_5.png"  , "keypad_up_5.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{6  , {600+56+56 , 135 , 600+56+56+56 , 135+57} , "keypad_dp_6.png"  , "keypad_up_6.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,

	{7  , {600       , 195 , 600+56       , 195+57} , "keypad_dp_7.png"  , "keypad_up_7.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{8  , {600+56    , 195 , 600+56+56    , 195+57} , "keypad_dp_8.png"  , "keypad_up_8.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{9  , {600+56+56 , 195 , 600+56+56+56 , 195+57} , "keypad_dp_9.png"  , "keypad_up_9.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,

	{10  , {600       , 252 , 600+56       , 252+57} , "keypad_dp_mi.png" , "keypad_up_mi.png" , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{0  , {600+56    , 252 , 600+56+56    , 252+57} , "keypad_dp_0.png"  , "keypad_up_0.png"  , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{11  , {600+56+56 , 252 , 600+56+56+56 , 252+57} , "keypad_dp_jh.png" , "keypad_up_jh.png" , PaintTermKeyPad , PaintTermKeyPadProc} ,
	{-1 , {0         , 0   , 0            , 0}      , NULL               , NULL               , NULL            , NULL}                ,
};

static void ButtonProcOther(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	task_t task = get_system_task();

	InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
	switch(message)
	{
		case MSG_LBUTTONDOWN:
			system_info.hdc = BeginPaint(hWnd);
			p->pReFlash(&p->rc, p->pData1);
			EndPaint(hWnd, system_info.hdc);
			break;
		case MSG_LBUTTONUP:
			switch(p->id)
			{
				case 0:
					KillTimer(hWnd, TIMERID_SETTING);
					EnterWin(hWnd, WelComeWinProc);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

static void PaintItemByOneW(int curPos, int index, char a)
{
	char Buf[20] = {0};

	BITMAP * bitmap = NULL;

	int x = gui_com_setting_pager_widget0[index].rc.left+curPos*13+120;
	int y = gui_com_setting_pager_widget0[index].rc.top+18;

	if(a>= 0x30 && a<=0x39)
	{
		sprintf(Buf, "10%d.png", a-0x30);
	}
	else if(a == '.')
	{
		sprintf(Buf, "1dot.png");
	}

	bitmap  = png_ha_res_find_by_name(Buf);
	FillBoxWithBitmap(system_info.hdc, x, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);
}

static void PaintItemByOneWEx(int curPos, int index, char a)
{
	char Buf[20] = {0};

	BITMAP * bitmap = NULL;

	int x = gui_com_setting_pager_widget0[index].rc.left+curPos*13+120;
	int y = gui_com_setting_pager_widget0[index].rc.top+18;

	if(a>= 0x30 && a<=0x39)
	{
		sprintf(Buf, "10%d.png", a-0x30);
	}
	else if(a == '.')
	{
		sprintf(Buf, "1dot.png");
	}

	bitmap  = png_ha_res_find_by_name(Buf);
	if(g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin <= curPos && curPos <g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd)
	{
		/*
		   if(g_Seting_Ctrl.flagDisp)
		   {
		   FillBoxWithBitmap(system_info.hdc, x, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);
		   }
		   */
		if(a>= 0x30 && a<=0x39)
		{
			sprintf(Buf, "10%d_r.png", a-0x30);
		}
		else if(a == '.')
		{
			sprintf(Buf, "1dot.png");
		}

		bitmap  = png_ha_res_find_by_name(Buf);
		FillBoxWithBitmap(system_info.hdc, x, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, x, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
}

static void SetListWidgetPtr(TYPE_GUI_AREA_HANDLE* ptr)
{
	g_plist_widget = ptr;
}

static void PaintButton(PRECT pRc, void* pData)
{
	BITMAP * bitmap = NULL;

	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	RECT rc;

	bitmap  = png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}

	memcpy(&rc, pRc, sizeof(RECT));
	rc.top+=20;
	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	if(p->id == 0)
	{
		if(term->language==1)
		{
			DrawText(system_info.hdc, "通用设置", -1, &rc, DT_CENTER);
		}
		else
		{
			DrawText(system_info.hdc, "Network", -1, &rc, DT_CENTER);
		}
	}
	else if(p->id == 1)
	{
		if(term->language==1)
		{
			DrawText(system_info.hdc, "本机信息", -1, &rc, DT_CENTER);
		}
		else
		{
			DrawText(system_info.hdc, "Device info", -1, &rc, DT_CENTER);
		}
	}
	else if(p->id == 2)
	{
		if(term->language==1)
		{
			DrawText(system_info.hdc, "恢复出厂设置", -1, &rc, DT_CENTER);
		}
		else
		{
			DrawText(system_info.hdc, "Restore", -1, &rc, DT_CENTER);
		}
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


static void PaintTermKeyPad(PRECT pRc, void* pData)
{
	BITMAP * bitmap = NULL;

	bitmap  = png_ha_res_find_by_name(pData);

	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
}

static void PaintTermSetting(PRECT pRc, void* pData)
{
	BITMAP * bitmap = NULL;

	bitmap  = png_ha_res_find_by_name(pData);

	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
}

static void CopyInfoToEditBar(void)
{
	char Buf[20] ;
	int BufEx[4] ;

	sprintf(Buf, "%05d", term->ncs_manager->id);
	memcpy(g_Seting_Ctrl.EditBuf[0], Buf, 5);
	g_Seting_Ctrl.EditBuf[0][5] = 0;

	sscanf(term->ncs_manager->local_ip, "%d.%d.%d.%d", &BufEx[0], &BufEx[1], &BufEx[2], &BufEx[3]);
	sprintf(Buf, "%03d.%03d.%03d.%03d", BufEx[0], BufEx[1], BufEx[2], BufEx[3]);
	memcpy(g_Seting_Ctrl.EditBuf[1], Buf, 15);
	g_Seting_Ctrl.EditBuf[1][15] = 0;

	sscanf(term->ncs_manager->server1_ip, "%d.%d.%d.%d", &BufEx[0], &BufEx[1], &BufEx[2], &BufEx[3]);
	sprintf(Buf, "%03d.%03d.%03d.%03d", BufEx[0], BufEx[1], BufEx[2], BufEx[3]);
	memcpy(g_Seting_Ctrl.EditBuf[2], Buf, 15);
	g_Seting_Ctrl.EditBuf[2][15] = 0;

	sscanf(term->ncs_manager->gw, "%d.%d.%d.%d", &BufEx[0], &BufEx[1], &BufEx[2], &BufEx[3]);
	sprintf(Buf, "%03d.%03d.%03d.%03d", BufEx[0], BufEx[1], BufEx[2], BufEx[3]);
	memcpy(g_Seting_Ctrl.EditBuf[3], Buf, 15);
	g_Seting_Ctrl.EditBuf[3][15] = 0;

	sscanf(term->ncs_manager->netmask, "%d.%d.%d.%d", &BufEx[0], &BufEx[1], &BufEx[2], &BufEx[3]);
	sprintf(Buf, "%03d.%03d.%03d.%03d", BufEx[0], BufEx[1], BufEx[2], BufEx[3]);
	memcpy(g_Seting_Ctrl.EditBuf[4], Buf, 15);
	g_Seting_Ctrl.EditBuf[4][15] = 0;
}

static void PaintTermSettingId(PRECT pRc, void* pData)
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
	PaintItemByOneW(0, 0, g_Seting_Ctrl.EditBuf[0][0]);
	PaintItemByOneW(1, 0, g_Seting_Ctrl.EditBuf[0][1]);
	PaintItemByOneW(2, 0, g_Seting_Ctrl.EditBuf[0][2]);
	PaintItemByOneW(3, 0, g_Seting_Ctrl.EditBuf[0][3]);
	PaintItemByOneW(4, 0, g_Seting_Ctrl.EditBuf[0][4]);
}

static void PaintTermSettingLocalIp(PRECT pRc, void* pData)
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

	PaintItemByOneW(0, 1, g_Seting_Ctrl.EditBuf[1][0]);
	PaintItemByOneW(1, 1, g_Seting_Ctrl.EditBuf[1][1]);
	PaintItemByOneW(2, 1, g_Seting_Ctrl.EditBuf[1][2]);
	PaintItemByOneW(3, 1, g_Seting_Ctrl.EditBuf[1][3]);
	PaintItemByOneW(4, 1, g_Seting_Ctrl.EditBuf[1][4]);
	PaintItemByOneW(5, 1, g_Seting_Ctrl.EditBuf[1][5]);
	PaintItemByOneW(6, 1, g_Seting_Ctrl.EditBuf[1][6]);
	PaintItemByOneW(7, 1, g_Seting_Ctrl.EditBuf[1][7]);
	PaintItemByOneW(8, 1, g_Seting_Ctrl.EditBuf[1][8]);
	PaintItemByOneW(9, 1, g_Seting_Ctrl.EditBuf[1][9]);
	PaintItemByOneW(10, 1, g_Seting_Ctrl.EditBuf[1][10]);
	PaintItemByOneW(11, 1, g_Seting_Ctrl.EditBuf[1][11]);
	PaintItemByOneW(12, 1, g_Seting_Ctrl.EditBuf[1][12]);
	PaintItemByOneW(13, 1, g_Seting_Ctrl.EditBuf[1][13]);
	PaintItemByOneW(14, 1, g_Seting_Ctrl.EditBuf[1][14]);
}
static void PaintTermSettingServerIp(PRECT pRc, void* pData)
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

	PaintItemByOneW(0, 2, g_Seting_Ctrl.EditBuf[2][0]);
	PaintItemByOneW(1, 2, g_Seting_Ctrl.EditBuf[2][1]);
	PaintItemByOneW(2, 2, g_Seting_Ctrl.EditBuf[2][2]);
	PaintItemByOneW(3, 2, g_Seting_Ctrl.EditBuf[2][3]);
	PaintItemByOneW(4, 2, g_Seting_Ctrl.EditBuf[2][4]);
	PaintItemByOneW(5, 2, g_Seting_Ctrl.EditBuf[2][5]);
	PaintItemByOneW(6, 2, g_Seting_Ctrl.EditBuf[2][6]);
	PaintItemByOneW(7, 2, g_Seting_Ctrl.EditBuf[2][7]);
	PaintItemByOneW(8, 2, g_Seting_Ctrl.EditBuf[2][8]);
	PaintItemByOneW(9, 2, g_Seting_Ctrl.EditBuf[2][9]);
	PaintItemByOneW(10, 2, g_Seting_Ctrl.EditBuf[2][10]);
	PaintItemByOneW(11, 2, g_Seting_Ctrl.EditBuf[2][11]);
	PaintItemByOneW(12, 2, g_Seting_Ctrl.EditBuf[2][12]);
	PaintItemByOneW(13, 2, g_Seting_Ctrl.EditBuf[2][13]);
	PaintItemByOneW(14, 2, g_Seting_Ctrl.EditBuf[2][14]);

}

static void PaintTermSettingGw(PRECT pRc, void* pData)
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

	PaintItemByOneW(0, 3, g_Seting_Ctrl.EditBuf[3][0]);
	PaintItemByOneW(1, 3, g_Seting_Ctrl.EditBuf[3][1]);
	PaintItemByOneW(2, 3, g_Seting_Ctrl.EditBuf[3][2]);
	PaintItemByOneW(3, 3, g_Seting_Ctrl.EditBuf[3][3]);
	PaintItemByOneW(4, 3, g_Seting_Ctrl.EditBuf[3][4]);
	PaintItemByOneW(5, 3, g_Seting_Ctrl.EditBuf[3][5]);
	PaintItemByOneW(6, 3, g_Seting_Ctrl.EditBuf[3][6]);
	PaintItemByOneW(7, 3, g_Seting_Ctrl.EditBuf[3][7]);
	PaintItemByOneW(8, 3, g_Seting_Ctrl.EditBuf[3][8]);
	PaintItemByOneW(9, 3, g_Seting_Ctrl.EditBuf[3][9]);
	PaintItemByOneW(10, 3, g_Seting_Ctrl.EditBuf[3][10]);
	PaintItemByOneW(11, 3, g_Seting_Ctrl.EditBuf[3][11]);
	PaintItemByOneW(12, 3, g_Seting_Ctrl.EditBuf[3][12]);
	PaintItemByOneW(13, 3, g_Seting_Ctrl.EditBuf[3][13]);
	PaintItemByOneW(14, 3, g_Seting_Ctrl.EditBuf[3][14]);

}

static void PaintTermSettingMask(PRECT pRc, void* pData)
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

	PaintItemByOneW(0, 4, g_Seting_Ctrl.EditBuf[4][0]);
	PaintItemByOneW(1, 4, g_Seting_Ctrl.EditBuf[4][1]);
	PaintItemByOneW(2, 4, g_Seting_Ctrl.EditBuf[4][2]);
	PaintItemByOneW(3, 4, g_Seting_Ctrl.EditBuf[4][3]);
	PaintItemByOneW(4, 4, g_Seting_Ctrl.EditBuf[4][4]);
	PaintItemByOneW(5, 4, g_Seting_Ctrl.EditBuf[4][5]);
	PaintItemByOneW(6, 4, g_Seting_Ctrl.EditBuf[4][6]);
	PaintItemByOneW(7, 4, g_Seting_Ctrl.EditBuf[4][7]);
	PaintItemByOneW(8, 4, g_Seting_Ctrl.EditBuf[4][8]);
	PaintItemByOneW(9, 4, g_Seting_Ctrl.EditBuf[4][9]);
	PaintItemByOneW(10, 4, g_Seting_Ctrl.EditBuf[4][10]);
	PaintItemByOneW(11, 4, g_Seting_Ctrl.EditBuf[4][11]);
	PaintItemByOneW(12, 4, g_Seting_Ctrl.EditBuf[4][12]);
	PaintItemByOneW(13, 4, g_Seting_Ctrl.EditBuf[4][13]);
	PaintItemByOneW(14, 4, g_Seting_Ctrl.EditBuf[4][14]);
}


static void PaintTermMacAddrInfo(PRECT pRc, void* pData)
{
	uint8_t buf[128];
	RECT rc;
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

	memcpy(&rc, pRc, sizeof(RECT));

	rc.left+=80;
	rc.top+=22;
	if(p->id == 0)
	{
		sprintf(buf, "(MAC:%s)", term->ncs_manager->mac_addr);
	}
	DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);

}

static void PaintTermConfigInfo(PRECT pRc, void* pData)
{
	uint8_t buf[128];
	RECT rc;
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

	memcpy(&rc, pRc, sizeof(RECT));

	rc.left+=80;
	rc.top+=20;
	if(p->id == 0)
	{
		sprintf(buf, "http://%s", term->ncs_manager->local_ip);
	}
	else if(p->id == 1)
	{
		sprintf(buf, "ftp://%s", term->ncs_manager->local_ip);
	}
	DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);

}

static void PaintTermConfigVersion(PRECT pRc, void* pData)
{
	uint8_t buf[128];
	RECT rc;

	memcpy(&rc, pRc, sizeof(RECT));

	rc.left+=80;
	rc.top+=15;
	sprintf(buf, "%s-%s.%s.%s.%s", term->version_info[0], term->version_info[1], term->version_info[2], term->version_info[3], term->version_info[4]);
	DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);
}

static void PaintTermConfigId(PRECT pRc, void* pData)
{
	uint8_t buf[128];
	RECT rc;

	memcpy(&rc, pRc, sizeof(RECT));

	rc.left+=80;
	rc.top+=10;
	sprintf(buf, "%05d", term->ncs_manager->id);
	DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);
}
static void PaintTermConfigSip(PRECT pRc, void* pData)
{
	uint8_t buf[128];
	RECT rc;
	uint8_t text_buf[20];

	memcpy(&rc, pRc, sizeof(RECT));

	rc.left+=80;
	rc.top+=5;
	sprintf(buf, "%s [%s]", term->version_info[5], term->sip_manager->registered()==0?get_str_from_id(UNREGISTED_ID,text_buf):get_str_from_id(REGISTED_ID,text_buf));
	DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);

}

static void PaintTermSettingEditWithCursor(int index)
{
	BITMAP * bitmap = NULL;

	PRECT pRc = &gui_com_setting_pager_widget0[index].rc;

	if(index == 0)
	{
		bitmap  = png_ha_res_find_by_name(gui_com_setting_pager_widget0[index].pData0);
	}
	else
	{
		bitmap  = png_ha_res_find_by_name(gui_com_setting_pager_widget0[index].pData1);
	}

	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", gui_com_setting_pager_widget0[index].pData0);
	}
	else
	{
		FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
	}
	if(!index)
	{
		PaintItemByOneWEx(0, index, g_Seting_Ctrl.EditBuf[index][0]);
		PaintItemByOneWEx(1, index, g_Seting_Ctrl.EditBuf[index][1]);
		PaintItemByOneWEx(2, index, g_Seting_Ctrl.EditBuf[index][2]);
		PaintItemByOneWEx(3, index, g_Seting_Ctrl.EditBuf[index][3]);
		PaintItemByOneWEx(4, index, g_Seting_Ctrl.EditBuf[index][4]);
	}
	else
	{
		PaintItemByOneWEx(0, index, g_Seting_Ctrl.EditBuf[index][0]);
		PaintItemByOneWEx(1, index, g_Seting_Ctrl.EditBuf[index][1]);
		PaintItemByOneWEx(2, index, g_Seting_Ctrl.EditBuf[index][2]);
		PaintItemByOneWEx(3, index, g_Seting_Ctrl.EditBuf[index][3]);
		PaintItemByOneWEx(4, index, g_Seting_Ctrl.EditBuf[index][4]);
		PaintItemByOneWEx(5, index, g_Seting_Ctrl.EditBuf[index][5]);
		PaintItemByOneWEx(6, index, g_Seting_Ctrl.EditBuf[index][6]);
		PaintItemByOneWEx(7, index, g_Seting_Ctrl.EditBuf[index][7]);
		PaintItemByOneWEx(8, index, g_Seting_Ctrl.EditBuf[index][8]);
		PaintItemByOneWEx(9, index, g_Seting_Ctrl.EditBuf[index][9]);
		PaintItemByOneWEx(10, index, g_Seting_Ctrl.EditBuf[index][10]);
		PaintItemByOneWEx(11, index, g_Seting_Ctrl.EditBuf[index][11]);
		PaintItemByOneWEx(12, index, g_Seting_Ctrl.EditBuf[index][12]);
		PaintItemByOneWEx(13, index, g_Seting_Ctrl.EditBuf[index][13]);
		PaintItemByOneWEx(14, index, g_Seting_Ctrl.EditBuf[index][14]);
	}
}



int GetSelItem(TYPE_GUI_AREA_HANDLE *p ,int x, int y) 
{ 
   int nRet = 0; 
   if(p==NULL) 
   { 
      return 0; 
   } 
    
   if(p->id == 0) 
   { 
      return 0; 
   } 
    
   nRet = (x - 95 - 118 -120) / 52; 
    
   return nRet; 
}

static void PaintTermSettingPrc(HWND hWnd, int message, int wParam, int lParam)
{
	int nSelItem = 0;

	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

	switch(message)
	{
		case MSG_LBUTTONDOWN:
			if(g_Seting_Ctrl.cur_sel_setting_foucs != p->id)
			{
				InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
				system_info.hdc = BeginPaint(hWnd);
				if(p->id == 0)
				{
					p->pReFlash(&p->rc, p->pData0);
				}
				else
				{
					p->pReFlash(&p->rc, p->pData1);
				}
				EndPaint(hWnd, system_info.hdc);
			}
			break;
		case MSG_LBUTTONUP:
			if(g_Seting_Ctrl.cur_sel_setting_foucs != p->id)
			{
				InvalidateRect_Ex(hWnd, &gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].rc, TRUE); //先清除区域, 再刷新
				system_info.hdc = BeginPaint(hWnd);
				if(g_Seting_Ctrl.cur_sel_setting_foucs == 0)
				{
					gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].pReFlash(&gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].rc,\
							gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].pData1);
				}
				else
				{
					gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].pReFlash(&gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].rc, \
							gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].pData0);
				}
				EndPaint(hWnd, system_info.hdc);
			}
				g_Seting_Ctrl.cur_sel_setting_foucs = p->id;

				nSelItem = GetSelItem(p,LOWORD(lParam), HIWORD (lParam)); 
					
				g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index = nSelItem; 
					
				switch(g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index) 
				{ 
				   case 0: 
					  	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=0; 
						if(p->id==0) 
						{ 
							g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=5; 
						}else 
						{ 
							g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=3; 
						} 
						g_Seting_Ctrl.cur_sel_setting_foucs_item_index=0; 
						break; 
				   case 1: 
					   	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=4; 
					   	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=7; 
						g_Seting_Ctrl.cur_sel_setting_foucs_item_index=4; 
						break; 
				   case 2: 
					   	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=8; 
					   	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=11; 
						g_Seting_Ctrl.cur_sel_setting_foucs_item_index=8; 
						break; 
				   case 3: 
					   	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=12; 
					   	g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=15; 
						g_Seting_Ctrl.cur_sel_setting_foucs_item_index=12; 
						break; 
				}

				/*
				g_Seting_Ctrl.cur_sel_setting_foucs_item_index = 0;
				g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index = 0;
				g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin = 0;
				if(g_Seting_Ctrl.cur_sel_setting_foucs==0)
				{
					g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=5;
				}
				else
				{
					g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=3;
				}
				*/
			
			break;
		case MSG_NCS_SETTING_KEY_VALUE:
			KeyValueProc(message, wParam, lParam);
			break;
	}
}

static void ResetFactoryOkProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

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
			reset_factory_settings();
			system_config_save();
			WEB_SYS_LOG(USRLOG_REBOOT);
			system("reboot");
			break;
	}
}

static void ResetFactoryCancleProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

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

			//SetTimer(hWnd, TIMERID_SETTING, 25);
			//KillTimer(hWnd, TIMERID_SETTING);
			//g_plist_widget = gui_com_setting_pager_widget0;
			g_Seting_Ctrl.cur_press_bar_id				   = 0;
			g_Seting_Ctrl.old_press_bar_id				   = 0;
			g_Seting_Ctrl.cur_sel_setting_foucs 		   = 0;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_index = 0;

			EnterWin(hWnd, SettingWinProc);
			break;
	}
}

static void PaintTermKeyPadProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

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
			SendNotifyMessage(system_info.main_hwnd, MSG_NCS_SETTING_KEY_VALUE, g_Seting_Ctrl.cur_sel_setting_foucs, p->id);
			break;
	}
}

static void MoveToxy(int x, int Max)
{
	/*
	   g_Seting_Ctrl.cur_sel_setting_foucs_item_index+=x;
	   if(g_Seting_Ctrl.cur_sel_setting_foucs_item_index > Max)
	   {
	   g_Seting_Ctrl.cur_sel_setting_foucs_item_index = 0;
	   }
	   else if(g_Seting_Ctrl.cur_sel_setting_foucs_item_index < 0)
	   {
	   g_Seting_Ctrl.cur_sel_setting_foucs_item_index = Max;
	   }
	   if(Max == 14)
	   {
	   if(g_Seting_Ctrl.cur_sel_setting_foucs_item_index== 11\
	   || g_Seting_Ctrl.cur_sel_setting_foucs_item_index== 7\
	   || g_Seting_Ctrl.cur_sel_setting_foucs_item_index== 3\
	   )
	   {
	   g_Seting_Ctrl.cur_sel_setting_foucs_item_index+=x;
	   }
	   }
	   */

	g_Seting_Ctrl.cur_sel_setting_foucs_item_index+=x;
	if(g_Seting_Ctrl.cur_sel_setting_foucs_item_index >= g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd)
	{
		g_Seting_Ctrl.cur_sel_setting_foucs_item_index = g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin;
	}
	else if(g_Seting_Ctrl.cur_sel_setting_foucs_item_index < g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin)
	{
		g_Seting_Ctrl.cur_sel_setting_foucs_item_index = g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin;
	}
}

static void MoveToxy2(int x, int Max)
{
	bool_t bReDraw = FALSE;
	g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index+=x;
	if(g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index<0)
	{
		//g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index = Max;
		g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index = 0;
		g_Seting_Ctrl.cur_sel_setting_foucs--;

		if
			(g_Seting_Ctrl.cur_sel_setting_foucs<0)
			{
				g_Seting_Ctrl.cur_sel_setting_foucs = 4;
				g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index=0;
			}

		bReDraw = TRUE;

	}

	if(g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index>Max)
	{
		g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index = 0;
		g_Seting_Ctrl.cur_sel_setting_foucs++;
		if
			(g_Seting_Ctrl.cur_sel_setting_foucs>=5)
			{
				g_Seting_Ctrl.cur_sel_setting_foucs = 0;
				g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index=0;
			}

		bReDraw = TRUE;
	}

	if(g_Seting_Ctrl.cur_sel_setting_foucs==0)
	{
		Max=0;
	}
	else
	{
		Max=3;
	}

	switch(g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index)
	{
		case 0:
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=0;
			if(Max==0)
			{
				g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=5;
			}
			else
			{
				g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=3;
			}
			g_Seting_Ctrl.cur_sel_setting_foucs_item_index=0;
			break;
		case 1:
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=4;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=7;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_index=4;
			break;
		case 2:
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=8;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=11;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_index=8;
			break;
		case 3:
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin=12;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd=15;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_index=12;
			break;
	}


	if(bReDraw==TRUE)
	{
		InvalidateRect_Ex(system_info.main_hwnd, &(gui_setting_retang[0].rc), TRUE); //先清除区域, 再刷新

		system_info.hdc = BeginPaint(system_info.main_hwnd);

		if(g_Seting_Ctrl.cur_sel_setting_foucs>0)
		{
			/*
			   void * pBak = gui_com_setting_pager_widget0[0].pData0;
			   gui_com_setting_pager_widget0[0].pData0 = gui_com_setting_pager_widget0[0].pData1;
			   gui_com_setting_pager_widget0[0].pData1 = pBak;
			   */
			gui_com_setting_pager_widget0[0].pData0 = &(g_pager_widget0_File[1][0]);

			gui_com_setting_pager_widget0[0].pData1 = &(g_pager_widget0_File[0][0]);

		}
		else
		{
			gui_com_setting_pager_widget0[0].pData0 = &(g_pager_widget0_File[0][0]);
			gui_com_setting_pager_widget0[0].pData1 =&(g_pager_widget0_File[1][0]);

		}




		HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_setting_retang, system_info.main_hwnd);
		HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_item, system_info.main_hwnd);

		HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_widget0, system_info.main_hwnd);


		EndPaint(system_info.main_hwnd, system_info.hdc);

	}





}

void NumberToStr(int nAddNum,int nMax)
{
	char sTemp[10];
	int k=0;
	int i=0,j=0;
	int nNum;
	memset(sTemp,0,10);
	for( i=g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin; i<g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd; i++)
	{
		sTemp[k] = g_Seting_Ctrl.EditBuf[g_Seting_Ctrl.cur_sel_setting_foucs][i];
		k++;
	}
	nNum = atoi(sTemp);
	if(nMax==14)
	{

		if(nNum>=100)
		{
			nNum = 0;
		}
	}
	else
	{
		if(nNum>9999)
		{
			nNum = 0;
		}
	}
	nNum = nNum *10 + nAddNum;
	if(nMax==14)
	{
		if(nNum>255)
		{
			nNum = 0;
		}
	}
	else
	{

		if(nNum>65000)
		{
			nNum = 0;
		}
	}
	for( j = g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd-1; j>= g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin; j--)
	{
		g_Seting_Ctrl.EditBuf[g_Seting_Ctrl.cur_sel_setting_foucs][j] = (nNum % 10) + '0';
		nNum = nNum / 10;
	}
}

static void KeyValueProc(int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

	switch(lParam)
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
			if(wParam == 0)
			{
				NumberToStr(lParam,4);//MoveToxy(1, 4);
			}
			else
			{
				NumberToStr(lParam,14);//MoveToxy(1, 14);
			}

			//  g_Seting_Ctrl.EditBuf[g_Seting_Ctrl.cur_sel_setting_foucs][g_Seting_Ctrl.cur_sel_setting_foucs_item_index] = (lParam+0x30);
			/*
			   if(wParam == 0)
			   {
			   MoveToxy(1, 4);
			   }
			   else
			   {
			   MoveToxy(1, 14);
			   }
			   */
			break;
		case 10:
			if(wParam == 0)
			{
				MoveToxy2(-1, 0);
			}
			else
			{
				MoveToxy2(-1, 3);
			}
			break;
		case 11:
			if(wParam == 0)
			{
				MoveToxy2(1, 0);
			}
			else
			{
				MoveToxy2(1, 3);
			}
			break;
		default:
			break;
	}
}

static void ButtonProc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_HANDLE *pTemp = &gui_com_setting_pager_item[g_Seting_Ctrl.old_press_bar_id];
	task_t task = get_system_task();
	RECT rc;
	if(p->id == pTemp->id)
	{
		return;
	}

	SetRect(&rc, 600, 75, 600+3*56, 252+57);
	InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
	switch(message)
	{
		case MSG_LBUTTONDOWN:
			InvalidateRect_Ex(hWnd, &rc, TRUE); //清除键盘区域
			system_info.hdc = BeginPaint(hWnd);
			if(p->id == 0)
			{
				p->pReFlash(&p->rc, p->pData0);
			}
			else
			{
				p->pReFlash(&p->rc, p->pData1);
			}
			g_Seting_Ctrl.cur_press_bar_id = p->id;
			switch(p->id)
			{
				case 0:

					SetListWidgetPtr(gui_com_setting_pager_widget0);
					g_Seting_Ctrl.cur_sel_setting_foucs                 = 0;
					g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin = 0;
					g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd   = 5;
					HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_keypad, hWnd);
					break;
				case 1:
					SetListWidgetPtr(gui_com_setting_pager_widget1);
					/*g_plist_widget = gui_com_setting_pager_widget0;*/
					break;
				case 2:
					SetListWidgetPtr(gui_com_setting_pager_widget2);
					break;
				default:
					g_plist_widget = gui_com_setting_pager_widget0;
					break;
			}
			EndPaint(hWnd, system_info.hdc);
			break;
		case MSG_LBUTTONUP:
			InvalidateRect_Ex(hWnd, &pTemp->rc, TRUE); //先清除区域, 再刷新
			InvalidateRect_Ex(hWnd, &gui_setting_retang[0].rc, TRUE); //先清除区域, 再刷新
			system_info.hdc = BeginPaint(hWnd);
			g_pWinPrivatePrt = pTemp;
			if(pTemp->id == 0)
			{
				pTemp->pReFlash(&pTemp->rc, pTemp->pData1);
			}
			else
			{
				pTemp->pReFlash(&pTemp->rc, pTemp->pData0);
			}
			if(p->id == 0)
			{
				gui_setting_retang[0].pReFlash(&gui_setting_retang[0].rc, gui_setting_retang[0].pData0);
			}
			else if(p->id == 1)
			{
				gui_setting_retang[0].pReFlash(&gui_setting_retang[0].rc, gui_setting_retang[0].pData1);
			}
			else if(p->id==2)
			{
				gui_setting_retang[0].pReFlash(&gui_setting_retang[0].rc, "setting_retang.png");
			}


			g_pWinPrivatePrt = p;
			if(p->id == 0)
			{
				p->pReFlash(&p->rc, p->pData0);
			}
			else
			{
				p->pReFlash(&p->rc, p->pData1);
			}
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)g_plist_widget, hWnd);
			EndPaint(hWnd, system_info.hdc);
			g_Seting_Ctrl.old_press_bar_id = g_Seting_Ctrl.cur_press_bar_id;
			break;
	}
}

static void ButtonProcEx(HWND hWnd, int message, int wParam, int lParam)
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
			InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
			system_info.hdc = BeginPaint(hWnd);
			p->pReFlash(&p->rc, p->pData0);
			EndPaint(hWnd, system_info.hdc);
			KillTimer(hWnd, TIMERID_SETTING);
			EnterWin(hWnd, SaveInfoWinProc);
			break;
	}
}


int SettingWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap = NULL;

	char Buf[16];
	int BufEx[4];

	if(g_firstEnter)
	{
		/*SetListWidgetPtr((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_widget0);*/
		SetTimer(hWnd, TIMERID_SETTING, 25);
		g_firstEnter = FALSE;
		CopyInfoToEditBar();
		g_plist_widget = gui_com_setting_pager_widget0;
		g_Seting_Ctrl.cur_press_bar_id                 = 0;
		g_Seting_Ctrl.old_press_bar_id				   = 0;
		g_Seting_Ctrl.cur_sel_setting_foucs            = 0;
		g_Seting_Ctrl.cur_sel_setting_foucs_item_index = 4;
		g_Seting_Ctrl.cur_sel_setting_foucs_Sec_index = 0;
		g_Seting_Ctrl.cur_sel_setting_foucs_item_indexBegin = 0;
		g_Seting_Ctrl.cur_sel_setting_foucs_item_indexEnd = 5;
	}
	switch(message)
	{
		case MSG_CREATE:
			system_info.hdc = BeginPaint(hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_statuebar_item, hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_setting_retang, hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_item, hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_setting_other_array, hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_widget0, hWnd);
			HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_keypad, hWnd);
			/*HandleCreateExx((TYPE_GUI_AREA_HANDLE*)gui_phone_book_item, hWnd, g_phone_book_ctrl.bar_cout);*/
			/*HandleCreateEx((TYPE_GUI_AREA_HANDLE*)gui_phone_book_other, hWnd);*/
			EndPaint(hWnd, system_info.hdc);
			g_plist_widget = gui_com_setting_pager_widget0;
			g_Seting_Ctrl.cur_press_bar_id                 = 0;
			g_Seting_Ctrl.cur_sel_setting_foucs            = 0;
			g_Seting_Ctrl.cur_sel_setting_foucs_item_index = 4;
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
			HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_item, hWnd, message, wParam, lParam);
			HandlePageProcXXXX((TYPE_GUI_AREA_HANDLE*)g_plist_widget, hWnd, message, wParam, lParam);
			if(g_Seting_Ctrl.cur_press_bar_id == 0)
			{
				HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_com_setting_pager_keypad, hWnd, message, wParam, lParam);
			}
			HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_setting_other_array, hWnd, message, wParam, lParam);
			break;

		case MSG_NCS_SETTING_KEY_VALUE:
			if(g_Seting_Ctrl.cur_press_bar_id == 0)
			{
				HandlePageProc((TYPE_GUI_AREA_HANDLE*)g_plist_widget, hWnd, message, wParam, lParam);
			}
			break;
		case MSG_TIMER:
			g_Seting_Ctrl.flagDisp = !g_Seting_Ctrl.flagDisp;
			if(g_Seting_Ctrl.cur_press_bar_id == 0)
			{
				InvalidateRect_Ex(hWnd, &gui_com_setting_pager_widget0[g_Seting_Ctrl.cur_sel_setting_foucs].rc, TRUE); //先清除区域, 再刷新
				system_info.hdc = BeginPaint(hWnd);
				PaintTermSettingEditWithCursor(g_Seting_Ctrl.cur_sel_setting_foucs);
				EndPaint(hWnd, system_info.hdc);
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
		case MSG_DESTROY:
			return 0;
		default:
			break;
	}
	return 1;
}

static void reset_factory_settings(void)
{
	system("cp /var/www/ini/ini_back/alone_cfg.txt /var/www/ini -f");
	system("cp /var/www/ini/ini_back/board_cfg.txt /var/www/ini -f");
	system("cp /mnt/nand1-2/ini/ini_bak/sys_cfg.txt /var/www/ini -f");
}

char* delete0(char* sSrc,char* sOut)
{
	char* sRet = sOut;
	int bFlag  = 1;
	while(*sSrc != 0)
	{
		if(bFlag == 0)
		{
			*sOut = *sSrc;
			sOut++;
		}
		else
		{
			if(*sSrc == '0')
			{
			}
			else
			{
				if(*sSrc=='.' && bFlag==1)
				{
					*sOut = '0';
					sOut++;
				}
				*sOut = *sSrc;
				sOut++;
				bFlag = 0;
			}
		}

		if(*sSrc=='.')
		{
			bFlag = 1;
		}
		sSrc++;
	}

	if(bFlag==1)
	{
		*sOut = '0';
	}

	return sRet;
}

void put_net_opt()
{
	LpConfig *lpconfig;
	char tmp[128];
	int i;

	lpconfig = lp_config_new(INI_CONFIG_FILE);
	if (lpconfig == NULL)
	{
		SPON_LOG_ERR("lp_config_new failed:%s\n", strerror(errno));
		return -1;
	}

	/*
	   lp_config_set_string(lpconfig, "terminal", "id", g_Seting_Ctrl.EditBuf[0]);
	   lp_config_set_string(lpconfig, "terminal", "ip", g_Seting_Ctrl.EditBuf[1]);
	   lp_config_set_string(lpconfig, "server_cfg", "ip_server1", g_Seting_Ctrl.EditBuf[2]);
	   lp_config_set_string(lpconfig, "terminal", "gate", g_Seting_Ctrl.EditBuf[3]);
	   lp_config_set_string(lpconfig, "terminal", "mask", g_Seting_Ctrl.EditBuf[4]);
	   */
	memset(tmp,0,128);
	lp_config_set_string(lpconfig, "terminal", "id", delete0(g_Seting_Ctrl.EditBuf[0],tmp));

	memset(tmp,0,128);
	lp_config_set_string(lpconfig, "terminal", "ip", delete0(g_Seting_Ctrl.EditBuf[1],tmp));

	memset(tmp,0,128);
	lp_config_set_string(lpconfig, "server_cfg", "ip_server1", delete0(g_Seting_Ctrl.EditBuf[2],tmp));

	memset(tmp,0,128);
	lp_config_set_string(lpconfig, "terminal", "gate", delete0(g_Seting_Ctrl.EditBuf[3],tmp));

	memset(tmp,0,128);
	lp_config_set_string(lpconfig, "terminal", "mask", delete0(g_Seting_Ctrl.EditBuf[4],tmp));

	lp_config_sync(lpconfig);
	lp_config_destroy(lpconfig);
}



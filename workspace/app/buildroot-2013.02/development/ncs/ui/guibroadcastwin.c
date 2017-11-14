#include  "guibroadcastwin.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guibroadcastwin_button_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_time_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_control_start_pause_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_tab_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_input_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_input_texture(broadcast_data_struct* ptr, char c);
static void guibroadcastwin_file_win_return(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_file_bar_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_delete_texture(broadcast_data_struct* ptr);
static void guibroadcastwin_texture_display(broadcast_data_struct* ptr, PRECT pRc, int idx);
static void guibroadcastwin_file_list_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_subbg_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_pagenr_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_time_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_control_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_keypad_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_term_select_proc(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_btn_func(HWND hWnd, int message, int wParam, int lParam);
static int guibroadcastwin_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void guibroadcastwin_control_start_pause_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_tab_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_file_select_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_file_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_start_all_area_outgoing(void);
static void guibroadcastwin_area_item_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_area_btn_func(HWND hWnd, int message, int wParam, int lParam);
static void guibroadcastwin_area_list_display(PRECT pRc, void* pData0, void* pData1, int enable);
static void guibroadcastwin_area_list_init(void);
static int guibroadcastwin_area_list_is_valide(void);
static int guibroadcastwin_area_list_check(int *dst, int len);

static broadcast_data_struct sg_broadcast_data;


TYPE_GUI_AREA_PTR gui_broadcast_widget_tab[] =
{
	{1 , 0  , {96     , 60 , 96+125   , 60+34}  , "bc_tab_term_down.png" ,	"bc_tab_term_up.png"    , guibroadcastwin_tab_display    , guibroadcastwin_tab_func} ,
	{0 , 1  , {96+125 , 60 , 96+125*2 , 60+34}  , "bc_tab_file_down.png" ,	"bc_tab_file_up.png"    ,  guibroadcastwin_tab_display    , guibroadcastwin_tab_func} ,
	/*{0 , 2	, {96+125*2 , 60 , 96+125*3 , 60+34}	, "tab_area_down.png" ,	"tab_area_up.png"	,  guibroadcastwin_tab_display	  , guibroadcastwin_tab_func} ,*/
	{0 , 2  , {92     , 90 , 92+616   , 90+368} , "bc_sub_background.png" , NULL                   , guibroadcastwin_button_display , NULL}                     , 
	{0 , -1 , {0      , 0  , 0        , 0  }    , NULL                    , NULL                   , NULL                           , NULL}                     , 
};

#define START_X				115
#define START_Y				110
#define BC_SUB_BG_W		    418	
#define BC_SUB_BG_H			330
#define FILE_LIST_X			START_X
#define FILE_LIST_Y			(START_Y + 5)
#define FILE_LIST_W			40
#define FILE_LIST_H			45 
#define FILE_PREV_ARROW_X	(FILE_LIST_X + 100)
#define FILE_NEXT_ARROW_X	(FILE_PREV_ARROW_X + 200)
#define FILE_CLEAR_BTN_X	(FILE_LIST_X + 160)
#define FILE_ALL_BTN_X		(FILE_LIST_X + 274)

TYPE_GUI_AREA_PTR gui_broadcast_widget_file[] =
{
	{0 , 0  , {START_X               , START_Y                        , START_X+BC_SUB_BG_W   , START_Y+BC_SUB_BG_H             } , NULL             , NULL               , guibroadcastwin_subbg_display     , NULL                            } ,
	{0 , 1  , {FILE_LIST_X           , FILE_LIST_Y                    , FILE_LIST_X+60+358    , FILE_LIST_Y+FILE_LIST_H         } , "file_text.png"  , NULL               , guibroadcastwin_file_list_display , guibroadcastwin_file_bar_func   } ,
	{0 , 2  , {FILE_LIST_X           , FILE_LIST_Y+FILE_LIST_H        , FILE_LIST_X+60+358    , FILE_LIST_Y+FILE_LIST_H*2       } , "file_text.png"  , NULL               , guibroadcastwin_file_list_display , guibroadcastwin_file_bar_func   } ,
	{0 , 3  , {FILE_LIST_X           , FILE_LIST_Y+FILE_LIST_H*2      , FILE_LIST_X+60+358    , FILE_LIST_Y+FILE_LIST_H*3       } , "file_text.png"  , NULL               , guibroadcastwin_file_list_display , guibroadcastwin_file_bar_func   } ,
	{0 , 4  , {FILE_LIST_X           , FILE_LIST_Y+FILE_LIST_H*3      , FILE_LIST_X+60+358    , FILE_LIST_Y+FILE_LIST_H*4       } , "file_text.png"  , NULL               , guibroadcastwin_file_list_display , guibroadcastwin_file_bar_func   } ,
	{0 , 5  , {FILE_LIST_X           , FILE_LIST_Y+FILE_LIST_H*4      , FILE_LIST_X+60+358    , FILE_LIST_Y+FILE_LIST_H*5       } , "file_text.png"  , NULL               , guibroadcastwin_file_list_display , guibroadcastwin_file_bar_func   } ,
	{0 , 6  , {FILE_LIST_X           , FILE_LIST_Y+FILE_LIST_H*5      , FILE_LIST_X+60+358    , FILE_LIST_Y+FILE_LIST_H*6       } , "file_text.png"  , NULL               , guibroadcastwin_file_list_display , guibroadcastwin_file_bar_func   } ,
	{0 , 7  , {FILE_PREV_ARROW_X     , FILE_LIST_Y+FILE_LIST_H*6 + 10 , FILE_PREV_ARROW_X+40  , FILE_LIST_Y+FILE_LIST_H*6+39    } , "pre_arrow.png"  , "pre_arrow_2.png"  , guibroadcastwin_button_display    , guibroadcastwin_file_btn_func   } ,
	{0 , 8  , {FILE_NEXT_ARROW_X     , FILE_LIST_Y+FILE_LIST_H*6 + 10 , FILE_NEXT_ARROW_X+40  , FILE_LIST_Y+FILE_LIST_H*6+39    } , "next_arrow.png" , "next_arrow_2.png" , guibroadcastwin_button_display    , guibroadcastwin_file_btn_func   } ,
	{0 , 9  , {FILE_PREV_ARROW_X+115 , FILE_LIST_Y+FILE_LIST_H*6+18   , FILE_PREV_ARROW_X+145 , FILE_LIST_Y+FILE_LIST_H*6+18+20 } , NULL             , NULL               , guibroadcastwin_pagenr_display    , NULL                            } ,
	{0 , -1 , {0                     , 0                              , 0                     , 0                               } , NULL             , NULL               , NULL                              , NULL                            } ,
};

#define AREA_SELECT_X		FILE_LIST_X
#define AREA_SELECT_W		45

#define AREA_LIST_X			FILE_LIST_X + 60
#define AREA_LIST_W			356

TYPE_GUI_AREA_PTR gui_broadcast_widget_area[] =
{
	{0 , 0  , {START_X               , START_Y                        , START_X+BC_SUB_BG_W         , START_Y+BC_SUB_BG_H             } , NULL                , NULL               , guibroadcastwin_subbg_display     , NULL                           } ,
	{0 , 1  , {AREA_SELECT_X         , FILE_LIST_Y                    , AREA_SELECT_X+AREA_SELECT_W , FILE_LIST_Y+FILE_LIST_H         } , "file_unselect.png" , "file_select.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_item_func } ,
	{0 , 2  , {AREA_SELECT_X         , FILE_LIST_Y+FILE_LIST_H        , AREA_SELECT_X+AREA_SELECT_W , FILE_LIST_Y+FILE_LIST_H*2       } , "file_unselect.png" , "file_select.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_item_func } ,
	{0 , 3  , {AREA_SELECT_X         , FILE_LIST_Y+FILE_LIST_H*2      , AREA_SELECT_X+AREA_SELECT_W , FILE_LIST_Y+FILE_LIST_H*3       } , "file_unselect.png" , "file_select.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_item_func } ,
	{0 , 4  , {AREA_SELECT_X         , FILE_LIST_Y+FILE_LIST_H*3      , AREA_SELECT_X+AREA_SELECT_W , FILE_LIST_Y+FILE_LIST_H*4       } , "file_unselect.png" , "file_select.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_item_func } ,
	{0 , 5  , {AREA_SELECT_X         , FILE_LIST_Y+FILE_LIST_H*4      , AREA_SELECT_X+AREA_SELECT_W , FILE_LIST_Y+FILE_LIST_H*5       } , "file_unselect.png" , "file_select.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_item_func } ,
	{0 , 6  , {AREA_SELECT_X         , FILE_LIST_Y+FILE_LIST_H*5      , AREA_SELECT_X+AREA_SELECT_W , FILE_LIST_Y+FILE_LIST_H*6       } , "file_unselect.png" , "file_select.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_item_func } ,
	{0 , 7  , {AREA_LIST_X           , FILE_LIST_Y                    , AREA_LIST_X+AREA_LIST_W     , FILE_LIST_Y+FILE_LIST_H         } , "bc_area_info.png"  , NULL               , guibroadcastwin_area_list_display , NULL                           } ,
	{0 , 8  , {AREA_LIST_X           , FILE_LIST_Y+FILE_LIST_H        , AREA_LIST_X+AREA_LIST_W     , FILE_LIST_Y+FILE_LIST_H*2       } , "bc_area_info.png"  , NULL               , guibroadcastwin_area_list_display , NULL                           } ,
	{0 , 9  , {AREA_LIST_X           , FILE_LIST_Y+FILE_LIST_H*2      , AREA_LIST_X+AREA_LIST_W     , FILE_LIST_Y+FILE_LIST_H*3       } , "bc_area_info.png"  , NULL               , guibroadcastwin_area_list_display , NULL                           } ,
	{0 , 10 , {AREA_LIST_X           , FILE_LIST_Y+FILE_LIST_H*3      , AREA_LIST_X+AREA_LIST_W     , FILE_LIST_Y+FILE_LIST_H*4       } , "bc_area_info.png"  , NULL               , guibroadcastwin_area_list_display , NULL                           } ,
	{0 , 11 , {AREA_LIST_X           , FILE_LIST_Y+FILE_LIST_H*4      , AREA_LIST_X+AREA_LIST_W     , FILE_LIST_Y+FILE_LIST_H*5       } , "bc_area_info.png"  , NULL               , guibroadcastwin_area_list_display , NULL                           } ,
	{0 , 12 , {AREA_LIST_X           , FILE_LIST_Y+FILE_LIST_H*5      , AREA_LIST_X+AREA_LIST_W     , FILE_LIST_Y+FILE_LIST_H*6       } , "bc_area_info.png"  , NULL               , guibroadcastwin_area_list_display , NULL                           } ,
	{0 , 13 , {FILE_PREV_ARROW_X     , FILE_LIST_Y+FILE_LIST_H*6 + 10 , FILE_PREV_ARROW_X+40        , FILE_LIST_Y+FILE_LIST_H*6+39    } , "pre_arrow.png"     , "pre_arrow_2.png"  , guibroadcastwin_button_display    , guibroadcastwin_area_btn_func  } ,
	{0 , 14 , {FILE_NEXT_ARROW_X     , FILE_LIST_Y+FILE_LIST_H*6 + 10 , FILE_NEXT_ARROW_X+40        , FILE_LIST_Y+FILE_LIST_H*6+39    } , "next_arrow.png"    , "next_arrow_2.png" , guibroadcastwin_button_display    , guibroadcastwin_area_btn_func  } ,
	{0 , 15 , {FILE_PREV_ARROW_X+115 , FILE_LIST_Y+FILE_LIST_H*6+18   , FILE_PREV_ARROW_X+145       , FILE_LIST_Y+FILE_LIST_H*6+18+20 } , NULL                , NULL               , guibroadcastwin_pagenr_display    , NULL                           } ,
	{0 , -1 , {0                     , 0                              , 0                           , 0                               } , NULL                , NULL               , NULL                              , NULL                           } ,
};


#define KEY_X            START_X
#define KEY_Y            165+10
#define KEY_W            95
#define KEY_H            65
#define KEY_X_OFFSET     68
#define KEY_Y_OFFSET     KEY_H
#define KEY_BTN_Y        KEY_Y+5
#define KEYPAD_BTN_START 13
#define KEYPAD_BTN_END   16
#define KEY_BTN_R        50

TYPE_GUI_AREA_PTR gui_broadcast_widget_keypad[] =
{
	{0                         , 0  , {START_X                 , START_Y                       , START_X+BC_SUB_BG_W                  , START_Y+BC_SUB_BG_H              } , NULL                      , NULL                        , guibroadcastwin_subbg_display  , NULL                             } ,
	{0                         , 1  , {KEY_X+KEY_BTN_R+KEY_W*2 , KEY_Y+KEY_Y_OFFSET*3          , KEY_X+KEY_BTN_R+KEY_W*2+KEY_X_OFFSET , KEY_Y+KEY_Y_OFFSET*3+KEY_H       } , "input_up_0.png"          , "input_dp_0.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 0
	{0                         , 2  , {KEY_X+KEY_BTN_R+KEY_W   , KEY_Y                         , KEY_X+KEY_BTN_R+KEY_W+KEY_X_OFFSET   , KEY_Y+KEY_H                      } , "input_up_1.png"          , "input_dp_1.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 1
	{0                         , 3  , {KEY_X+KEY_BTN_R+KEY_W*2 , KEY_Y                         , KEY_X+KEY_BTN_R+KEY_W*2+KEY_X_OFFSET , KEY_Y+KEY_H                      } , "input_up_2.png"          , "input_dp_2.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 2
	{0                         , 4  , {KEY_X+KEY_BTN_R+KEY_W*3 , KEY_Y                         , KEY_X+KEY_BTN_R+KEY_W*3+KEY_X_OFFSET , KEY_Y+KEY_H                      } , "input_up_3.png"          , "input_dp_3.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 3
	{0                         , 5  , {KEY_X+KEY_BTN_R+KEY_W   , KEY_Y+KEY_Y_OFFSET            , KEY_X+KEY_BTN_R+KEY_W+KEY_X_OFFSET   , KEY_Y+KEY_Y_OFFSET+KEY_H         } , "input_up_4.png"          , "input_dp_4.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 4
	{0                         , 6  , {KEY_X+KEY_BTN_R+KEY_W*2 , KEY_Y+KEY_Y_OFFSET            , KEY_X+KEY_BTN_R+KEY_W*2+KEY_X_OFFSET , KEY_Y+KEY_Y_OFFSET+KEY_H         } , "input_up_5.png"          , "input_dp_5.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 5
	{0                         , 7  , {KEY_X+KEY_BTN_R+KEY_W*3 , KEY_Y+KEY_Y_OFFSET            , KEY_X+KEY_BTN_R+KEY_W*3+KEY_X_OFFSET , KEY_Y+KEY_Y_OFFSET+KEY_H         } , "input_up_6.png"          , "input_dp_6.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 6
	{0                         , 8  , {KEY_X+KEY_BTN_R+KEY_W   , KEY_Y+KEY_Y_OFFSET*2          , KEY_X+KEY_BTN_R+KEY_W+KEY_X_OFFSET   , KEY_Y+KEY_Y_OFFSET*2+KEY_H       } , "input_up_7.png"          , "input_dp_7.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 7
	{0                         , 9  , {KEY_X+KEY_BTN_R+KEY_W*2 , KEY_Y+KEY_Y_OFFSET*2          , KEY_X+KEY_BTN_R+KEY_W*2+KEY_X_OFFSET , KEY_Y+KEY_Y_OFFSET*2+KEY_H       } , "input_up_8.png"          , "input_dp_8.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 8
	{0                         , 10 , {KEY_X+KEY_BTN_R+KEY_W*3 , KEY_Y+KEY_Y_OFFSET*2          , KEY_X+KEY_BTN_R+KEY_W*3+KEY_X_OFFSET , KEY_Y+KEY_Y_OFFSET*2+KEY_H       } , "input_up_9.png"          , "input_dp_9.png"            , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // 9
	{0                         , 11 , {KEY_X+KEY_BTN_R+KEY_W   , KEY_Y+KEY_Y_OFFSET*3          , KEY_X+KEY_BTN_R+KEY_W+KEY_X_OFFSET   , KEY_Y+KEY_Y_OFFSET*3+KEY_H       } , "input_up_mi.png"         , "input_dp_mi.png"           , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // *
	{0                         , 12 , {KEY_X+KEY_BTN_R+KEY_W*3 , KEY_Y+KEY_Y_OFFSET*3          , KEY_X+KEY_BTN_R+KEY_W*3+KEY_X_OFFSET , KEY_Y+KEY_Y_OFFSET*3+KEY_H       } , "input_up_jh.png"         , "input_dp_jh.png"           , guibroadcastwin_button_display , guibroadcastwin_keypad_func      } , // #
	#undef KEY_Y_OFFSET
	#define KEY_Y_OFFSET 40
	#undef KEY_BTN_Y
	#define KEY_BTN_Y KEY_Y -5
	{1                         , 13 , {KEY_X                   , KEY_BTN_Y                     , KEY_X+128                            , KEY_BTN_Y+56                     } , "term_down.png"           , "term_up.png"               , guibroadcastwin_button_display , guibroadcastwin_term_select_proc } , // 终端广播
	{0                         , 14 , {KEY_X                   , KEY_BTN_Y+KEY_Y_OFFSET+30     , KEY_X+128                            , KEY_BTN_Y+KEY_Y_OFFSET+56+30     } , "area_down.png"           , "area_up.png"               , guibroadcastwin_button_display , guibroadcastwin_term_select_proc } , // 分区广播
	{0                         , 15 , {KEY_X                   , KEY_BTN_Y+KEY_Y_OFFSET*2+30*2 , KEY_X+128                            , KEY_BTN_Y+KEY_Y_OFFSET*2+56+30*2 } , "select_area_list_up.png" , "select_area_list_down.png" , guibroadcastwin_button_display , guibroadcastwin_term_select_proc } , // 选择分区表
	{0                         , 16 , {KEY_X                   , KEY_BTN_Y+KEY_Y_OFFSET*3+30*3 , KEY_X+128                            , KEY_BTN_Y+KEY_Y_OFFSET*3+56+30*3 } , "all_area_down.png"       , "all_area_up.png"           , guibroadcastwin_button_display , guibroadcastwin_term_select_proc } , // 全区广播
	{0                         , 17 , {KEY_X+420-75            , KEY_Y-40-10                   , KEY_X+420-25                         , KEY_Y-10-10                      } , "pre_arrow.png"           , "pre_arrow_2.png"           , guibroadcastwin_button_display , guibroadcastwin_term_select_proc } , // 返回
	{0                         , 18 , {KEY_X                   , KEY_Y-55-10                   , KEY_X+420-75                         , KEY_Y-5-10                       } , "bc_input_bar.png"        , NULL                        , guibroadcastwin_input_display  , NULL                             } , // 键盘条 guibroadcastwin_term_select_proc
	{0                         , -1 , {0                       , 0                             , 0                                    , 0                                } , NULL                      , NULL                        , NULL                           , NULL                             } ,
	#undef KEY_Y_OFFSET
	#define KEY_Y_OFFSET KEY_H
	#undef KEY_BTN_Y
	#define KEY_BTN_Y KEY_Y +5
};

#define WIDGET_CTR_Y		200
#define WIDGET_CTR_OFFSET	80
#define WIDGET_CTR_X		560

TYPE_GUI_AREA_PTR gui_broadcast_widget_control[] =
{
    {0 , 0  , {WIDGET_CTR_X - 15 , 110                              , WIDGET_CTR_X+35*4 , 110+55                              } , "123456"             , NULL               , guibroadcastwin_time_display                , NULL                                     } , //guibroadcastwin_time_func
    {0 , 1  , {WIDGET_CTR_X      , WIDGET_CTR_Y+WIDGET_CTR_OFFSET*2 , WIDGET_CTR_X+122  , WIDGET_CTR_Y+WIDGET_CTR_OFFSET*2+56 } , "return_up.png"  , "return_down.png"  , guibroadcastwin_button_display              , guibroadcastwin_control_func             } ,
    {0 , 2  , {WIDGET_CTR_X      , WIDGET_CTR_Y                     , WIDGET_CTR_X+122  , WIDGET_CTR_Y+56                     } , "start_up.png"   , "start_down.png"   , guibroadcastwin_control_start_pause_display , guibroadcastwin_control_start_pause_func } ,
    {0 , 3  , {WIDGET_CTR_X      , WIDGET_CTR_Y+WIDGET_CTR_OFFSET   , WIDGET_CTR_X+122  , WIDGET_CTR_Y+WIDGET_CTR_OFFSET+56   } , "bc_stop_up.png" , "bc_stop_down.png" , guibroadcastwin_button_display              , guibroadcastwin_control_func             } ,
    {0 , -1 , {0                 , 0                                , 0                 , 0                                   } , NULL             , NULL               , NULL                                        , NULL                                     } ,
};

static TYPE_GUI_AREA_PTR *guibroadcastwin_terminal_items[] =
{
	gui_broadcast_widget_tab,
	gui_broadcast_widget_keypad,
	gui_broadcast_widget_control,	
};

static TYPE_GUI_AREA_PTR *guibroadcastwin_file_items[] =
{
	gui_broadcast_widget_tab,
	gui_broadcast_widget_file,
	gui_broadcast_widget_control,
};

static TYPE_GUI_AREA_PTR *guibroadcastwin_area_list_items[] =
{
	gui_broadcast_widget_tab,
	gui_broadcast_widget_area,
	gui_broadcast_widget_control,
};

static void guibroadcastwin_bootup_out(HWND hWnd)
{
	int i = 0;
	TYPE_GUI_AREA_PTR *pre = NULL;

	memset(&sg_broadcast_data,0,sizeof(sg_broadcast_data));
	sg_broadcast_data.type = BC_TERMINAL;
	set_system_task(TASK_NCS_TERM_BROADCAST_OUTGOING);
	
	for(i = 0; i<4; i++)
	{
		pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,i+ KEYPAD_BTN_START);
		SetWidgetEnable(pre, (i == BC_TERMINAL));
	}

	pre = HandleWidgetInfo(guibroadcastwin_terminal_items,2,1);
	SetWidgetEnable(pre, 1);
	
	sg_broadcast_data.status = BC_START;
	//guibroadcastwin_start_all_area_outgoing();
	SetTimer(hWnd, TIMERID_GUIBROADCAST_TIME, 50);
}


static void guibroadcastwin_data_reset(void)
{
	int i = 0;
	TYPE_GUI_AREA_PTR *pre = NULL;

	memset(&sg_broadcast_data,0,sizeof(sg_broadcast_data));
	sg_broadcast_data.type = BC_TERMINAL;
	//set_system_task(TASK_NCS_TERM_BROADCAST_OUTGOING);
	
	for(i = 0; i<4; i++)
	{
		pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,i+ KEYPAD_BTN_START);
		SetWidgetEnable(pre, (i == BC_TERMINAL));
	}

	for(i = 0; i < 6; i++)
	{
		pre = HandleWidgetInfo(guibroadcastwin_file_items,1,i+1);
		SetWidgetEnable(pre, 0);
	}
}

static void guibroadcastwin_emergency_reset(HWND hWnd)
{
	int i = 0;
	TYPE_GUI_AREA_PTR *pre = NULL;

	memset(&sg_broadcast_data, 0, sizeof(sg_broadcast_data));
	sg_broadcast_data.type = BC_ALL_TERMINAL;
	set_system_task(TASK_NCS_ALL_AREA_BROADCAST_OUTGOING);
	
	for(i = 0; i<3; i++)
	{
		pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,i+ KEYPAD_BTN_START);
		SetWidgetEnable(pre, (i==BC_ALL_TERMINAL));
	}

	pre = HandleWidgetInfo(guibroadcastwin_terminal_items,2,1);
	SetWidgetEnable(pre, 1);
	
	sg_broadcast_data.status = BC_START;
	guibroadcastwin_start_all_area_outgoing();
	SetTimer(hWnd, TIMERID_GUIBROADCAST_TIME, 50);
}

void guibroadcastwin_newscreen(HWND hWnd, int scrn_id)
{
	int pre_id = get_screen_id();

	if(pre_id < NCS_EMERGENCY_SCREEN_ID || pre_id > NCS_BROADCAST_SCREEN_ID)
	{
		KillTimer(hWnd, TIMERID_GUIBROADCAST_TIME);

		if(scrn_id == NCS_EMERGENCY_SCREEN_ID)
		{
			SPON_LOG_INFO("guibroadcastwin_newscreen NCS_EMERGENCY_SCREEN_ID\n");
			guibroadcastwin_emergency_reset(hWnd);
		}
		else if(scrn_id == NCS_BOOTUP_BC_SCREEN_ID)
		{
			guibroadcastwin_bootup_out(hWnd);
		}		
		else
		{
			guibroadcastwin_data_reset();
		}
		
		EnterWin(hWnd, guibroadcastwin_screen_proc);
	}
}


static int guibroadcastwin_str_parse(int *data, int max_len, char delimiter)
{
    char *p_start = sg_broadcast_data.input;
    char *p_end = sg_broadcast_data.input;
    char tmp[128];
    int j = 0;

    while (1)
    {
        if (*p_end == delimiter)
        {
            if (p_end != p_start)
            {
                memset(tmp, 0, sizeof(tmp));
                memcpy(tmp, p_start, p_end-p_start);
                data[j++] = atoi(tmp);
                if (j >= max_len)
				{
                    return j;
				}
            }
            p_end++;
            p_start = p_end;
        }
        else if (*p_end >= '0' && *p_end <= '9')
        {
            p_end++;
        }
        else
        {
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

static void guibroadcastwin_stop(HWND hWnd)
{
    struct broadcast *bc = term->ncs_manager->bc;
    struct ncs_manager *ncs_manager = term->ncs_manager;
    struct audio_manager *audio_manager = term->audio_manager;

	if (bc->bc_outgoing(bc))
	{
		WEB_SYS_LOG(USRLOG_BCOUT_END);
		bc->ncs_broadcast_outgoing_stop(bc, BC_STOP_RING);
	}
	sg_broadcast_data.time = 0;
	sg_broadcast_data.status = BC_STOP;
	sg_broadcast_data.input_position = 0;
	memset(sg_broadcast_data.input, 0 ,sizeof(sg_broadcast_data.input));	
	set_system_task(TASK_NONE);
	KillTimer(hWnd, TIMERID_GUIBROADCAST_TIME);
}

static void guibroadcastwin_start_all_area_outgoing(void)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct broadcast *bc = ncs_manager->bc;

	WEB_SYS_LOG(USRLOG_BCOUT_START);
	
	ncs_manager->ncs_call_stop_all(ncs_manager, FALSE);
	term->sip_manager->call_stop();


	if (term->ncs_manager->online)
		bc->ncs_global_broadcast_request(bc);
	else
		bc->ncs_broadcast_outgoing_start(bc, BC_START_RING);

	return;
}


static void guibroadcastwin_start_term_outgoing(void)
{
    int *term_list, term_list_len;
    int ret;
    struct broadcast *bc = term->ncs_manager->bc;

    term_list_len = 1000;
    term_list = (int *)malloc(term_list_len * sizeof(int));
    ret = guibroadcastwin_str_parse(term_list, term_list_len, '*');

	WEB_SYS_LOG(USRLOG_BCOUT_START);
    bc->ncs_term_broadcast_request(bc, term_list, ret);

    free(term_list);
}

static void guibroadcastwin_start_area_outgoing(void)
{
    int *area_list, area_list_len;
    int ret;
    struct broadcast *bc = term->ncs_manager->bc;

    area_list_len = 128;
    area_list = (int *)malloc(area_list_len * sizeof(int));
    ret = guibroadcastwin_str_parse(area_list, area_list_len, '*');
	WEB_SYS_LOG(USRLOG_BCOUT_START);
    bc->ncs_area_broadcast_request(bc, area_list, ret);
    free(area_list);
}

static void guibroadcastwin_start_tab_area_list_outgoing(void)
{
    int *area_list, area_list_len;
    int ret;
    struct broadcast *bc = term->ncs_manager->bc;

    area_list_len = 128;
    area_list = (int *)malloc(area_list_len * sizeof(int));
    ret = guibroadcastwin_area_list_check(area_list, area_list_len);
	WEB_SYS_LOG(USRLOG_BCOUT_START);
    bc->ncs_area_broadcast_request(bc, area_list, ret);
    free(area_list);
}


static void guibroadcastwin_img_display(int x, int y, char *path)
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

static void guibroadcastwin_term_select_down_handle(HWND hWnd)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_PTR *pre = NULL;
	int max = 0;
	
	if(p->id >= KEYPAD_BTN_START && p->id <= KEYPAD_BTN_END)
	{
		if(sg_broadcast_data.type != p->id - KEYPAD_BTN_START)
		{
			pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,sg_broadcast_data.type + KEYPAD_BTN_START);
			SetWidgetEnable(pre, 0);	
			InvalidateRect_Ex(hWnd, &pre->rc, TRUE);
			system_info.hdc = BeginPaint(hWnd);
			pre->pReFlash(&pre->rc, pre->pData0, pre->pData1, 0);
			EndPaint(hWnd, system_info.hdc);

			//SetWidgetEnable(p, 1);	
			InvalidateRect_Ex(hWnd, &p->rc, TRUE);
			system_info.hdc = BeginPaint(hWnd);
			p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
			EndPaint(hWnd, system_info.hdc);

			sg_broadcast_data.type = p->id - KEYPAD_BTN_START;
			sg_broadcast_data.input_position = 0;
			memset(sg_broadcast_data.input, 0 ,sizeof(sg_broadcast_data.input));
            if(sg_broadcast_data.type != BC_AREA_SELECT)
            {
                SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,1), WIDGET_IDX(0,18));
            }
            else
            {
                SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, max));
            }
        }
	}
	else
	{
		InvalidateRect_Ex(hWnd, &p->rc, TRUE);
		system_info.hdc = BeginPaint(hWnd);
		p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
		EndPaint(hWnd, system_info.hdc);
	}	
}

static void guibroadcastwin_term_select_up_handle(HWND hWnd)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;

	if(p->id >= KEYPAD_BTN_START && p->id <= KEYPAD_BTN_END)
	{
		InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
		SetWidgetEnable(p, 1);
		system_info.hdc = BeginPaint(hWnd);
		p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
		EndPaint(hWnd, system_info.hdc);
	}
	else
	{
		InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
		system_info.hdc = BeginPaint(hWnd);
		p->pReFlash(&p->rc, p->pData0, p->pData1, 0);
		EndPaint(hWnd, system_info.hdc);
		guibroadcastwin_delete_texture(&sg_broadcast_data);
        SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,1), WIDGET_IDX(0,18));
	}
}

static void guibroadcastwin_keypad_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;

	InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
	switch(message)
	{
		case MSG_LBUTTONDOWN:
			system_info.hdc = BeginPaint(hWnd);
			p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
			EndPaint(hWnd, system_info.hdc);
			break;
		case MSG_LBUTTONUP:
			system_info.hdc = BeginPaint(hWnd);
			p->pReFlash(&p->rc, p->pData0, p->pData1, 0);
			EndPaint(hWnd, system_info.hdc);
			if(sg_broadcast_data.status == BC_STOP && sg_broadcast_data.type != BC_ALL_TERMINAL)
			{
				if(sg_broadcast_data.input_position < BROADCAST_INPUT_NUM)
				{
					if(p->id>= 1 && p->id <= 10)
					{
						guibroadcastwin_input_texture(&sg_broadcast_data,0x30+p->id-1);
					}
					else if(p->id == 11)
					{
						guibroadcastwin_input_texture(&sg_broadcast_data,'*');
					}
					else if(p->id == 12)
					{
						guibroadcastwin_input_texture(&sg_broadcast_data,'#');
					}
					SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,1), WIDGET_IDX(0,18));
				}
			}
			break;
	}
}

static void guibroadcastwin_term_select_proc(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
    switch(message)
    {
        case MSG_LBUTTONDOWN:
			if(sg_broadcast_data.status == BC_STOP)
			{
				guibroadcastwin_term_select_down_handle(hWnd);
			}
            break;
        case MSG_LBUTTONUP:
			if(sg_broadcast_data.status == BC_STOP)
			{
				guibroadcastwin_term_select_up_handle(hWnd);
			}
#if 0			
			if(p->id == 13)
				set_system_task(TASK_NCS_TERM_BROADCAST_OUTGOING);
			else if(p->id == 14)
				set_system_task(TASK_NCS_ALL_AREA_BROADCAST_OUTGOING);
			else if(p->id == 15)
				set_system_task(TASK_NCS_AREA_BROADCAST_OUTGOING);
#endif			
			break;
    }
}

static void guibroadcastwin_control_start_pause_display(PRECT pRc, void* pData0, void* pData1, int enable)
{	
	BITMAP * bitmap = NULL;
	char *data;
	char pause[2][64] = {"pause_up.png","pause_down.png"};
	char start[2][64] = {"start_up.png","start_down.png"};

	if(sg_broadcast_data.status == BC_START)
	{
		data = pause[enable];
	}
	else
	{
		data = start[enable];
	}

	guibroadcastwin_img_display(pRc->left, pRc->top, data);
}

static void guibroadcastwin_control_start_pause_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	int empty_input = 0;

	if(sg_broadcast_data.type == BC_TERMINAL || sg_broadcast_data.type == BC_AREA)
	{
		if(strlen(sg_broadcast_data.input) == 0)
		{
			empty_input = 1;
		}
	}
	
	if(sg_broadcast_data.type == BC_AREA_SELECT)
	{
		empty_input = 0;
		if(guibroadcastwin_area_list_is_valide() == 0)
		{
			empty_input = 1;
			/*SPON_LOG_INFO("area list empty_input %d\n");*/
		}
	}
	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            guibroadcastwin_control_start_pause_display(&p->rc, p->pData0, p->pData1, 1);
            EndPaint(hWnd, system_info.hdc);
			if(empty_input == 0 && sg_broadcast_data.status == BC_STOP)
			{
				task_t task = get_system_task();
				sg_broadcast_data.status = BC_START;
				
				if(sg_broadcast_data.tab != 2)
				{
					if(sg_broadcast_data.type != BC_AREA_SELECT)
					{
						SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,1), WIDGET_IDX(0,18));
					}
					switch(sg_broadcast_data.type)
					{
						case BC_TERMINAL:
							set_system_task(TASK_NCS_TERM_BROADCAST_OUTGOING);
							guibroadcastwin_start_term_outgoing();
							break;
						case BC_AREA:
							set_system_task(TASK_NCS_AREA_BROADCAST_OUTGOING);
							guibroadcastwin_start_area_outgoing();
							break;
						case BC_ALL_TERMINAL:
							set_system_task(TASK_NCS_ALL_AREA_BROADCAST_OUTGOING);
							guibroadcastwin_start_all_area_outgoing();
							break;
						case BC_AREA_SELECT:
                            set_system_task(TASK_NCS_AREA_BROADCAST_OUTGOING);
                            guibroadcastwin_start_tab_area_list_outgoing();
                            break;

					}
				}
				SetTimer(hWnd, TIMERID_GUIBROADCAST_TIME, 50);
				SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,2), WIDGET_IDX(0,0));
			}
			else if(sg_broadcast_data.status == BC_START)
			{
				guibroadcastwin_stop(hWnd);
				if(sg_broadcast_data.type != BC_AREA_SELECT)
				{
					SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,1), WIDGET_IDX(0,18));
				}
				SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,2), WIDGET_IDX(0,0));
			}
            break;
        case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            guibroadcastwin_control_start_pause_display(&p->rc, p->pData0, p->pData1, 0);
            EndPaint(hWnd, system_info.hdc);
			break;
    	}

}


static void guibroadcastwin_time_display(PRECT pRc, void* pData0, void* pData1, int enable)
{
	int i = 0;
	int pos_x = 0;
	int pos_y = 0;
	int offset = 34;
	RECT rc;
	char Buf[30];
	int high = sg_broadcast_data.time/60%60;
	int low = sg_broadcast_data.time%60;
	int bit[5];

	bit[0] = high/10;
	bit[1] = high%10;
	bit[2] = 0xffff;
	bit[3] = low/10;
	bit[4] = low%10;
	
    pos_y = pRc->top;
	sprintf(Buf, "%d.png", bit[0]);
    pos_x = pRc->left;
	guibroadcastwin_img_display(pos_x,pos_y,Buf);
	sprintf(Buf, "%d.png", bit[1]);
    pos_x = pRc->left+offset;
	guibroadcastwin_img_display(pos_x,pos_y,Buf);
	sprintf(Buf, "dot.png");
    pos_x = pRc->left+offset*2+3;
	guibroadcastwin_img_display(pos_x,pRc->top+10,Buf);
	sprintf(Buf, "%d.png", bit[3]);
    pos_x = pRc->left+offset*2+10;
	guibroadcastwin_img_display(pos_x,pos_y,Buf);
	sprintf(Buf, "%d.png", bit[4]);
    pos_x = pRc->left+offset*3+10;
	guibroadcastwin_img_display(pos_x,pos_y,Buf);
}

static void guibroadcastwin_timer_handle(HWND hWnd, int timer_id)
{
	TYPE_GUI_AREA_PTR* data_item;
	if(timer_id == TIMERID_GUIBROADCAST_TIME)
	{
		data_item = &gui_broadcast_widget_control[0];
		InvalidateRect_Ex(hWnd, &data_item->rc, TRUE);
		system_info.hdc = BeginPaint(hWnd);
		sg_broadcast_data.time++;
		guibroadcastwin_time_display(&data_item->rc,NULL, NULL, FALSE);
		EndPaint(hWnd, system_info.hdc);
	}	
}
static void guibroadcastwin_time_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
	switch(message)
	{
		case MSG_NCS_BROADCAST_UPDATE:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, p->enable);
            EndPaint(hWnd, system_info.hdc);
			break;
	}
}

static void guibroadcastwin_control_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	struct audio_manager *audio_manager = term->audio_manager;
	struct call_manager *call_manager = term->call_manager;
	call_t *call = call_manager->current_call_get(call_manager);
    int max = 0;
    int i;
    TYPE_GUI_AREA_PTR *pre = NULL;
	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
            EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 0);
            EndPaint(hWnd, system_info.hdc);
			switch(p->id)
			{
				case 3://STOP
					if(sg_broadcast_data.status == BC_START)
					{
						guibroadcastwin_stop(hWnd);
						if(sg_broadcast_data.type != BC_AREA_SELECT)
						{
							SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,1), WIDGET_IDX(0,18));
						}
						SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,2), WIDGET_IDX(0,0));
						SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0,2), WIDGET_IDX(0,2));
					}
					break;
				case 1:
                    if(sg_broadcast_data.type == BC_AREA_SELECT)
                    {
                        sg_broadcast_data.type = BC_TERMINAL;
                        gui_broadcast_widget_control[2].id = 1;
                        for(i = 0; i<4; i++)
                        {
                            pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,i+ KEYPAD_BTN_START);
                            SetWidgetEnable(pre, (i == BC_TERMINAL));
                        }
                        SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, max));
                    }
                    else
                    {
                        SPON_LOG_INFO("setting file_play_statue bc_stop\n");
                        SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_IDLE_SCREEN_ID);
                        sg_broadcast_data.file_play_statue = BC_STOP;
                    }
					break;
			}
            break;
    }
}

static void guibroadcastwin_input_texture(broadcast_data_struct* ptr, char c)
{
    ptr->input[ptr->input_position++] = c;
}

static void guibroadcastwin_delete_texture(broadcast_data_struct* ptr)
{
    if(ptr->input_position)
    {
    	ptr->input_position--;
        ptr->input[ptr->input_position] = '\0';
    }
}

static void guibroadcastwin_texture_display(broadcast_data_struct* ptr, PRECT pRc, int idx)
{
    BITMAP * bitmap = NULL;
    char data[30];

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

	guibroadcastwin_img_display(pRc->left+idx*20+20, pRc->top+20, data);
	
}

static void guibroadcastwin_pagenr_display(PRECT pRc, void* pData0, void* pData1, int enable)
{	
	char  Buf[128];

	SetPenColor(system_info.hdc, COLOR_lightwhite);
	SetTextColor(system_info.hdc, COLOR_lightwhite);
	SetBkMode(system_info.hdc, BM_TRANSPARENT);
	if(sg_broadcast_data.type == BC_AREA_SELECT)
	{
		sprintf(Buf, "%d/%d", sg_broadcast_data.area_current_page+1, sg_broadcast_data.area_all_pages);
	}
	else
	{
		sprintf(Buf, "%d/%d", sg_broadcast_data.file_current_page+1, sg_broadcast_data.file_all_pages);
	}
	DrawText(system_info.hdc, Buf, -1, pRc, DT_CENTER);
}

static void guibroadcastwin_button_display(PRECT pRc, void* pData0, void* pData1, int enable)
{	
	void *pData = NULL;
	if(enable)
		pData = pData1;
	else
		pData = pData0;
	guibroadcastwin_img_display(pRc->left, pRc->top, pData);
}


static void guibroadcastwin_subbg_display(PRECT pRc, void* pData0, void* pData1, int enable)
{
}

static void guibroadcastwin_term_button_display(PRECT pRc, void* pData0, void* pData1, int enable)
{	
	void *pData = NULL;
	if(enable)
		pData = pData1;
	else
		pData = pData0;
	guibroadcastwin_img_display(pRc->left, pRc->top, pData);
}


static void guibroadcastwin_input_display(PRECT pRc, void* pData0, void* pData1, int enable)
{
	int i = 0;
	RECT rc;

	if(sg_broadcast_data.status == BC_START)
	{
		guibroadcastwin_img_display(pRc->left, pRc->top, "broadcast_start.png");
	}
	else
	{
		int len = strlen(sg_broadcast_data.input);	

		if(len == 0)
		{
#if 0
			SetPenColor(system_info.hdc, COLOR_darkgray);
			SetTextColor(system_info.hdc, COLOR_darkgray);
			SetBkMode(system_info.hdc, BM_TRANSPARENT);
			rc.left = pRc->left;
			rc.top = pRc->top + 20;
			rc.right = pRc->right;
			rc.bottom = pRc->bottom;			
			switch(sg_broadcast_data.type)
			{
				case BC_TERMINAL:
					DrawText(system_info.hdc, "请输入:ID+*+ID", -1, &rc, DT_CENTER);
					break;
				case BC_AREA:
					DrawText(system_info.hdc, "请输入:分区+*+分区", -1, &rc, DT_CENTER);
					break;
				case BC_ALL_TERMINAL:
					DrawText(system_info.hdc, "全区广播", -1, &rc, DT_CENTER);
					break;
			}
#else
			switch(sg_broadcast_data.type)
			{
				case BC_TERMINAL:
					guibroadcastwin_img_display(pRc->left, pRc->top, "broadcast_info_id.png");
					break;
				case BC_AREA:
					guibroadcastwin_img_display(pRc->left, pRc->top, "broadcast_info_area.png");
					break;
				case BC_ALL_TERMINAL:
					guibroadcastwin_img_display(pRc->left, pRc->top, "broadcast_info_all.png");
					break;
			}		
#endif
		}
		else
		{
			guibroadcastwin_button_display(pRc, pData0, pData1, enable);
		
			for(i=0;i<len;i++)
			{
				guibroadcastwin_texture_display(&sg_broadcast_data, pRc, i);
			}
		}
	}
}

static void guibroadcastwin_tab_display(PRECT pRc, void* pData0, void* pData1, int enable)
{
	void *pData;
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;

	if(sg_broadcast_data.tab == p->id)
		pData = pData1;
	else
		pData = pData0;
	guibroadcastwin_img_display(pRc->left, pRc->top, pData);
}

static void guibroadcastwin_tab_down(HWND hWnd)
{
	char *data = NULL;
	int pre_id,pre1_id;	
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_PTR *pre = NULL,*pre1 = NULL;

	switch(p->id)
	{
		case 0:
			pre_id = 1;
            /*pre1_id = 2;*/
			break;
		case 1:
			pre_id = 0;
			/*pre1_id = 2;*/
			break;
		/*case 2:*/
			/*pre_id = 0;*/
			/*pre1_id = 1;*/
			/*break;*/
	}
    pre = &gui_broadcast_widget_tab[pre_id];
    /*pre1 = &gui_broadcast_widget_tab[pre1_id];*/

	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE);
    system_info.hdc = BeginPaint(hWnd);
    p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
    EndPaint(hWnd, system_info.hdc);

	g_pWinPrivatePrt = pre;
	InvalidateRect_Ex(hWnd, &pre->rc, TRUE);
	system_info.hdc = BeginPaint(hWnd);
	pre->pReFlash(&pre->rc, pre->pData0, pre->pData1, 0);
	EndPaint(hWnd, system_info.hdc);

	/*g_pWinPrivatePrt = pre1;*/
	/*InvalidateRect_Ex(hWnd, &pre1->rc, TRUE);*/
	/*system_info.hdc = BeginPaint(hWnd);*/
	/*pre1->pReFlash(&pre1->rc, pre1->pData0, pre1->pData1, 0);*/
	/*EndPaint(hWnd, system_info.hdc);*/
}


static void guibroadcastwin_tab_func(HWND hWnd, int message, int wParam, int lParam)
{
	int i,max = 0;
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_PTR *pre = NULL;
	
    struct broadcast *bc = term->ncs_manager->bc;
	struct audio_manager *audio_manager = term->audio_manager;
	
    //InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            break;
        case MSG_LBUTTONUP:
			if(sg_broadcast_data.tab != p->id)
            {
				if(p->id == 1)
				{
					/*if(bc->bc_outgoing(bc))*/
					{
						sg_broadcast_data.tab = p->id;
						guibroadcastwin_tab_down(hWnd);
                        SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, max));
                        //这里需要注意，强制设置ｉｄ＝＝－１，让ｐｒｏｃ函数中直接遇到－１跳出循环不刷
                        gui_broadcast_widget_control[2].id = -1;
                        sg_broadcast_data.file_current_id  = -1;
						sg_broadcast_data.old_sel_page_num = -1;

						sg_broadcast_data.type             = BC_TERMINAL;
						for(i = 0; i<4; i++)
						{
							pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,i+ KEYPAD_BTN_START);
							SetWidgetEnable(pre, (i == BC_TERMINAL));
						}
                        
					}
				}
				else if(p->id == 0)
				{
                    sg_broadcast_data.type = BC_TERMINAL;
					for(i = 0; i<4; i++)
					{
						pre = HandleWidgetInfo(guibroadcastwin_terminal_items,1,i+ KEYPAD_BTN_START);
						SetWidgetEnable(pre, (i == BC_TERMINAL));
					}
					
					sg_broadcast_data.tab = p->id;
					guibroadcastwin_tab_down(hWnd);
                    SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, max));
                    gui_broadcast_widget_control[2].id = 1;
					if(bc->bc_outgoing(bc))
					{
						bc->ncs_file_broadcast_stop(bc);
						bc->ncs_realtime_broadcast_start(bc);
					}
				}
				/*else if(p->id == 2)*/
				/*{*/
					/*sg_broadcast_data.tab = p->id;*/
					/*guibroadcastwin_tab_down(hWnd);*/
					/*SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, max));*/
					/*gui_broadcast_widget_control[2].id = 1;*/
				/*}*/
			}
            break;
    }
}

static void guibroadcastwin_file_select_up(HWND hWnd, PRECT pRc, int id)
{	
	//BITMAP * bitmap = NULL;
	char *data;
	char select[2][32] = {"file_unselect.png","file_select.png"};

	if(sg_broadcast_data.select[id] == 1)
	{
		data = select[1];
	}
	else
	{
		data = select[0];
	}
	
	guibroadcastwin_img_display(pRc->left, pRc->top, data);
}


static void guibroadcastwin_file_bar_func(HWND hWnd, int message, int wParam, int lParam)
{
    TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
    TYPE_GUI_AREA_PTR *pOld = NULL;
    int index = 0; 
	struct audio_manager *audio_manager = term->audio_manager;
	char file_path[128] = {0};
    struct broadcast *bc = term->ncs_manager->bc;

    switch(message)
    {
        case MSG_LBUTTONDOWN:
			if(bc->bc_outgoing(bc))
			{
				if(strlen(sg_broadcast_data.file_name[p->id-1]))
				{
					sg_broadcast_data.file_current_id = sg_broadcast_data.file_current_page*FILE_NUMBER_PER_PAGE+p->id-1;
					if(sg_broadcast_data.old_sel_page_num == sg_broadcast_data.file_current_page)
					{
						pOld = &gui_broadcast_widget_file[sg_broadcast_data.old_sel_index];
						g_pWinPrivatePrt = pOld;
						InvalidateRect_Ex(hWnd, &pOld->rc, TRUE);
						system_info.hdc = BeginPaint(hWnd);
						pOld->pReFlash(&pOld->rc, pOld->pData0, pOld->pData1, pOld->enable);
						EndPaint(hWnd, system_info.hdc);
					}
				}
			}
            break;
        case MSG_LBUTTONUP:
			if(bc->bc_outgoing(bc))
			{
				if(strlen(sg_broadcast_data.file_name[p->id-1]))
				{
					InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
					system_info.hdc = BeginPaint(hWnd);
					p->pReFlash(&p->rc, p->pData0, p->pData1, 0);
					EndPaint(hWnd, system_info.hdc);
					sg_broadcast_data.old_sel_index    = p->id;
					sg_broadcast_data.old_sel_page_num = sg_broadcast_data.file_current_page;
					sprintf(file_path, "%s%s", file_list_obj_get_play_path(&s_file_list_obj), sg_broadcast_data.file_name[p->id-1]);
					if(bc->ncs_file_broadcasting(bc)) 
					{
						bc->ncs_file_broadcast_stop(bc);
					}
					else
					{
						bc->ncs_realtime_broadcast_stop(bc);
					}
					bc->ncs_file_broadcast_start(bc, file_path);
                    sg_broadcast_data.file_play_statue = BC_START;
				}
			}
            break;
    }
}

static void guibroadcastwin_file_select_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
    int index = p->id-1;
	
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            sg_broadcast_data.file_select[index] = !sg_broadcast_data.file_select[index];
			SetWidgetEnable(p, sg_broadcast_data.file_select[index]);
			file_list_update_sel_statue(&s_file_list_obj.file_broadcast_list, sg_broadcast_data.file_current_page*FILE_NUMBER_PER_PAGE+index, sg_broadcast_data.file_select[index]);
            break;
    	case MSG_LBUTTONUP:
            InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
            system_info.hdc = BeginPaint(hWnd);
            /*guibroadcastwin_file_select_up(hWnd, &p->rc, p->id);*/
			p->pReFlash(&p->rc, p->pData0, p->pData1, p->enable);
            EndPaint(hWnd, system_info.hdc);
            break;
    }
}

static void guibroadcastwin_file_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_PTR *ptr;
	int i = 0;

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
            EndPaint(hWnd, system_info.hdc);
            break;
    	case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 0);
            EndPaint(hWnd, system_info.hdc);

			if(p->id == 7)
			{
				get_previous_page();
                SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 1));
			}
			else if(p->id == 8)
			{
				get_next_page();
                SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 1));
			}
            break;
    }

}

static void guibroadcastwin_file_win_return(HWND hWnd, int message, int wParam, int lParam)
{
    struct broadcast *bc = term->ncs_manager->bc;
    TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;

    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
            EndPaint(hWnd, system_info.hdc);
            break;
        case MSG_LBUTTONUP:
            /*system_info.hdc = BeginPaint(hWnd);*/
            /*p->pReFlash(&p->rc, p->pData0, p->pData1, 0);*/
            /*EndPaint(hWnd, system_info.hdc);*/

            sg_broadcast_data.tab = 0;
            guibroadcastwin_tab_down(hWnd);
            SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 0));
            gui_broadcast_widget_control[1].id = 1;
            if(bc->bc_outgoing(bc))
            {
                bc->ncs_file_broadcast_stop(bc);
                bc->ncs_realtime_broadcast_start(bc);
            }
            break;
        default:
            break;
	}
}


void fill_file_name(int AllSum)
{
	file_t *file;
	int i = 0;
	int j = 0;
	int id = 0;

	for(i = 0; i < FILE_NUMBER_PER_PAGE; i++)
	{
		id = sg_broadcast_data.file_current_page * FILE_NUMBER_PER_PAGE + i;
		if(id > AllSum)
		{
            for(j = i-1; j < FILE_NUMBER_PER_PAGE; j++)
            {
                sg_broadcast_data.file_name[j][0] = 0;
                sg_broadcast_data.file_name[j][0] = 0;
                sg_broadcast_data.file_select[j] = NOT_FILE_SEL;
            }
			break;
		}
		file = file_find_by_id(&s_file_list_obj.file_broadcast_list,  id);
		if(file)
		{
			strcpy(sg_broadcast_data.file_name[i], file->name);
			sg_broadcast_data.file_select[i] = file->file_sel_status;
		}
	}
}
static void get_next_page(void)
{
	int AllSum = file_list_count(&s_file_list_obj.file_broadcast_list);
	int PageNum = AllSum/FILE_NUMBER_PER_PAGE;

	if(!AllSum)
	{
		return;
	}
	if(AllSum%FILE_NUMBER_PER_PAGE)
	{
		PageNum++;
	}
	sg_broadcast_data.file_all_pages = PageNum;
	if(sg_broadcast_data.file_current_page >= PageNum-1)
	{
		sg_broadcast_data.file_current_page = 0;
	}
	else
	{
		sg_broadcast_data.file_current_page++;
	}
	fill_file_name(AllSum);
}

static void get_previous_page(void)
{
	int AllSum = file_list_count(&s_file_list_obj.file_broadcast_list);
	int PageNum = AllSum/FILE_NUMBER_PER_PAGE;
	

	if(!AllSum)
	{
		return;
	}
	if(AllSum%FILE_NUMBER_PER_PAGE)
	{
		PageNum++;
	}
	sg_broadcast_data.file_all_pages = PageNum;
	if(sg_broadcast_data.file_current_page)
	{
		sg_broadcast_data.file_current_page--;
	}
	else
	{
		sg_broadcast_data.file_current_page = PageNum-1;
	}
	fill_file_name(AllSum);
}

static void guibroadcastwin_file_list_display(PRECT pRc, void* pData0, void* pData1, int enable)
{
	int y           = 0;
	BITMAP * bitmap = NULL;
	void *pData     = NULL;
	char buf[128];
	RECT rc;
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	
	if(enable)
		pData = pData1;
	else
		pData = pData0;

	bitmap	= png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
        y = pRc->top;
        FillBoxWithBitmap(system_info.hdc, pRc->left, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);

        SetRect(&rc, pRc->left, y+10, pRc->right, pRc->bottom);
        
        SetPenColor(system_info.hdc, COLOR_lightwhite);
        if(sg_broadcast_data.file_current_id == sg_broadcast_data.file_current_page*FILE_NUMBER_PER_PAGE+p->id-1)
        {
            SetTextColor(system_info.hdc, RGB2Pixel(system_info.hdc, 0, 255, 0));
        }
        else
        {
            SetTextColor(system_info.hdc, COLOR_lightwhite);
        }
        SetBkMode(system_info.hdc, BM_TRANSPARENT);
        DrawText(system_info.hdc, sg_broadcast_data.file_name[p->id-1], -1, &rc, DT_CENTER);
	}
}

static void init(void)
{
	sg_broadcast_data.file_current_page = -1;
}

static int guibroadcastwin_area_list_is_valide(void)
{
	int ret = 0;
	int i = 0,j = 0;

	for(i=0; i<BROADCAST_AREA_MAX_PAGE; i++)
	{
		for(j=0; j<FILE_NUMBER_PER_PAGE; j++)
		{
			if(sg_broadcast_data.area_select[i][j] != 0)
			{
				ret = 1;
		    	break;
			}
	    }
	}
	return ret;
}

static int guibroadcastwin_area_list_check(int *dst, int len)
{
	int i = 0,j = 0,k = 0;

	for(i=0; i<BROADCAST_AREA_MAX_PAGE; i++)
	{
		for(j=0; j<FILE_NUMBER_PER_PAGE; j++)
		{
			if(sg_broadcast_data.area_select[i][j] != 0 && sg_broadcast_data.area_id[i][j] != 0)
			{
		    	dst[k++] = sg_broadcast_data.area_id[i][j];
				//printf("%d ",sg_broadcast_data.area_id[i][j]);
			}
	    }
	}
	//printf("\n%d \n",k);
	return k;
}

void guibroadcastwin_area_list_set_name(int AllSum)
{
	file_t *file;
	int i = 0,j = 0,id = 0,idx = 0;
	char* name =  NULL;
	struct ncs_manager *ncs_manager = term->ncs_manager;
	TYPE_GUI_AREA_PTR *ptr = NULL;

	//SPON_LOG_INFO("allsum %d\n",AllSum);

	memset(sg_broadcast_data.area_id[sg_broadcast_data.area_current_page],0,sizeof(sg_broadcast_data.area_id[sg_broadcast_data.area_current_page]));
	for(i = 0; i < FILE_NUMBER_PER_PAGE; i++)
	{
		idx = sg_broadcast_data.area_current_page * FILE_NUMBER_PER_PAGE + i;// + 1
		if(idx >= AllSum)
		{
			//SPON_LOG_INFO("id %d > allsum %d i %d\n",id,AllSum,i);
            for(j = i; j < FILE_NUMBER_PER_PAGE; j++)
            {
                sg_broadcast_data.area_name[j][0] = 0;
                sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][j] = 0;
                sg_broadcast_data.area_select[sg_broadcast_data.area_current_page][j] = NOT_FILE_SEL;

				ptr = HandleWidgetInfo(guibroadcastwin_area_list_items,1,j+1);
				SetWidgetEnable(ptr, sg_broadcast_data.area_select[sg_broadcast_data.area_current_page][j]);
				
				//SPON_LOG_INFO("sg_broadcast_data.area_id[%d][%d]\n",j,sg_broadcast_data.area_current_page,sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][j]);
            }
			break;
		}
		name = ncs_manager->ncs_area_name_get_by_index(ncs_manager,idx);
		id =  ncs_manager->ncs_area_id_get_by_index(ncs_manager,idx);
		if(name)
		{
			strcpy(sg_broadcast_data.area_name[i], name);
		}
		
		sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][i] = id;
	}
}
static void guibroadcastwin_area_list_get_next_page(void)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
	int AllSum = ncs_manager->ncs_area_name_list_size_get(ncs_manager);	
	int PageNum = AllSum/FILE_NUMBER_PER_PAGE;

	if(!AllSum)
	{
		return;
	}
	if(AllSum%FILE_NUMBER_PER_PAGE)
	{
		PageNum++;
	}
	sg_broadcast_data.area_all_pages = PageNum;
	if(sg_broadcast_data.area_current_page >= PageNum-1)
	{
		sg_broadcast_data.area_current_page = 0;
	}
	else
	{
		sg_broadcast_data.area_current_page++;
	}
	guibroadcastwin_area_list_set_name(AllSum);
}

static void guibroadcastwin_area_list_get_previous_page(void)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
	int AllSum = ncs_manager->ncs_area_name_list_size_get(ncs_manager);	
	int PageNum = AllSum/FILE_NUMBER_PER_PAGE;
	

	if(!AllSum)
	{
		return;
	}
	if(AllSum%FILE_NUMBER_PER_PAGE)
	{
		PageNum++;
	}
	sg_broadcast_data.area_all_pages = PageNum;
	if(sg_broadcast_data.area_current_page)
	{
		sg_broadcast_data.area_current_page--;
	}
	else
	{
		sg_broadcast_data.area_current_page = PageNum-1;
	}
	guibroadcastwin_area_list_set_name(AllSum);
}

static void guibroadcastwin_area_list_display(PRECT pRc, void* pData0, void* pData1, int enable)
{
	int y           = 0;
	BITMAP * bitmap = NULL;
	void *pData     = NULL;
	char buf[128];
	RECT rc;
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	
	if(enable)
		pData = pData1;
	else
		pData = pData0;

	bitmap	= png_ha_res_find_by_name(pData);
	if(bitmap == NULL)
	{
		SPON_LOG_ERR("file: %s load error\n", pData);
	}
	else
	{
        y = pRc->top;
        FillBoxWithBitmap(system_info.hdc, pRc->left, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);

        SetRect(&rc, pRc->left, y+10, pRc->right, pRc->bottom);
        
        SetPenColor(system_info.hdc, COLOR_lightwhite);
        SetTextColor(system_info.hdc, COLOR_lightwhite);
        SetBkMode(system_info.hdc, BM_TRANSPARENT);

		
		if(strlen(sg_broadcast_data.area_name[p->id-7]) > 0)
		{
			sprintf(buf, "%d - %s",sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][p->id-7],sg_broadcast_data.area_name[p->id-7]);
		}
		else
		{
			memset(buf,0,sizeof(buf));
			if(sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][p->id-7] != 0)
			{
				sprintf(buf, "%d - Area%d",sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][p->id-7],sg_broadcast_data.area_id[sg_broadcast_data.area_current_page][p->id-7]);
			}
		}

        DrawText(system_info.hdc, buf, -1, &rc, DT_CENTER);
	}
}


static void guibroadcastwin_area_item_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	int page_id = sg_broadcast_data.area_current_page;
	TYPE_GUI_AREA_PTR *ptr;

	if(sg_broadcast_data.area_id[page_id][p->id-1] == 0 || sg_broadcast_data.status == BC_START)
	{
		return;
	}
	
    InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, sg_broadcast_data.area_select[page_id][p->id-1]);
            EndPaint(hWnd, system_info.hdc);
            break;
    	case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, (1-sg_broadcast_data.area_select[page_id][p->id-1]));
            EndPaint(hWnd, system_info.hdc);
			sg_broadcast_data.area_select[page_id][p->id-1] = 1 - sg_broadcast_data.area_select[page_id][p->id-1];

			
			//SPON_LOG_INFO("area item %d page_id %d \n",sg_broadcast_data.area_select[page_id][p->id-1],page_id,p->id-1);
			ptr = HandleWidgetInfo(guibroadcastwin_area_list_items,1,p->id);
			SetWidgetEnable(ptr, sg_broadcast_data.area_select[page_id][p->id-1]);
            break;
    }

}

static void guibroadcastwin_area_btn_func(HWND hWnd, int message, int wParam, int lParam)
{
	TYPE_GUI_AREA_PTR *p = (TYPE_GUI_AREA_PTR*)g_pWinPrivatePrt;
	TYPE_GUI_AREA_PTR *ptr;
	int i = 0;

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); 
    switch(message)
    {
        case MSG_LBUTTONDOWN:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 1);
            EndPaint(hWnd, system_info.hdc);
            break;
    	case MSG_LBUTTONUP:
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0, p->pData1, 0);
            EndPaint(hWnd, system_info.hdc);

			if(p->id == 13)
			{
			
				guibroadcastwin_area_list_get_previous_page();
				for(i = 0; i < FILE_NUMBER_PER_PAGE; i++)
				{
					ptr = HandleWidgetInfo(guibroadcastwin_area_list_items,1,i+1);
					SetWidgetEnable(ptr, sg_broadcast_data.area_select[sg_broadcast_data.area_current_page][i]);
				}
                SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 1));
			}
			else if(p->id == 14)
			{
				guibroadcastwin_area_list_get_next_page();
				for(i = 0; i < FILE_NUMBER_PER_PAGE; i++)
				{
					ptr = HandleWidgetInfo(guibroadcastwin_area_list_items,1,i+1);
					SetWidgetEnable(ptr, sg_broadcast_data.area_select[sg_broadcast_data.area_current_page][i]);
				}				
                SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 1));
			}
            break;
    }

}

static void guibroadcastwin_area_list_init(void)
{
	int i = 0;
	TYPE_GUI_AREA_PTR *ptr;

	for(i = 0; i < FILE_NUMBER_PER_PAGE; i++)
	{
		ptr = HandleWidgetInfo(guibroadcastwin_area_list_items,1,i+1);
		SetWidgetEnable(ptr, 0);
	}
	for(i = 0; i < BROADCAST_AREA_MAX_PAGE; i++)
	{
		memset(sg_broadcast_data.area_id[i],0,sizeof(sg_broadcast_data.area_id[0]));
	}
	memset(sg_broadcast_data.area_select,0,sizeof(sg_broadcast_data.area_select));
}

static int guibroadcastwin_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	BITMAP * bitmap;
	int timer_id = (int)wParam;
    TYPE_GUI_AREA_PTR** data_item;
    struct broadcast *bc = term->ncs_manager->bc;
	
	if(g_firstEnter)
	{
		g_firstEnter = FALSE;
        if(sg_broadcast_data.file_play_statue == BC_STOP)
		{
			init();
			get_next_page();
            SPON_LOG_INFO("init\n");
			sg_broadcast_data.file_current_id = -1;
		}
		
		sg_broadcast_data.area_current_page = -1;
		guibroadcastwin_area_list_init();
		guibroadcastwin_area_list_get_next_page();
	}
	
	switch(message)
    {
		case MSG_CREATE:
			pthread_mutex_lock(&term->mutex);
			if(sg_broadcast_data.tab == 0)
			{
				data_item = guibroadcastwin_terminal_items;
			}
			else if(sg_broadcast_data.tab == 1)
			{
				data_item = guibroadcastwin_file_items;
			}
            /*else*/
            /*{*/
                /*data_item = guibroadcastwin_area_list_items;*/
            /*}*/
            if(sg_broadcast_data.type == BC_AREA_SELECT)
            {
                data_item = guibroadcastwin_area_list_items;
            }
			HandleCreatePtr(data_item, hWnd, BROADCAST_TERM_WIDGETS_NUM);
			pthread_mutex_unlock(&term->mutex);
			break;
		case MSG_TIMER:
			guibroadcastwin_timer_handle(hWnd, timer_id);
			break;
		case MSG_NCS_BROADCAST_UPDATE:
			pthread_mutex_lock(&term->mutex);
			if(sg_broadcast_data.tab == 0)
			{
				data_item = guibroadcastwin_terminal_items;
			}
			else if(sg_broadcast_data.tab == 1)
			{
				data_item = guibroadcastwin_file_items;
			}
            /*else*/
            /*{*/
                /*data_item = guibroadcastwin_area_list_items;*/
            /*}*/
            if(sg_broadcast_data.type == BC_AREA_SELECT)
            {
                data_item = guibroadcastwin_area_list_items;
            }

			HandleWidgetsUpdateProcPtr(data_item, hWnd, message, wParam, lParam);
			pthread_mutex_unlock(&term->mutex);
			break;			
		case MSG_NCS_BROADCAST_FILE_LIST_UPDATE:
			init();
			get_next_page();
			SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 1));
			break;
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
		case MSG_NCS_BROADCAST_HANDLE:
			pthread_mutex_lock(&term->mutex);
			if(sg_broadcast_data.tab == 0)
			{
				data_item = guibroadcastwin_terminal_items;
			}
			else if(sg_broadcast_data.tab == 1)
			{
				data_item = guibroadcastwin_file_items;
			}
			/*else*/
			/*{*/
				/*data_item = guibroadcastwin_area_list_items;*/
			/*}*/
            if(sg_broadcast_data.type == BC_AREA_SELECT)
            {
                data_item = guibroadcastwin_area_list_items;
            }

			HandlePageProcPtr(data_item, hWnd, message, wParam, lParam,BROADCAST_TERM_WIDGETS_NUM);
			pthread_mutex_unlock(&term->mutex);
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
					guibroadcastwin_stop(hWnd);
					break;
			}
			break;
		case MSG_NCS_SWITCH_WND:
			if(!sg_broadcast_data.tab)
			{
				guibroadcastwin_stop(hWnd);
				EnterNewScreen(hWnd,(int)lParam);
			}
			else
			{
				sg_broadcast_data.tab = 0;
				g_pWinPrivatePrt = &gui_broadcast_widget_tab[0];
				guibroadcastwin_tab_down(hWnd);
				SendNotifyMessage(hWnd, MSG_NCS_BROADCAST_UPDATE, WIDGET_IDX(0xff,1), WIDGET_IDX(0, 0));
				gui_broadcast_widget_control[2].id = 1;
				if(bc->bc_outgoing(bc))
				{
					bc->ncs_file_broadcast_stop(bc);
					bc->ncs_realtime_broadcast_start(bc);
				}
			}
			break;
		case MSG_DESTROY:
			return 0;
		default:
			break;
    }
	return 1;
}



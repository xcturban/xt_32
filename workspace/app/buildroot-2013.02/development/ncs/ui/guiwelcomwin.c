
#include <sys/ioctl.h>
#include "language.h"
#include "guiwelcomwin.h"
#include "gui_call_incomming.h"
#include "gui_calloutcomming.h"
#include "gui_conference_window.h"
#include "guininewarring.h"

struct time_st g_RtcTime;

static void PaintDate(PRECT pRc, void* pData);
static void PaintNetBmp(PRECT pRc, void* pData);
static void PaintIdNum(PRECT pRc, void * pData);
static void PaintNameNum(PRECT pRc, void * pData);
static void PaintVolBmp(PRECT pRc, void * pData);
static void We_Date_Proc(HWND hWnd, int message, int wParam, int lParam);
static void PaintButton(PRECT pRc, void* pData);
static void PaintMissedCallNum(PRECT pRc, void * pData);
static void We_Button_Proc(HWND hWnd, int message, int wParam, int lParam);
static void Def_NetButton_Proc(HWND hWnd, int message, int wParam, int lParam);
static void PaintVolBmp_Proc(HWND hWnd, int message, int wParam, int lParam);
static void Def_NameDispProc(HWND hWnd, int message, int wParam, int lParam);
int VolWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
extern pthread_t tid_main_window_DispatchMessage;

#define XJWLMQ
#ifdef XJWLMQ
const TYPE_GUI_AREA_HANDLE gui_welcome_page_item[] =
{
    {1   , {DATE_LEFT       , DATE_TOP , DATE_LEFT + 190      , DATE_TOP + 161 } , NULL                     , NULL                   , PaintDate   , We_Date_Proc   } , 
    {2   , {POS_BROADCAST_X-20 , 355      , POS_BROADCAST_X-20 + 100 , 355 + 93       } , "emegcywarring_up.png" , "emegcywarring_down.png" , PaintButton , We_Button_Proc } , //sos
    {3   , {POS_TALK_X      , 350      , POS_TALK_X + 50      , 350 + 93       } , "talk_pd.png"            , "talk_pu.png"          , PaintButton , We_Button_Proc } , // 对讲
    {4   , {POS_JT_X        , 350      , POS_JT_X + 70        , 350 + 93       } , "broadcast_pd.png"       , "broadcast_pu.png"     , PaintButton , We_Button_Proc } , // 广播
    {5   , {POS_SETTING_X   , 350      , POS_SETTING_X + 50   , 350 + 93       } , "setting_pd.png"         , "setting_pu.png"       , PaintButton , We_Button_Proc } , // 设置
    {6   , {650             , 80       , 650 + 147            , 80 + 74        } , "calllog_up.png"         , "calllog_down.png"     , PaintButton , We_Button_Proc } , // 未接来电
    { -1 , {DATE_LEFT       , DATE_TOP , DATE_RIGHT           , DATE_BOTTOOM }   , NULL                     , NULL                   , NULL        , NULL           } , 
};
#else
const TYPE_GUI_AREA_HANDLE gui_welcome_page_item[] =
{
    {1   , {DATE_LEFT       , DATE_TOP , DATE_LEFT + 190      , DATE_TOP + 161 } , NULL               , NULL               , PaintDate          , We_Date_Proc   } ,
    {2   , {POS_BROADCAST_X , 350      , POS_BROADCAST_X + 70 , 350 + 93       } , "broadcast_pd.png" , "broadcast_pu.png" , PaintButton        , We_Button_Proc } , // 广播
    {3   , {POS_TALK_X      , 350      , POS_TALK_X + 50      , 350 + 93       } , "talk_pd.png"      , "talk_pu.png"      , PaintButton        , We_Button_Proc } , // 对讲
    {4   , {POS_JT_X        , 350      , POS_JT_X + 70        , 350 + 93       } , "jt_pd.png"        , "jt_pu.png"        , PaintButton        , We_Button_Proc } , // 监听
    {5   , {POS_SETTING_X   , 350      , POS_SETTING_X + 50   , 350 + 93       } , "setting_pd.png"   , "setting_pu.png"   , PaintButton        , We_Button_Proc } , // 设置
    {6   , {650             , 80       , 650 + 147            , 80 + 74        } , "calllog_up.png"   , "calllog_down.png" , PaintButton        , We_Button_Proc } , // 未接来电
    { -1  , {DATE_LEFT       , DATE_TOP , DATE_RIGHT         , DATE_BOTTOOM } , NULL               , NULL               , NULL               , NULL           } ,
};
#endif

const TYPE_GUI_AREA_HANDLE gui_welcome_page_statuebar_item[] =
{
    {0  , {770       , 0        , 800        , 30}           , "offline.png" , "online.png" , PaintNetBmp  , Def_NetButton_Proc} ,
    {1  , {700       , 0        , 760        , 30}           , "vol.png"     , "vol_1.png"  , PaintVolBmp  , PaintVolBmp_Proc}   ,
    {2  , {20        , 8        , 220        , 30}           , NULL          , NULL         , PaintIdNum   , NULL}               ,
    {3  , {150       , 8        , 150 + 260    , 30}           , NULL          , NULL         , PaintNameNum , Def_NameDispProc}   ,
    { -1 , {DATE_LEFT , DATE_TOP , DATE_RIGHT , DATE_BOTTOOM} , NULL          , NULL         , NULL         , NULL}               ,
};

static void Def_NetButton_Proc(HWND hWnd, int message, int wParam, int lParam)
{
    TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    system_info.hdc = BeginPaint(hWnd);
    if(term->ncs_manager->online == TRUE)
    {
        //在线
        p->pReFlash(&p->rc, p->pData1);
    }
    else
    {
        p->pReFlash(&p->rc, p->pData0);
    }
    EndPaint(hWnd, system_info.hdc);
}

static void PaintVolBmp_Proc(HWND hWnd, int message, int wParam, int lParam)
{
    TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

    if(term->image_load != 1)
    {
        return;
    }
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
            if(!g_vol_change_win_flag)
            {
                EnterWin(hWnd, VolWinProc);
            }
            else
            {
                EnterWin(hWnd, g_StauteFunOld);
            }
            break;
        case MSG_NCS_STATUE_BAR_PAGE:
            InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
            system_info.hdc = BeginPaint(hWnd);
            p->pReFlash(&p->rc, p->pData0);
            EndPaint(hWnd, system_info.hdc);
            break;
    }
}

static void Def_NameDispProc(HWND hWnd, int message, int wParam, int lParam)
{
    TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;

    InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
    system_info.hdc = BeginPaint(hWnd);
    if(lParam)
    {
        p->pReFlash(&p->rc, p->pData0);
    }
    EndPaint(hWnd, system_info.hdc);
}

static void We_Date_Proc(HWND hWnd, int message, int wParam, int lParam)
{
    TYPE_GUI_AREA_HANDLE *p = (TYPE_GUI_AREA_HANDLE*)g_pWinPrivatePrt;
    if(message == MSG_NCS_WELCOM_PAGE)
    {
        InvalidateRect_Ex(hWnd, &p->rc, TRUE); //先清除区域, 再刷新
        system_info.hdc = BeginPaint(hWnd);
        p->pReFlash(&p->rc, p->pData0);
        EndPaint(hWnd, system_info.hdc);
    }
}

static void We_Button_Proc(HWND hWnd, int message, int wParam, int lParam)
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
            switch(p->id)
            {
                case 2:
#ifdef XJWLMQ
                    EnterWin(hWnd, guininewarring_proc);
#else
                    guibroadcastwin_newscreen(hWnd, NCS_BROADCAST_SCREEN_ID);
#endif
                    break;
                case 3:
                    if (term->dial_rapid == DIAL_NCS)
                    {
                        set_system_task(TASK_NCS_CALL_OUTGOING);
                    }
                    else
                    {
                        set_system_task(TASK_SIP_CALL_OUTGOING);
                    }
                    InputDigtal_NewScreen(hWnd);
                    break;
                case 4:
#ifdef XJWLMQ
                    guibroadcastwin_newscreen(hWnd, NCS_BROADCAST_SCREEN_ID);
#else
                    //Init(&g_input_ctrl_type, p->id);
                    set_system_task(TASK_NCS_MONITOR_OUTGOING);
                    InputDigtal_NewScreen(hWnd);
#endif
                    break;
                case 5:
                    guiloginwin_newscreen(hWnd)	;
                    //EnterWin(hWnd, SettingWinProc);
                    break;
                case 6:
                    //guimissedcall_newscreen(hWnd);
                    guicalllog_newscreen(hWnd);
                default:
                    break;
            }
            break;
    }
}

int DispTimeUnit(int lastx , int y, int Value, int flag)
{
    char buf[128] = {0};
    int time_left = lastx;
    BITMAP * bitmap;

    if(flag)
    {
        if(Value == -1)
        {
            sprintf(buf, "dot.png");
        }
        else
        {
            sprintf(buf, "%d.png", Value);
        }
    }
    bitmap  = png_ha_res_find_by_name(buf);
    FillBoxWithBitmap(system_info.hdc, time_left, y, bitmap->bmWidth, bitmap->bmHeight, bitmap);
    time_left += bitmap->bmWidth;
    return time_left;
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

/***************************************************************************************************
 * 函 数 名: DispDateWithPic
 * 日    期: 2016-10-27 08:53
 * 文 件 名: ui/guiwelcomwin.c
 * 功    能: 绘制日期函数
 * 参    数: int left, int top, int Value, char *pData
 * 返 回 值: int
****************************************************************************************************/
static int DispDateWithPic(int left, int top, int Value, char *pData)
{
    char buf[128];
    int left2 = left;
    BITMAP * bitmap = NULL;

    if(pData != NULL)
    {
        bitmap  = png_ha_res_find_by_name(pData);
    }
    else
    {
        sprintf(buf, "T%d.png", Value);
        bitmap  = png_ha_res_find_by_name(buf);
    }
    FillBoxWithBitmap(system_info.hdc, left2, top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
    left2 += bitmap->bmWidth;
    return left2;
}

static void PaintDate(PRECT pRc, void* pData)
{
    char text_buf[128];
    char buf[128] = {0};
    RECT text_rect;
    BITMAP * bitmap = NULL;
    static BOOL b_dot = TRUE;
    int i;
    char tmp[28]  = {0};
    char str1[20] = {0};
    char str2[20] = {0};
    int left = pRc->left;
    int time_left = pRc->left;

    time_left = DispTimeUnit(pRc->left + 4, pRc->top + 5, g_RtcTime.hour / 10, 1);
    time_left = DispTimeUnit(time_left, pRc->top + 5, g_RtcTime.hour % 10, 1);
    if(b_dot)
    {
        time_left = DispTimeUnit(time_left + 8, pRc->top + 15, -1, 1);
    }
    else
    {
        time_left += 17;
    }
    b_dot = !b_dot;
    time_left = DispTimeUnit(time_left + 8, pRc->top + 5, g_RtcTime.minute / 10, 1);
    time_left = DispTimeUnit(time_left + 4, pRc->top + 5, g_RtcTime.minute % 10, 1);

    text_rect.left   = pRc->left + 10;
    text_rect.top    = pRc->top + 90;
    text_rect.right  = pRc->right;
    text_rect.bottom = pRc->bottom;
    switch(g_RtcTime.weekday)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            sprintf(tmp, "week_%d.png", g_RtcTime.weekday);
            break;
        case 0:
        default:
            sprintf(tmp, "%s", "week_7.png");
            break;
    }
    bitmap  = png_ha_res_find_by_name(tmp);
    FillBoxWithBitmap(system_info.hdc, text_rect.left + 43, text_rect.top, bitmap->bmWidth, bitmap->bmHeight, bitmap);

    text_rect.top += 40;
    left = DispDateWithPic(text_rect.left, text_rect.top, g_RtcTime.year / 1000, NULL);
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.year / 100 % 10, NULL);
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.year / 10 % 10, NULL);
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.year % 10, NULL);
    left = DispDateWithPic(left, text_rect.top, 0, "space_.png");
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.month / 10, NULL);
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.month % 10, NULL);
    left = DispDateWithPic(left, text_rect.top, 0, "space_.png");
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.day / 10, NULL);
    left = DispDateWithPic(left, text_rect.top, g_RtcTime.day % 10, NULL);

}

static void PaintNameNum(PRECT pRc, void * pData)
{
    char Buf[60];

    SetPenColor(system_info.hdc, COLOR_lightwhite);
    SetTextColor(system_info.hdc, COLOR_lightwhite);
    SetBkMode(system_info.hdc, BM_TRANSPARENT);

    if(term->ncs_manager->name && term->ncs_manager->online)
    {
        if(term->language == 1)
        {
            sprintf(Buf, "终端名称:%s", term->ncs_manager->name);
        }
        else if(term->language == 0)
        {
            sprintf(Buf, "Term Name:  %s", term->ncs_manager->name);
        }
        DrawText(system_info.hdc, Buf, -1, pRc, DT_LEFT);
    }
}

static void PaintIdNum(PRECT pRc, void * pData)
{
    char Buf[30];
    SetPenColor(system_info.hdc, COLOR_lightwhite);
    SetTextColor(system_info.hdc, COLOR_lightwhite);
    SetBkMode(system_info.hdc, BM_TRANSPARENT);

    if(term->language == 1)
    {
        sprintf(Buf, "ID号:%05d", term->ncs_manager->id);
    }
    else if(term->language == 0)
    {
        sprintf(Buf, "ID # : %05d", term->ncs_manager->id);
    }
    DrawText(system_info.hdc, Buf, -1, pRc, DT_LEFT);
}

static void PaintVolBmp(PRECT pRc, void * pData)
{
    BITMAP * bitmap = NULL;
    RECT rect;
    struct audio_manager *audio_manager = term->audio_manager;
    char Buf[30];

    bitmap  = png_ha_res_find_by_name( pData);
    FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);

    memcpy(&rect, pRc, sizeof(RECT));
    SetPenColor(system_info.hdc, COLOR_lightwhite);
    SetTextColor(system_info.hdc, COLOR_lightwhite);
    SetBkMode(system_info.hdc, BM_TRANSPARENT);
    rect.left += 30;
    rect.top += 7;
    sprintf(Buf, "%d", audio_manager->get_talk_out_volume(audio_manager));
    DrawText(system_info.hdc, Buf, -1, &rect, DT_LEFT);
}

static void PaintMissedCallNum(PRECT pRc, void * pData)
{
    BITMAP * bitmap = NULL;
    int total_num = missed_call_count_get();
    char tmp[28]  = {0};

    sprintf(tmp, "T%d.png", total_num);
    bitmap  = png_ha_res_find_by_name(tmp);

    FillBoxWithBitmap(system_info.hdc, pRc->left + 10, pRc->top + 10, bitmap->bmWidth, bitmap->bmHeight, bitmap);
}

static void PaintNetBmp(PRECT pRc, void * pData)
{
    BITMAP * bitmap = NULL;

    if(net_link() == FALSE)
    {
        bitmap  = png_ha_res_find_by_name("offline.png");
    }
    else
    {
        if(term->ncs_manager->online || term->sip_manager->registered())
        {
            bitmap  = png_ha_res_find_by_name("online.png");
        }
        else
        {
            bitmap  = png_ha_res_find_by_name("outserver.png");
        }
    }
    FillBoxWithBitmap(system_info.hdc, pRc->left, pRc->top, bitmap->bmWidth, bitmap->bmHeight, bitmap);
}

int DefaultWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    bmp_res_t * bitmap;
    RECT rect = {0, 0, 800, 30};
    int scn_id = get_screen_id();
    task_t task = get_system_task();
    struct call_manager *call_manager = term->call_manager;

    switch(message)
    {
        case MSG_CREATE:
            break;
        case MSG_NCS_STATUE_BAR_PAGE:
            HandleStatueBarPageProc((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_statuebar_item, hWnd, message, wParam, lParam);
            break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
            lcd_screen_lock_status_init();
            if(PtInRect(&rect, LOWORD(lParam), HIWORD (lParam)))
            {
                SPON_LOG_INFO("lbuttonup  down\n");
                HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_statuebar_item, hWnd, message, wParam, lParam);
                return 0;
            }
            return 1;
        case MSG_KEYUP:
            switch (wParam)
            {
                case kKeyCallIn:
                    if(call_manager->calls_total_get(call_manager) == 1)
                    {
                        lcd_screen_lock_status_init();
                        EnterWin(hWnd, guicall_incomming_proc);
                    }
                    else if(call_manager->calls_total_get(call_manager) == 2)
                    {
                        EnterWin(hWnd, guicall_conference_proc);
                    }
                    return 0;
                    break;
                case kKeyCallOut:
                    if(task == TASK_NCS_MONITOR_OUTGOING)
                    {
                        EnterWin(hWnd, guicall_chatting_proc);
                    }
                    else
                    {
                        EnterWin(hWnd, guicall_out_proc);
                    }
                    return 0;
                case kKeyBcStop:
                    if(scn_id < NCS_EMERGENCY_SCREEN_ID || scn_id > NCS_BROADCAST_SCREEN_ID)
                    {
                        return 0;
                    }
                    break;
                case kKeyTaskSwitch:
                    guibroadcast_newscreen(hWnd, get_system_task());
                    return 0;
                case kKeyBcOut:
                    SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_BOOTUP_BC_SCREEN_ID);
                    return 0;
                case kKeySOSOut:
                    SPON_LOG_INFO("kKeySOSOut send\n");
                    SendNotifyMessage(hWnd, MSG_NCS_SWITCH_WND, 0, NCS_EMERGENCY_SCREEN_ID);
                    return 0;
                    //case kKeyIpRequestFailed:
                case kKeyCallStop:
                case kKeyCallReject:
                    //audio_channel_mode_set(AUDIO_CHANNEL_IDLE);
                    //set_system_task(TASK_NONE);
                    //如果三方对讲挂断一方，还不能进入ｗｅｌｃｏｍｅ界面,加以判断
                    if(call_manager->calls_total_get(call_manager) == 0)
                    {
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
                    else
                    {
                        //三方对讲，有一方挂断，进入对讲页
                        EnterWin(hWnd, guicall_chatting_proc);
                    }
                    return 0;
            }
            break;
        default:
            break;
    }
    return 1;
}

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    BITMAP * bitmap = NULL;
    struct audio_manager *audio_manager = term->audio_manager;

    if(g_firstEnter)
    {
        g_firstEnter = FALSE;
        audio_manager->audio_input_unmute(audio_manager);
    }
    switch(message)
    {
        case MSG_CREATE:
            system_info.flagCreate = TRUE;
            term->calllog_flag = 0;
            HandleCreate((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_item, hWnd);
            break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
        case MSG_NCS_WELCOM_PAGE:
            /*HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_statuebar_item, hWnd, message, wParam, lParam);*/
            HandlePageProc((TYPE_GUI_AREA_HANDLE*)gui_welcome_page_item, hWnd, message, wParam, lParam);
            return 0;
        case MSG_PAINT:
            return 0;
        case MSG_NCS_BROADCAST_WIN:
            guibroadcast_newscreen(hWnd, get_system_task());
            break;
        case MSG_ERASEBKGND:
            bitmap  = png_ha_res_find_by_name("mainwindowbg.png");
            FillBoxWithBitmap((HDC)wParam, 0, 0, bitmap->bmWidth, bitmap->bmHeight, bitmap);
            return 0;
        case MSG_NCS_SWITCH_WND:
            EnterNewScreen(hWnd, (int)lParam);
            break;
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

void EnterNewScreen(HWND hWnd, int scrn_id)
{
    switch(scrn_id)
    {
        case NCS_IDLE_SCREEN_ID:
            set_system_task(TASK_NONE);
            EnterWin(hWnd, WelComeWinProc);
            break;
        case NCS_BOOTUP_BC_SCREEN_ID:
        case NCS_EMERGENCY_SCREEN_ID:
        case NCS_BROADCAST_SCREEN_ID:
            guibroadcastwin_newscreen(hWnd, scrn_id);
            break;
        case NCS_OUTGOINGCALL_SCREEN_ID:
            EnterWin(hWnd, guicall_out_proc);
            break;
        case NCS_INCOMMINGCALL_SCREEN_ID:
            lcd_screen_lock_status_init();
            EnterWin(hWnd, guicall_incomming_proc);
            break;
        case NCS_CHATTING_SCREEN_ID:
            EnterWin(hWnd, guicall_chatting_proc);
            break;
        case NCS_MISSEDCALL_SCREEN_ID:
            //guimissedcall_newscreen(hWnd);
            guicalllog_newscreen(hWnd);
            break;
        case NCS_SETTING_SCREEN_ID:
            EnterWin(hWnd, SettingWinProc);
            //guiloginwin_newscreen(hWnd);
            break;
        case NCS_INPUTDIGITAL_SCREEN_ID:
            InputDigtal_NewScreen(hWnd);
            break;
        case NCS_OTHER_SCREEN_ID:
            break;
        default:
            break;
    }
    set_screen_id(scrn_id);
}

/***************************************************************************************************
 * 函 数 名: MainWinProc
 * 日    期: 2016-09-22 11:40
 * 文 件 名: main_window.c
 * 功    能: 主任务处理
 ****************************************************************************************************/
int MainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    system_info.main_hwnd = hWnd;
    if(!DefaultWinProc(hWnd, message, wParam, lParam))
    {
        return 0;
    }

    if(g_StauteFun != NULL)
    {
        if(!g_StauteFun(hWnd, message, wParam, lParam))
        {
            return 0;
        }
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}




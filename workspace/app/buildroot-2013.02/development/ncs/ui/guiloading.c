#include  "guiloading.h"

int WelComeWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static int guiloading_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

#define LOAD_IMAGE_NUM		4
static BITMAP sg_load_bitmap[LOAD_IMAGE_NUM];
static pthread_t sg_load_tid;

static void *image_load_thread(void *arg)
{
    struct broadcast *bc = term->ncs_manager->bc;
    terminal_t *terminal = (terminal_t *)term;

    HDC hdc = GetClientDC(system_info.main_hwnd);

    terminal->image_load = 2;
    system_bmp_res_loading_ex(hdc);
    terminal->image_load = 1;
    ReleaseDC(hdc);
    ncs_login(0);
	/*system("cd /var/www/shell/ && chmod 777 boa_start.sh && ./boa_start.sh &");*/

    SendNotifyMessage(system_info.main_hwnd, MSG_NCS_SWITCH_WND, 0, NCS_IDLE_SCREEN_ID);
    pthread_exit(NULL);
    return NULL;
}

void guiloading_newscreen(HWND hWnd)
{
    int i = 0;
    char buf[64] = {0};

    for(i = 0; i < LOAD_IMAGE_NUM; i++)
    {
        sprintf(buf, "%sload_%d_point.png", SYSTEM_PNG_RES_PATH, i + 1);
        LoadBitmap(system_info.hdc, &sg_load_bitmap[i], buf);
    }
    pthread_create(&hWnd, NULL, image_load_thread, hWnd);

    EnterWin(hWnd, guiloading_screen_proc);
}

static void guibroadcast_image_display(char *pData, short x, short y)
{
    BITMAP bm;
    char buf[128] = {0};

    sprintf(buf, "%s%s", SYSTEM_PNG_RES_PATH, pData);
    LoadBitmap(system_info.hdc, &bm, buf);
    FillBoxWithBitmap(system_info.hdc,
                      x,
                      y,
                      bm.bmWidth,
                      bm.bmHeight,
                      &bm);
    UnloadBitmap(&bm);

}

static int guiloading_screen_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static int i = 0;

    if(g_firstEnter)
    {
        g_firstEnter = FALSE;
    }

    switch(message)
    {
        case MSG_CREATE:
            break;
        case MSG_NCS_WELCOM_PAGE:
            if(term->image_load == 2)
            {
                RECT rc = {255, 215, 255 + 368, 215 + 55};

                InvalidateRect_Ex(hWnd, &rc, TRUE);
                system_info.hdc = BeginPaint(hWnd);
                FillBoxWithBitmap(system_info.hdc, rc.left, rc.top, 368, 55, &sg_load_bitmap[i]);
                EndPaint(hWnd, system_info.hdc);

                if(i++ >= LOAD_IMAGE_NUM)
                {
                    i = 0;
                }
            }
            break;
        case MSG_LBUTTONDOWN:
        case MSG_LBUTTONUP:
            break;
        case MSG_PAINT:
            return 0;
        case MSG_ERASEBKGND:
            return 0;
        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
        case MSG_NCS_SWITCH_WND:
            for(i = 0; i < LOAD_IMAGE_NUM; i++)
            {
                UnloadBitmap(&sg_load_bitmap[i]);
            }
            EnterNewScreen(hWnd, (int)lParam);
            break;
        case MSG_DESTROY:
            return 0;
        default:
            break;
    }
    return 1;
}

#include <string.h>

#include "status_bar.h"
#include "system_info.h"
#include "term.h"


static void draw_volume(HDC hdc)
{
    char buf[20];
    /*struct audio_manager *audio_manager = term->audio_manager;*/

    SetPenColor(hdc, COLOR_lightwhite);
    SetBrushColor(hdc, system_info.status_bar_bg_color);
    SetTextColor(hdc, COLOR_lightwhite);
    SetBkMode(hdc, BM_TRANSPARENT);
    FillBox(hdc, g_rcScr.right - 43, 0, 20, system_info.status_bar_height);

	/*sprintf(buf, "%d", audio_manager->get_talk_out_volume(audio_manager));*/
	sprintf(buf, "%d", 12);
	TextOut(hdc, g_rcScr.right - 43, (system_info.status_bar_height - GetFontHeight(HDC_SCREEN)) / 2, buf);
}

void draw_status_bar_1081(HDC hdc)
{
    RECT id_rect;
    char buf[128] = {0};
    int len, height;
    SIZE sz;
    BITMAP bm;

    memset(buf, 0, 16);
    /*if (term->ncs_manager->name != NULL)*/
    /*{*/
        /*if (strlen(term->ncs_manager->name))*/
        /*{*/
            /*if (strlen(term->ncs_manager->name) > 16)*/
            /*{*/
                /*strncpy(buf, term->ncs_manager->name, 16);*/
            /*}*/
            /*else*/
                /*strcpy(buf, term->ncs_manager->name);*/
        /*}*/
    /*}*/


    SetBrushColor(hdc, RGB2Pixel(HDC_SCREEN, 46, 49, 54));
    FillBox(hdc, 0, 0, g_rcScr.right, system_info.status_bar_height);

    height = GetFontHeight(hdc);
    SetTextColor(hdc, COLOR_lightwhite);
    SetBkMode(hdc, BM_TRANSPARENT);
    TextOut(hdc, 0, (system_info.status_bar_height - height) / 2, buf);

	draw_volume(hdc);

	memset(buf, 0, sizeof(buf));

	if (net_link() == FALSE)
		sprintf(buf, "%s/%s", SYSTEM_PNG_RES_PATH, "net_error.bmp");
	else
	{
		/*if (term->ncs_manager->online || term->sip_manager->registered())*/
			sprintf(buf, "%s/%s", SYSTEM_PNG_RES_PATH, "online.bmp");
		/*else*/
			/*sprintf(buf, "%s/%s", SYSTEM_PNG_RES_PATH, "offline.bmp");*/
	}

	LoadBitmap(hdc, &bm, buf);
	
	bm.bmType = BMP_TYPE_COLORKEY;
	bm.bmColorKey = GetPixelInBitmap(&bm, 0, 0);
  
	FillBoxWithBitmap(hdc, g_rcScr.right - bm.bmWidth, (system_info.status_bar_height - bm.bmHeight) / 2, bm.bmWidth, bm.bmHeight, &bm);
	UnloadBitmap(&bm);

	sprintf(buf, "%s/%s", SYSTEM_PNG_RES_PATH, "volume.bmp");
	LoadBitmap(hdc, &bm, buf);
	
	bm.bmType = BMP_TYPE_COLORKEY;
	bm.bmColorKey = GetPixelInBitmap(&bm, 0, 0);
  
	FillBoxWithBitmap(hdc, g_rcScr.right - 55, (system_info.status_bar_height - bm.bmHeight) / 2, bm.bmWidth, bm.bmHeight, &bm);
	UnloadBitmap(&bm);
}


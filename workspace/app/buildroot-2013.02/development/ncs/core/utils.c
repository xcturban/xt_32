
#include "utils.h"
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

uint8_t dec_2_bcd(int dec)
{
    return (((dec / 10 ) << 4) | dec % 10);
}

int draw_box(HDC hdc, int x0, int y0, int x1, int y1, int bordcolor, int fillcolor)
{
    SetPenColor(hdc, bordcolor);
    MoveTo(hdc, x0, y0);
    LineTo(hdc, x0, y1);
    LineTo(hdc, x1, y1);
    LineTo(hdc, x1, y0);
    LineTo(hdc, x0, y0);
	SetBrushColor(hdc, fillcolor);
	FillBox(hdc, x0 + 1, y0 + 1, x1 - x0 -1, y1-y0-1);
}




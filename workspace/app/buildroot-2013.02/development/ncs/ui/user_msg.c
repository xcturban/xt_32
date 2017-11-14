#include "user_msg.h"
#include "system_info.h"
#include "term.h"

static screen_id_t focus_screen_id = NCS_OTHER_SCREEN_ID;

void send_user_message(int msg, WPARAM wparam, LPARAM lparam)
{
	SendNotifyMessage(system_info.main_hwnd, msg, wparam, lparam);
}


void virtual_key_send(int key)
{
	send_user_message(MSG_KEYUP,key,0);
}

void set_screen_id(int scrn_id)
{
	focus_screen_id = scrn_id;
}

int get_screen_id(void)
{
	return focus_screen_id;
}

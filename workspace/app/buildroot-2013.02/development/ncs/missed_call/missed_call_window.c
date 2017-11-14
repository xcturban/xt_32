#include <errno.h>

#include <ncs_event/ncs_event.h>

#include "missed_call_window.h"
#include "language.h"
#include "system_info.h"
/*#include "keypad.h"*/
/*#include "window_focus.h"*/
/*#include "missed_call.h"*/
#include "lpconfig.h"
#include "call.h"
#include "term.h"
#include "ncs.h"


/*void missed_call_win_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)*/
/*{*/
	/*int target_id;*/
	/*int board_id;*/
	/*int idx;*/
	/*struct ncs_manager *ncs_manager = term->ncs_manager;*/

	/*switch (message) {*/

	/*case MSG_KEYUP:*/
		/*switch (wParam) {*/
		/*case kKeyC:*/
			/*if (term->customer_type == CUSTOMER_GXGS)*/
				/*missed_call_clear();*/
			/*set_focus_window(WELCOME_WINDOW);*/
			/*InvalidateRect(hWnd, NULL, TRUE);*/
			/*break;*/

		/*case kKeyF2:*/
		/*case kKeyF3:*/
		/*case kKeyF4:*/
		/*case kKeyF5:*/
		/*case kKeyF6:*/
		/*case kKeyF7:*/
		/*case kKeyF8:*/

			/*idx = missed_call_count_get() - (wParam - 12);*/
			/*if (idx >= 0) {*/
				/*missed_call_id_get(idx, &target_id, &board_id);*/
				/*SPON_LOG_INFO("=====>%s - idx:%d target_id:%d board_id:%d\n", __func__, idx, target_id, board_id);*/
				/*missed_call_del(idx);*/
				/*if (target_id > 0) {*/
					/*int ret = ncs_manager->ncs_ip_request(ncs_manager, target_id, board_id, TALK);*/
					/*if (ret >= 0) {*/
						/*set_system_task(TASK_NCS_CALL_OUTGOING);*/
						/*////virtual_key_send(kKeyCallOut);*/
					/*}*/
				/*}*/
			/*}*/
			/*break;*/

		/*case kKeyTalk:*/

			/*if (term->dial_rapid == DIAL_NCS)*/
				/*set_system_task(TASK_NCS_CALL_OUTGOING);*/
			/*else*/
				/*set_system_task(TASK_SIP_CALL_OUTGOING);*/
			/*ENTER_CALL_DIAL_WINDOW();*/
			/*break;*/
		/*case kKeyInfo:*/
			/*ENTER_LOGIN_WINDOW();*/
			/*break;*/
		/*case kKeyFile:*/
			/*ENTER_FILE_PLAY_LIST_WINDOW();*/
			/*break;*/
		/*case kKeyBC:*/
			/*ENTER_BC_MENU_WINDOW();*/
			/*break;*/
		/*case kKeyMonitor:*/

			/*set_system_task(TASK_NCS_MONITOR_OUTGOING);*/
			/*ENTER_CALL_DIAL_WINDOW();*/
			/*break;*/

		/*case kKeyCallIn:*/
			/*ENTER_CALL_INCOMING_WINDOW();*/
			/*break;*/

		/*case kKeyCallOut:*/
			/*ENTER_CALL_OUTGOING_WINDOW();*/
			/*break;*/

		/*case kKeyTaskSwitch:*/
			/*ENTER_BROADCASTING_WINDOW();*/
			/*break;*/

		/*case kKeyBcOut:*/
			/*ENTER_BROADCASTING_WINDOW();*/
			/*break;*/

		/*default:*/
			/*break;*/
		/*}*/
		/*break;*/


	/*default:*/
		/*break;*/
	/*}*/
/*}*/

/*static int missed_call_list_empty_window(HDC hdc)*/
/*{*/
	/*RECT text_rect;*/
	/*int y_start = system_info.step;*/


	/*char text_buf[128] = {0};*/

	/*RoundRect(hdc, system_info.margin,y_start - system_info.title_bar_height,*/
				/*g_rcScr.right - system_info.margin, system_info.step * 3 + y_start,*/
				/*system_info.round_size, system_info.round_size);*/

	/*get_str_from_id(MISSED_CALL_STR_ID, text_buf);*/
	/*draw_title_bar(hdc, system_info.margin,y_start - system_info.title_bar_height,*/
				/*g_rcScr.right - system_info.margin, system_info.step * 3 + y_start, text_buf);*/

	/*text_rect.left = system_info.margin;*/
	/*text_rect.top = system_info.status_bar_height + 100;*/
	/*text_rect.right = g_rcScr.right - system_info.margin;*/
	/*text_rect.bottom = system_info.status_bar_height + 200;*/

	/*get_str_from_id(MISSED_CALL_EMPTY_STR_ID, text_buf);*/

	/*DrawText(hdc, text_buf, -1, &text_rect, DT_CENTER);*/

	/*return 0;*/
/*}*/

/*int draw_missed_call_window(HDC hdc)*/
/*{*/
	/*char text_buf[128];*/
	/*int margin = system_info.margin;*/
	/*int step = system_info.step;*/
	/*int round_size = system_info.round_size;*/
	/*int y_start = step - system_info.title_bar_height;*/
	/*int text_height = GetFontHeight(hdc);*/
	/*int i;*/
	/*char *name;*/
	/*LpConfig *cfg;*/
	/*char section[128];*/
	/*int total = missed_call_count_get();*/
	/*struct ncs_manager *ncs_manager = term->ncs_manager;*/

	/*if (total == 0) {*/
		/*missed_call_list_empty_window(hdc);*/
		/*return 0;*/
	/*}*/


	/*RoundRect(hdc, margin, y_start, g_rcScr.right - margin, step*total + system_info.title_bar_height + y_start, round_size, round_size);*/

	/*get_str_from_id(MISSED_CALL_STR_ID, text_buf);*/
	/*draw_title_bar(hdc, margin, y_start, g_rcScr.right - margin, step*total + system_info.title_bar_height + y_start, text_buf);*/

	/*y_start += system_info.title_bar_height;*/

	/*SetTextColor(hdc, COLOR_black);*/

	/*cfg = lp_config_new(INI_MISSED_CALL_FILE);*/
	/*if (cfg == NULL)*/
	/*{*/
		/*SPON_LOG_ERR("=====>%s - lp_config_new failed:%s\n", __func__, strerror(errno));*/
		/*return -1;*/
	/*}*/

	/*total = lp_config_get_int(cfg, "missed_call_total", "total", 0);*/

	/*for (i = 0; i < total; i++)*/
	/*{*/
		/*int target_id, board_id;*/
		/*char *date;*/

		/*sprintf(section, "missed_call_%d", total - i);*/
		/*target_id = lp_config_get_int(cfg, section, "target_id", 0);*/
		/*board_id = lp_config_get_int(cfg, section, "board_id", 0);*/
		/*date = lp_config_get_string(cfg, section, "date", NULL);*/


		/*if (target_id > 0)*/
		/*{*/
			/*name = ncs_manager->ncs_term_name_get_by_id(ncs_manager, target_id);*/
			/*if (name)*/
			/*{*/
				/*if (board_id > 0)*/
					/*sprintf(text_buf, "%s - %d - %d", name, target_id, board_id);*/
				/*else*/
					/*sprintf(text_buf, "%s - %d", name, target_id);*/
			/*}*/
			/*else*/
			/*{*/
				/*if (board_id > 0)*/
					/*sprintf(text_buf, "%d - %d", target_id, board_id);*/
				/*else*/
					/*sprintf(text_buf, "%d", target_id);*/
			/*}*/

			/*if (date)*/
				/*TextOut(hdc, margin*2, (step/2 - text_height) / 2 + y_start, date);*/

			/*TextOut(hdc, margin*2, (step/2 - text_height) / 2 + y_start + step/2, text_buf);*/
		/*}*/


		/*if (i != total - 1)  {*/
			/*y_start += step;*/
			/*MoveTo(hdc, margin, y_start);*/
			/*LineTo(hdc, g_rcScr.right - margin, y_start);*/
		/*}*/
	/*}*/

	/*lp_config_destroy(cfg);*/

	/*return 0;*/
/*}*/

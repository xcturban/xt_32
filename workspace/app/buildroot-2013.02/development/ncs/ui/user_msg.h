#ifndef __USER_MSG_H__
#define __USER_MSG_H__


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


#define MSG_NCS_CALL_INCOMING						(MSG_USER + 1)
#define MSG_NCS_CALL_START	 						(MSG_USER + 2)
#define MSG_NCS_CALL_END	 						(MSG_USER + 3)
#define MSG_NCS_CALL_STOP							(MSG_USER + 4)

#define MSG_NCS_CLASS_KEY_UP 						(MSG_USER + 5)
#define MSG_NCS_BROADCAST_INCOMING					(MSG_USER + 6)
#define MSG_NCS_BROADCAST_INCOMING_END				(MSG_USER + 7)
#define MSG_NCS_MONITOR_INCOMING					(MSG_USER + 8)


#define MSG_NCS_MEDIA_STATUS_UPDATE					(MSG_USER + 9)
#define MSG_NCS_MEDIA_LINK_UPDATE					(MSG_USER + 10)
#define MSG_NCS_VIDEO_RECV_BEGIN					(MSG_USER + 11)
#define MSG_NCS_VIDEO_RECV_END						(MSG_USER + 12)
#define MSG_NCS_AOD_BROADCAST_INCOMING				(MSG_USER + 13)


#define MSG_NCS_SPEARKER_MUTE_UPDATE				(MSG_USER + 14)

#define MSG_FILE_PLAY_MUSIC_BAR_UPDATE				(MSG_USER + 15)
#define MSG_FILE_PLAY_MUSIC_END						(MSG_USER + 16)
#define MSG_FILE_PLAY_STATUS_UPDATE					(MSG_USER + 17)
#define MSG_FILE_PLAY_MODE_UPDATE					(MSG_USER + 18)
#define MSG_FILE_PLAY_LOOP_UPDATE					(MSG_USER + 19)
#define MSG_FILE_PLAY_LIST_UPDATE					(MSG_USER + 20)

#define MSG_AOD_PLAY_MUSIC_BAR_UPDATE				(MSG_USER + 21)
#define MSG_AOD_PLAY_MUSIC_END						(MSG_USER + 22)
#define MSG_AOD_PLAY_STATUS_UPDATE					(MSG_USER + 23)
#define MSG_AOD_PLAY_MODE_UPDATE					(MSG_USER + 24)
#define MSG_AOD_PLAY_LOOP_UPDATE					(MSG_USER + 25)
#define MSG_AOD_PLAY_LIST_UPDATE					(MSG_USER + 26)
#define MSG_AOD_PLAY_THREAD_END						(MSG_USER + 27)
#define MSG_AOD_PLAY_PATH_UPDATE					(MSG_USER + 28)
#define MSG_NCS_SPEARKER_VOLUME_UPDATE				(MSG_USER + 29)
#define MSG_AOD_PLAY_LOOP_AB_END						(MSG_USER + 30)

#define MSG_NCS_PASSWD_BAR_UPDATE						(MSG_USER + 31)
#define MSG_NCS_PASSWD_WINDOW_TRIGGER					(MSG_USER + 32)
#define MSG_NCS_TERM_NAME_UPDATE						(MSG_USER + 33)

#define MSG_NCS_CHANGE_WINDOW_ENTER						(MSG_USER + 34)

// MESSAGE
#define MSG_NCS_WELCOM_PAGE                (MSG_USER + 0x100)
#define MSG_NCS_STATUE_BAR_PAGE            (MSG_USER + 0x101)
#define MSG_NCS_INPUT_DIGTAL_PAGE          (MSG_USER + 0x102)
#define MSG_NCS_ONLY_CHECK                 (MSG_USER + 0x103)
#define MSG_NCS_SETTING_DIGTAL_PAGE        (MSG_USER + 0x104)
#define MSG_NCS_ENTRY_PAGE                 (MSG_USER + 0x105)
#define MSG_NCS_SWITCH_PAGE                (MSG_USER + 0x106)
#define MSG_NCS_CHATTING_REFRESH           (MSG_USER + 0x107)
#define MSG_NCS_SETTING_KEY_VALUE          (MSG_USER + 0x108)
#define MSG_NCS_BROADCAST_UPDATE           (MSG_USER + 0x109)
#define MSG_NCS_BROADCAST_HANDLE           (MSG_USER + 0x10A)
#define MSG_NCS_SWITCH_WND                 (MSG_USER + 0x200)
#define MSG_NCS_BROADCAST_FILE_LIST_UPDATE (MSG_USER + 0x201)
#define MSG_NCS_BROADCAST_WIN              (MSG_USER + 0x202)
#define MSG_NCS_VOL_VALUE_UPDATE           (MSG_USER + 0x203)
#define MSG_NCS_REFLESH_GUI                (MSG_USER + 0x204)

// WPARAM
#define WPARAM_0                   (MSG_USER + 0x100)
#define WPARAM_1                   (MSG_USER + 0x100 +1)

typedef enum 
{
	NCS_IDLE_SCREEN_ID 			= 0x0000,
	NCS_EMERGENCY_SCREEN_ID,
	NCS_BOOTUP_BC_SCREEN_ID,	
	NCS_BROADCAST_SCREEN_ID,
	NCS_OUTGOINGCALL_SCREEN_ID,
	NCS_INCOMMINGCALL_SCREEN_ID,
	NCS_CHATTING_SCREEN_ID,
	NCS_MISSEDCALL_SCREEN_ID,
	NCS_INPUTDIGITAL_SCREEN_ID,
	NCS_SETTING_SCREEN_ID,
	NCS_OTHER_SCREEN_ID,
}screen_id_t;

#define WPARAMBASE          0x200
#define kKeyCallIn          (WPARAMBASE+1)
#define kKeyCallStop        (WPARAMBASE+2)
#define kKeyCallBusy        (WPARAMBASE+3)
#define kKeyCallReject      (WPARAMBASE+4)
#define kKeyIpRequestFailed (WPARAMBASE+5)
#define kKeyCallStart       (WPARAMBASE+6)
#define kKeyOffhook       	(WPARAMBASE+7)
#define kKeyAudioChnSwitch  (WPARAMBASE+8)
#define kKeyCallOut       	(WPARAMBASE+9)
#define kKeyTaskSwitch      (WPARAMBASE+0x10)
#define kKeyTaskIdle       	(WPARAMBASE+0x11)
#define kKeyLoginOk       	(WPARAMBASE+0x12)
#define kKeyLoginExit       (WPARAMBASE+0x13)
#define kKeyBcOut       	(WPARAMBASE+0x14)
#define kKeyBcStop       	(WPARAMBASE+0x15)
#define kKeyCallInfoUpdate  (WPARAMBASE+0x16)
#define kKeySOSOut       	(WPARAMBASE+0x17)
#define KKEYWARINGBEGIN       	(WPARAMBASE+0x18)
#define KKEYWARINGOVER       	(WPARAMBASE+0x19)


// LPARAM
#define LPARAM_0                   (MSG_USER + 0x100)
#define LPARAM_1                   (LPARAM_0+1)

void send_user_message(int msg, WPARAM wparam, LPARAM lparam);
void virtual_key_send(int key);
void set_screen_id(int scrn_id);
int get_screen_id(void);

#endif


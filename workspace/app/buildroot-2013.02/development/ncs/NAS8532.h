#ifndef __NAS8532_H__
#define __NAS8532_H__

//SIP cal
#define NAS8532_SIP				1
//#define __NCS_SUPPORT_ONVIF__			1
#define __NCS_AUDIO_THREAD_UPDATE__		1
#define __NCS_SUPPORT_CAMERA__			1

//PATH
//#define SYSTEM_PNG_RES_PATH 		"/usr/share/system/"
#define SYSTEM_PNG_RES_PATH 	"/mnt/nand1-2/system-res/png/"

#define SYSTEM_RES_PATH 		"/usr/share/system"

#define SYSTEM_LANGUAGE_PATH 	"/usr/share/language"

#define LINPHONE_CONFIG_FILE 	"/root/linphonerc"

#define INI_BOARD_FILE 			"/var/www/ini/board_cfg.txt"

#define INI_PHONEBOOK_FILE 		"/var/www/ini/board_cfg.txt"
#define INI_WARRINGCFG_FILE		"/var/www/ini/warring_cfg.txt"


#define FILE_SDCARD_DIR 			"/mnt/sdcard/SPON/"

#define FILE_USB_DIR          "/mnt/nandcard/SPON/"


#define INI_CONFIG_FILE 		"/var/www/ini/sys_cfg.txt"

#define INI_CONFIG_BAK_FILE 	"/mnt/nand1-2/ini/ini_bak/sys_cfg.txt"

#define INI_VERSION_FILE		"/var/www/ini/VersionInfo.txt"

#define INI_MISSED_CALL_FILE	"/var/www/ini/missed_call.txt"
#define INI_DIALED_CALL_FILE	"/var/www/ini/dialed_call.txt"
#define INI_RECEIVED_CALL_FILE	"/var/www/ini/received_call.txt"

#define WEB_CONFIG_PIPE 		"web_cfg_pipe"

#define INI_OFFLINE_CALL_FILE	"/var/www/ini/alone_cfg.txt"

#define RING_FILE_PATH			"/mnt/nand1-2/system-res/ringfile/"
#define FILE_FTP_PATH         "/mnt/nand1-2/sd/user-res/remotefile/"
#define REMOTE_RING				"/usr/share/sounds/linphone/ringback.wav"

#define LOCAL_RING				"/usr/share/sounds/linphone/rings/oldphone.wav"

//#define BC_START_RING 			"/mnt/nand1-2/system-res/ringfile/BroadcastStart_22k.wav"

//#define BC_STOP_RING 			"/mnt/nand1-2/system-res/ringfile/BroadcastEnd_22k.wav"
#define BC_START_RING 			"/mnt/nand1-2/sd/user-res/ringfile/BroadcastStart_22k.wav"
#define BC_STOP_RING 			"/mnt/nand1-2/sd/user-res/ringfile/BroadcastEnd_22k.wav"

//DATA
#define LCD_WIDTH			800
#define LCD_HEIGH			480

//COMMON BTN
#define BTN_W				149
#define BTN_H				56
#define BTN_BACK_L			(LCD_WIDTH-BTN_W-2)
#define BTN_BACK_T			(LCD_HEIGH-BTN_H+2)
#define BTN_BACK_R			(LCD_WIDTH)
#define BTN_BACK_B			(LCD_HEIGH)

#define BTN_OK_UP			"key-confirm-up.png"
#define BTN_OK_DOWN 		"key-confirm-down.png"

#define BTN_DEL_UP			"key-del-up.png"
#define BTN_DEL_DOWN 		"key-del-down.png"

#define BTN_BACK_UP			"key-return-up.png"
#define BTN_BACK_DOWN 		"key-return-down.png"

#define MAX_MISSED_CALL 		(8)
#define MAX_DIALED_CALL 		(8)
#define MAX_RECEIVED_CALL		(8)
#define MAX_CALLLOG_NUM			(8)

#define CALL_END_Y_OFFSET		(0-2)

#define MSG_NCS_CLASS_KEY_UP 						(MSG_USER + 5)
#define MSG_NCS_TALK_KEY_UP 						(MSG_USER + 6)


#endif

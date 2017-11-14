
/* sponlog.h */

#ifndef __SPONLOG_H__
#define __SPONLOG_H__

//#define SPON_RELEASE

#define SPON_LOGLVL_FATAL	0			/* 致命错误, 会导致程序退出 */
#define SPON_LOGLVL_ERR		1			/* 一般错误,不会导致程序退出 */
#define SPON_LOGLVL_SYS     2         /* 提示信息: 必要的日志信息输出 */ 
#define SPON_LOGLVL_INFO	4			/* 提示信息: 必要时才加输出 */
#define SPON_LOGLVL_DBG		8			/* 调试信息，需带上DBG功能号，以便于指定输出特定功能的调试信息 */

#define SPON_LOGTYPE_PRINTF		0x1		/* printf打印输出 */
#define SPON_LOGTYPE_SYSLOG		0x2		/* syslog输出 */
#define SPON_LOGTYPE_BOTH		SPON_LOGTYPE_PRINTF | SPON_LOGTYPE_SYSLOG


/* 自定义DEBUG功能号 */

#define MAX_DBG_VERSION		1024
/* 不能超过MAX_DBG_VERSION */


#define USRLOG_STRAT    	0
#define USRLOG_LOGIN    	1
#define USRLOG_REBOOT   	2
#define USRLOG_OFFLINE  	3
#define USRLOG_TALKSTART	4
#define USRLOG_TALKEND		5
#define USRLOG_BCOUT_START	6
#define USRLOG_BCOUT_END	7
#define USRLOG_BCIN_START	8
#define USRLOG_BCIN_END		9


static char sys_log_info_en[][64] = 
{
	"Equipment start-up success",
	"The login server successfully",
	"Equipment to restart",
	"Disconnect from the server",	
	"Talk start",
	"Talk end",	
	"Broadcast out start",
	"Broadcast out end",
	"Broadcast in start",
	"Broadcast in end",
};



const char* sys_log_module;
void SPON_LOG_INIT(const char *module_name, unsigned int log_type, unsigned int logout_level);
void user_log_init();
void user_log(unsigned int LOG_IDX, ...);
void web_user_log(unsigned int LOG_IDX, ...);


#ifdef SPON_RELEASE

#define SPON_LOG_FATAL(fmt, arg...)	
#define SPON_LOG_ERR(fmt, arg...)
#define SPON_LOG_INFO(fmt, arg...)
//#define SPON_LOG_SYS(fmt, arg...)
#define SPON_LOG_DBG(dbg_version, fmt, arg...)

#define PRINT_BUF(buf, len)

#else

void SPON_LOG_PRINT(int logout_level,int dbg_version, const char *fmt, ...);

#define SPON_LOG_FATAL(fmt, arg...)	do{SPON_LOG_PRINT(SPON_LOGLVL_FATAL, 0, "[FTL][%s][%s:%d]"fmt, sys_log_module, __FILE__, __LINE__, ##arg);abort();}while(0)
#define SPON_LOG_ERR(fmt, arg...)		SPON_LOG_PRINT(SPON_LOGLVL_ERR, 0, "[ERR][%s][%s:%d]"fmt, sys_log_module, __FILE__, __LINE__, ##arg)
#define SPON_LOG_INFO(fmt, arg...)		SPON_LOG_PRINT(SPON_LOGLVL_INFO, 0, "[INF][%s][%s:%d]"fmt, sys_log_module, __FILE__, __LINE__, ##arg)
//#define SPON_LOG_INFO(fmt, arg...)		printf("[INFO][%s][%s:%d]"fmt, sys_log_module, __FILE__, __LINE__, ##arg)
//#define SPON_LOG_SYS(fmt, arg...)      SPON_LOG_PRINT(SPON_LOGLVL_SYS, 0, "[SYS][%s]"fmt, sys_log_module, ##arg) 
#define SPON_LOG_DBG(dbg_version, fmt, arg...)		SPON_LOG_PRINT(SPON_LOGLVL_DBG, dbg_version, "[DBG][%s][%s:%d]"fmt, sys_log_module, __FILE__, __LINE__, ##arg)


#define PRINT_BUF(buf, len)		do{			\
	int i = 0;								\
	for(i = 0; i < len; i++)					\
		SPON_LOG_INFO("0x%x ", buf[i]);		\
	SPON_LOG_INFO("\n");					\
}while(0)


#endif
#define WEB_SYS_LOG(INDEX) 	web_user_log(INDEX)


#endif


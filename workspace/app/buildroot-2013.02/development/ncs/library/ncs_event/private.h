#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <common_lib/common_lib.h>
#include "ncs_event.h"




#define UDP_MAX_SIZE (1500)

/* 锟斤拷通锟斤拷息头锟斤拷锟斤拷 */
#define NCS_EVENT_DATA_HEADER_LEN (8)

/* 锟斤拷通锟斤拷息锟结构 */
typedef struct
{
    uint16_t id; /* 锟秸讹拷ID锟脚硷拷1 */
    uint8_t msg; /* 锟斤拷息锟斤拷锟斤拷 */
    uint8_t param1; /* 锟斤拷锟斤拷1 */
    uint8_t param2; /* 锟斤拷锟斤拷2 */
    uint8_t param3; /* 锟斤拷锟斤拷3 */
    uint8_t param4; /* 锟斤拷锟斤拷4 */
    uint8_t param5; /* 锟斤拷锟斤拷5 */
    uint8_t extra_data[1]; /* 锟斤拷锟斤拷锟斤拷锟斤拷 */
} __attribute__ ((packed)) ncs_event_data_t;


/* 锟姐播锟斤拷锟斤拷 */
typedef enum
{
    MULTICAST, /* 锟介播 */
    UNICAST, /* 锟斤拷锟斤拷 */
} broadcast_type_t;

typedef struct
{
    uint16_t id; /* 锟秸讹拷ID */
    bool_t online; /* 锟秸讹拷锟斤拷锟竭憋拷锟斤拷 */
    int sfd; /* socket锟侥硷拷锟斤拷锟斤拷锟斤拷 */
    uint16_t boards_status; /* 锟斤拷锟斤拷状态锟斤拷锟斤拷锟节达拷锟斤拷锟斤拷锟斤拷锟借备 */

    struct sockaddr server_sockaddr; /* 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷址 */
    struct sockaddr server_master_sockaddr; /* 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷址 */
    struct sockaddr server_back_sockaddr; /* 锟斤拷锟矫凤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷址 */
    uint8_t server_type; //服务器类型, 0为备用服务器，　１为主服务器
    pthread_t event_get_tid; /* 锟斤拷通锟斤拷息锟斤拷锟斤拷锟竭筹拷ID */

    pthread_mutex_t event_send_mutex;

    uint8_t multi_ip_1; /* 锟介播锟斤拷址1锟轿ｏ拷锟斤拷锟节凤拷锟酵广播 */
    uint8_t multi_ip_2; /* 锟介播锟斤拷址2锟轿ｏ拷锟斤拷锟节凤拷锟酵广播 */
    uint8_t multi_ip_3; /* 锟介播锟斤拷址3锟轿ｏ拷锟斤拷锟节凤拷锟酵广播 */
    struct sockaddr msg_multi_sockaddr; /* 锟介播锟斤拷址锟斤拷锟斤拷锟节凤拷锟酵广播 */
    struct sockaddr data_multi_sockaddr;
    broadcast_type_t bc_type; /* 锟姐播锟斤拷锟斤拷锟斤拷锟斤拷 */
	int server_type_host_rcdno;            // 设备本身登陆服务器类型记录号

    int handshake_period; /* 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街碉拷时锟斤拷锟斤拷锟斤拷 */
    int handshake_timeout; /* 锟斤拷锟街筹拷时时锟斤拷 */
    int handshake_timeout_count; /* 锟斤拷锟街筹拷时锟斤拷锟斤拷=handshake_timeout/handshake_period */
    int login_period; /* 锟斤拷锟斤拷锟斤拷锟斤拷陆锟斤拷时锟斤拷锟斤拷锟斤拷 */
    timer_t login_timer_id; /* 锟斤拷陆锟斤拷时锟斤拷ID */
//	timer_t login_back_timer_id; /* 锟斤拷锟矫碉拷陆锟斤拷时锟斤拷ID */
    timer_t handshake_timer_id; /* 锟斤拷锟街讹拷时锟斤拷ID */
//	timer_t handshake_back_timer_id; /* 锟斤拷锟斤拷锟斤拷锟街讹拷时锟斤拷ID */
    bool_t login_timer_start; /* 锟斤拷陆锟斤拷时锟斤拷锟斤拷始锟斤拷锟斤拷 */
//	bool_t login_back_timer_start;/* 锟斤拷锟矫碉拷陆锟斤拷时锟斤拷锟斤拷始锟斤拷锟斤拷 */
    bool_t handshake_timer_start; /* 锟斤拷锟街讹拷时锟斤拷锟斤拷始锟斤拷锟斤拷 */
//	bool_t handshake_back_timer_start; /* 锟斤拷锟斤拷锟斤拷锟街讹拷时锟斤拷锟斤拷始锟斤拷锟斤拷 */

    void *private_data; /* 锟斤拷锟节憋拷锟斤拷锟矫伙拷锟斤拷锟斤拷私锟斤拷锟斤拷锟斤拷 */

//	char hostname_ip[20];	//锟斤拷时锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷ip锟斤拷锟皆憋拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷ip锟饺斤拷
//	bool_t master_server_login_success;		//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷陆锟角凤拷锟斤拷陆锟缴癸拷锟侥憋拷锟斤拷
//	bool_t backup_server_login_success;		//锟斤拷锟矫凤拷锟斤拷锟斤拷锟斤拷陆锟缴癸拷锟斤拷锟斤拷

//	bool_t back_server_enable;	//锟斤拷锟矫凤拷锟斤拷锟斤拷锟斤拷锟斤拷使锟斤拷

    ncs_event_ops_t *ops;

    ncs_audio_encode_t talk_audio_encode;
    ncs_audio_encode_t bc_audio_encode;
} ncs_t;

extern ncs_t *ncs;
extern int error_code;

static inline uint8_t encrypt_board_id(uint8_t board_id)
{
#if 0
    uint8_t tmp = (((board_id - 1) << 2) & 0x0c) | (((board_id - 1) >> 2) & 0x03);
    return ((tmp ^ 0x05) << 4) | tmp;
#else
    uint8_t tmp = board_id - 1;
    return ((tmp ^ 0x05) << 4) | tmp;
#endif
}



#endif

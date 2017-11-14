#ifndef __NCS_H__
#define __NCS_H__

#include <stdint.h>
#include <common_lib/common_lib.h>
#include <ncs_event/ncs_event.h>
#include "call.h"
#include "broadcast.h"


struct ncs_manager
{
    uint16_t id;
    bool_t online;                           // 登陆标记
    char *name;

    char *server1_ip;                        // 主服务器
    int server1_port;
    char *server2_ip;                        // 备用服务器
    int server2_port;

    char *local_ip;                          // 本地网络设置
    int local_port;
    char *netmask;
    char *gw;
    char *dns1;
    char *dns2;
	char *mac_addr;
    int max_calls;

    ncs_event_ops_t ops;                     // 内通回调函数

    ncs_audio_encode_t talk_audio_encode;    // 对讲音频数据编码类型
    ncs_audio_encode_t bc_audio_encode;      // 广播音频数据编码类型
    ncs_task_priority_t priority;            // 对讲广播优先级

    struct sockaddr server_sockaddr;         // 服务软件网络地址

    struct sockaddr third_party_record_addr; // 第三方录音软件网络地址
    bool_t third_party_record;               // 第三方录音标记

    bool_t conference_enable;                // 会议模式使能标记

    video_fmt_t video_request_fmt;      // 视频请求格式
    int video_port;                     // 本地视频接收端口
    char *video_ip_route;               // 跨路由IP
    int video_port_route;               // 跨路由的映射端口
    int server_type_device_rcdno;       // 呼叫对方服务器类型记录号
    bool_t flag_qurry_name_call_in;
    ncs_ptt_trigger_t ptt_trigger_type; // ptt触发方式

    bool_t queueing_enable;   // 排队使能
    int queueing_number;      // 排队个数

    bool_t dtreboot_enable;   // 定时重启使能
    int dtreboot_days_number; // 定时隔天数
    int dtreboot_time;        // 定时整点数
	int curdtreboot_days;        // 当前重启的天数
	int curdtreboot_Hour;        // 当前重启的小时数

    int short_out_value;      // 短路输出常开常闭状态

    int auto_answer;          // 自动接听
    char term_qglw_name[18];  // 全国联网，临时查询终端名称
    char kfwqName[18];        // 夸服务器终端名称
    char dev_ecpt_type;       // 对讲设备是否加密, 1加密0不加密

    int trigger_time;

    struct broadcast *bc;

    int (*ncs_network_cfg)(struct ncs_manager *ncs_manager);                                                                                                 // 配置网络
    int (*ncs_start)(struct ncs_manager *ncs_manager);                                                                                                       // 启动内通
    int (*ncs_call_start)(struct ncs_manager *ncs_manager, call_t *call, bool_t passive);                                                                    // 对讲开始
    int (*ncs_call_stop)(struct ncs_manager *ncs_manager, call_t *call, bool_t passive);                                                                     // 停止对讲
    int (*ncs_ip_request)(struct ncs_manager *ncs_manager, uint16_t target_id, uint8_t board_id, ncs_call_t type);                                           // ip请求
    bool_t (*ncs_video_recv_start)(struct ncs_manager *ncs_manager);                                                                                         // 视频接收开始
    char *(*ncs_term_name_get_by_id)(struct ncs_manager *ncs_manager, uint16_t id);                                                                          // 获取终端名字
    void (*ncs_term_name_modify_by_id)(struct ncs_manager *ncs_manager, uint16_t id, char *pName);                                                           // 修改终端名字
    char *(*ncs_area_name_get_by_id)(struct ncs_manager *ncs_manager, uint16_t id);                                                                          // 获取分区名字
    char *(*ncs_area_name_get_by_index)(struct ncs_manager *ncs_manager, uint16_t id);                                                                          // 获取分区名字
    char *(*ncs_area_id_get_by_index)(struct ncs_manager *ncs_manager, uint16_t id);                                                                          // 获取分区名字
    char *(*ncs_boardext_name_get_by_id)(struct ncs_manager *ncs_manager, uint32_t id);                                                                      // 获取分区名字
    int (*ncs_term_name_list_size_get)(struct ncs_manager *ncs_manager);
    int (*ncs_area_name_list_size_get)(struct ncs_manager *ncs_manager);
    void (*ncs_door_open)(struct ncs_manager *ncs_manager, call_t *call);
    void (*ncs_term_status_ctl)(struct ncs_manager *ncs_manager, call_t *call, uint8_t ctl_type, uint8_t port, uint8_t port_status, uint16_t trigger_times);
    void (*ncs_ptt_status_send)(struct ncs_manager *ncs_manager, call_t *call, ncs_ptt_status_t s);
    void (*ncs_call_transfer)(struct ncs_manager *ncs_manager, uint16_t target_id);
    void (*ncs_video_switch)(struct ncs_manager *ncs_manager, call_t *call, video_class_t class, uint8_t board_id);
    void (*ncs_video_request)(struct ncs_manager *ncs_manager, call_t *call);
    void (*ncs_call_stop_all)(struct ncs_manager *ncs_manager, bool_t passive);
    bool_t (*ncs_call_stop_all_ex_talking)(struct ncs_manager *ncs_manager, bool_t passive);
    void (*ncs_monitor_stop)(struct ncs_manager *ncs_manager);
    void (*ncs_short_out)(struct ncs_manager *ncs_manager, call_t *call);
    void (*ncs_ctrl_haoyun_term_short)(struct ncs_manager *ncs_manager, int keyNum);
};

struct ncs_manager *ncs_manager_malloc(void);
void ncs_manager_free(struct ncs_manager *ncs_manager);
int ncs_login(uint16_t boards_status);


#endif

#ifndef __NCS_EVENT_H__
#define __NCS_EVENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>

#include <common_lib/common_lib.h>

#include "ncs_error_code.h"

#define NCS_PCM_LEN 440
#define NCS_ADPCM_LEN 116

#define MASTER_SERVER	1
#define BACKUP_SERVER	0
#define NOTHING_SERVER	-1
typedef struct
{
    void *evt_data; /* ��ͨЭ���� */
    int evt_data_len; /* Э�������� */
    struct sockaddr *target_sockaddr; /* Э������Ŀ�ĵػ�����Դ�� */
} ncs_event_t;

typedef enum
{
    PTT_INVALID = 0,
    PTT_VALID = 1,
} ncs_ptt_status_t;

typedef enum
{
    PRESS_ONCE = 0,
    PRESS_HOLD,
    PRESS_NONE,
} ncs_ptt_trigger_t;

/* ��Ƶ���ݱ�����ʽ */
typedef enum
{
    AUDIO_ENCODE_NONE = 0x00,
    AUDIO_ENCODE_ADPCM_SPON = 0x01,
    AUDIO_ENCODE_ADPCM_MS = 0x02,
    AUDIO_ENCODE_ALAW = 0x03,
    AUDIO_ENCODE_INVALID = 0xff,
} ncs_audio_encode_t;

/* �������� */
typedef enum
{
    IDLE_TASK = 0x0,
    IDLE_TASK_OLD = 0xff,
    MP3_BROADCAST_TASK = 0x1,
    WAVE_BROADCAST_TASK = 0x11,
} ncs_task_t;

typedef enum
{
    STOP_VIDEO = 0x00, /* ֹͣ��Ƶ */
    BOARD_VIDEO = 0x01, /* ����ʽ������Ƶ����һ��ʽ�ն���Ƶ */
    EXT1_VIDEO = 0x02, /* �ⲿ��Ƶ1 */
    EXT2_VIDEO = 0x03, /* �ⲿ��Ƶ2 */
    VIDEO_BUSY = 0xff, /* ��Ƶæ */
} video_class_t, video_state_t;

typedef enum
{
    VIDEO_CIF_FMT 	= 0x00,
    VIDEO_VGA_FMT 	= 0x01,
    VIDEO_D1_FMT 	= 0x02,
    VIDEO_720P_FMT 	= 0x03,
    VIDEO_1080P_FMT = 0x04,
} video_fmt_t;

typedef enum
{
    TALK = 0x0, /* �Խ� */
    MONITOR = 0x1, /* ���� */
    SDK_TALK,
    SDK_MONITOR,
} ncs_call_t;

typedef enum
{
    IN,
    OUT,
} call_dir_t;

// 对讲广播优先级
typedef enum
{
    PRIO_TALK,
    PRIO_BROADCAST,
} ncs_task_priority_t;

/* �û���ע���Ļص��ӿ� */
typedef struct
{
    /* �������� */
    void (*ncs_volume_get)(uint8_t volume, void *private_data);

    /* �ն����� */
    void (*ncs_login_exit_get)(void *private_data);

    /**
     * ncs_login_ok_get - �ɹ���½������
     *
     * @bc_samplerate: �㲥������
     * @bc_encode: �㲥���ݱ�������
     * @talk_samplerate: �Խ�������
     * @talk_encode: �Խ����ݱ�������
     * @have_task: ��½��ʱ���Ƿ���������ִ��
     * @task: �������ͣ�ֻ��have_task=TRUE��ʱ���˲�������Ч
     * @multi_ip: �鲥��ַ��ֻ��have_task=TRUE��ʱ���˲�������Ч
     */
    void (*ncs_login_ok_get)(
        int bc_samplerate,
        int talk_samplerate,
        bool_t have_task,
        ncs_task_t task,
        char *multi_ip,
        void *private_data);

    /**
     * ncs_call_stop_get - �Խ�ֹͣ
     *
     * @target_sockaddr: ��Դ������ַ
     * @target_id: ��Դ�ն�ID
     * @board_id: ��Դ����
     */
    void (*ncs_call_stop_get)(struct sockaddr *target_sockaddr,
                              uint16_t target_id,
                              uint8_t board_id,
                              void *private_data);

    /* �Խ����� */
    void (*ncs_call_request_get)(struct sockaddr *target_sockaddr,
                                 uint16_t target_id,
                                 uint8_t board_id,
                                 int samplerate,
                                 ncs_audio_encode_t encode,
                                 void *private_data);

    /* �������� */
    void (*ncs_monitor_request_get)(struct sockaddr *target_sockaddr,
                                    uint16_t target_id,
                                    uint8_t board_id,
                                    int samplerate,
                                    ncs_audio_encode_t encode,
                                    void *private_data);

    /* �Խ�Ӧ�� */
    void (*ncs_call_answer_get)(struct sockaddr *target_sockaddr,
                                uint16_t target_id,
                                uint8_t board_id,
                                int samplerate,
                                ncs_audio_encode_t encode,
                                void *private_data);

    /* �Խ���ʼ */
    void (*ncs_call_start_get)(struct sockaddr *target_sockaddr,
                               uint16_t target_id,
                               uint8_t board_id,
                               int samplerate,
                               ncs_audio_encode_t encode,
                               void *private_data);

    /* �ܾ� */
    void (*ncs_call_reject_get)(struct sockaddr *target_sockaddr,
                                uint16_t target_id,
                                uint8_t board_id,
                                void *private_data);

    /* æµ */
    void (*ncs_call_busy_get)(struct sockaddr *target_sockaddr,
                              uint16_t target_id,
                              uint8_t board_id,
                              void *private_data);

    /* ����IP�ɹ� */
    void (*ncs_ip_request_ok_get)(char *ip, uint16_t port, void *private_data);
    //跨服务器终端加密数据获取
    void (*ncs_type_ecpt_get)(char *pdata, void *private_data);
    char (*ncs_get_term_custom_type)(void);

    /* ����IPʧ�� */
    void (*ncs_ip_request_failed_get)(void *private_data);

    void (*ncs_task_switch_get)(char *multi_ip, ncs_task_t task, void *private_data);

    void (*ncs_task_idle_get)(void *private_data);

    void (*ncs_realtime_mp3_data_get)(uint8_t *mp3_data, int mp3_data_len, void *private_data);

    void (*ncs_realtime_wave_data_get)(uint8_t *wave_data, int wave_data_len, void *private_data);

    void (*ncs_talk_wave_data_get)(struct sockaddr *target_sockaddr,
                                   uint8_t *wave_data,
                                   int wave_data_len,
                                   void *private_data);

    void (*ncs_video_state_get)(struct sockaddr *target_sockaddr,
                                video_state_t state,
                                uint8_t board_id,
                                char *ip,
                                uint16_t port,
                                void *private_data);

    void (*ncs_video_request_get)(struct sockaddr *target_sockaddr,
                                  video_class_t class,
                                  video_fmt_t fmt,
                                  uint8_t board_id,
                                  char *ip,
                                  uint16_t port,
                                  void *private_data);

    void (*ncs_terminal_password_get)(char *password, void *private_data);

    void (*ncs_terminal_info_get)(uint16_t terminal_id,
                                  char *server_ip,
                                  uint16_t server_port,
                                  char *gw,
                                  char *mask,
                                  char *mac_addr,
                                  char *terminal_ip,
                                  uint16_t terminal_port,
                                  void *private_data);

    void (*ncs_third_party_record_start_get)(struct sockaddr *third_party_record_addr,
            void *private_data);

    void (*ncs_third_party_record_stop_get)(void *private_data);

    void (*ncs_term_name_bulk_get)(void *bulk, void *private_data);

    void (*ncs_term_name_qglw_get)(void *bulk, void *private_data);

    void (*ncs_term_boardext_name_qglw_get)(void *bulk, char lineNum, void *private_data);

    void (*ncs_broadcast_start_get)(bool_t ring, void *private_data);

    void (*ncs_broadcast_stop_get)(void *private_data);

    void (*ncs_system_time_get)(int year, int month, int day, int hour, int minute, int second, int weekday, void *private_data);

    void (*ncs_area_name_bulk_get)(void *bulk, void *private_data);

    void (*ncs_sdk_call_stop_get)(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data);

    void (*ncs_sdk_call_request_get)(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data);

    void (*ncs_set_device_server_type_rcd)(int server_type_rec);

    int (*ncs_get_device_server_type_rcd)(void);

    void (*ncs_sdk_monitor_request_get)(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data);

    void (*ncs_sdk_call_start_get)(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, void *private_data);

    void (*ncs_nat_transfer_get)(char *ip, uint16_t port, void *private_data);

    void (*ncs_port_out_ctl_get)(int port, int status);

    void (*ncs_warring_get)(char param1, char param2,  char *multi_ip);

    void (*ncs_auto_answer_ctl_get)(int status);

    void (*ncs_kfwqName_set)(void *pName);

    char* (*ncs_kfwqName_get)(void);

    char (*ncs_ecpt_type_get)(void);

    void (*ncs_ecpt_type_set)(char);
} ncs_event_ops_t;


int ncs_get_error_code(void);

/**
 * ncs_init - ��ͨЭ������ʼ��
 *
 * @main_server_ip: ��������IP
 * @main_server_port: ���������˿�
 * @bak_server_ip: ���÷�����IP
 * @bak_server_port: ���÷������˿�
 * @id: �ն�ID
 * @local_port: ����UDP�˿�
 * @ops: ncs��Ϣ�ص��ӿ�
 * @private_data: Ӧ�ò�˽������
 */
int ncs_init(char *main_server_ip, int main_server_port, char *bak_server_ip, int bak_server_port, uint16_t id, int local_port, ncs_event_ops_t *ops, void *private_data);

/* ������������������ʱ������ */
int ncs_handshake_period_set(int seconds);

/* �������ֳ�ʱʱ�� */
int ncs_handshake_timeout_set(int seconds);

/* ���õ�½������½ʱ������ */
int ncs_login_period_set(int seconds);

/**
 * ncs_login - ������½
 *
 * @boards_status: ����״̬�����ڴ��������ն��豸
 */
// int ncs_login(uint16_t boards_status, int back_server_enable);
int ncs_login(uint16_t boards_status);

/* ������ͨ��Ϣ */
int ncs_event_send(ncs_event_t *evt);

int ncs_multicast_join(char *multi_ip);

int ncs_multicast_drop(char *multi_ip);

void ncs_talk_audio_encode_set(ncs_audio_encode_t encode);

void ncs_bc_audio_encode_set(ncs_audio_encode_t encode);

void ncs_qglw_ecpt_type_get(void);

/*===================================================== ��ͨ��Ϣ�����ӿ� ==================================================*/

/* ����������½��������Ϣ��һ��Ӧ�ò㲻��Ҫ���ã���Э���㴦�� */
// ncs_event_t *ncs_login_request_event_build(int server_type);
ncs_event_t *ncs_login_request_event_build(void);

/* ��������������������Ϣ�� һ��Ӧ�ò㲻��Ҫ���ã���Э���㴦�� */
// ncs_event_t *ncs_handshake_event_build(int server_type);
ncs_event_t *ncs_handshake_event_build(void);

/**
 * ncs_ip_request_event_build - ����������������IP��������Ϣ
 *
 * @target_id: ������ӦĿ��ID��IP������Ϊ0�ͷ���ӳ���Ŀ���Ŀ��IP
 * @board_id: Ŀ�������ţ�һ�����ں����������ն�
 * @type: ��������--�Խ����߼���
 */
ncs_event_t *ncs_ip_request_event_build(uint16_t target_id, uint8_t board_id, ncs_call_t type, int server_type_rcdno);

/**
 * ncs_monitor_request_event_build - ������Ŀ�귢�ͼ�����������Ϣ
 *
 * @target_sockaddr: Ŀ��������ַ
 * @target_id: Ŀ��ID
 * @board_id: Ŀ������
 * @samplerate: ������
 * @audio_encode: ��Ƶ���뷽ʽ
 */
ncs_event_t *ncs_monitor_request_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, int samplerate, ncs_audio_encode_t audio_encode);

/**
 *
 * ncs_nat_event_build - ����������Ϣ���յ�������������Ϣ��ת����ָ��Ŀ�꣬һ��Ӧ�ò㲻��Ҫ���ã���Э���㴦��
 *
 * @target_sockaddr: Ŀ��������ַ
 * @ip: Ŀ��IP
 * @port: Ŀ���˿�
 */
ncs_event_t *ncs_nat_event_build(struct sockaddr *target_sockaddr, char *ip, uint16_t port);

/* ����֪ͨ�������Խ�ֹͣ����Ϣ */
ncs_event_t *ncs_call_stop_notify_server_event_build(uint16_t target_id, uint8_t board_id);

/* ����������ֹͣ�Խ�����Ϣ */
ncs_event_t *ncs_call_stop_by_caller_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id);

/* �����ɱ���ֹͣ�Խ�����Ϣ */
ncs_event_t *ncs_call_stop_by_callee_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id);

/* ����֪ͨ�������Խ���ʼ����Ϣ */
ncs_event_t *ncs_call_start_notify_server_event_build(uint16_t target_id, uint8_t board_id);

/**
 * ncs_call_start_event_build - �����Խ���ʼ��Ϣ
 *
 * @target_sockaddr: Ŀ��������ַ
 * @target_id: Ŀ��ID
 * @board_id: Ŀ������
 * @samplerate: ������
 * @audio_encode: ��Ƶ���뷽ʽ
 */
ncs_event_t *ncs_call_start_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, int samplerate, ncs_audio_encode_t audio_encode);

/**
 * ncs_call_request_event_build - �����Խ�������Ϣ
 *
 * @target_sockaddr: Ŀ��������ַ
 * @target_id: Ŀ��ID
 * @board_id: Ŀ������
 * @samplerate: ������
 * @audio_encode: ��Ƶ���뷽ʽ
 */
ncs_event_t *ncs_call_request_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, int samplerate, ncs_audio_encode_t audio_encode);

/* �����Խ��ܽ���Ϣ */
ncs_event_t *ncs_call_reject_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id);

/* ����æ��Ϣ */
ncs_event_t *ncs_call_busy_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id);

/**
 * ncs_call_answer_event_build - ������Ŀ�귢�ͶԽ�Ӧ������Ϣ
 *
 * @target_sockaddr: Ŀ��������ַ
 * @target_id: Ŀ��ID
 * @board_id: Ŀ������
 * @samplerate: ������
 * @audio_encode: ��Ƶ���뷽ʽ
 */
ncs_event_t *ncs_call_answer_event_build(struct sockaddr *target_sockaddr, uint16_t target_id, uint8_t board_id, int samplerate, ncs_audio_encode_t audio_encode);

/**
 * ncs_area_broadcast_request_event_build - ���������������ͷ����㲥��������Ϣ
 *
 * @area_list: �ն��б���ÿһ��int�ͱ�ʾһ������
 * @area_list_len: �б�����
 */
ncs_event_t *ncs_area_broadcast_request_event_build(int *area_list, int area_list_len);

/* ���������㲥��������Ϣ */
ncs_event_t *ncs_area_broadcast_stop_event_build(void);

/**
 * ncs_term_broadcast_request_event_build - �����������������ն˹㲥��������Ϣ
 *
 * @term_list: �ն��б���ÿһ��int�ͱ�ʾһ���ն�
 * @term_list_len: �б�����
 */
ncs_event_t *ncs_term_broadcast_request_event_build(int *term_list, int term_list_len);

/* �����ն˹㲥��������Ϣ */
ncs_event_t *ncs_term_broadcast_stop_event_build(void);

/* ����ȫ���㲥������Ϣ */
ncs_event_t *ncs_all_area_broadcast_request_event_build(void);

/* ����ȫ���㲥������Ϣ */
ncs_event_t *ncs_all_area_broadcast_stop_event_build(void);

/* ���췢�͹㲥������Ϣ */
ncs_event_t *ncs_realtime_wave_data_event_build(uint8_t *wave_data, int wave_data_len);

/* ���췢�ͶԽ�������Ϣ */
ncs_event_t *ncs_call_wave_data_event_build(struct sockaddr *target_sockaddr, uint8_t *wave_data, int wave_data_len);

/**
 * ncs_video_requeset_event_build - ������Ŀ���ն˷�����Ƶ��������Ϣ
 *
 * @target_sockaddr: Ŀ���ն�������ַ
 * @class: ��������Ƶ����
 * @board_id: Ŀ������
 * @ip: ��Ƶ����IP��ַ
 * @port: ��Ƶ���ն˿�
 *
 * ��Ƶ��ͨ��rtpЭ������h264��ʽ���͵�ip:port
 */
ncs_event_t *ncs_video_request_event_build(struct sockaddr *target_sockaddr, video_class_t class, video_fmt_t fmt, uint8_t board_id, char *ip, uint16_t port);

/* ������Ŀ���ն˷�����Ƶ״̬����Ϣ */
ncs_event_t *ncs_video_state_event_build(struct sockaddr *target_sockaddr, video_state_t state, uint8_t board_id, char *ip, uint16_t port);

/**
 * ncs_term_name_bulk_query_event_build - ������ѯ�ն���
 *
 * @start: ��ʼ��ѯid
 * @count: ��ѯ����
 */
ncs_event_t *ncs_term_name_bulk_query_event_build(uint16_t start, int count);

ncs_event_t *ncs_area_name_bulk_query_event_build(uint16_t start, int count);

ncs_event_t *ncs_boardext_name_bulk_query_event_build(uint32_t *pdata, int count);

ncs_event_t *ncs_door_open_event_build(struct sockaddr *target_sockaddr);

ncs_event_t *ncs_call_transfer_event_build(uint16_t target_id);

ncs_event_t *ncs_ptt_control_event_build(struct sockaddr *target_sockaddr, ncs_ptt_status_t s);

ncs_event_t *ncs_offline_task_switch_event_build(ncs_task_t t);

ncs_event_t *close_emergency_alarm_event_build(uint16_t id1, uint16_t id2);

ncs_event_t *ncs_short_in_event_build(int gpi, int state);

ncs_event_t *ncs_term_status_ctl_event_build(struct sockaddr *target_sockaddr, uint8_t ctl_type, uint8_t port, uint8_t port_status, uint16_t trigger_times);

void hostname_parse(char *hostname, int server_port);		//��������IP
ncs_event_t *PackProcWithBuf(char *pBufArray);

/*=========================================================================================================================*/

#endif

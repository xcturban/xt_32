
#include <stdlib.h>
#include "gpio.h"
#include "obj_vol_dev_src_select.h"
#include "logicprocessing.h"

typedef enum
{
    GPI_LINE_IN,
    GPI_SHORT_IN,
    //GPI_TF_IN,
    GPI_HEADSET_MIC_IN,
    GPI_ALARM,
    GPO_HEADSET_RECEIVER,
    GPO_LINE_OUT,

    GPO_SHORT_OUT,
    GPO_NETLED_OUT,
    GPO_LCD,
    GPIO_SIZE,
} GPIO_DEFINE;

#define LED_ON      0
#define LED_OFF     1

#define GPI_SIZE  3
#define GPIO_VDD_HIGH	1
#define GPIO_VDD_LOW	0


#define GPG_A   0
#define GPG_B   1
#define GPG_C   2
#define GPG_D   3
#define GPG_E   4
#define GPG_G   5

#define GPG_OFFSET 32
#define GPIO(group, pin)  ((group) * GPG_OFFSET + pin)

#define GPIO_SCAN_TIME_PERIOD	(100*1000)
#define GPIO_SCAN_TIME_WAIT		(10)
#define LCD_SCREEN_TRIGGER_WAIT	(3*60*10)				//30s

#define GPIO_INFO_DEFINE(x, y)     \
    {                           \
		.name = x,              \
		.pin_num = y,              \
	}

typedef struct
{
    const char *name;
    int pin_num;
} n32926_gpio_info;

static n32926_gpio_info n32926_gpio[GPIO_SIZE] =
{
    /* intput */
    GPIO_INFO_DEFINE("GPI_LINE_IN", GPIO(GPG_G, 14)),  //line input 1:open 0:close
    GPIO_INFO_DEFINE("GPI_SHORT_IN", GPIO(GPG_G, 9)),  //short in 0:open 1:close
    //GPIO_INFO_DEFINE("GPI_TF_IN", GPIO(GPG_G, 7)),  //TF in 1:open 0:close
    GPIO_INFO_DEFINE("GPI_HEADSET_MIC_IN", GPIO(GPG_D, 14)),  //headset mic in 1:open 0:close
    GPIO_INFO_DEFINE("GPI_ALARM", GPIO(GPG_G, 15)),   //alarm 1:open 0:close
    GPIO_INFO_DEFINE("GPO_HEADSET_RECEIVER", GPIO(GPG_D, 15)),	//headset reciever in 1:open 0:close
    GPIO_INFO_DEFINE("GPO_LINE_OUT", GPIO(GPG_D, 12)),	//line out 1:open 0:close
    /* output */
    GPIO_INFO_DEFINE("GPO_SHORT_OUT", GPIO(GPG_G, 8)),  //short out
    GPIO_INFO_DEFINE("GPO_NETLED_OUT", GPIO(GPG_G, 13)),  //net led

    GPIO_INFO_DEFINE("GPO_LCD", GPIO(GPG_E, 1)),  //lcd
};


/*void obj_gpio_plugin_array_init(void)*/
/*{*/
/*for(int i = 0 ; i < 3; i++)*/
/*{*/


/*}*/
/*}*/

static void gpio_set_direction(int io_index, char *direction)
{
    FILE *fp_dir = NULL;
    char path[256] = {0};

    sprintf(path, "/sys/class/gpio/gpio%d/direction", n32926_gpio[io_index].pin_num);
    fp_dir = fopen(path, "rb+");
    if(!fp_dir)
    {
        SPON_LOG_ERR("open %s err: %s\n", path, strerror(errno));
        return;
    }

    fprintf(fp_dir, direction);
    fclose(fp_dir);
}


static void gpio_set_value(int io_index, int val)
{
    FILE *fp_val = NULL;
    char path[256] = {0};
    char buffer[10];

    sprintf(path, "/sys/class/gpio/gpio%d/value", n32926_gpio[io_index].pin_num);

    fp_val = fopen(path, "rb+");
    if(!fp_val)
    {
        SPON_LOG_ERR("open %s err: %s\n", path, strerror(errno));
        return;
    }

    if(val)
    {
        strcpy(buffer, "1");
    }
    else
    {
        strcpy(buffer, "0");
    }
    fwrite(buffer, sizeof(char), sizeof(buffer) - 1, fp_val);
    fclose(fp_val);

    return;
}

static int gpio_get_value(int io_index)
{
    FILE *fp_val = NULL;
    char path[256] = {0};
    char buffer[10];
    int val = 0;

    sprintf(path, "/sys/class/gpio/gpio%d/value", n32926_gpio[io_index].pin_num);
    fp_val = fopen(path, "rb");
    if(fp_val == NULL)
    {
        SPON_LOG_FATAL("open %s: %s\n", path, strerror(errno));
        return 0;
    }

    fread(buffer, sizeof(char), sizeof(buffer) - 1, fp_val);
    val = atoi(buffer);
    fclose(fp_val);

    return val;
}

int gpio_input(int io_index)
{
    if(io_index < 0 || io_index >= GPIO_SIZE)
    {
        return 0;
    }
    return gpio_get_value(io_index);
}


void gpio_output(int io_index, int val)
{
    if(io_index < GPO_SHORT_OUT || io_index >= GPIO_SIZE)
    {
        return;
    }

    if(val)
    {
        gpio_set_value(io_index, GPIO_VDD_HIGH);
    }
    else
    {
        gpio_set_value(io_index, GPIO_VDD_LOW);
    }

    return;
}


void gpio_uninit(void)
{
    int i = 0;
    char cmd[256] = {0};

    for(i = 0; i < GPIO_SIZE; i++)
    {
        sprintf(cmd, "echo %d > /sys/class/gpio/unexport", n32926_gpio[i].pin_num);
        system(cmd);
    }
}

static pthread_mutex_t gpio_mutex;
void gpio_init(void)
{
    /*
    echo 99 > /sys/class/gpio/export
    echo out > /sys/class/gpio/gpio99/direction
    echo 1 > /sys/class/gpio/gpio99/value
    */
    int i = 0;
    char cmd[256] = {0};

    for(i = GPI_LINE_IN; i <= GPO_LINE_OUT; i++)
    {
        sprintf(cmd, "echo %d > /sys/class/gpio/export", n32926_gpio[i].pin_num);
        system(cmd);
        gpio_set_direction(i, "in");
    }

    for(i = GPO_SHORT_OUT; i < GPIO_SIZE; i++)
    {
        sprintf(cmd, "echo %d > /sys/class/gpio/export", n32926_gpio[i].pin_num);
        system(cmd);
        gpio_set_direction(i, "out");
    }

    gpio_output(GPO_SHORT_OUT, GPIO_VDD_LOW);

    if(net_link())
    {
        gpio_output(GPO_NETLED_OUT, GPIO_VDD_HIGH);
    }
    else
    {
        gpio_output(GPO_NETLED_OUT, GPIO_VDD_LOW);
    }

    lcd_screen_lock_status_init();
    pthread_mutex_init(&gpio_mutex, NULL);
    return;
}

static int lcd_screen_lock_status_flag = TRUE;
int lcd_screen_lock_status_get(void)
{
    return lcd_screen_lock_status_flag;
}

int lcd_screen_lock_status_set(int status)
{
    if(lcd_screen_lock_status_flag != status)
    {
        lcd_screen_lock_status_flag = status;
        gpio_output(GPO_LCD, lcd_screen_lock_status_flag ? GPIO_VDD_LOW : GPIO_VDD_HIGH);
    }
    return lcd_screen_lock_status_flag;
}

unsigned int lcd_screen_lock_wait_count = 0;
void lcd_screen_lock_wait_count_set(int c)
{
    lcd_screen_lock_wait_count = c;
}

void lcd_screen_lock_status_init(void)
{
    //SPON_LOG_INFO("lcd_screen_lock_status_init\n");
    lcd_screen_lock_status_set(1);
    lcd_screen_lock_wait_count_set(0);
}

void lcd_screen_trigger(void)
{
    //屏幕正在点亮时
    if(lcd_screen_lock_status_get())
    {
        lcd_screen_lock_wait_count_set(lcd_screen_lock_wait_count + 1);
        if(LCD_SCREEN_TRIGGER_WAIT < lcd_screen_lock_wait_count)
        {
            //SPON_LOG_INFO("lcd_screen_trigger\n");
            lcd_screen_lock_status_set(0);
            lcd_screen_lock_wait_count_set(0);
        }
    }
}


static int headset_receiver_detect = 0;
int headset_receiver_get_status(void)
{
    return headset_receiver_detect;
}

void headset_receiver_set_status(int status)
{
    struct audio_manager *audio_manager = term->audio_manager;

    SPON_LOG_INFO("headset_receiver_set_status : %d \n", status);
    headset_receiver_detect = status;
    if(status)
    {
        //插上耳机的听筒，关掉扬声器
        M_SET_AUDIO_OUT_DEV(ENUM_DEV_OUT_HEARPHONE, TRUE);
        audio_manager->set_rt3261regEx(audio_manager, 01, 15, 1);
    }
    else
    {
        //拔掉耳机的听筒，打开扬声器
        M_SET_AUDIO_OUT_DEV(ENUM_DEV_OUT_HEARPHONE, FALSE);
        audio_manager->set_rt3261regEx(audio_manager, 01, 15, 0);
    }
    audio_channel_mode_set(AUDIO_CHANNEL_DEFAULT);
}

static int headset_mic_detect = 0;
int headset_mic_status_get(void)
{
    return headset_mic_detect;
}

void headset_mic_set_status(int status)
{
    struct audio_manager *audio_manager = term->audio_manager;
    struct broadcast *bc = term->ncs_manager->bc;
    struct call_manager *call_manager = term->call_manager;

    SPON_LOG_INFO("headset_mic_set_status : %d \n", status);
    headset_mic_detect = status;
    if(headset_mic_detect == 1)
    {
        M_SET_AUDIO_IN_DEV(ENUM_DEV_IN_MIC, TRUE);
        obj_vol_dev_src_set_head_in(TRUE);
    }
    else
    {
        M_SET_AUDIO_IN_DEV(ENUM_DEV_IN_MIC, FALSE);
        obj_vol_dev_src_set_head_in(FALSE);
    }
    Turn_obj_vol_dev_src();
}


static int linein_detect = 0;
int linein_get_status(void)
{
    return linein_detect;
}

void linein_set_status(int status)
{

    SPON_LOG_INFO("linein_set_status : %d \n", status);
    linein_detect = status;
    if(linein_detect == 1)
    {
        M_SET_AUDIO_IN_DEV(ENUM_DEV_IN_LINEIN, TRUE);
        obj_vol_dev_src_set_line_in(TRUE);
    }
    else
    {
        M_SET_AUDIO_IN_DEV(ENUM_DEV_IN_LINEIN, FALSE);
        obj_vol_dev_src_set_line_in(FALSE);
    }
    Turn_obj_vol_dev_src();
}

void LineoutSetRegValue(bool_t flag)
{
	struct audio_manager *audio_manager = term->audio_manager;
	
	if(flag)
	{
		//线路输出
		M_SET_AUDIO_OUT_DEV(ENUM_DEV_OUT_LINEOUT, TRUE);
		audio_manager->set_rt3261regEx(audio_manager, 01, 15, 1);
		audio_manager->set_rt3261regEx(audio_manager, 02, 15, 0);
	}
	else
	{
		M_SET_AUDIO_OUT_DEV(ENUM_DEV_OUT_LINEOUT, FALSE);
		audio_manager->set_rt3261regEx(audio_manager, 01, 15, 0);
		audio_manager->set_rt3261regEx(audio_manager, 02, 15, 1);
	}
}

static int line_out_detect = 0;
int lineout_get_status(void)
{
	return line_out_detect;
}

void lineout_set_status(int status)
{
    SPON_LOG_INFO("lineout_set_status : %d \n", status);

	line_out_detect = status;

	if(term->customer_type == CUSTOMER_XMBR)
	{
		if(IsCurTaskBroadcast())
		{
			LineoutSetRegValue(status);
		}
		else
		{
			LineoutSetRegValue(FALSE);
		}
	}
	else
	{
		LineoutSetRegValue(status);
	}
}


void short_out_close(void)
{
    gpio_output(GPO_SHORT_OUT, GPIO_VDD_LOW);
}


void short_out_open(void)
{
    gpio_output(GPO_SHORT_OUT, GPIO_VDD_HIGH);
}
#if 1
void netlink_led_open(void)
{
    gpio_output(GPO_NETLED_OUT, GPIO_VDD_HIGH);
}

void netlink_led_close(void)
{
    gpio_output(GPO_NETLED_OUT, GPIO_VDD_LOW);
}
#endif
int gpio_sdcard_init(void)
{
#if 0
    int device_fd = open(SD_CARD, O_RDWR | O_NOCTTY | O_SYNC);
    if (device_fd < 0)
    {
        SPON_LOG_INFO("open %s", SD_CARD);
        return -1;
    }
    else
    {
        SPON_LOG_INFO("sdcard detect %d\n", device_fd);
    }
    close(device_fd);
    return device_fd;
#endif
}


#define MEDIA_CALL_KEY_UP_WAIT_SEC		3
static void *gpio_monitor_thread(void *arg)
{
    terminal_t *terminal = (terminal_t *)arg;
    struct audio_manager *audio_manager = term->audio_manager;
    task_t oldtask = get_system_task();
    task_t newtask = get_system_task();
    static int flag = 0;

    int gpio_i1_value_prev, gpio_i2_value_prev, gpio_i3_value_prev,
        gpio_i4_value_prev, gpio_i5_value_prev, gpio_i6_value_prev, gpio_i7_value_prev;
    int gpio_i1_value_cur, gpio_i2_value_cur, gpio_i3_value_cur,
        gpio_i4_value_cur, gpio_i5_value_cur, gpio_i6_value_cur, gpio_i7_value_cur;


    audio_manager->set_rt3261regEx(audio_manager, dec_2_bcd(61), 0, 0);
    gpio_output(GPO_SHORT_OUT, 0);
    gpio_output(GPI_SHORT_IN, 1);

    gpio_i1_value_prev = gpio_input(GPI_LINE_IN);
    gpio_i2_value_prev = gpio_input(GPI_SHORT_IN);
    //gpio_i3_value_prev = gpio_input(GPI_TF_IN);
    gpio_i4_value_prev = gpio_input(GPI_HEADSET_MIC_IN);
    gpio_i5_value_prev = gpio_input(GPO_HEADSET_RECEIVER);
    gpio_i6_value_prev = gpio_input(GPI_ALARM);
    gpio_i7_value_prev = gpio_input(GPO_LINE_OUT);

    headset_receiver_set_status(gpio_i5_value_prev);
    headset_mic_set_status(gpio_i4_value_prev);
    linein_set_status(gpio_i1_value_prev);
    lineout_set_status(gpio_i7_value_prev);

    while(1)
    {
        /*//避免白噪声*/
        newtask = get_system_task();
        /*if(oldtask != newtask)*/
        /*{*/
        /*oldtask = newtask;*/
        /*if(newtask == TASK_NONE)*/
        /*{*/
        /*//关闭扬声器电源*/
        /*flag = 0;*/
        /*audio_manager->set_rt3261regEx(audio_manager, dec_2_bcd(61), 0, 0);*/
        /*}*/
        /*else*/
        /*{*/
        /*if(!flag)*/
        /*{*/
        /*flag = 1;*/
        /*//打开扬声器电源*/
        /*audio_manager->set_rt3261regEx(audio_manager, dec_2_bcd(61), 0, 1);*/
        /*}*/
        /*}*/
        /*}*/

        gpio_i1_value_cur = gpio_input(GPI_LINE_IN);
        gpio_i2_value_cur = gpio_input(GPI_SHORT_IN);
        //gpio_i3_value_cur = gpio_input(GPI_TF_IN);
        gpio_i4_value_cur = gpio_input(GPI_HEADSET_MIC_IN);
        gpio_i5_value_cur = gpio_input(GPO_HEADSET_RECEIVER);
        gpio_i6_value_cur = gpio_input(GPI_ALARM);
        gpio_i7_value_cur = gpio_input(GPO_LINE_OUT);

        if (gpio_i1_value_prev != gpio_i1_value_cur)
        {
            if(gpio_i1_value_cur == GPIO_VDD_HIGH)
            {
                linein_set_status(1);
            }
            else
            {
                linein_set_status(0);
            }
        }

        if (gpio_i2_value_prev != gpio_i2_value_cur)
        {
            ncs_short_in_request(1, !gpio_i2_value_cur);
        }

        if (gpio_i4_value_prev != gpio_i4_value_cur)
        {
            if(gpio_i4_value_cur == GPIO_VDD_HIGH)
            {
                headset_mic_set_status(1);
            }
            else
            {
                headset_mic_set_status(0);
            }
        }

        if (gpio_i5_value_prev != gpio_i5_value_cur)
        {
            if(gpio_i5_value_cur == GPIO_VDD_HIGH)
            {
                headset_receiver_set_status(1);
            }
            else
            {
                headset_receiver_set_status(0);
            }
        }

        if(gpio_i6_value_cur != gpio_i6_value_prev)
        {
            //ALARM KEY
            if(!gpio_i6_value_cur)
            {
                lcd_screen_lock_status_init();
                emergency_bc_timer_start();
            }
            else
            {
                emergency_bc_timer_stop();
            }
        }

        if (gpio_i7_value_prev != gpio_i7_value_cur)
        {
            if(gpio_i7_value_cur == GPIO_VDD_HIGH)
            {
                lineout_set_status(1);
            }
            else
            {
                lineout_set_status(0);
            }
        }

        gpio_i1_value_prev = gpio_i1_value_cur;
        gpio_i2_value_prev = gpio_i2_value_cur;
        gpio_i3_value_prev = gpio_i3_value_cur;
        gpio_i4_value_prev = gpio_i4_value_cur;
        gpio_i5_value_prev = gpio_i5_value_cur;
        gpio_i6_value_prev = gpio_i6_value_cur;
        gpio_i7_value_prev = gpio_i7_value_cur;

        if(terminal->image_load == 1)
        {
            if(newtask == TASK_NONE)
            {
                lcd_screen_trigger();
            }
            else
            {
                lcd_screen_lock_status_init();
            }
        }
        logicDamodule();
        usleep(GPIO_SCAN_TIME_PERIOD);
    }

    pthread_exit(NULL);
    return NULL;
}


static int gpio_monitor_thread_create(void *arg)
{
    gpio_init();
    pthread_t tid_short_io;

	if (pthread_create(&tid_short_io, NULL, gpio_monitor_thread, arg) < 0)
	{
		SPON_LOG_ERR("create short io thread failed:%s\n", strerror(errno));
		return -1;
	}

    return 0;
}

void gpio_monitor_init(void *arg)
{
    gpio_monitor_thread_create(arg);
}

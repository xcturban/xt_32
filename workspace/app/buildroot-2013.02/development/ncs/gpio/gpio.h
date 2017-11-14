
#include "common.h"
#include "term.h"



void gpio_monitor_init(void *arg);
int headset_receiver_get_status(void);
int headset_mic_status_get(void);
int linein_get_status(void);
int lineout_get_status(void);

void short_out_open(void);
void short_out_close(void);
int gpio_sdcard_init(void);
void lcd_screen_lock_status_init(void);

void netlink_led_open(void);
void netlink_led_close(void);



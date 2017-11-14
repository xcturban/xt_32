#include <stdlib.h>
#include "led_control.h"

void mic_led_open(void)
{
	//system("echo 1 > /sys/class/gpio/gpio172/value");
}

void mic_led_close(void)
{
	//system("echo 0 > /sys/class/gpio/gpio172/value");
}

void mic_led_gpio_init(void)
{
	//system("echo 172 > /sys/class/gpio/export");
	//system("echo out > /sys/class/gpio/gpio172/direction");

	mic_led_close();
}


#if 0
#include <stdlib.h>
#include "short_io.h"
#include "term.h"


// ��·��������ֻ�ܶ�ѡһ��Ŀǰ����Ϊ��·����
void short_io_init(void)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
	// GPG8 ��·����
	system("echo 168 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio168/direction");

	// GPA3 ��·����
	// system("echo 3 > /sys/class/gpio/export");
	// system("echo in > /sys/class/gpio/gpio3/direction");

	if (ncs_manager->short_out_value == 1)
		system("echo 1 > /sys/class/gpio/gpio168/value");
	else
		system("echo 0 > /sys/class/gpio/gpio168/value");
}

// ��·��������
void short_out_trigger(void)
{
	struct ncs_manager *ncs_manager = term->ncs_manager;
	if (ncs_manager->short_out_value == 1) {
		system("echo 0 > /sys/class/gpio/gpio168/value");
		usleep(100 * 1000); // ����100ms
		system("echo 1 > /sys/class/gpio/gpio168/value");
	} else {
		system("echo 1 > /sys/class/gpio/gpio168/value");
		usleep(100 * 1000);
		system("echo 0 > /sys/class/gpio/gpio168/value");
	}
}

// �˿ڱպ�
void short_out_close(void)
{
	system("echo 1 > /sys/class/gpio/gpio168/value");
}

// �˿ڶϿ�
void short_out_open(void)
{
	system("echo 0 > /sys/class/gpio/gpio168/value");
}


#endif

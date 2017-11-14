#include "checksum.h"

uint16_t inline uint16_checksum_calculate(uint16_t *data, int len)
{
	int i;
	uint16_t checksum = 0;

	for (i = 0; i < len; i++)
		checksum += *data++;

	return checksum;
}

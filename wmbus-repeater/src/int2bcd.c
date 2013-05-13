#include <stdint.h>
#include "int2bcd.h"

void int2bcd(uint8_t bcdSize, uint64_t *intSource, uint8_t *bcdDest)
{
	uint64_t tmp = *intSource;
	while(bcdSize != 0)
	{
		*bcdDest = (tmp % 10);
		tmp -= (tmp % 10);
		tmp = tmp / 10;
		bcdSize--;
		if (bcdSize == 0)
			break;
		*bcdDest += ((tmp % 10) << 4);
		tmp -= (tmp % 10);
		tmp = tmp / 10;
		bcdDest++;
		bcdSize--;
	}
}


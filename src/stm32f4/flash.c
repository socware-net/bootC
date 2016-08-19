#include "flash.h"
#include "io.h"
#include "_soc.h"
#include "term.h"

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#include <stdio.h>
#include <string.h>

struct chip_info {

};

extern struct chip_info *spi_chip_info;

unsigned flash_page, flash_sz, flash_align, flash_off;

void _flash_init(void)
{

}

int _flash_erase(unsigned off, unsigned sz)
{

	return 0;
}

int _flash_write(unsigned off, unsigned sz, void *data)
{
	return 0;
}

int _flash_read(unsigned off, unsigned sz, void *data)
{
	//unsigned addr = BASE_FLASH + off;
	//memcpy(data, (void *)addr, sz);
	return 0;
}

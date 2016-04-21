#include <string.h>

#include "flash.h"
#include "io.h"
#include "_soc.h"
#include "term.h"

void flash_config(void);
void flash_init(int security_enable);

int flash_read_jedec_id(unsigned char *buf, int buf_size);
int flash_read_manu_id(unsigned char *buf, int buf_size);
int flash_read_manu_id_quad(unsigned char *buf, int buf_size);

int flash_read_sr(unsigned char *val);
int flash_read_sr2(unsigned char *val);

int flash_read(unsigned char *buf, unsigned int from, int len);
int flash_fast_read(unsigned char *buf, unsigned int from, int len,
		    int dummy_cycle);

int flash_erase_sector(unsigned int offs);
int flash_erase_sector_32k(unsigned int offs);
int flash_erase_page(unsigned int offs);
int flash_erase(unsigned int offs, int len);

int flash_write(const unsigned char *buf, unsigned int to, int len);

int flash_erase_write(const unsigned char *buf, unsigned int offs, int count);
int flash_erase_chip(void);

int flash_protect(void);
int flash_unprotect(void);
int flash_write_enable(void);
int flash_write_disable(void);

int flash_wait_ready(int sleep_ms);
void flash_switch_mode(unsigned long mode);

struct chip_info {
	char *name;
	unsigned char id;
	unsigned long jedec_id;
	unsigned long page_size;
	unsigned int n_pages;

	unsigned char quad_program_cmd;
	unsigned char quad_read_cmd;
	unsigned char qpi_read_cmd;

};

extern struct chip_info *spi_chip_info;

unsigned flash_page, flash_sz, flash_align, flash_off;

void _flash_init(void)
{
	flash_config();
	flash_switch_mode(1);
	flash_page = 0x1000;	//spi_chip_info->page_size;
	flash_sz = flash_page * spi_chip_info->n_pages;
	flash_align = __builtin_ffs(flash_page) - 1;
	flash_off = 0x10000000;
}

int _flash_erase(unsigned off, unsigned sz)
{
	int i, r;
	if (sz & BI_TMSK(flash_align)) {
		return -1;
	}
	flash_write_enable();
	flash_unprotect();
	// erase in 4K unit
	for (i = 0; i < sz; i += (4 << 10)) {
		if ((r = flash_erase_page(off + i)))
			return r;
	}
	return 0;
}

int _flash_write(unsigned off, unsigned sz, void *data)
{
#define MAX_LEN  (128)
	unsigned left;
	unsigned l = 0;
	int r;
	left = sz;
	while (MAX_LEN <= left) {
		l = MAX_LEN;
		if ((r = flash_write(data, off, l)))
			return r;
		data += l;
		off += l;
		left -= l;
	}
	if ((r = flash_write(data, off, left)))
		return r;
	return 0;
}

int _flash_read(unsigned off, unsigned sz, void *data)
{
	unsigned addr = BASE_FLASH + off;
	memcpy(data, (void *)addr, sz);
	return 0;
}

#ifndef FLASH0322
#define FLASH0322

extern unsigned flash_page, flash_sz, flash_align, flash_off;

void _flash_init(void);

int _flash_erase(unsigned off, unsigned sz);

int _flash_write(unsigned off, unsigned sz, void *data);

int _flash_read(unsigned off, unsigned sz, void *data);

#endif

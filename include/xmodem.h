#ifndef __XMODEM_H__
#define __XMODEM_H__

typedef void (*xmodem_rx_t) (void *ptr, unsigned char *buffer, int len);

int xmodem_block_rx(void *ptr,
		    unsigned char *dest, int destsz,
		    xmodem_rx_t function_pointer);

#endif

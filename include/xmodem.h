#ifndef XMODEM0325
#define XMODEM0325

typedef void (*xmodem_rx_t) (void *ptr, unsigned char *buffer, int len);

int xmodem_block_rx(xmodem_rx_t function_pointer, void *priv);

int xmodem_tx(unsigned char *src, int srcsz);

#endif

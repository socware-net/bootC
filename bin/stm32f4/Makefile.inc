INCLUDE+=-I$(PREFIX)/include/stm32f4xx

CFLAGS += -DUSE_STDPERIPH_DRIVER -DSTM32F40_41xxx
LDFLAGS+= -L$(PREFIX)/lib

brd-dbg:
	openocd -f /usr/share/openocd/scripts/board/stm32f4discovery.cfg

brd-console:
	echo "pu port             /dev/ttyUSB0" >~/.minirc.usb0
	echo "pu lock             /var/lock" >>~/.minirc.usb0
	echo "pu baudrate         $(BAUD)" >>~/.minirc.usb0
	echo "pu bits             8" >>~/.minirc.usb0
	echo "pu parity           N" >>~/.minirc.usb0
	echo "pu stopbits         1" >>~/.minirc.usb0
	minicom usb0

.PHONY:bootc.flash
bootc.flash:
	$(MAKE) clean
	$(MAKE) CONFIG="-DFA=0x8000000 -DVA=0x20000000 -DWAIT=6 -DBAUD=$(BAUD) -DHISTORY=3" \
		LSCRIPT=flash.ld
	mv  main.elf.bin $@

st-flash:
	st-flash write bootc.flash 0x8000000

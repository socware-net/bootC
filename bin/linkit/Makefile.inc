
brd-dbg:
	openocd -f bin/$(PLT)/cmsis.cfg -s bin

brd-console:
	echo "pu port             /dev/ttyACM0" >~/.minirc.cdc
	echo "pu lock             /var/lock" >>~/.minirc.cdc
	echo "pu baudrate         $(BAUD)" >>~/.minirc.cdc
	echo "pu bits             8" >>~/.minirc.cdc
	echo "pu parity           N" >>~/.minirc.cdc
	echo "pu stopbits         1" >>~/.minirc.cdc
	minicom cdc

.PHONY:bootc.brom
bootc.brom:
	$(MAKE) clean
	$(MAKE) CONFIG="-DFA=0x20000000 -DVA=0x20000000 -DWAIT=6 -DBAUD=115200 -DHISTORY=3"
	mv  main.elf.bin $@

.PHONY:bootc.flash
bootc.flash:
	$(MAKE) clean
	$(MAKE) CONFIG="-DFA=0x10000000 -DVA=0x20000000 -DWAIT=6 -DBAUD=$(BAUD) -DHISTORY=3"
	mv  main.elf.bin $@

PREFIX ?= $(shell pwd)/../prefix/$(CROSS:%-=%)
NAME   :=bootC
TARGET :=arm-none-eabi
CROSS  :=$(TARGET)-
CPU    :=arm
PLT?=linkit
INCLUDE:=-Iinclude -Isrc/$(PLT)

COPTS  ?=-march=armv7-m -mthumb -fno-strict-aliasing
AARCH  :=7
MOPTS  :=$(COPTS) \
	-DCFG_AARCH=$(AARCH) \
	-fsingle-precision-constant -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
	-fno-builtin -fno-common \
	-ffunction-sections -fdata-sections

CONFIG :=-DMTK_BOOTLOADER_MENU_ENABLE -DLA=0x10000000 -DVA=0x20000000 -DWAIT=6 -DNDEBUG
ASFLAGS:=$(MOPTS) $(CONFIG) -O1 -g -Wall -Werror -D __ASSEMBLY__
CFLAGS :=$(MOPTS) $(CONFIG) -Os -g -Wall -Werror -fno-builtin
LSCRIPT?=ram.ld
LDFLAGS:=$(MOPTS) -g -nostartfiles -nodefaultlibs -L lib -T$(LSCRIPT)
	
MSCRIPT:=$(PREFIX)/share/mod.ld
LIB    :=lib$(NAME).a

ALL    :=lib install startup.o main.elf main.elf.bin
CLEAN  :=
CPU    :=arm

VPATH  :=src src/$(PLT) 
VOBJ   :=$(patsubst %.S,%.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.cpp, %.o, \
			$(notdir $(foreach DIR,$(VPATH),\
				$(wildcard $(DIR)/*.S)	\
				$(wildcard $(DIR)/*.c) 	\
				$(wildcard $(DIR)/*.cpp))))))
default:all

include Makefile.rule

F?=main.elf

openocd.gdb:
	echo target remote 127.0.0.1:3333 > $@
	echo monitor reset init> $@

ddd:
	ddd --debugger $(CROSS)gdb -x openocd.gdb $(F)
	
gdb:
	$(CROSS)gdb -x openocd.gdb $(F)

brd-dbg:
	openocd -f bin/$(PLT)/cmsis.cfg -s bin

brd-console:
	@echo "ctrl+o and set 115200"
	@sleep 2
	echo "pu port             /dev/ttyACM0" >>~/.minirc.cdc
	echo "pu lock             /var/lock" >>~/.minirc.cdc
	minicom cdc
	
fmt:
	bin/m-fmt.sh .
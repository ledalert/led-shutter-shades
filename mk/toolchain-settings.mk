#Default toolchain to arm-none-eabi-
TOOLCHAIN ?= arm-none-eabi-

ifneq ($(origin TOOLCHAIN), undefined)
	
	#Update variables if TOOLCHAIN is set
	CC = $(TOOLCHAIN)gcc
	OBJCOPY = $(TOOLCHAIN)objcopy
	OBJDUMP = $(TOOLCHAIN)objdump
	SIZE = $(TOOLCHAIN)size
	AR = $(TOOLCHAIN)ar

endif
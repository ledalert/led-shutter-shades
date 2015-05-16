#We are using a local libopencm3
LIBOPENCM3_LOCATION = lib/libopencm3/

ifeq ($(origin LIBOPENCM3_LOCATION), undefined)
$(error LIBOPENCM3_LOCATION is not specified!)
endif


LIBOPENCM3_INCLUDES = $(LIBOPENCM3_LOCATION)include
LIBOPENCM3_LIB = $(LIBOPENCM3_LOCATION)lib/libopencm3_stm32f1.a
LIB_INCLUDES = $(LIBOPENCM3_LOCATION)lib/stm32/f1/ $(LIBOPENCM3_LOCATION)lib
LIB_NAMES += opencm3_stm32f1

$(LIBOPENCM3_LIB):
	$(MAKE) -C $(LIBOPENCM3_LOCATION) lib/stm32/f1
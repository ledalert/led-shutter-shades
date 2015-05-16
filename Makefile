NAME = ws2812_blink
BUILD_DIR = build/

SOURCES = ws2812_blink.c

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)%.o)

LIBS = $(LIBOPENCM3_LIB) $(LIB_LEDALERT_STM32F100_LIB)

vpath %.c src

INCLUDE_DIRS = inc $(LIBOPENCM3_INCLUDES) $(LIB_LEDALERT_STM32F100_INCLUDES)
  
LINK_SCRIPT = stm32f100x6.ld

default: target

#$TARGET_BIN is set in common.mk but first rule is default so we set default to a phony rule that we have at the end instead

include mk/common.mk
include mk/libopencm3-settings.mk
include mk/libledalertstm32f100-settings.mk


target: $(TARGET_BIN)
	$(SIZE) $(TARGET_ELF)


.PHONY: default target
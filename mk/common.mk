#Uncomment following line for dry run
#TOOLCHAIN = @echo #This comment makes sure there is a trailing space

include mk/toolchain-settings.mk

TARGET ?= $(BUILD_DIR)$(NAME)
TARGET_ELF ?= $(TARGET).elf
TARGET_BIN ?= $(TARGET).bin

CPU_FLAGS = -mthumb -mcpu=cortex-m3 -msoft-float -DSTM32F1
CFLAGS = -std=gnu99 -Os -Wall -fno-common -ffunction-sections -fdata-sections $(CPU_FLAGS) $(INCLUDE_DIRS:%=-I%)

ifeq ($(origin LINK_SCRIPT), undefined)
$(error LINK_SCRIPT is not specified!)
endif

LINK_FLAGS = 	$(CPU_FLAGS) \
				$(LIB_INCLUDES:%=-L%) \
				--static -nostartfiles \
				-T$(LINK_SCRIPT) \
				-Wl,--start-group -lc -lgcc -lnosys $(LIB_NAMES:%=-l%) -Wl,--end-group \
				-Wl,--gc-sections


ifeq ($(origin LINK_SCRIPT), undefined)
$(error LINK_SCRIPT is not specified!)
endif



$(BUILD_DIR).placeholder:
	mkdir -p $(BUILD_DIR)
	touch $(BUILD_DIR).placeholder

$(OBJECTS): $(BUILD_DIR)%.o: %.c $(LIBS)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<
	@cp $(BUILD_DIR)$*.d $(BUILD_DIR)$*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $(BUILD_DIR)$*.d >> $(BUILD_DIR)$*.P; \
	rm -f $(BUILD_DIR)$*.d

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET_BIN)

$(TARGET_ELF): $(BUILD_DIR.placeholder) $(OBJECTS) $(LIBS)
	$(CC) $(OBJECTS) $(LINK_FLAGS) -o $@


clean:
	rm -f $(OBJECTS) $(SOURCES:%.c=$(BUILD_DIR)%.P)

.PHONY: clean

-include $(SOURCES:%.c=$(BUILD_DIR)%.P)
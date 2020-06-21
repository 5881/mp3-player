PROJECT = mp3
BUILD_DIR = bin

SHARED_DIR = ./inc ./fatfs
CFILES = mp3.c
CFILES += st7735_128x160.c st_printf.c sdcard.c
CFILES += ff.c diskio.c ffunicode.c
CFILES += vs1011e.c  8key.c

OPT_FLAGS ?= -O2
CFLAGS += -Wall $(OPT_FLAGS) -std=gnu99 -MD -I.
LDFLAGS += $(OPT_FLAGS)

#AFILES += api-asm.S

# TODO - you will need to edit these two lines!
DEVICE=stm32f103c8t6
#OOCD_FILE = interface/stlink-v2.cfg
OOCD_INTERFACE = stlink-v2
OOCD_TARGET = stm32f1x

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=./libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ./rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

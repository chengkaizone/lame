LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := lameConvert
LOCAL_SRC_FILES := lameConvert.c \
lamelib/bitstream.c \
lamelib/fft.c \
lamelib/id3tag.c \
lamelib/mpglib_interface.c \
lamelib/presets.c \
lamelib/quantize.c \
lamelib/reservoir.c \
lamelib/tables.c \
lamelib/util.c \
lamelib/VbrTag.c \
lamelib/encoder.c \
lamelib/gain_analysis.c \
lamelib/lame.c \
lamelib/newmdct.c \
lamelib/psymodel.c \
lamelib/quantize_pvt.c \
lamelib/set_get.c \
lamelib/takehiro.c \
lamelib/vbrquantize.c \
lamelib/version.c \

LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)

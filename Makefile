TOP_DIR     :=$(shell pwd)/$(lastword $(MAKEFILE_LIST))
TOP_DIR     :=$(shell dirname $(TOP_DIR))

ARM_CXX      = $(CROSS_COMPILE)/home/liaoxw/rk1808/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
CC          = $(CROSS_COMPILE)gcc
CXX         = $(CROSS_COMPILE)g++
STRIP       = $(CROSS_COMPILE)strip
MAKE        ?= make
RM          ?= rm -rf
INSTALL     ?= install -c
PREFIX      ?= /usr/local

DEFFLAGS    = -g -pthread -ffunction-sections -fdata-sections -fPIC
CFLAGS     += -D DEBUG_LOG
CFLAGS      += $(DEFFLAGS) -lstdc++
CXXFLAGS     += -D DEBUG_LOG
CXXFLAGS    += $(DEFFLAGS) -std=c++11

LDFLAGS     += -lcurl -pthread -lparson -lavformat -lavcodec -lnmea
LDFLAGS     += -Wl,--gc-sections --shared
LDFLAGS     += -Wl,--gc-sections
			   
EXTRA_CFLAGS = -I./ -I./infoscreen/ -I./lamp/ -I./command/ -I./gps/include -L./gps/lib
EXTRA_CXXFLAGS = -I./ -I./infoscreen/ -I./lamp/ -I./command/ -I./gps/include -L./gps/lib
EXTRA_CFLAGS += $(shell pkg-config --cflags --libs gstreamer-1.0)
EXTRA_CXXFLAGS += $(shell pkg-config --cflags --libs gstreamer-1.0)
LDFLAGS     += $(EXTRA_LDFLAGS)
CFLAGS      += $(EXTRA_CFLAGS)
CXXFLAGS    += $(EXTRA_CXXFLAGS)

SRCS:= \
    ./command/serial.c \
    ./lamp/ym_lamp_device_ability.c \
    ./lamp/ym_lamp_device.c \
    ./lamp/YmLamp.cc \
    ./infoscreen/CreatProgramVsn.cc \
    ./infoscreen/ColtCommand.cc \
    ./infoscreen/InfoScreenColt.cc \
    ./gps/GpsDevice.cc \
    ./gps/GpsDeviceBN.cc \
    ./gps/hserial.c \
	./gps/JsonParse.cc \
    ./brocast/gst_play.cc \
    ./brocast/BrocastDevice.cc \
    DeviceCommand.cc \
    DeviceInterface.cc \
    MethodParamParser.cc \
    MethodParser.cc \

DEPS:=$(patsubst %.cc,%.dep,$(patsubst %.c,%.dep,$(SRCS)))
OBJS:=$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SRCS)))

LIB_SO      :=libhallib.so
LIB_A       :=libhallib.a


.PHONY : all clean install


all: $(LIB_SO)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(LIB_SO): $(OBJS)
	$(CC) $(SRCS) $(LDFLAGS) $(CFLAGS) -o $@
#$(LDPATH) 
$(LIB_A): $(OBJS)
	$(AR) -crv $@ $(OBJS)

clean:
	$(RM) $(DEPS) $(OBJS) $(GENS) $(LIB_SO) $(LIB_A)

install: all
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include/infoscreen
	$(INSTALL) -s --strip-program=$(STRIP) $(LIB_SO) $(DESTDIR)$(PREFIX)/lib
	$(INSTALL) -m 644 $(HDRS) $(DESTDIR)$(PREFIX)/include/infoscreen


%.dep: %.c $(GENS)
	@echo "Depending $< ..."
	@set -e; \
    $(RM) $@.tmp; \
    $(CC) $(CFLAGS) -E -MM $< > $@.tmp; \
    sed 's#^\(.*\.o\):#$(@D)/\1 $@: #g' < $@.tmp >$@; \
    $(RM) $@.tmp

%.dep: %.cc $(GENS)
	@echo "Depending $< ... "
	@set -e; \
    $(RM) $@.tmp; \
    $(CXX) $(CXXFLAGS) -E -MM $< > $@.tmp; \
    sed 's#^\(.*\.o\):#$(@D)/\1 $@: #g' < $@.tmp >$@; \
    $(RM) $@.tmp

%.o: %.c
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	@echo "Compiling $< ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@


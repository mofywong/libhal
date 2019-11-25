TOP_DIR     :=$(shell pwd)/$(lastword $(MAKEFILE_LIST))
TOP_DIR     :=$(shell dirname $(TOP_DIR))

CC          = $(CROSS_COMPILE)gcc
CXX         = $(CROSS_COMPILE)g++
STRIP       = $(CROSS_COMPILE)strip
MAKE        ?= make
RM          ?= rm -rf
INSTALL     ?= install -c

DEFFLAGS    := -g -pthread -ffunction-sections -fdata-sections -I$(TOP_DIR)
CFLAGS      += $(DEFFLAGS)
CXXFLAGS    += $(DEFFLAGS) -std=c++11
LDFLAGS     += -pthread -lparson -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0\
               

EXTRA_CFLAGS = -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
EXTRA_CXXFLAGS = -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
LDFLAGS     += $(EXTRA_LDFLAGS)
CFLAGS      += $(EXTRA_CFLAGS)
CXXFLAGS    += $(EXTRA_CXXFLAGS)

SRCS:= \
     BrocastDevice.cc\
     main.cc\
     gst_play.cc\

CONF:= config.json

DEPS:=$(patsubst %.cc,%.dep,$(patsubst %.c,%.dep,$(SRCS)))
OBJS:=$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(SRCS)))

ifndef EXECUTABLE
EXECUTABLE = gst
endif

.PHONY : all clean install

all: $(EXECUTABLE)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(EXECUTABLE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(CFLAGS) -o $@ $(LIB_DIR)

install: $(EXECUTABLE)
	$(INSTALL) -d $(DESTDIR)/usr/bin
	$(INSTALL) -s --strip-program=$(STRIP) $(EXECUTABLE) $(DESTDIR)/usr/bin

clean:
	$(RM) $(DEPS) $(OBJS) $(EXECUTABLE)

%.dep: %.c
	@echo "Depending $< ..."
	@set -e; \
    $(RM) $@.tmp; \
    $(CC) $(CFLAGS) -E -MM $< > $@.tmp; \
    sed 's#^\(.*\.o\):#$(@D)/\1 $@: #g' < $@.tmp >$@; \
    $(RM) $@.tmp

%.dep: %.cc
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

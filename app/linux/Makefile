################################################################################
# Makefile to make zw api test application
################################################################################

CC=/home/tiny.hui/software/android-ndk-r11b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc
AR=/home/tiny.hui/software/android-ndk-r11b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-ar
CFLAGS=-pie -fPIE -I/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/include
LDFLAGS=-pie -fPIE -L/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/lib

RM := rm -rf

LIBS := -luzbctl -lserial_api

ifeq (,$(findstring DOS_MAC_X,$(CFLAGS)))
	
else
	LDFLAGS = -arch i386
endif

SRC_OBJS = \
zw_api_test_intf.o \
zw_api_test_intf2.o \
zw_api_test_nw.o \
zw_api_test_ui.o

LIB_FILES = \
../../src/libuzbctl.a \
../../lib/libserial_api.a

SRC_HEADERS = \
../../include/zw_plt_linux.h ../../include/zw_hci_platform.h \
../../include/zw_hci_util.h ../../include/zw_hci_transport.h \
../../include/zw_plt_linux.h ../../include/zw_hci_frame.h \
../../include/zw_hci_session.h ../../include/zw_hci_application.h \
../../include/zw_hci_fl_prog.h \
../../include/zw_api.h ../../include/zwave/ZW_transport_api.h \
../../include/zwave/ZW_classcmd.h \
zw_api_test.h



# All Target
all: zw_api_tst

# Compile c source file
%.o: %.c $(SRC_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) -O3 -Wall -DOS_LINUX $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
zw_api_tst: $(SRC_OBJS) $(LIB_FILES)
	@echo 'Building target: $@'
	$(CC) -L"../../lib" -L"../../src" $(LDFLAGS) -o zw_api_tst $(SRC_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) $(SRC_OBJS) zw_api_tst
	-@echo ' '

.PHONY: all clean

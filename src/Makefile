################################################################################
# Makefile to make uzbctl library
################################################################################

CC=/home/tiny.hui/software/android-ndk-r11b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc
AR=/home/tiny.hui/software/android-ndk-r11b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-ar
CFLAGS=-pie -fPIE -I/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/include
LDFLAGS=-pie -fPIE -L/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/lib

RM := rm -rf

LIB_OBJS = \
zw_api_base.o \
zw_api_intf.o \
zw_api_network.o \
zw_api_util.o \
zw_poll.o \
zw_sec_aes.o \
zw_sec_layer.o \
zw_sec_random.o


LIB_HEADERS = \
 ../include/zw_hci_error.h ../include/zw_hci_platform.h \
 ../include/zw_hci_util.h ../include/zw_hci_transport.h \
 ../include/zw_plt_linux.h ../include/zw_hci_frame.h \
 ../include/zw_hci_session.h ../include/zw_hci_application.h \
 ../include/zw_hci_fl_prog.h \
 ../include/zw_api_pte.h ../include/zw_api.h \
 ../include/zw_security.h ../include/zw_api_util.h \
 ../include/zw_poll.h \
 ../include/zwave/ZW_controller_api.h \
 ../include/zwave/ZW_transport_api.h ../include/zwave/ZW_classcmd.h


# All Target
all: libuzbctl.a

# Compile c source file
%.o: %.c $(LIB_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) -O3 -Wall -DOS_LINUX $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
libuzbctl.a: $(LIB_OBJS)
	@echo 'Building target: $@'
	$(AR) -r "libuzbctl.a" $(LIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) $(LIB_OBJS) libuzbctl.a
	-@echo ' '

.PHONY: all clean

################################################################################
# Makefile to make serial_api library
################################################################################

CC=/home/tiny.hui/software/android-ndk-r11b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-gcc
AR=/home/tiny.hui/software/android-ndk-r11b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-ar
CFLAGS=-pie -fPIE -I/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/include
LDFLAGS=-pie -fPIE -L/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/lib

RM := rm -rf

LIB_OBJS = \
zw_hci_application.o \
zw_hci_frame.o \
zw_hci_platform.o \
zw_hci_session.o \
zw_hci_transport.o \
zw_hci_fl_prog.o \
zw_hci_util.o


LIB_HEADERS = \
../include/zw_hci_error.h ../include/zw_hci_platform.h \
../include/zw_hci_util.h ../include/zw_hci_transport.h \
../include/zw_plt_linux.h ../include/zw_hci_frame.h \
../include/zw_hci_session.h ../include/zw_hci_application.h \
../include/zw_hci_fl_prog.h \
../include/zwave/ZW_SerialAPI.h \
../include/zwave/ZW_transport_api.h \
../include/zw_plt_linux.h

# All Target
all: libserial_api.a

# Compile c source file
%.o: %.c $(LIB_HEADERS)
	@echo 'Compiling file: $<'
	$(CC) -O3 -Wall -DOS_LINUX $(CFLAGS) -c -o"$@" "$<"
	@echo 'Finished compiling: $<'
	@echo ' '

# Tool invocations
libserial_api.a: $(LIB_OBJS)
	@echo 'Building target: $@'
	$(AR) -r "libserial_api.a" $(LIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) $(LIB_OBJS) libserial_api.a
	-@echo ' '

.PHONY: all clean

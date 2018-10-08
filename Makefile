################################################################################
# Makefile to make everything
################################################################################

TARGET_PLATFORM = LINUX

ifeq ($(TARGET_PLATFORM),OSX)
	MAKE_PARAM_CFLAG = 'CFLAGS=-DOS_MAC_X -arch i386'
endif
ifeq ($(TARGET_PLATFORM),LINUX)
	MAKE_PARAM_CFLAG = 'CFLAGS=-DFLASH_PROG_API -pie -fPIE -I/home/tiny.hui/software/android-ndk-r11b/platforms/android-23/arch-arm/usr/include'
endif

# All Target
all::

all \
clean ::
	@cd lib && ${MAKE} ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd src && ${MAKE} ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@
	@cd app/linux && ${MAKE} ${MAKE_PARAM_CC} ${MAKE_PARAM_CFLAG} ${MAKE_PARAM_AR} $@

.PHONY: all clean

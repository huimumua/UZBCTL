
#ifndef _ZW400USB_USER_H
#define _ZW400USB_USER_H

#include <initguid.h>

#ifndef CTL_CODE
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define FILE_DEVICE_UNKNOWN             0x00000022

// Define the method codes for how buffers are passed for I/O and FS controls
#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

// Define the access check value for any access
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
// FILE_SPECIAL_ACCESS is checked by the NT I/O system the same as FILE_ANY_ACCESS.
// The file systems, however, may add additional access checks for I/O and FS controls
// that use this value.
#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe
#endif

// {b5c66d0b-f385-481e-b612-f89eb9e1a41b}
DEFINE_GUID(GUID_CLASS_ZENSYS_ZW400USB, 
0xb5c66d0b, 0xf385, 0x481e, 0xb6, 0x12, 0xf8, 0x9e, 0xb9, 0xe1, 0xa4, 0x1b);
#define ZW400USB_PORT1		"USBPORT1"
#define ZW400USB_PORT2		"USBPORT2"

#define ZW400USB_IOCTL_INDEX             0x0000


#define IOCTL_ZW400USB_GET_CONFIG_DESCRIPTOR	CTL_CODE(FILE_DEVICE_UNKNOWN,	\
                                                     ZW400USB_IOCTL_INDEX,		\
                                                     METHOD_BUFFERED,			\
                                                     FILE_ANY_ACCESS)
                                                   
#define IOCTL_ZW400USB_RESET_DEVICE				CTL_CODE(FILE_DEVICE_UNKNOWN,	\
                                                     ZW400USB_IOCTL_INDEX + 1,	\
                                                     METHOD_BUFFERED,			\
                                                     FILE_ANY_ACCESS)

#define IOCTL_ZW400USB_RESET_PORT				CTL_CODE(FILE_DEVICE_UNKNOWN,	\
                                                     ZW400USB_IOCTL_INDEX + 2,	\
                                                     METHOD_BUFFERED,			\
                                                     FILE_ANY_ACCESS)

#endif

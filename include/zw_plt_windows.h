/**
@file   zw_plt_windows.h - Z-wave host controller interface Windows platform header file.

        Definition of minimum Windows version requirement and include files.

@author David Chow

@version    1.0 31-5-10  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_PLT_WINDOWS_DAVID_
#define _ZW_PLT_WINDOWS_DAVID_

#define _WIN32_WINNT 0x0501				//Minimum requirement: XP system

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>

#endif /* _ZW_PLT_WINDOWS_DAVID_ */


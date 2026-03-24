#pragma once

#define WINVER       0x0A00		// Windows 10
#define _WIN32_WINNT 0x0A00		// Windows 10

#ifdef CLEARBITS_QT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <wincrypt.h>
#include <atlstr.h>
#include <cassert>

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

#ifndef VERIFY
#ifdef NDEBUG
#define VERIFY(x) ((void)(x))
#else
#define VERIFY(x) assert(x)
#endif
#endif

#else

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <mmsystem.h> 
#include <mmreg.h> 
#include <ks.h> 
#include <ksmedia.h>
#include <Msacm.h>
#include <wincrypt.h>	// for MS Crypto API

#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

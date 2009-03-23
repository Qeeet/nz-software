// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__321D9716_61C3_4B2D_8986_18930907152A__INCLUDED_)
#define AFX_STDAFX_H__321D9716_61C3_4B2D_8986_18930907152A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define WINVER	0x0400
#define _WIN32_IE		0x0500

//#include <windows.h>
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// custom added
#include <afxtempl.h>
#include <afxpriv.h>
#include <afxole.h>

// for filewatch
#include <afxmt.h>

// for ms xml parser
#include <msxml2.h>
//#import "msxml4.dll"
//using namespace MSXML2;

// for html help
#include <htmlhelp.h>

// for Open From URL
#include <shlwapi.h>

#include "Downloaded Components\SizingCBar\sizecbar.h"
#include "Downloaded Components\SizingCBar\scbarg.h"
#include "Globals.h"

#define SIZEBAR_BASECLASS		CSizingControlBarG




// Bonfire Debug/Release Level Switches
#ifdef _DEBUG
// debug:
#define BON_EDIT_XMLTREE
#else
// release:

#endif

// both modes:




#include <COMUTIL.H>
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__321D9716_61C3_4B2D_8986_18930907152A__INCLUDED_)

#pragma once

#define _WIN32_WINNT		0x0500
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include <ole2.h>

// for html help
#include <htmlhelp.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <float.h>

#include <string>
#include <vector>

#include "CalcClasses.h"
#include "resource.h"

using namespace Calc;

#define SMALL_CY			13
#define INPUT_CY			20
#define BORDER				0

#define MINWIDTH			100
#define MINHEIGHT			100

#define MAX_PREVCOMMANDS	10

// for SetLayeredWindowAttributes (this also prevents link errors in win95, 98, etc.)
typedef WINUSERAPI BOOL (WINAPI *SETLAYEREDWINDOWATTRIBUTESFN)
	(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

// globals
std::string g_strShortVersion;
std::string g_strLongVersion;
HINSTANCE g_hInstance;
HMODULE g_arrLoadedLibs[10];
HWND g_wndMain;
HWND g_wndInput;
HWND g_wndOutput;
HFONT g_fntInput;
HBRUSH g_brushBkInput;
COLORREF g_clrBkOutput;
COLORREF g_clrTxInput;
COLORREF g_clrTxOutput;
COLORREF g_clrTxOutputError;
HIMAGELIST g_imlSymbolIcons;
WINDOWPLACEMENT g_wpOrig;

LONG_PTR g_oldInputProc;
std::vector<std::string> g_prevCommands;
int g_nSelPrevCommand;

Eval g_eval("",1.0000000,false);
BOOL g_bHoldOutput;

// interface flags
#define PROGDATA_ALWAYS_ON_TOP	1
#define PROGDATA_TRANSLUCENT	2
#define PROGDATA_AUTO_COPY		3
#define PROGDATA_GRIP_CX		4

#define PROGDATA_DEGREES		10
#define PROGDATA_SAVE_SYMBOLS	11

int g_arrProgData[20];

// global functions
BOOL InitSymbols();
//BOOL TokenizeStatement(LPCSTR, std::vector<Token>&, int&);
void ProcessInput(LPCSTR);
std::string ValueToStr(const Value& val, bool bQuote = true);
void AddReturnOutput(LPCSTR, const Return&, BOOL = TRUE);

#define OUTSTYLE_DEFAULT		0
#define OUTSTYLE_RESULT			1
#define OUTSTYLE_SYSMESSAGE		2
#define OUTSTYLE_ERROR			3
#define OUTSTYLE_UNDERLINE		4
void AddOutput(LPCSTR, BOOL = TRUE, UINT = OUTSTYLE_DEFAULT);

void ClearStatement(Eval&, Return&, Value*, int);
void DeleteStatement(Eval&, Return&, Value*, int);
void PrintStatement(Eval&, Return&, Value*, int);

BOOL LoadSettings();
void SaveSettings();

BOOL RegisterClasses();
BOOL InitInstance();
void ExitInstance();
void UpdateInterface();
void DrawMainWindow(HWND, HDC, RECT&);
COLORREF BlendColors(COLORREF clr1, COLORREF clr2, UINT percentage);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InputSubProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OptionsDlgProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK SymbolListSortProc(LPARAM, LPARAM, LPARAM);
void UpdateSymbolTableItem(HWND, int, Symbol*);
LRESULT CALLBACK SymbolTableDlgProc(HWND, UINT, WPARAM, LPARAM);
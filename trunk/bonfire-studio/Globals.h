#ifndef NZ_BONFIRE_GLOBALS_1_IS_DEFINED
#define NZ_BONFIRE_GLOBALS_1_IS_DEFINED

#if _MSC_VER > 1000
#pragma once
#endif

#include <vector>

CString GetFileExtension(CString cszPath);
CString GetFilePath(CString cszPath);
CString GetFileTitle(CString cszPath);
CString AddSlash(CString cszPath);

HICON GetIconForFile(CString strPath, BOOL bLargeIcon = FALSE);
int GetIconIndex(CString ext, CImageList* pImageList);

void ShowPopup(CWnd* pOwner, CMenu *pMenu);

BOOL CreateBSTR(BSTR bstrDest, int nDestSize, char* strSource, int nSourceSize);

BOOL CreateDisabledImage(CBitmap& bmp);

BOOL SplitString(const CString& str, std::vector<CString>& arr, char cSep = ',');
BOOL IsStringPresent(const CString& str, const CString& strTest, char cSep = ',');

CString EncodeString(CString str);
CString DecodeString(CString str);

#define FILTER_FILES		2000
#define FILTER_PROJECTS		2002

CString GetFileFilter(UINT nType);
CString GetDefExt(UINT nType);

int OpenFilesDialog(CWnd* pOwner, CStringArray& arrOutFiles, BOOL bAllowMultipleFiles = FALSE,
	LPCTSTR strDefaultFile = NULL, LPCTSTR strTitle = NULL,
	LPCTSTR strFilter = "All Files (*.*)|*.*||", int nDefaultFilterIndex = 1,
	LPCTSTR strDefExt = NULL, BOOL* pbReadOnly = FALSE);

int SaveFileDialog(CWnd* pOwner, CString& strOutFile, LPCTSTR strDefaultFile = NULL, LPCTSTR strTitle = NULL,
	LPCTSTR strFilter = "All Files (*.*)|*.*||", int nDefaultFilterIndex = 1,
	LPCTSTR strDefExt = NULL);

#endif

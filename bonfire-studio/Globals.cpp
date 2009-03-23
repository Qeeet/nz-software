#include "stdafx.h"
#include "Globals.h"
#include <cderr.h>
#include <io.h>

#include <math.h> // for pow

CString GetFileExtension(CString cszPath)
{
	int lastDot = cszPath.ReverseFind('.');
	int lastSlash = cszPath.ReverseFind('\\');

	if (lastDot == -1 || lastDot < lastSlash)
		return "";
	else
		return cszPath.Mid(lastDot + 1);
}

CString GetFilePath(CString cszPath)
{
	int lastSlash = cszPath.ReverseFind('\\');

	if (lastSlash == -1)
		return "";
	else
		return cszPath.Left(lastSlash + 1);
}

CString GetFileTitle(CString cszPath)
{
	int lastSlash = cszPath.ReverseFind('\\');

	if (lastSlash == -1)
		return cszPath;
	else
		return cszPath.Mid(lastSlash + 1);
}

CString AddSlash(CString cszPath)
{
	if (cszPath.Right(1) == '\\')
		return cszPath;
	else
		return cszPath + "\\";
}

HICON GetIconForFile(CString strPath, BOOL bLargeIcon)
{
	SHFILEINFO fInfo; memset(&fInfo,0,sizeof(fInfo));
	SHGetFileInfo(strPath,0,&fInfo,sizeof(SHFILEINFO),
		SHGFI_ICON | (bLargeIcon ? SHGFI_LARGEICON : SHGFI_SMALLICON));

	return fInfo.hIcon; // must be eventually destroyed by calling function
}

//CList <CString, CString&> hello;
//CMap <CString, CString, int, int> treeIconIndexes(20);
CMapStringToPtr treeIconIndexes(20);

int GetIconIndex(CString file, CImageList* pImageList)
{
	CString ext = GetFileExtension(file);
	char* index = NULL;
	int in = -1;

	// first make sure that the file exists
	struct _finddata_t c_file;
	if (_findfirst(file, &c_file) == -1L)
		return in;

	if (ext.GetLength() > 0)
	{
		// see if pair exists in hash (dont do .exe's and .ico's)
		if ( !treeIconIndexes.Lookup(ext, (void*&)index) 
			|| ext.Compare("exe") == 0 || ext.Compare("ico") == 0)
		{
			// create the icon
			HICON rIcon = GetIconForFile(file);

			// insert the icon into image list
			if(rIcon != NULL) 
				in = pImageList->Add(rIcon);

			DestroyIcon(rIcon);

			if (in != -1)
			{
				index = (char*)malloc(4);
				index = itoa(in, index, 10);
				// add to hash
				treeIconIndexes.SetAt(ext, index);
			}
		}
		else
		{
			in = atoi(index);
		}
	}

	return in;
}

void ShowPopup(CWnd* pOwner, CMenu *pMenu)
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
		ptCursor.x,ptCursor.y,pOwner,NULL);
}

BOOL CreateBSTR(BSTR bstrDest, int nDestSize, char* strSource, int nSourceSize)
{
	if (MultiByteToWideChar(CP_ACP,0,
		strSource,nSourceSize,
		(LPWSTR)bstrDest,nDestSize) == 0)
		return FALSE;
	
	return TRUE;
}

BOOL CreateDisabledImage(CBitmap& bmp)
{
	DIBSECTION dib;
	if (!bmp.GetObject(sizeof(DIBSECTION),&dib))
		return FALSE;

	COLORREF clr = GetSysColor(COLOR_BTNFACE);

	const double contrast = 0.3;

	// color table version
	if (dib.dsBm.bmBitsPixel == 8 ||
		dib.dsBm.bmBitsPixel == 4 ||
		dib.dsBm.bmBitsPixel == 1)
	{
		int nColors = (dib.dsBmih.biClrUsed == 0) ? (int)pow(2,dib.dsBmih.biBitCount) : dib.dsBmih.biClrUsed;

		RGBQUAD* arrColors = new RGBQUAD[nColors];

		CClientDC dcScreen(NULL);
		CDC dc; dc.CreateCompatibleDC(&dcScreen);

		CBitmap* oldbmp = dc.SelectObject(&bmp);

		BOOL bOkay = TRUE;

		if (!GetDIBColorTable(dc,0,nColors,arrColors))
			bOkay = FALSE;

		for (int i = 0; i < nColors; i++)
		{
			// skip magenta mask
			if (arrColors[i].rgbRed == 255 &&
				arrColors[i].rgbGreen == 0 &&
				arrColors[i].rgbBlue == 255)
				continue;

			arrColors[i].rgbRed = (BYTE)(double)(arrColors[i].rgbRed * contrast + GetRValue(clr) * (1 - contrast));
			arrColors[i].rgbGreen = (BYTE)(double)(arrColors[i].rgbGreen * contrast + GetGValue(clr) * (1 - contrast));
			arrColors[i].rgbBlue = (BYTE)(double)(arrColors[i].rgbBlue * contrast + GetBValue(clr) * (1 - contrast));

			//arrColors[i].rgbRed = arrColors[i].rgbGreen = arrColors[i].rgbBlue = 
			//	(arrColors[i].rgbRed + arrColors[i].rgbGreen + arrColors[i].rgbBlue) / 3;
		}

		if (!SetDIBColorTable(dc,0,nColors,arrColors))
			bOkay = FALSE;

		dc.SelectObject(oldbmp);
		dc.DeleteDC();

		delete [] arrColors;

		return bOkay;
	}
	else
	{
		BYTE* pBits = (BYTE*)dib.dsBm.bmBits;
		for (int i = 0; i < dib.dsBmih.biWidth * dib.dsBmih.biHeight; i++,
			pBits += (dib.dsBm.bmBitsPixel / 8))
		{
			// skip magenta mask
			if (pBits[2] == 255 &&
				pBits[1] == 0 &&
				pBits[0] == 255)
				continue;

			pBits[2] = (BYTE)(double)(pBits[2] * contrast + GetRValue(clr) * (1 - contrast));
			pBits[1] = (BYTE)(double)(pBits[1] * contrast + GetGValue(clr) * (1 - contrast));
			pBits[0] = (BYTE)(double)(pBits[0] * contrast + GetBValue(clr) * (1 - contrast));

			//pBits[2] = pBits[1] = pBits[0] = (pBits[2] + pBits[1] + pBits[0]) / 3;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL SplitString(const CString& str, std::vector<CString>& arr, char cSep)
{
	int nPos = 0;
	int nNextPos = 0;
	
	while (nPos < str.GetLength())
	{
		nNextPos = str.Find(cSep,nPos);
		if (nNextPos == -1)
		{
			if (nPos)
				arr.push_back(str.Mid(nPos));
			else
				arr.push_back(str);
			return TRUE;
		}

		arr.push_back(str.Mid(nPos,nNextPos - nPos));

		nPos = nNextPos + 1;
	}

	return TRUE;
}

BOOL IsStringPresent(const CString& str, const CString& strTest, char cSep)
{
	int nPos		= 0;
	int nNextPos	= 0;
	while (nPos < str.GetLength())
	{
		nNextPos = str.Find(cSep,nPos);
		if (nNextPos == -1)
		{
			if (nPos && str.Mid(nPos) == strTest)
				return TRUE;
			else if (str == strTest)
				return TRUE;
			return FALSE;
		}

		if (str.Mid(nPos,nNextPos - nPos) == strTest)
			return TRUE;

		nPos = nNextPos + 1;
	}

	return FALSE;
}

CString EncodeString(CString str)
{
	CString strReturn = str;
	strReturn.Replace("&", "&amp;");
	return strReturn;
}

CString DecodeString(CString str)
{
	CString strReturn = str;
	strReturn.Replace("&amp;", "&");
	return strReturn;
}

CString GetFileFilter(UINT nType)
{
	CString str;
	str.LoadString(nType);
	return str;
}

CString GetDefExt(UINT nType)
{
	CString str;
	str.LoadString(nType + 1);
	return str;
}

int OpenFilesDialog(CWnd* pOwner, CStringArray& arrOutFiles, BOOL bAllowMultipleFiles, 
					LPCTSTR strDefaultFile, LPCTSTR strTitle, LPCTSTR strFilter,
					int nDefaultFilterIndex, LPCTSTR strDefExt, BOOL* pbReadOnly)
{
	// prompt the user for an open file name
	TCHAR* lfnm = (TCHAR*)malloc((size_t)MAX_PATH * 10); // usually no more than 10 files
	if (strDefaultFile)
		strcpy(lfnm,strDefaultFile);
	else
		lfnm[0] = 0;

	DWORD dwMulti = bAllowMultipleFiles ? OFN_ALLOWMULTISELECT : 0;
	DWORD dwHideReadOnly = (pbReadOnly == NULL) ? OFN_HIDEREADONLY : 0;

	OPENFILENAME ofn; memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pOwner->m_hWnd;
	ofn.Flags = OFN_FILEMUSTEXIST | dwHideReadOnly | OFN_EXPLORER | dwMulti;
	ofn.nMaxFile = MAX_PATH * 10;
	ofn.lpstrFile = lfnm;
	ofn.lpstrDefExt = strDefExt;
	char strFilter2[256]; strcpy(strFilter2,strFilter);
	char* p = strFilter2;
	while (p = strchr(p,'|')) *p++ = 0; // replace all | with nulls
	ofn.lpstrFilter = strFilter2;
	ofn.nFilterIndex = nDefaultFilterIndex;
	ofn.lpstrTitle = strTitle;

	int nResult = GetOpenFileName(&ofn);
	if (nResult == 0 && (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL))
	{
		// resize the buffer to what is needed by the dialog box
		WORD wNeededSize;
		memcpy(&wNeededSize,lfnm,sizeof(WORD)); // copy first two bytes to get needed size
		free(lfnm); lfnm = (TCHAR*)malloc((size_t)wNeededSize);
		nResult = GetOpenFileName(&ofn); // try again with new buffer
	}

	// if the function caller supplied a boolean pointer for the read only state,
	// fill it with the user's choice of readonly or not readonly
	if (pbReadOnly)
		*pbReadOnly = (ofn.Flags & OFN_READONLY) ? TRUE : FALSE;

	if (nResult != 0)
	{
		if (bAllowMultipleFiles)
		{
			TCHAR* strCur = ofn.lpstrFile;
			do
			{
				arrOutFiles.Add(strCur);
				strCur += strlen(strCur) + 1;
			}
			while (strCur[0] != 0);
		}
		else
			arrOutFiles.Add(ofn.lpstrFile);
	}

	free(lfnm);

	return nResult;
}

int SaveFileDialog(CWnd* pOwner, CString& strOutFile, LPCTSTR strDefaultFile,
				   LPCTSTR strTitle, LPCTSTR strFilter, int nDefaultFilterIndex,
				   LPCTSTR strDefExt)
{
	// prompt the user for a save file name
	TCHAR* lfnm = (TCHAR*)malloc((size_t)MAX_PATH);
	if (strDefaultFile)
		strcpy(lfnm,strDefaultFile);
	else
		lfnm[0] = 0;

	OPENFILENAME ofn; memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pOwner->m_hWnd;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = lfnm;
	ofn.lpstrDefExt = strDefExt;
	char strFilter2[256]; strcpy(strFilter2,strFilter);
	char* p = strFilter2;
	while (p = strchr(p,'|')) *p++ = 0; // replace all | with nulls
	ofn.lpstrFilter = strFilter2;
	ofn.nFilterIndex = nDefaultFilterIndex;
	ofn.lpstrTitle = strTitle;

	int nResult = GetSaveFileName(&ofn);
	if (nResult == 0 && (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL))
	{
		// resize the buffer to what is needed by the dialog box
		WORD wNeededSize;
		memcpy(&wNeededSize,lfnm,sizeof(WORD)); // copy first two bytes to get needed size
		free(lfnm); lfnm = (TCHAR*)malloc((size_t)wNeededSize);
		nResult = GetSaveFileName(&ofn); // try again with new buffer
	}

	if (nResult != 0)
		strOutFile = ofn.lpstrFile;

	free(lfnm);

	return nResult;
}

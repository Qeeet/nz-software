// FindInFiles.cpp: implementation of the CFindInFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bonfire.h"
#include "MainFrm.h"
#include "FindInFiles.h"
#include "FindInFilesDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMainFrame* g_pMainFrame;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindInFiles::CFindInFiles()
{

}

CFindInFiles::~CFindInFiles()
{

}

static int FindStringHelper(LPCTSTR pszFindWhere, LPCTSTR pszFindWhat, BOOL bWholeWord)
{
	ASSERT(pszFindWhere != NULL);
	ASSERT(pszFindWhat != NULL);
	int nCur = 0;
	int nLength = lstrlen(pszFindWhat);
	for (;;)
	{
#ifdef _UNICODE
		LPCTSTR pszPos = wcsstr(pszFindWhere, pszFindWhat);
#else
		LPCTSTR pszPos = strstr(pszFindWhere, pszFindWhat);
#endif
		if (pszPos == NULL)
			return -1;
		if (! bWholeWord)
			return nCur + (pszPos - pszFindWhere);
		if (pszPos > pszFindWhere && (isalnum(pszPos[-1]) || pszPos[-1] == _T('_')))
		{
			nCur += (pszPos - pszFindWhere);
			pszFindWhere = pszPos + 1;
			continue;
		}
		if (isalnum(pszPos[nLength]) || pszPos[nLength] == _T('_'))
		{
			nCur += (pszPos - pszFindWhere + 1);
			pszFindWhere = pszPos + 1;
			continue;
		}
		return nCur + (pszPos - pszFindWhere);
	}
	ASSERT(FALSE);		// Unreachable
	return -1;
}

// recursive function that traverses a folder, creating CXNodes for subfolders
BOOL CFindInFiles::SearchFolder(CString strFindWhat, CString strFolderPath, CString strFileFilter, BOOL bSubfolders)
{
	// go through each file/folder
	struct _finddata_t c_file;
	long hFile = 0;
		
	hFile = _findfirst(AddSlash(strFolderPath) + "*", &c_file);
	if (hFile == -1L)
		return FALSE;

	// go through all file filters to search files
	CString t_strFileFilter = strFileFilter;
	t_strFileFilter += ";";
	int i = 0;
	while( ((i = t_strFileFilter.FindOneOf(",")) > -1) ||
			((i = t_strFileFilter.FindOneOf(";")) > -1) )
	{
		CString ext = t_strFileFilter.Left(i);
		t_strFileFilter = t_strFileFilter.Right(t_strFileFilter.GetLength() - i - 1);
		hFile = _findfirst(AddSlash(strFolderPath) + ext, &c_file);
		if (hFile == -1L)
			continue;

		do
		{
			if (strcmp(c_file.name, ".") == 0 || strcmp(c_file.name, "..") == 0) continue;
			
			if (! (c_file.attrib & _A_SUBDIR) ) //files
			{
				CString strFileName = AddSlash(strFolderPath) + GetFileTitle(c_file.name);
				FindInFile(strFileName, strFindWhat);
			}
		}
		while (_findnext(hFile, &c_file ) != -1L );
	}

	if (bSubfolders)
	{
		hFile = _findfirst(AddSlash(strFolderPath) + "*", &c_file);
		if (hFile == -1L)
			return FALSE;

		// search subdirectories
		do
		{
			if (strcmp(c_file.name, ".") == 0 || strcmp(c_file.name, "..") == 0) continue;
			
			if (c_file.attrib & _A_SUBDIR) //folder
			{
				CString strNewFolder = AddSlash(strFolderPath) + GetFileTitle(c_file.name);
				
				// search the subfolders
				SearchFolder(strFindWhat, strNewFolder, strFileFilter, bSubfolders);
			}
		}
		while (_findnext(hFile, &c_file ) != -1L );
	}

	return TRUE;
}

BOOL CFindInFiles::FindInFiles()
{
	CFindInFilesDlg FifDlg;
	
	if (FifDlg.DoModal() == IDOK)
	{
		// show the wait cursor until everything is done
		CWaitCursor wc;

		if (FifDlg.m_strSearchPath.GetLength() < 1 || 
			//FifDlg.m_strFileTypes.GetLength() < 1 ||
			FifDlg.m_strSearchText.GetLength() < 1)
			return FALSE;

		if (FifDlg.m_strFileTypes.GetLength() < 1)
			FifDlg.m_strFileTypes = "*.*";
		
		m_dwLastSearchFlags = 0;
		if (FifDlg.m_bMatchCase)
			m_dwLastSearchFlags |= FIND_MATCH_CASE;
		if (FifDlg.m_bWholeWord)
			m_dwLastSearchFlags |= FIND_WHOLE_WORD;

		// set the pointer for access in the other functions
		//m_dlgFindInFiles = &FifDlg;

		g_pMainFrame->m_wndOutputBar.ClearList(OUTPUT_FIF_RESULT);

		// search the folder
		BOOL retVal = SearchFolder(FifDlg.m_strSearchText, FifDlg.m_strSearchPath, FifDlg.m_strFileTypes, FifDlg.m_bSearchSubfolders);

		CString strTotalFIF;
		strTotalFIF.Format("%d item(s) found.", g_pMainFrame->m_wndOutputBar.GetFIFCount() );

		// create the message object
		OutputMessage* pMsg = new OutputMessage();
		pMsg->enType		= OUTPUT_FIF_RESULT;
		pMsg->enResult		= MSG_NONE;
		pMsg->strMessage	= strTotalFIF;

		g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
					
		// reset the pointer to nothing
		//m_dlgFindInFiles = NULL;

		return retVal;
	}

	return FALSE; //FindInFile("C:\\winzip2.log", "search");
}

BOOL CFindInFiles::FindInFile(LPCTSTR pszFileName, LPCTSTR pszWhat)
{	
	//int nCrlfStyle = 0; // windows 
	const char *crlf = "\x0d\x0a"; //windows
	const char crlf1 = '\x0d'; 
	const char crlf2 = '\x0a'; 

	char* pcLineBuf = 0;
	char* pcFileBuf = 0;

	BOOL bMatchCase = m_dwLastSearchFlags & FIND_MATCH_CASE;
	BOOL bWholeWord = m_dwLastSearchFlags & FIND_WHOLE_WORD;

	int nLines = 1;

	HANDLE hFile = NULL;
	BOOL bSuccess = FALSE;

	try
	{
		DWORD dwFileAttributes = ::GetFileAttributes(pszFileName);
		if (dwFileAttributes == (DWORD) -1)
			throw "";

		// open the file for reading
		hFile = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw "";

		// get the search string
		CString strWhat = pszWhat;
		if (!bMatchCase)
			strWhat.MakeUpper();
		
		const DWORD dwBufSize	= 32768;
		DWORD		dwLineSize	= 256;
		DWORD		dwCurSize	= 0;
		DWORD		dwFilePos	= 0;
		DWORD		dwLinePos	= 0;

		pcLineBuf = (char*) calloc(dwLineSize, sizeof(char));
		//pcLineBuf = new char[dwLineSize];
		pcFileBuf = (char*) _alloca(dwBufSize);

		// read more text from file
		if (! ::ReadFile(hFile, pcFileBuf, dwBufSize, &dwCurSize, NULL))
			throw "";

		USES_CONVERSION;
		while (dwFilePos < dwCurSize)
		{
			char c = pcFileBuf[dwFilePos++];
			pcLineBuf[dwLinePos++] = c;

			if (dwLinePos == dwLineSize)
			{
				// reallocate line buffer
				dwLineSize += 256;
				pcLineBuf = (char*)realloc(pcLineBuf, dwLineSize);
				if (!pcLineBuf)
					throw "";
			}

			if (c == crlf2)
			{
				//pcLineBuf[dwLinePos - nCrlfPtr] = 0;
				CString strLine;
				strLine.Format("%s", pcLineBuf);
				//if ((dwFlags & FIND_MATCH_CASE) == 0)
				if (!bMatchCase)
					strLine.MakeUpper();
				
				// Perform search in the line
				pcLineBuf[dwLinePos] = '\0';
				int nCol	= 0; //different from nPos bc it increases with multiple searches
				int nPos	= FindStringHelper(strLine, strWhat, bWholeWord);
				while (nPos > -1)
				{
					nCol	+= nPos;
					// create the message object
					OutputMessage* pMsg = new OutputMessage();
					pMsg->enType		= OUTPUT_FIF_RESULT;
					pMsg->enResult		= MSG_FIF;
					pMsg->strMessage	= pszFileName;
					pMsg->nLine			= nLines;
					pMsg->nCol			= nCol + 1;
					pMsg->nSelSize		= strWhat.GetLength();
					pMsg->strLine.Format("%s", pcLineBuf);

					g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

					// look again on the same line
					strLine	= strLine.Right(strLine.GetLength() - nPos);
					nPos	= FindStringHelper(
								strLine.Right(strLine.GetLength() - 1), 
								strWhat, bWholeWord);

					// incerement it here because we removed 1 extra char before
					if (nPos > -1)	nPos	++;
				}
				
				pcLineBuf[0] = '\0';
				memset(pcLineBuf, '\0', dwLinePos);
				nLines++;
				dwLinePos = 0;
			}

			// read more if needed
			if (dwFilePos == dwCurSize && dwCurSize == dwBufSize)
			{
				if (! ::ReadFile(hFile, pcFileBuf, dwBufSize, &dwCurSize, NULL))
					throw "";
				dwFilePos = 0;
			}
		}

		bSuccess = TRUE;
	}
	catch(...)
	{
		bSuccess = FALSE;
	}

	if (pcLineBuf != NULL)
		free(pcLineBuf);
	if (hFile != NULL)
		::CloseHandle(hFile);

	return bSuccess;
}

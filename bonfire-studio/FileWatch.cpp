// FileWatch.cpp: implementation of the CFileWatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileWatch.h"
#include "Bonfire.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CBonfireApp theApp;

extern UINT g_msgFileWatchNotify;

bool GetLastWriteTime(LPCTSTR lpszFileName, __int64 &ftLastWriteTime)
{
	CFile file;
	if (file.Open(lpszFileName, CFile::shareDenyNone))
	{
		BY_HANDLE_FILE_INFORMATION info;
		if (GetFileInformationByHandle((HANDLE)file.m_hFile, &info))
		{
			ftLastWriteTime = (__int64(info.ftLastWriteTime.dwHighDateTime)<<32) + info.ftLastWriteTime.dwLowDateTime;
			return true;
		}
	}
	TRACE("GetLastWriteTime failed for %s", lpszFileName);
	return false;
}

CString GetFileFolder(LPCTSTR lpszFileName)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	_tsplitpath(lpszFileName, szDrive, szDir, NULL, NULL);
	return CString(szDrive) + CString(szDir);
}

bool IsFolder(LPCTSTR lpszFileName)
{
	return lpszFileName[strlen(lpszFileName)-1] == '\\';
}

template <class T> T maximum(const T& x, const T& y)
{
	return x>y ? x:y;
}

CSize GetTextExtend(CWnd* pWnd, LPCTSTR lpszText)
{
	CDC *pDC = pWnd->GetDC();
	CFont *pFont = pWnd->GetFont();
	CFont *pOldFont = pDC->SelectObject(pFont);
	TEXTMETRIC tm; pDC->GetTextMetrics(&tm);

	LONG cx=0, cy=0;
	while (lpszText)
	{
		LPTSTR lpsz = _tcschr(lpszText,'\n');
		CSize sz = pDC->GetTextExtent(lpszText, lpsz ? maximum(1,lpsz-lpszText) : strlen(lpszText));
		cx = maximum(sz.cx+tm.tmAveCharWidth, cx);
		cy += sz.cy;
		if (lpsz == NULL)
			break;
		lpszText = lpsz+1;
	}

	pDC->SelectObject(pOldFont);
	pWnd->ReleaseDC(pDC);

	return CSize(cx,cy);
}




/////////////////////////////////////////////////////////////////////////////
/* CFileWatchDlg dialog

class CFileWatchDlg : public CDialog
{
// Construction
public:
	CFileWatchDlg(LPCTSTR lpszText, BOOL* pAutoReload=NULL, BOOL* pAutoReload2=NULL, CWnd* pParent=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileWatchDlg)
	enum { IDD = IDD_FILEWATCH };
	BOOL	*m_pbAutoReload;
	BOOL	*m_pbAutoReload2;
	//}}AFX_DATA
	LPCTSTR m_lpszText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileWatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	enum { CENTER_HORZ=1, CENTER_VERT=2 };
	void CenterDlgItem(int* pID, int n, int flags=CENTER_HORZ|CENTER_VERT);
	void ShowDlgItem(int nID, BOOL bShow=TRUE);
	CRect SizeToContent(int nID);
	// Generated message map functions
	//{{AFX_MSG(CFileWatchDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CFileWatchDlg::CFileWatchDlg(LPCTSTR lpszText, BOOL* pAutoReload, BOOL* pAutoReload2, CWnd* pParent )
	: CDialog(CFileWatchDlg::IDD, pParent)
{
	m_lpszText = lpszText;
	//{{AFX_DATA_INIT(CFileWatchDlg)
	m_pbAutoReload = pAutoReload;
	m_pbAutoReload2 = pAutoReload2;
	//}}AFX_DATA_INIT
}


void CFileWatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileWatchDlg)
	//}}AFX_DATA_MAP
	if (m_pbAutoReload)
		DDX_Check(pDX, IDC_CHECK1, *m_pbAutoReload);
	if (m_pbAutoReload2)
		DDX_Check(pDX, IDC_CHECK2, *m_pbAutoReload2);
}


BEGIN_MESSAGE_MAP(CFileWatchDlg, CDialog)
	//{{AFX_MSG_MAP(CFileWatchDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileWatchDlg message handlers

BOOL CFileWatchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	TCHAR szAppName[_MAX_PATH];
	LPCTSTR pszAppName;
	if (this != NULL)
		pszAppName = AfxGetApp()->m_pszAppName;
	else
	{
		pszAppName = szAppName;
		GetModuleFileName(NULL, szAppName, _MAX_PATH);
	}
	SetWindowText(pszAppName);

	GetDlgItem(IDC_STATIC1)->SetWindowText(m_lpszText);
	CRect rcText = SizeToContent(IDC_STATIC1);

	CRect rc;
	GetWindowRect(&rc);
	int rightNew = rc.left + rcText.right + 20;
	if (rightNew > rc.right)
	{
		rc.right = rightNew;
		MoveWindow(&rc);
	}
	int nID[] = { IDOK,IDCANCEL };
	CenterDlgItem(nID, sizeof(nID)/sizeof(int), CENTER_HORZ);

	ShowDlgItem(IDC_CHECK1, m_pbAutoReload != NULL);
	ShowDlgItem(IDC_CHECK2, m_pbAutoReload2 != NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileWatchDlg::ShowDlgItem(int nID, BOOL bShow)
{
	GetDlgItem(nID)->SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOMOVE| (bShow ? SWP_SHOWWINDOW:SWP_HIDEWINDOW) );
}

CRect CFileWatchDlg::SizeToContent(int nID) 
{
	CWnd *pWnd = GetDlgItem(nID);

	CString sText;
	pWnd->GetWindowText(sText);
	CSize szText = GetTextExtend(pWnd, sText);

	CRect rcWnd;
	pWnd->GetWindowRect(&rcWnd); ScreenToClient(&rcWnd);
	rcWnd.right = rcWnd.left + szText.cx;
	pWnd->MoveWindow(&rcWnd);

	return rcWnd;
}

void CFileWatchDlg::CenterDlgItem(int* pID, int n, int flags) 
{
	CRect rc,rcItem;
	for (int i=0; i<n; i++)
	{
		GetDlgItem(pID[i])->GetWindowRect(&rcItem);
		rc = i ? rc|rcItem : rcItem;
	}

	CRect rcDlg;
	GetWindowRect(&rcDlg);
	CPoint dX = rc.CenterPoint() - rcDlg.CenterPoint();
	if (!(flags & CENTER_HORZ))
		dX.x = 0;
	if (!(flags & CENTER_VERT))
		dX.y = 0;

	for (i=0; i<n; i++)
	{
		GetDlgItem(pID[i])->GetWindowRect(&rcItem); ScreenToClient(&rcItem);
		rcItem -= dX;
		GetDlgItem(pID[i])->MoveWindow(&rcItem);
	}
}

*/


/////////////////////////////////////////////////////////////////////////////
// CFileWatch

CMap<DWORD, const DWORD&, FILEWATCHITEM, const FILEWATCHITEM&>		CFileWatch::m_FileMap;
CArrayEx<CString, const CString&>									CFileWatch::m_arFolder;
CArrayEx<HANDLE, const HANDLE&>										CFileWatch::m_arNotifyHandle;

CCriticalSection	CFileWatch::m_csDataLock;
CEvent				CFileWatch::m_EventUpdate;
DWORD				CFileWatch::m_dwNextHandle			= 1;
bool				CFileWatch::m_bStopWatch			= false;
CWinThread*			CFileWatch::m_pThread				= NULL;


void CFileWatch::StartThread()
{
	if (m_pThread == NULL)
	{
		m_bStopWatch = false;
		m_EventUpdate.SetEvent();
		m_pThread = AfxBeginThread(Watch, NULL, THREAD_PRIORITY_LOWEST);
		TRACE("FileWatch thread started\n");
	}
}
void CFileWatch::Stop()
{
	if (m_pThread)
	{
		m_csDataLock.Lock();
		m_bStopWatch = true;
		m_EventUpdate.SetEvent();
		m_csDataLock.Unlock();

		while (m_pThread)
			Sleep(10);
	}
}

DWORD CFileWatch::AddFileFolder(LPCTSTR lpszFileName, HWND hWnd, CDocument* pDocument)
{
	m_csDataLock.Lock();

	DWORD dwHandle = 0;
	CString sFolder = GetFileFolder(lpszFileName);
	__int64 ftLastWriteTime=0;
	if (IsFolder(lpszFileName) || GetLastWriteTime(lpszFileName, ftLastWriteTime))
	{

		if (g_msgFileWatchNotify == 0)
			g_msgFileWatchNotify = ::RegisterWindowMessage("FileWatchNotify");

		FILEWATCHITEM fwItem;
		fwItem.sFileName		= lpszFileName;
		fwItem.ftLastWriteTime	= ftLastWriteTime;
		fwItem.hWnd				= hWnd;
		fwItem.pDocument		= pDocument;
		//fwItem.bAutoReload		= false;
		//fwItem.pbAutoReload		= NULL;

		dwHandle = m_dwNextHandle++;
		m_FileMap.SetAt(dwHandle, fwItem);

		// new directory to watch?
		if (m_arFolder.Find(sFolder) == -1)
		{
			m_EventUpdate.SetEvent();
			HANDLE hChangeNotification = FindFirstChangeNotification(sFolder, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE);
			ASSERT(hChangeNotification != INVALID_HANDLE_VALUE);
			m_arFolder.Add(sFolder);
			m_arNotifyHandle.Add(hChangeNotification);
		}

		StartThread();
	}

	m_csDataLock.Unlock();
	return dwHandle;
}

void CFileWatch::SetData(DWORD dwHandle, DWORD dwData)
{
	m_csDataLock.Lock();
	FILEWATCHITEM fwItem;
	if (m_FileMap.Lookup(dwHandle, fwItem))
	{
		fwItem.dwData = dwData;
		m_FileMap.SetAt(dwHandle, fwItem);
	}
	m_csDataLock.Unlock();
}

void CFileWatch::SetAutoReload(DWORD dwHandle, BOOL* pbAutoReload)
{
	m_csDataLock.Lock();
	FILEWATCHITEM fwItem;
	if (m_FileMap.Lookup(dwHandle, fwItem))
	{
		//fwItem.pbAutoReload = pbAutoReload;
		m_FileMap.SetAt(dwHandle, fwItem);
	}
	m_csDataLock.Unlock();
}

void CFileWatch::RemoveHandle(DWORD dwHandle)
{
	m_csDataLock.Lock();

	FILEWATCHITEM fwItem;
	if (m_FileMap.Lookup(dwHandle, fwItem))
	{
		m_FileMap.RemoveKey(dwHandle);
		CString sFolder = GetFileFolder(fwItem.sFileName);
		bool bDeleteDir = true;
		POSITION pos = m_FileMap.GetStartPosition();
		while (pos)
		{
			DWORD dwHandle;
			FILEWATCHITEM fwItem;
			m_FileMap.GetNextAssoc(pos, dwHandle, fwItem);
			if (GetFileFolder(fwItem.sFileName) == sFolder)
			{
				bDeleteDir = false;
				break;
			}
		}

		if (bDeleteDir)
		{
			m_EventUpdate.SetEvent();
			int i = m_arFolder.Find(sFolder);
			FindCloseChangeNotification(m_arNotifyHandle[i]);
			m_arFolder.RemoveAt(i);
			m_arNotifyHandle.RemoveAt(i);
		}
	}

	m_csDataLock.Unlock();
	if (m_FileMap.IsEmpty())
		Stop();
}

bool CFileWatch::Dialog(LPCTSTR lpszPathFile, CWnd* pParent)
{
	CString sText;
	sText.Format(_T("%s\n\n%s"), lpszPathFile, _T("This file has been modified outside. Do you want to reload it and possibly lose the changes you made?"));

	// uncomment the next line for a simple message box
	return AfxMessageBox(sText, MB_YESNO | MB_ICONQUESTION) == IDYES;

	//CFileWatchDlg dlg(sText, pAutoReload, pParent);
	//return dlg.DoModal() == IDOK;
}

UINT CFileWatch::Watch(LPVOID)
{
	CArray<HANDLE, const HANDLE&> arHandle;
	arHandle.Add(m_EventUpdate);

	for (;;)
	{
		// wait for event or notification
		DWORD dwResult = WaitForMultipleObjects(arHandle.GetSize(), arHandle.GetData(), FALSE, INFINITE);
		m_csDataLock.Lock();
		TRACE("Notification\n");

		if (m_bStopWatch)
			break;

		int nObject = dwResult - WAIT_OBJECT_0;
		if (nObject==0)
		{
			TRACE("Update\n");
			m_EventUpdate.ResetEvent();

			// refresh list
			arHandle.SetSize(1);
			arHandle.Append(m_arNotifyHandle);
		}
		else if (nObject>0 && nObject<arHandle.GetSize())
		{
			POSITION pos = m_FileMap.GetStartPosition();
			TRACE("Notification Directory = %s\n", m_arFolder[nObject-1]);
			while (pos)
			{
				DWORD dwHandle;
				FILEWATCHITEM fwItem;
				m_FileMap.GetNextAssoc(pos, dwHandle, fwItem);
				if (GetFileFolder(fwItem.sFileName) == m_arFolder[nObject-1])
				{
					TRACE("Folder File = %s\n", fwItem.sFileName);
					__int64 ftLastWriteTime=0;
					if (IsFolder(fwItem.sFileName) || (GetLastWriteTime(fwItem.sFileName, ftLastWriteTime) && fwItem.ftLastWriteTime!=ftLastWriteTime))
					{
						TRACE("Notification File = %s\n", fwItem.sFileName);
						CWnd *pWnd = CWnd::FromHandle(fwItem.hWnd);
						if (fwItem.pDocument)
						{
							POSITION pos = fwItem.pDocument->GetFirstViewPosition();
							pWnd = fwItem.pDocument->GetNextView(pos);
						}
						
						if (pWnd && (theApp.m_opOptions.general.dwReloadFiles 
							//fwItem.bAutoReload || (fwItem.pbAutoReload && *fwItem.pbAutoReload)
							|| Dialog(fwItem.sFileName, AfxGetMainWnd()) 
							))
							pWnd->PostMessage(g_msgFileWatchNotify, WPARAM(fwItem.dwData), LPARAM(LPCTSTR(fwItem.sFileName)));
						
						fwItem.ftLastWriteTime = ftLastWriteTime;
						m_FileMap.SetAt(dwHandle, fwItem);
					}
				}
			}
			FindNextChangeNotification(arHandle[nObject]);
		}

		m_csDataLock.Unlock();
	}

	for (int i=0; i<m_arNotifyHandle.GetSize(); i++)
		FindCloseChangeNotification(m_arNotifyHandle[i]);
	m_FileMap.RemoveAll();
	m_arFolder.RemoveAll();
	m_arNotifyHandle.RemoveAll();

	m_csDataLock.Unlock();


	m_pThread = NULL;
	return 0;
}

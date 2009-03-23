// AddFolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bonfire.h"
#include "AddFolderDlg.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFolderDlg dialog


CAddFolderDlg::CAddFolderDlg(CWnd* pParent)
	: CDialog(CAddFolderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFolderDlg)
	m_strPath = _T("");
	m_bAddSubFolders = FALSE;
	m_strAddFiles = _T("");
	//}}AFX_DATA_INIT
}


void CAddFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFolderDlg)
	DDX_Control(pDX, IDC_ADD_FILES_EX, m_pAddFiles);
	DDX_Text(pDX, IDC_FOLDER_PATH, m_strPath);
	DDX_Check(pDX, IDC_OPT_ADD_SUBFOLDERS, m_bAddSubFolders);
	DDX_CBString(pDX, IDC_ADD_FILES_EX, m_strAddFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddFolderDlg, CDialog)
	//{{AFX_MSG_MAP(CAddFolderDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFolderDlg message handlers

BOOL CAddFolderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// load the choices from registry
	m_pAddFiles.SetMRURegKey( _T("Add Folder\\File Types") );
	m_pAddFiles.SetMRUValueFormat( _T("File Type #%d") );
	m_pAddFiles.SetAutoRefreshAfterAdd( TRUE );
	m_pAddFiles.SetMaxMRUSize(8);
	m_pAddFiles.LoadMRU();

	// update the controls
	m_pAddFiles.RefreshCtrl();

	// set initial values
	if (m_pAddFiles.GetCount() == 0)
	{
		m_pAddFiles.AddString("*.*");
		m_pAddFiles.AddString("*.xml;*.xsl;*.xsd;*.html;*.htm;*.xhtml;*.css");
	}

	UpdateData(FALSE);

	m_pAddFiles.SetCurSel(0);

	return TRUE;
}

void CAddFolderDlg::OnBrowse() 
{
	BROWSEINFO bi; memset(&bi,0,sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.lpszTitle = "Select the folder you wish to add below.";
	
	// first update UI -> variables
	UpdateData();

	LPITEMIDLIST pIdl = SHBrowseForFolder(&bi);
	if (pIdl)
	{
		char szbuf[MAX_PATH];
		SHGetPathFromIDList(pIdl,szbuf);
		m_strPath = szbuf;

		// update variables -> UI
		UpdateData(FALSE);

		LPMALLOC pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(pIdl);
			pMalloc->Release();
		}
	}
}

void CAddFolderDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (!this->UpdateData())
		return;

	// see if the folder actually exists
	struct _finddata_t c_file;

	if( _findfirst(m_strPath, &c_file) == -1L )
	{
		AfxMessageBox("The specified folder does not exist. Please try again.", MB_OK | MB_ICONEXCLAMATION, 0);
		return;
	}
	
	CString strAddFiles;
    
	// save the choices in registry
	m_pAddFiles.GetWindowText ( strAddFiles );
	m_pAddFiles.AddToMRU( strAddFiles );
	m_pAddFiles.SaveMRU();

	CDialog::OnOK();
}

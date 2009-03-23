// FindInFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bonfire.h"
#include "FindInFilesDlg.h"
#include "Downloaded Components\CrystalEdit\editreg.h" //registry name defs

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CBonfireApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFindInFilesDlg dialog

CFindInFilesDlg::CFindInFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindInFilesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindInFilesDlg)
	m_bWholeWord = FALSE;
	m_bMatchCase = FALSE;
	m_bSearchSubfolders = FALSE;
	m_strSearchText = _T("");
	m_strSearchPath = _T("");
	m_strFileTypes = _T("");
	//}}AFX_DATA_INIT
}


void CFindInFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindInFilesDlg)
	DDX_Control(pDX, IDC_SEARCH_FILETYPES_EX, m_pFileTypes);
	DDX_Control(pDX, IDC_SEARCH_PATH_EX, m_pSearchPath);
	DDX_Control(pDX, IDC_SEARCH_TEXT_EX, m_pSearchText);
	DDX_Check(pDX, IDC_MATCH_WHOLE_WORD, m_bWholeWord);
	DDX_Check(pDX, IDC_OPT_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_OPT_SEARCH_SUBFOLDERS, m_bSearchSubfolders);
	DDX_CBString(pDX, IDC_SEARCH_TEXT_EX, m_strSearchText);
	DDX_CBString(pDX, IDC_SEARCH_PATH_EX, m_strSearchPath);
	DDX_CBString(pDX, IDC_SEARCH_FILETYPES_EX, m_strFileTypes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindInFilesDlg, CDialog)
	//{{AFX_MSG_MAP(CFindInFilesDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindInFilesDlg message handlers

void CFindInFilesDlg::OnBrowse() 
{
	if (!UpdateData())
		return;

	BROWSEINFO bi; memset(&bi,0,sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.lpszTitle = "Select the folder you wish to search.";

	LPITEMIDLIST pIdl = SHBrowseForFolder(&bi);
	if (pIdl)
	{
		char szbuf[MAX_PATH];
		SHGetPathFromIDList(pIdl,szbuf);
		m_strSearchPath = szbuf;
		UpdateData(FALSE);

		LPMALLOC pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			pMalloc->Free(pIdl);
			pMalloc->Release();
		}
	}
}

void CFindInFilesDlg::OnOK() 
{
	if (!this->UpdateData())
		return;

	theApp.WriteProfileInt("Find", REG_MATCH_CASE, m_bMatchCase);
	theApp.WriteProfileInt("Find", REG_WHOLE_WORD, m_bWholeWord);
	theApp.WriteProfileInt("Find", REG_SUBFOLDERS, m_bSearchSubfolders);

	CString strSearchText;
    CString strSearchFileTypes;
    CString strSearchPath;
    
	// save the choices in registry
	m_pSearchText.GetWindowText ( strSearchText );
	m_pSearchText.AddToMRU( strSearchText );
	m_pSearchText.SaveMRU();
	m_pFileTypes.GetWindowText ( strSearchFileTypes );
	m_pFileTypes.AddToMRU( strSearchFileTypes );
	m_pFileTypes.SaveMRU();
	m_pSearchPath.GetWindowText ( strSearchPath );
	m_pSearchPath.AddToMRU( strSearchPath );
	m_pSearchPath.SaveMRU();

	CDialog::OnOK();
}

BOOL CFindInFilesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bMatchCase		= theApp.GetProfileInt("Find",REG_MATCH_CASE,0x00000000);
	m_bWholeWord		= theApp.GetProfileInt("Find",REG_WHOLE_WORD,0x00000000);
	m_bSearchSubfolders = theApp.GetProfileInt("Find",REG_SUBFOLDERS,0x00000000);


	// load the choices from registry
	m_pSearchText.SetMRURegKey( _T("Find\\Search String") );
	m_pSearchText.SetMRUValueFormat( _T("Search String #%d") );
	//m_pSearchText.SetAutoRefreshAfterAdd( TRUE );
	m_pSearchText.SetMaxMRUSize(8);
	m_pSearchText.LoadMRU();
	
	m_pFileTypes.SetMRURegKey( _T("Find\\File Types") );
	m_pFileTypes.SetMRUValueFormat( _T("File Type #%d") );
	//m_pFileTypes.SetAutoRefreshAfterAdd( TRUE );
	m_pFileTypes.SetMaxMRUSize(8);
	m_pFileTypes.LoadMRU();
	
	m_pSearchPath.SetMRURegKey( _T("Find\\Search Path") );
	m_pSearchPath.SetMRUValueFormat( _T("Search Path #%d") );
	//m_pSearchPath.SetAutoRefreshAfterAdd( TRUE );
	m_pSearchPath.SetMaxMRUSize(8);
	m_pSearchPath.LoadMRU();
	
	// update the controls
	m_pSearchText.RefreshCtrl();
	m_pFileTypes.RefreshCtrl();
	m_pSearchPath.RefreshCtrl();

	// set initial values
	if (m_pFileTypes.GetCount() == 0)
		m_pFileTypes.AddString("*.*");
	
	if (m_pSearchPath.GetCount() == 0)
		m_pSearchPath.AddString("C:\\");

	UpdateData(FALSE);

	// select the last inputted items
	m_pSearchText.SetCurSel(0);
	m_pFileTypes.SetCurSel(0);
	m_pSearchPath.SetCurSel(0);

	return TRUE;
}

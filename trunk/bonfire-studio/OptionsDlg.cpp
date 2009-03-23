// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsDlg.h"
#include "OptionsPages.h"
#include "BonfireDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CBonfireApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSelItem = 0;
}

COptionsDlg::~COptionsDlg()
{
	int i = m_arrPages.GetSize();
	while (i--)
		delete m_arrPages[i].pDialog;
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_TABS, m_wndTabs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABS, OnSelChangeTabs)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COptionsDlg::SelectTab(int nIndex)
{
	int nCount = m_arrPages.GetSize();
	for (int i = 0; i < nCount; i++)
		if (m_arrPages[i].pDialog->m_hWnd != NULL)
			m_arrPages[i].pDialog->ShowWindow(SW_HIDE);

	m_nSelItem = nIndex;
	if (m_arrPages[nIndex].pDialog->m_hWnd == NULL)
		m_arrPages[nIndex].pDialog->Create(m_arrPages[nIndex].nDlgID,this);

	POINT pt = {0,0}; ClientToScreen(&pt);
	CRect rc;
	m_wndTabs.GetWindowRect(rc);
	rc -= pt;
	m_wndTabs.AdjustRect(FALSE,rc);

	m_arrPages[nIndex].pDialog->SetWindowPos(NULL,
		rc.left,rc.top,rc.Width(),rc.Height(),0);
	m_arrPages[nIndex].pDialog->ShowWindow(SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// create dialogs
	m_arrPages.Add(PropPageData("General",IDD_OPTIONS_GENERAL_PAGE,new CGeneralPageDlg()));
	m_arrPages.Add(PropPageData("Project Bar",IDD_OPTIONS_PROJECTMGR_PAGE,new CProjectMgrPageDlg()));
	m_arrPages.Add(PropPageData("Editor",IDD_OPTIONS_EDITOR_PAGE,new CEditorPageDlg()));
	m_arrPages.Add(PropPageData("XML Tree",IDD_OPTIONS_XMLTREE_PAGE,new CXMLTreePageDlg()));
	m_arrPages.Add(PropPageData("Views",IDD_OPTIONS_VIEWS_PAGE,new CViewsPageDlg()));
	
	//m_arrPages.Add(PropPageData("Output",IDD_OPTIONS_OUTPUT_PAGE,new COutputPageDlg()));

	// create tabs
	CString str;
	int nCount = m_arrPages.GetSize();
	for (int i = 0; i < nCount; i++)
		m_wndTabs.InsertItem(i,m_arrPages[i].strCaption);

	SelectTab(m_nSelItem);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnSelChangeTabs(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SelectTab(m_wndTabs.GetCurSel());
	
	*pResult = 0;
}

void COptionsDlg::OnApply() 
{
	DoApply();
}

void COptionsDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void COptionsDlg::OnOK() 
{
	if (DoApply())	
		CDialog::OnOK();
}

BOOL COptionsDlg::DoApply()
{
	int nCount = m_arrPages.GetSize();
	for (int i = 0; i < nCount; i++)
		if (m_arrPages[i].pDialog->m_hWnd != NULL &&
			!m_arrPages[i].pDialog->SendMessage(WM_DOAPPLY))
			return FALSE;

	// traverse the document templates and the documents and do save on each
	POSITION posDocT = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = NULL;
	while ( posDocT != NULL)
	{
		pTemplate = theApp.GetNextDocTemplate( posDocT );
		ASSERT_KINDOF(CDocTemplate, pTemplate);
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		while (posDoc != NULL)
		{
			CBonfireDoc* pDoc = (CBonfireDoc*)pTemplate->GetNextDoc( posDoc );
			pDoc->UpdateAllViews(NULL, 1);
		}
	}

	return TRUE;
}

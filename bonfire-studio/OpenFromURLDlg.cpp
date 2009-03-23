// OpenFromURLDlg.cpp : implementation file
//

#include "stdafx.h"
#include <shlwapi.h>

#include "bonfire.h"
#include "OpenFromURLDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenFromURLDlg dialog


COpenFromURLDlg::COpenFromURLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenFromURLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenFromURLDlg)
	m_strURL = _T("");
	//}}AFX_DATA_INIT
}


void COpenFromURLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenFromURLDlg)
	DDX_Text(pDX, IDC_URL, m_strURL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenFromURLDlg, CDialog)
	//{{AFX_MSG_MAP(COpenFromURLDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenFromURLDlg message handlers

BOOL COpenFromURLDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SHAutoComplete(GetDlgItem(IDC_URL)->m_hWnd,SHACF_DEFAULT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

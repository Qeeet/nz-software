// GoToDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bonfire.h"
#include "GoToDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoToDlg dialog


CGoToDlg::CGoToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoToDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoToDlg)
	m_strGoToValue = _T("");
	//}}AFX_DATA_INIT
}


void CGoToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoToDlg)
	DDX_Control(pDX, IDC_GOTO_VALUE, m_pGoToValueCtrl);
	DDX_Text(pDX, IDC_GOTO_VALUE, m_strGoToValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGoToDlg, CDialog)
	//{{AFX_MSG_MAP(CGoToDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoToDlg message handlers

void CGoToDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	m_pGoToValueCtrl.SetFocus();
}

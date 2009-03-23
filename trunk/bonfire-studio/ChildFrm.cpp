// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Bonfire.h"
#include "BonfireDoc.h"
#include "ChildFrm.h"
#include "ProjectBar.h"
#include "MainFrm.h"
#include "SourceView.h"
#include "XMLTreeView.h"
#include "BrowserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_hIcon = NULL;	
}

CChildFrame::~CChildFrame()
{
	if (m_hIcon)
		DestroyIcon(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndTabBar.Create(this,20,
		WS_VISIBLE | WS_CHILD | CBRS_BOTTOM,
		AFX_IDW_CONTROLBAR_FIRST + 32))
	{
		TRACE0("Failed to create tab bar for frame\n");
		return -1;
	}

	m_wndTabBar.m_bBorderParentMaximized = TRUE;
	//m_wndTabBar.AddTab("Source");
	//m_wndTabBar.AddTab("XML Tree");
	//m_wndTabBar.AddTab("Browser Preview");

	return 0;
}

BOOL CChildFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if (wParam == (WPARAM)GetWindowLong(m_wndTabBar,GWL_ID))
	{
		NMTABBAR* pMessage = (NMTABBAR*)lParam;

		if (pMessage->tab != m_wndTabBar.GetSelTab())
			((CBonfireDoc*)GetActiveDocument())->SwitchToView( m_wndTabBar.GetTab(pMessage->tab) );
		else
			pMessage->cancel = TRUE;
	}
	else
	{
		NMHDR* pNotify = (NMHDR*)lParam;
		
		//TRACE("%d\t**",pNotify->code);
	
	}

	return CMDIChildWnd::OnNotify(wParam, lParam, pResult);
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
	//select tree node
	SelectCorrespondingNode();

	if (GetActiveView())
		GetActiveView()->SetFocus();

	CMDIChildWnd::OnSetFocus(pOldWnd);
}

void CChildFrame::SelectCorrespondingNode()
{
	// select the appropriate tree node in project explorer
	CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
	CProjectBar*	pProjectBar = &pMainFrame->m_wndProjectBar;
	CBonfireDoc*	pDoc		= (CBonfireDoc*)GetActiveDocument(); 
	
	// check if there is a pending node to be selected
	if (pDoc)
	{
		CXFile*		pXFile		= pDoc->m_pXFile;
		pProjectBar->SelectTreeNode(pXFile);
	}
}

void CChildFrame::UpdateIcon(CString strPathName)
{
	if (m_hIcon)
		DestroyIcon(m_hIcon);

	if (strPathName != "")
	{
		// get file icon
		//hIcon = GetIconIndex(strPathName,g_pMainFrame->m_wndProjectBar.Get 
		m_hIcon = GetIconForFile(strPathName, FALSE);
	}
	else
	{
		//return;
		m_hIcon = (HICON)LoadImage(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_BONFIRE_TYPE),
			IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),0);
	}

	SetIcon(m_hIcon, FALSE);
	UpdateWindow();
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	RecalcLayout();
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return m_wndSplitter.Create(this, 2, 2, CSize(30, 30), pContext);
	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~FWS_ADDTOTITLE;
	return CMDIChildWnd::PreCreateWindow(cs);
}

void CChildFrame::DrawTabs(CStringList *pTabList)
{
	// first remove all previous tabs
	int nSelTab = m_wndTabBar.GetSelTab();
	m_wndTabBar.ClearTabs(FALSE);

	// add the tabs for each view
	POSITION p = pTabList->GetTailPosition();
	while (p != NULL)
	{
		m_wndTabBar.AddTab(pTabList->GetPrev(p),FALSE);
	}
	m_wndTabBar.SetSelTab(nSelTab);
	m_wndTabBar.RedrawWindow();
}
